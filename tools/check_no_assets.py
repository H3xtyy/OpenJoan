#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-or-later
"""Asset lint (ADR-0001 item 6): does any file of the repository contain a run of bytes of the game's files?

Usage (from anywhere):
  check_no_assets.py [--game <JOAN_GAME_DIR>] [--repo <dir>] [--window 32] [--allow <file>] [--strict]
                     [--max-report 20] [--require-game] [--verbose]

Reference (what may not be in the repository): every file of the game install `--game` (default: the environment
variable JOAN_GAME_DIR), read whole, except the PE executables and drivers in it (`*.exe`, `*.dll`, `*.sys`): of those
only `joan.exe`'s resource section `.rsrc` (icon, dialog) is a reference. The executable's `.rdata`/`.data` are NOT a
reference: their values are the committed typed source of src/data/ (src/data/README.md); its
`.text` is machine code, which review keeps out (no lint can tell a port from a paste). Point `--game` at the pristine
install, never at the JOAN_WORK_DIR copy that holds the built executable: a build of this repository matches the
repository.

Files checked: what git tracks plus untracked files that are not ignored (`git ls-files --cached --others
--exclude-standard`). Each file is searched
  raw         as bytes (images, blobs, anything copied verbatim),
and a text file (source, scripts, docs) also as the byte streams its literals spell:
  u8 / u16 / u32 / u64   consecutive integer literals (0x.., decimal, 'c', MASM 0..h) as little-endian values of that
                         width (a literal that does not fit ends the run),
  f32 / f64              consecutive floating literals (with / without an f suffix),
  str                    the bytes of its string literals (C escapes decoded), adjacent literals concatenated.
A hit is a run of at least --window bytes (default 32; 512 against a save game, which is a memory dump that holds
the executable's static data as well) that equals a run of a game file, has at least 6 distinct byte values (fills
like zero or 0xFF runs are not data) and is not trivial: a counting / constant-step or monotonic index sequence at
some word width, a repeated short pattern, zero padding around fewer than 16 non-zero bytes, one identifier-like
name of at most 64 bytes, one line of printable text of at most 80 bytes (a quoted name, path or script line is a
quotation, not a copied file), or a public standard constant (the T.81 Huffman tables that every JPEG carries, the
ASF GUIDs that every .wmv / .wma carries). Everything longer or denser is reported: a dump of a script, a text
table, a model, an image or a sound cannot pass. Every hit is reported with file, line, stream, length and
the game file and offset it equals.

Method: the repository's streams are indexed by 16-byte keys taken every (window - 15) bytes (a shared run of
`window` bytes contains one of them; keys with fewer than 3 distinct bytes are not indexed, so a run whose only
aligned key is a fill can be missed, as with joan-re's check_no_game_bytes.py); the game files are hashed at every
byte offset with numpy and looked up in the sorted key table; every candidate is verified byte by byte and extended
to the maximal run. About 1.1 GB of game files take two to four minutes.

Allow list (default tools/check_no_assets.allow, absent = empty): lines `<glob>  <reason>`; hits in matching files
are reported as ALLOWED and do not fail the check. --strict ignores the allow list (the publish gate). The list is
meant to stay empty: nothing in this repository may be a copy of a game file.

Exit code: 0 no (unallowed) hits, or no game folder available (prints SKIP; --require-game makes that an error);
1 hits; 2 usage / setup error.
"""
import argparse
import bisect
import fnmatch
import os
import re
import struct
import subprocess
import sys

try:
    import numpy as np
except ImportError:  # pragma: no cover
    np = None

KEY = 16                       # bytes per index key
MIN_DISTINCT = 6               # distinct byte values a reported run must have
MIN_KEY_DISTINCT = 3           # distinct byte values an indexed key must have (fills carry no information)
MAX_PER_HASH = 8               # index entries kept per key value
MAX_CANDIDATES_PER_HASH = 64   # game positions verified per key value and chunk (repeated patterns)
CHUNK = 16 << 20               # game file bytes hashed at once
PE_EXT = {'.exe', '.dll', '.sys'}
OUR_PRODUCTS = ('joan_re', 'openjoan', 'libsdl3.so', 'libopenal.so', 'sdl3.dll', 'soft_oal.dll')
TEXT_EXT = {'.c', '.cc', '.cpp', '.cxx', '.h', '.hh', '.hpp', '.hxx', '.inc', '.inl', '.asm', '.s', '.py', '.txt', '.md',
            '.rc', '.json', '.jsonl', '.ini', '.csv', '.tsv', '.cmake', '.sh', '.bat', '.ps1', '.yml', '.yaml', '.toml',
            '.cfg', '.def', '.xml', '.html', '.gitignore', '.allow', '.java', '.glsl', '.hlsl', ''}
C1 = np.uint64(0x9E3779B97F4A7C15) if np else None
C2 = np.uint64(0xC2B2AE3D27D4EB4F) if np else None


# --------------------------------------------------------------------------------------------------- reference
def pe_sections(data, name):
    pe = struct.unpack_from('<I', data, 0x3C)[0]
    if data[pe:pe + 4] != b'PE\0\0':
        raise SystemExit('check_no_assets: %s is not a PE file' % name)
    nsec = struct.unpack_from('<H', data, pe + 6)[0]
    optsz = struct.unpack_from('<H', data, pe + 20)[0]
    out = {}
    so = pe + 24 + optsz
    for _ in range(nsec):
        sname = data[so:so + 8].rstrip(b'\0').decode('latin1')
        vsize, va, rsize, roff = struct.unpack_from('<IIII', data, so + 8)
        out[sname] = data[roff:roff + (min(rsize, vsize) if vsize else rsize)]
        so += 40
    return out


def reference_files(game):
    """[(label, loader)] of the game's files; loader() returns the bytes"""
    out = []
    for root, dirs, files in os.walk(game):
        dirs.sort()
        for fn in sorted(files):
            path = os.path.join(root, fn)
            rel = os.path.relpath(path, game)
            low = fn.lower()
            if any(low.startswith(p) for p in OUR_PRODUCTS) or low.endswith('.log'):
                continue   # a build of this repository installed into the game folder: not a reference
            ext = os.path.splitext(low)[1]
            if ext in PE_EXT:
                if low == 'joan.exe' and root == game:
                    out.append((rel + ':.rsrc', (lambda p: lambda: pe_sections(open(p, 'rb').read(), p)['.rsrc'])(path)))
                continue
            if os.path.islink(path) or not os.path.isfile(path):
                continue
            out.append((rel, (lambda p: lambda: open(p, 'rb').read())(path)))
    return out


# ------------------------------------------------------------------------------------------------- text streams
TOKEN = re.compile(r'''
    (?P<str>"(?:[^"\\\n]|\\.)*")
  | (?P<chr>'(?:[^'\\\n]|\\.){1,4}')
  | (?P<flt>(?<![\w.])(?:\d+\.\d*|\.\d+|\d+(?=[eE]))(?:[eE][+-]?\d+)?(?P<fs>[fF])?(?![\w.]))
  | (?P<hex>(?<![\w.])0[xX][0-9a-fA-F]+(?:[uU]?[lL]{0,2}|[lL]{1,2}[uU])(?![\w.]))
  | (?P<masm>(?<![\w.])[0-9][0-9a-fA-F]*[hH](?![\w.]))
  | (?P<dec>(?<![\w.])\d+(?:[uU]?[lL]{0,2}|[lL]{1,2}[uU])(?![\w.]))
  | (?P<nl>\n)
''', re.X)
ESC = {'n': 10, 't': 9, 'r': 13, '0': 0, 'a': 7, 'b': 8, 'f': 12, 'v': 11, '\\': 92, '"': 34, "'": 39, '?': 63}
WIDTH = {'u8': 1, 'u16': 2, 'u32': 4, 'u64': 8, 'f32': 4, 'f64': 8}


def unescape(body):
    out = bytearray()
    i = 0
    while i < len(body):
        c = body[i]
        if c != '\\' or i + 1 >= len(body):
            out += c.encode('utf-8', 'replace')
            i += 1
            continue
        e = body[i + 1]
        if e == 'x':
            m = re.match(r'[0-9a-fA-F]+', body[i + 2:])
            if m:
                out.append(int(m.group(0)[-2:], 16))
                i += 2 + len(m.group(0))
                continue
        if e in '01234567':
            m = re.match(r'[0-7]{1,3}', body[i + 1:])
            out.append(int(m.group(0), 8) & 0xFF)
            i += 1 + len(m.group(0))
            continue
        out.append(ESC.get(e, ord(e) & 0xFF))
        i += 2
    return bytes(out)


class Stream:
    """one byte stream of a repository file: raw bytes, or the segments one kind of literal spells"""
    def __init__(self, file, kind):
        self.file = file
        self.kind = kind
        self.segments = []      # [bytearray, [token offsets], [token lines]]; raw: one segment, no tokens
        self.cur = None

    def put(self, b, line):
        if self.cur is None:
            self.cur = [bytearray(), [], []]
            self.segments.append(self.cur)
        self.cur[1].append(len(self.cur[0]))
        self.cur[2].append(line)
        self.cur[0] += b

    def brk(self):
        self.cur = None


def text_streams(file, text):
    names = ('u8', 'u16', 'u32', 'u64', 'f32', 'f64', 'str')
    st = {k: Stream(file, k) for k in names}
    line = 1
    for m in TOKEN.finditer(text):
        g = m.lastgroup
        if g == 'nl':
            line += 1
            continue
        if g == 'str':
            st['str'].put(unescape(m.group(0)[1:-1]), line)
            continue
        if g == 'flt':
            for k in ('u8', 'u16', 'u32', 'u64'):
                st[k].brk()
            try:
                v = float(m.group(0).rstrip('fF'))
            except ValueError:
                continue
            if m.group('fs'):
                try:
                    st['f32'].put(struct.pack('<f', v), line)
                except OverflowError:
                    st['f32'].brk()
                st['f64'].brk()
            else:
                st['f64'].put(struct.pack('<d', v), line)
                st['f32'].brk()
            continue
        if g == 'chr':
            b = unescape(m.group(0)[1:-1])
            v = b[0] if len(b) == 1 else None
        else:
            t = m.group(0).rstrip('uUlL')
            v = int(t[:-1], 16) if g == 'masm' else int(t, 0) if g == 'hex' else int(t, 10)
        st['f32'].brk()
        st['f64'].brk()
        for k, w in (('u8', 1), ('u16', 2), ('u32', 4), ('u64', 8)):
            if v is not None and v < (1 << (8 * w)):
                st[k].put(v.to_bytes(w, 'little'), line)
            else:
                st[k].brk()
    return [st[k] for k in names]


IDENT = re.compile(rb'[A-Za-z0-9_.]+')
MAX_NAME = 64
MAX_LINE = 80                  # bytes of printable text that count as a quotation, not a copy
MAX_SPARSE = 16                # non-zero bytes below which a run is a padded name / header, not data
MAX_STEPS = 8                  # distinct values of a monotonic word sequence that is an index table
SAVE_WINDOW = 512              # minimum run against a save game (a memory dump of the game's static data too)
PRINTABLE = frozenset(range(0x20, 0x7f)) | {0x09, 0x0a, 0x0d}


def _guid(text):
    a, b, c, d, e = text.split('-')
    return (int(a, 16).to_bytes(4, 'little') + int(b, 16).to_bytes(2, 'little') + int(c, 16).to_bytes(2, 'little')
            + bytes.fromhex(d) + bytes.fromhex(e))


# Public standard constants that every implementation and every file of the format carries: not copies of a game file.
# ITU-T T.81 Annex K.3 typical Huffman tables (also the DHT segments of every baseline JPEG), and the GUIDs of the
# ASF specification (also the headers of every .wmv / .wma).
KNOWN_STANDARD = [
    bytes.fromhex('00010501010101010100000000000000000102030405060708090a0b'),   # DC luminance bits + values
    bytes.fromhex('00030101010101010101010000000000000102030405060708090a0b'),   # DC chrominance bits + values
    bytes.fromhex(         # AC luminance bits + values
        '0002010303020403050504040000017d01020300041105122131410613516107227114328191a1082342b1c11552d1f0'
        '2433627282090a161718191a25262728292a3435363738393a434445464748494a535455565758595a63646566676869'
        '6a737475767778797a838485868788898a92939495969798999aa2a3a4a5a6a7a8a9aab2b3b4b5b6b7b8b9bac2c3c4c5'
        'c6c7c8c9cad2d3d4d5d6d7d8d9dae1e2e3e4e5e6e7e8e9eaf1f2f3f4f5f6f7f8f9fa'),
    bytes.fromhex(         # AC chrominance bits + values
        '00020102040403040705040400010277000102031104052131061241510761711322328108144291a1b1c109233352f0'
        '156272d10a162434e125f11718191a262728292a35363738393a434445464748494a535455565758595a636465666768'
        '696a737475767778797a82838485868788898a92939495969798999aa2a3a4a5a6a7a8a9aab2b3b4b5b6b7b8b9bac2c3'
        'c4c5c6c7c8c9cad2d3d4d5d6d7d8d9dae2e3e4e5e6e7e8e9eaf2f3f4f5f6f7f8f9fa'),
]
ASF_GUIDS = [_guid(g) for g in (
    '75B22630-668E-11CF-A6D9-00AA0062CE6C', '75B22636-668E-11CF-A6D9-00AA0062CE6C',   # header, data
    '8CABDCA1-A947-11CF-8EE4-00C00C205365', 'B7DC0791-A9B7-11CF-8EE6-00C00C205365',   # file / stream properties
    'F8699E40-5B4D-11CF-A8FD-00805F5C442B', 'BC19EFC0-5B4D-11CF-A8FD-00805F5C442B',   # audio / video media
    'BFC3CD50-618F-11CF-8BB2-00AA00B4E220', '20FB5700-5B55-11CF-A8FD-00805F5C442B',   # audio spread / no error corr.
    '5FBF03B5-A92E-11CF-8EE3-00C00C205365', '86D15240-311D-11D0-A3A4-00A0C90348F6',   # header extension, codec list
    '33000890-E5B1-11CF-89F4-00A0C90349CB', '75B22633-668E-11CF-A6D9-00AA0062CE6C',   # simple index, content descr.
    'D2D0A440-E307-11D2-97F0-00A0C95EA850', '7BF875CE-468D-11D1-8D82-006097C9A2B2',   # ext. content descr., bitrates
)]


def known_standard(run):
    """True when the run lies inside a public standard constant or is a sequence of ASF GUIDs"""
    for k in KNOWN_STANDARD:
        if run in k:
            return True
    n = len(run)
    for a in range(16):
        head, body = run[:a], run[a:]
        if head and not any(g.endswith(head) for g in ASF_GUIDS):
            continue
        ok = True
        i = 0
        while i + 16 <= len(body):
            if body[i:i + 16] not in ASF_GUIDS:
                ok = False
                break
            i += 16
        tail = body[i:]
        if ok and (not tail or any(g.startswith(tail) for g in ASF_GUIDS)):
            return True
    return False


def trivial_run(run):
    """True for byte runs any program spells without copying anything: a counting / constant-step sequence at some
    word width and alignment (index tables, 00 01 02 .., 01 00 02 00 .., ff ff ff fe ff ff ff ..), a repetition of
    a short pattern, or one identifier-like token (a name of the game's API; the executable's names are code strings,
    ADR-0001) of at most MAX_NAME bytes"""
    n = len(run)
    for p in range(1, 9):
        if n > p and all(run[i] == run[i + p] for i in range(n - p)):
            return True
    for w in (1, 2, 4, 8):
        for a in range(w):
            k = (n - a) // w
            if k < 3:
                continue
            words = [int.from_bytes(run[a + i * w:a + (i + 1) * w], 'little') for i in range(k)]
            step = (words[1] - words[0]) % (1 << (8 * w))
            if all((words[i + 1] - words[i]) % (1 << (8 * w)) == step for i in range(k - 1)):
                return True
    if n <= MAX_NAME and IDENT.fullmatch(run):
        return True
    printable = [c in PRINTABLE for c in run]
    if any(printable):
        first, last = printable.index(True), n - 1 - printable[::-1].index(True)
        if first <= 4 and n - 1 - last <= 4 and last - first < MAX_LINE and all(printable[first:last + 1]):
            return True     # one line of text (with a terminator / length byte at the ends): a quotation of a
                            # name, a path or a script line, not a copied file
    if sum(1 for c in run if c) < MAX_SPARSE:
        return True     # zero padding around a short name or count
    for w in (1, 2, 4, 8):
        for a in range(w):
            k = (n - a) // w
            if k < 3:
                continue
            words = [int.from_bytes(run[a + i * w:a + (i + 1) * w], 'little') for i in range(k)]
            if len(set(words)) <= MAX_STEPS and (words == sorted(words) or words == sorted(words, reverse=True)):
                return True     # an index table: few distinct values in order
    if known_standard(run):
        return True
    return False


def data_like(kind, seg, offs, st, ln):
    """False for runs of literals that any code spells: counting / index sequences (constant step) and short runs of
    small integers in a wide stream (0, 1, 2, 3 as u32 are 0x00-padded bytes that tables of game files also have)"""
    w = WIDTH.get(kind)
    if w is None:
        return True
    i = bisect.bisect_left(offs, st)
    j = bisect.bisect_left(offs, st + ln - w + 1)
    vals = [int.from_bytes(seg[o:o + w], 'little') for o in offs[i:j]]
    if len(vals) < 3:
        return True
    steps = {(vals[k + 1] - vals[k]) for k in range(len(vals) - 1)}
    if len(steps) <= 1:
        return False
    if w > 1 and kind.startswith('u') and max(vals) < 0x100 and len(vals) < 24:
        return False
    return True


# --------------------------------------------------------------------------------------------------------- files
def repo_files(repo):
    out = subprocess.run(['git', '-C', repo, 'ls-files', '-z', '--cached', '--others', '--exclude-standard'],
                         capture_output=True, check=True).stdout
    return sorted({f for f in out.decode('utf-8', 'replace').split('\0') if f})


def load_allow(path):
    rules = []
    if path and os.path.exists(path):
        for ln in open(path, encoding='utf-8'):
            ln = ln.strip()
            if not ln or ln.startswith('#'):
                continue
            parts = ln.split(None, 1)
            rules.append((parts[0], parts[1] if len(parts) > 1 else ''))
    return rules


# --------------------------------------------------------------------------------------------------------- index
def u64_at_every_offset(b):
    """uint64 little-endian value at every byte offset of the uint8 array b (length len(b) - 7)"""
    m = len(b) - 7
    out = b[0:m].astype(np.uint64)
    for j in range(1, 8):
        out |= b[j:m + j].astype(np.uint64) << np.uint64(8 * j)
    return out


def key_hashes(b):
    """hash of the 16-byte key at every offset of the uint8 array b (length len(b) - 15)"""
    u = u64_at_every_offset(b)
    m = len(b) - KEY + 1
    return (u[:m] * C1) ^ (u[8:8 + m] * C2)


class Index:
    """sorted table: key hash -> (segment id, offset) for the keys taken every `stride` bytes of every segment"""
    def __init__(self, segments, stride):
        self.segments = segments            # [(stream, segment)]
        hs, ids, offs = [], [], []
        for sid, (stream, seg) in enumerate(segments):
            data = seg[0]
            if len(data) < KEY:
                continue
            b = np.frombuffer(bytes(data), dtype=np.uint8)
            pos = np.arange(0, len(b) - KEY + 1, stride)
            keys = np.lib.stride_tricks.sliding_window_view(b, KEY)[pos]
            distinct = (np.diff(np.sort(keys, axis=1), axis=1) != 0).sum(axis=1) + 1
            pos = pos[distinct >= MIN_KEY_DISTINCT]
            if len(pos) == 0:
                continue
            h = key_hashes(b)[pos]
            hs.append(h)
            ids.append(np.full(len(pos), sid, dtype=np.int32))
            offs.append(pos.astype(np.int64))
        if not hs:
            self.h = np.zeros(0, dtype=np.uint64)
            self.sid = np.zeros(0, dtype=np.int32)
            self.off = np.zeros(0, dtype=np.int64)
            return
        h = np.concatenate(hs)
        order = np.argsort(h, kind='stable')
        h = h[order]
        sid = np.concatenate(ids)[order]
        off = np.concatenate(offs)[order]
        # at most MAX_PER_HASH entries per key value
        first = np.ones(len(h), dtype=bool)
        first[1:] = h[1:] != h[:-1]
        group_start = np.maximum.accumulate(np.where(first, np.arange(len(h)), 0))
        keep = (np.arange(len(h)) - group_start) < MAX_PER_HASH
        self.h, self.sid, self.off = h[keep], sid[keep], off[keep]

    def lookup(self, hashes):
        """for each query hash: index of its first entry, or -1"""
        if len(self.h) == 0:
            return np.full(len(hashes), -1, dtype=np.int64)
        idx = np.searchsorted(self.h, hashes)
        idx_c = np.minimum(idx, len(self.h) - 1)
        found = self.h[idx_c] == hashes
        return np.where(found, idx_c, -1)


def extend(a, i, b, j):
    """maximal run shared by bytes a around i and bytes b around j: (start in a, start in b, length)"""
    lo = 0
    while lo < i and lo < j and a[i - lo - 1] == b[j - lo - 1]:
        lo += 1
    hi = 0
    na, nb = len(a), len(b)
    while i + hi < na and j + hi < nb and a[i + hi] == b[j + hi]:
        hi += 1
    return i - lo, j - lo, lo + hi


# ---------------------------------------------------------------------------------------------------------- main
def main():
    here = os.path.dirname(os.path.abspath(__file__))
    ap = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    ap.add_argument('--game', default=os.environ.get('JOAN_GAME_DIR', ''), help='the game install (default: $JOAN_GAME_DIR)')
    ap.add_argument('--repo', default=os.path.dirname(here))
    ap.add_argument('--window', type=int, default=32)
    ap.add_argument('--allow', default=os.path.join(here, 'check_no_assets.allow'))
    ap.add_argument('--strict', action='store_true')
    ap.add_argument('--max-report', type=int, default=20, help='hits listed per file')
    ap.add_argument('--require-game', action='store_true', help='fail instead of SKIP when no game folder is available')
    ap.add_argument('--verbose', action='store_true')
    a = ap.parse_args()
    if not a.game or not os.path.isdir(a.game):
        msg = 'check_no_assets: SKIP: no game folder (set JOAN_GAME_DIR or pass --game)'
        if a.require_game:
            print(msg.replace('SKIP', 'ERROR'), file=sys.stderr)
            return 2
        print(msg)
        return 0
    if np is None:
        print('check_no_assets: numpy is required (dnf install python3-numpy)', file=sys.stderr)
        return 2
    if a.window < KEY + 1:
        print('check_no_assets: --window must be >= %d' % (KEY + 1), file=sys.stderr)
        return 2
    stride = a.window - KEY + 1
    allow = [] if a.strict else load_allow(a.allow)

    # 1. the repository's streams
    files = repo_files(a.repo)
    segments = []        # [(stream, segment)]
    total_bytes = 0
    for rel in files:
        path = os.path.join(a.repo, rel)
        if not os.path.isfile(path) or os.path.islink(path):
            continue
        data = open(path, 'rb').read()
        total_bytes += len(data)
        raw = Stream(rel, 'raw')
        raw.segments.append([data, [], []])
        segments.append((raw, raw.segments[0]))
        ext = os.path.splitext(rel)[1].lower()
        if ext in TEXT_EXT and b'\0' not in data[:4096]:
            for s in text_streams(rel, data.decode('utf-8', 'replace')):
                for seg in s.segments:
                    if len(seg[0]) >= a.window:
                        segments.append((s, seg))
    index = Index(segments, stride)
    if a.verbose:
        print('check_no_assets: %d files (%d bytes), %d streams, %d index keys' % (
            len(files), total_bytes, len(segments), len(index.h)))

    # 2. the game's files, hashed at every offset
    refs = reference_files(a.game)
    hits = {}            # stream id -> [(start, length, game label, game offset)]
    covered = {}         # stream id -> [(start, end)]
    scanned = 0
    overlap = a.window + KEY
    for label, load in refs:
        data = load()
        scanned += len(data)
        window = max(a.window, SAVE_WINDOW) if label.lower().endswith('.sav') else a.window
        if len(data) < window:
            continue
        for base in range(0, len(data), CHUNK):
            chunk = data[base:base + CHUNK + overlap]
            if len(chunk) < KEY:
                break
            b = np.frombuffer(chunk, dtype=np.uint8)
            h = key_hashes(b)
            found = index.lookup(h)
            cand = np.nonzero(found >= 0)[0]
            if len(cand) == 0:
                continue
            # bound the work per key value (repeated patterns)
            order = np.argsort(h[cand], kind='stable')
            cand = cand[order]
            hc = h[cand]
            first = np.ones(len(hc), dtype=bool)
            first[1:] = hc[1:] != hc[:-1]
            gstart = np.maximum.accumulate(np.where(first, np.arange(len(hc)), 0))
            cand = cand[(np.arange(len(hc)) - gstart) < MAX_CANDIDATES_PER_HASH]
            for g in cand.tolist():
                k = int(found[g])
                hv = h[g]
                while k < len(index.h) and index.h[k] == hv:
                    sid, off = int(index.sid[k]), int(index.off[k])
                    k += 1
                    cov = covered.get(sid)
                    if cov and any(s <= off < e for s, e in cov):
                        continue
                    stream, seg = index.segments[sid]
                    sdata = seg[0]
                    if sdata[off:off + KEY] != chunk[g:g + KEY]:
                        continue    # hash collision
                    st, gt, ln = extend(sdata, off, chunk, g)
                    if ln < window or len(set(sdata[st:st + ln])) < MIN_DISTINCT:
                        continue
                    if trivial_run(bytes(sdata[st:st + ln])):
                        continue
                    if stream.kind != 'raw' and not data_like(stream.kind, sdata, seg[1], st, ln):
                        continue
                    covered.setdefault(sid, []).append((st, st + ln))
                    hits.setdefault(sid, []).append((st, ln, label, base + gt))

    # 3. report per repository file
    by_file = {}
    for sid, lst in hits.items():
        stream, seg = index.segments[sid]
        for st, ln, label, goff in lst:
            if stream.kind == 'raw':
                data = seg[0]
                line = data.count(b'\n', 0, st) + 1 if b'\0' not in data[:4096] else 0
            else:
                line = seg[2][max(0, bisect.bisect_right(seg[1], st) - 1)]
            by_file.setdefault(stream.file, []).append((stream.kind, line, ln, label, goff))
    failed = allowed = 0
    for rel in sorted(by_file):
        found = by_file[rel]
        reason = next((r for g, r in allow if fnmatch.fnmatch(rel, g)), None)
        nbytes = sum(f[2] for f in found)
        tag = 'ALLOWED' if reason is not None else 'HIT'
        print('%s %s: %d run(s), %d bytes of game files%s' % (tag, rel, len(found), nbytes,
                                                              (' - ' + reason) if reason else ''))
        if reason is None:
            failed += 1
            for kind, line, ln, label, goff in sorted(found, key=lambda f: (-f[2], f[1]))[:a.max_report]:
                print('    line %6d  %-4s %6d bytes = %s +0x%x' % (line, kind, ln, label, goff))
        else:
            allowed += 1
    print('check_no_assets: %d files (%d bytes) against %d game files (%d bytes) of %s (window %d bytes): %d with hits%s' % (
        len(files), total_bytes, len(refs), scanned, a.game, a.window, failed,
        (', %d allowed (%s)' % (allowed, os.path.relpath(a.allow, a.repo))) if allowed else ''))
    return 1 if failed else 0


if __name__ == '__main__':
    sys.exit(main())
