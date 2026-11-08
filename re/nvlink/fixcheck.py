# Semantic check of the [Fixes] Shaders=1 vertex shaders (NVLINK.md 13).
#
#   c++ -std=c++20 -O2 -o nvlink_test src/render/nvlink_test.cpp src/render/nvlink.cpp
#   python3 fixcheck.py SHADER_DIR path/to/nvlink_test [samples]
#
# For every fragment list the game can build (gen.corpus(3), 1810 lists) it links the original
# program (nvlink.py, = nvlinker.dll) and the corrected one (`nvlink_test --fixed`, the C++ core
# with fixes on; `--exact` for the original, checked against nvlink.py), runs both with vsrun.py on random vertices / constants laid out the way the game
# uploads them (light constants by GetConstantSlot instance = light index of its type, game upload
# order), and compares every output with a plain reference implementation of what the fragments
# are meant to compute: D3D-style lighting (diffuse N.L, Blinn-Phong specular with a local viewer
# and the material power, point / spot attenuation 1/(a0 + a1 d + a2 d^2) with the range test,
# linear spot cone, directional lights), linear / height fog, DOT3 light vector, 2-bone skinning,
# clip plane. Each program also runs twice with different garbage in r0..r11 (an output that
# changes reads a never-written register), and nvlink_test reports every temporary read before
# it was written (UNINIT: after allocation such a read aliases another value's register).
# Scattering lists are not modelled: their outputs must be identical in both programs.
import math, os, random, subprocess, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import gen, nvlink, vsrun, vsdis

DECL = {1: gen.DECL1, 2: gen.DECL2, 5: gen.DECL5, 7: gen.DECL7}
LOG2E = 1.4426950408889634


def dot3(a, b): return a[0] * b[0] + a[1] * b[1] + a[2] * b[2]
def dot4(a, b): return sum(a[k] * b[k] for k in range(4))
def sub(a, b): return [a[k] - b[k] for k in range(3)]
def add(a, b): return [a[k] + b[k] for k in range(3)]
def scale(a, s): return [x * s for x in a]
def norm(a):
    l = math.sqrt(dot3(a, a)); return [x / l for x in a[:3]]
def length(a): return math.sqrt(dot3(a, a))


def rotation(rnd):
    q = [rnd.gauss(0, 1) for _ in range(4)]; l = math.sqrt(sum(x * x for x in q)); w, x, y, z = [v / l for v in q]
    return [[1 - 2 * (y * y + z * z), 2 * (x * y - z * w), 2 * (x * z + y * w)],
            [2 * (x * y + z * w), 1 - 2 * (x * x + z * z), 2 * (y * z - x * w)],
            [2 * (x * z - y * w), 2 * (y * z + x * w), 1 - 2 * (x * x + y * y)]]


def inputs(decl, rnd):
    v = [[0.0, 0.0, 0.0, 1.0] for _ in range(16)]
    for t in decl.split():
        t = int(t, 16)
        if t >> 28 != 4: continue
        ty, reg = (t >> 16) & 0xf, t & 0x1f
        if ty == 0: v[reg] = [rnd.uniform(-1, 1), 0, 0, 1]
        elif ty == 1: v[reg] = [rnd.uniform(-1, 1), rnd.uniform(-1, 1), 0, 1]
        elif ty == 2: v[reg] = [rnd.uniform(-3, 3), rnd.uniform(-3, 3), rnd.uniform(-3, 3), 1.0]
        elif ty == 4: v[reg] = [rnd.uniform(0, 1) for _ in range(4)]
        elif ty == 6: v[reg] = [float(3 * rnd.randrange(0, 23)), float(3 * rnd.randrange(0, 23)), 0, 1]
    if v[13] != [0.0, 0.0, 0.0, 1.0]:  # bone weights (FLOAT2): w0 + w1 = 1
        w = rnd.uniform(0, 1); v[13] = [w, 1 - w, 0, 1]
    for r in (3, 8, 9):  # normals / tangents: unit length in object space
        if v[r][:3] != [0.0, 0.0, 0.0]: v[r] = norm(v[r]) + [1.0]
    return v


class Scene:
    """Constants for one sample: fixed registers (NVLINK.md 5) + per-light values."""
    def __init__(self, rnd, lights, vtype):
        c = [[rnd.uniform(-1, 1) for _ in range(4)] for _ in range(96)]  # garbage everywhere else
        c[0] = [0.0, 0.5, 1.0, LOG2E]
        R = rotation(rnd); s = rnd.uniform(0.5, 2.0); T = [rnd.uniform(-20, 20) for _ in range(3)]
        if vtype == 7: R = [[1, 0, 0], [0, 1, 0], [0, 0, 1]]; s = 1.0; T = [0, 0, 0]  # identity world (0xa18a78)
        for k in range(3): c[5 + k] = [R[k][0] * s, R[k][1] * s, R[k][2] * s, T[k]]
        for k in range(4): c[1 + k] = [rnd.uniform(-1, 1) for _ in range(4)]
        for k in range(4): c[13 + k] = [rnd.uniform(-1, 1) for _ in range(4)]
        c[9] = [rnd.uniform(0, .3) for _ in range(4)]      # global ambient
        c[11] = [rnd.uniform(0, 1) for _ in range(4)]     # material ambient / diffuse / specular
        c[12] = [rnd.uniform(0, 1) for _ in range(4)]
        c[20] = [rnd.uniform(0, 1) for _ in range(4)]
        c[22][3] = rnd.uniform(2, 30)                     # material power
        self.eye = [rnd.uniform(-30, 30) for _ in range(3)]
        c[10] = self.eye + [rnd.uniform(-1, 1)]           # .w: garbage (stack layout, NVLINK.md / STAGE2)
        self.c = c; self.rnd = rnd; self.lights = []
        self.fog = [rnd.uniform(-5, 5), rnd.uniform(0.01, 0.2), 0, 0]
        start, end = rnd.uniform(0, 20), rnd.uniform(40, 80)
        self.fog_range = (start, end); self.fog[2] = 1 / (end - start); self.fog[3] = -start / (end - start)
        self.clip = [rnd.uniform(-1, 1) for _ in range(4)]
        self.dot3 = norm([rnd.uniform(-1, 1) for _ in range(3)])
        for kind in lights:
            L = {'kind': kind, 'diffuse': [rnd.uniform(0, 1) for _ in range(4)], 'specular': [rnd.uniform(0, 1) for _ in range(4)]}
            if kind in (1, 2):
                L['position'] = [rnd.uniform(-30, 30) for _ in range(3)]
                L['att'] = [rnd.uniform(0.2, 1), rnd.uniform(0, 0.1), rnd.uniform(0, 0.01)]
                L['range'] = rnd.uniform(20, 90)
            if kind in (2, 3):
                L['direction'] = norm([rnd.uniform(-1, 1) for _ in range(3)])
            if kind == 2:
                theta = rnd.uniform(0.3, 1.5); phi = theta + rnd.uniform(0.2, 1.5)
                L['theta'], L['phi'], L['falloff'] = theta, phi, 1.0
            self.lights.append(L)

    def upload(self, slots, vtype):
        """GetConstantSlot(name, instance) -> register, in the game's order (0x61ae30 / 0x61c390)."""
        c = self.c
        def put(name, k, value):
            s = slots.get(name, [])
            if k < len(s) and s[k] >= 0: c[s[k]] = list(value)
        n = {1: 0, 2: 0, 3: 0}
        for L in self.lights:
            i = n[L['kind']]; n[L['kind']] += 1
            if L['kind'] == 1:
                put('c_point_light_position', i, L['position'] + [1.0])
                put('c_point_light_attenuation', i, L['att'] + [L['range']])
                put('c_point_light_diffuse', i, L['diffuse']); put('c_point_light_specular', i, L['specular'])
            elif L['kind'] == 3:
                put('c_directional_light_direction', i, L['direction'] + [0.0])
                put('c_directional_light_diffuse', i, L['diffuse']); put('c_directional_light_specular', i, L['specular'])
            else:
                put('c_spot_light_position', i, L['position'] + [1.0])
                put('c_spot_light_direction', i, L['direction'] + [0.0])
                put('c_spot_light_attenuation', i, L['att'] + [L['range']])
                put('c_spot_light_diffuse', i, L['diffuse']); put('c_spot_light_specular', i, L['specular'])
                ct, cp = math.cos(L['theta'] / 2), math.cos(L['phi'] / 2)
                put('c_spot_light_falloff', i, [ct, cp, L['falloff'], 1 / (ct - cp)])
        put('c_fog_constant', 0, self.fog)
        put('c_clip_plane', 0, self.clip)
        put('c_dot3_light_direction', 0, self.dot3 + [0.0])
        for name in ('c_sun_direction', 'c_sun_color', 'c_f_hg', 'c_beta1_plus_beta2', 'c_one_over_beta1_plus_beta2',
                     'c_beta_dash_1', 'c_beta_dash_2', 'c_terrain_reflectance', 'c_term_multpliers',
                     'c_texture_coordinate_transformation1_1', 'c_texture_coordinate_transformation1_2'):
            put(name, 0, [self.rnd.uniform(0.1, 1) for _ in range(4)])
        return c


def reference(names, v, sc, c):
    """What the fragment list means (independent of the fragment code). Output -> 4 values, None = don't care."""
    o = {}
    def out(key, comps, value):
        cur = o.setdefault(key, [None] * 4)
        for k in comps: cur[k] = value[k]
    OPOS, OFOG, OPTS = (4, 0), (4, 1), (4, 2)
    W = [c[5], c[6], c[7]]
    P = N = None; d = e2v = None; diff = spec = None; fog = [None, None]; spec_lit = False
    p = c[22][3]
    def world_pos(x): return [dot4(x, W[k]) for k in range(3)]
    def world_dir(x): return [dot3(x, W[k]) for k in range(3)]
    def light(L, specular):
        nonlocal diff, spec, spec_lit
        if L['kind'] == 3:
            Ld = scale(L['direction'], -1); att = 1.0
        else:
            to = sub(L['position'], P); dist = length(to); Ld = scale(to, 1 / dist)
            a = L['att']; att = (1.0 if L['range'] >= dist else 0.0) / (a[0] + a[1] * dist + a[2] * dist * dist)
            if L['kind'] == 2:
                cos_a = dot3(scale(Ld, -1), L['direction'])
                ct, cp = math.cos(L['theta'] / 2), math.cos(L['phi'] / 2)
                att *= min(max((cos_a - cp) / (ct - cp), 0.0), 1.0)
        NL = dot3(N, Ld)
        for k in range(3): diff[k] += L['diffuse'][k] * max(NL, 0.0) * att
        if specular:
            V = norm(sub(sc.eye, P)); H = norm(add(V, Ld)); NH = dot3(N, H)
            f = NH ** p if NL > 0 and NH > 0 else 0.0
            for k in range(3): spec[k] += L['specular'][k] * f * att
            spec_lit = True
    counts = {1: 0, 2: 0, 3: 0}
    def next_light(kind):
        i = counts[kind]; counts[kind] += 1
        return [L for L in sc.lights if L['kind'] == kind][i]
    for name in names:
        if name == 'transform_and_write_position': out(OPOS, range(4), [dot4(v[0], c[1 + k]) for k in range(4)])
        elif name == 'write_untransformed_position': out(OPOS, range(4), v[0])
        elif name == 'write_vertex_psize': out(OPTS, [0], v[4])
        elif name == 'set_default_diffuse_color': diff = [1.0] * 4
        elif name == 'set_default_specular_color': spec = [0.0] * 4
        elif name == 'clear_diffuse_color': diff = [0.0, 0.0, 0.0, 1.0]
        elif name == 'clear_specular_color': spec = [0.0, 0.0, 0.0, 1.0]
        elif name == 'write_vertex_diffuse_color': out((5, 0), range(4), v[5])
        elif name == 'write_vertex_specular_color': out((5, 1), range(4), v[6])
        elif name == 'write_diffuse_color_with_vertex_diffuse':   # diffuse source = vertex colour
            out((5, 0), range(4), [diff[k] * v[5][k] + c[9][k] * c[11][k] for k in range(3)] + [v[5][3]])
        elif name == 'write_diffuse_color_with_material_diffuse':  # D3D: alpha = material diffuse alpha
            out((5, 0), range(4), [diff[k] * c[12][k] + c[9][k] * c[11][k] for k in range(3)] + [c[12][3]])
        elif name == 'write_diffuse_color': out((5, 0), range(4), [diff[k] + c[9][k] for k in range(4)])
        elif name.startswith('write_specular_color'):
            m = v[6] if name.endswith('vertex_specular') else c[20] if name.endswith('material_specular') else [1.0] * 4
            out((5, 1), range(4), [spec[k] * m[k] for k in range(4)])
            if spec_lit: o[(5, 1)][3] = None   # specular alpha: not displayed
        elif name.startswith('write_default_texture_coordinate_'):
            n = int(name[-1]); out((6, n), range(4), v[7 + n])
        elif name == 'clear_texture_coordinates':
            for n in range(8): out((6, n), range(4), [0.0] * 4)
        elif name == 'calculate_objectspace_position':
            if P is None: P = world_pos(v[0])   # (after skinning the world position is the skinned one)
        elif name == 'calculate_objectspace_normal': N = norm(world_dir(v[3]))
        elif name == 'transform_to_eyespace': pass
        elif name == 'create_normalized_eye_to_vertex':
            if P is None: P = world_pos(v[0])   # the world position, whoever computes it
            d = length(sub(P, sc.eye))
        elif name in ('add_point_light', 'add_point_light_no_specular'): light(next_light(1), not name.endswith('specular'))
        elif name in ('add_directional_light', 'add_directional_light_no_specular'): light(next_light(3), not name.endswith('specular'))
        elif name in ('add_spot_light', 'add_spot_light_no_specular'): light(next_light(2), not name.endswith('specular'))
        elif name == 'calculate_dot3_light':
            T, B, Nn = world_dir(v[8]), world_dir(v[9]), world_dir(v[3])
            l = norm([dot3(T, sc.dot3), dot3(B, sc.dot3), dot3(Nn, sc.dot3)])
            spec = [x * 0.5 + 0.5 for x in l] + [1.0]
            for n in range(3): out((6, n), range(4), v[7])
            out(OFOG, [0], [0.0])
        elif name == 'calculate_linear_normal_fog_factor':   # D3D linear fog on the eye distance
            start, end = sc.fog_range; fog = [(end - d) / (end - start), 0.0]
        elif name == 'calculate_height_fog_factor':
            fog = [d * sc.fog[2], (P[2] + sc.fog[0]) * sc.fog[1]]
        elif name == 'write_d3d_fog_factor': out(OFOG, [0], fog)
        elif name.startswith('write_fog_to_texture_coordinate_'): out((6, int(name[-1])), [0, 1], fog)
        elif name == 'calculate_skin_vertex':
            def bone(i, x, n):
                return [(dot4 if n == 4 else dot3)(x, c[(17 + i + k) % 96]) for k in range(3)]
            w0, w1 = v[13][0], v[13][1]; b0, b1 = int(v[12][0]), int(v[12][1])
            Pm = add(scale(bone(b0, v[0], 4), w0), scale(bone(b1, v[1], 4), w1)) + [1.0]
            Nm = add(scale(bone(b0, v[8], 3), w0), scale(bone(b1, v[9], 3), w1))
            out(OPOS, range(4), [dot4(Pm, c[1 + k]) for k in range(4)])
            P = world_pos(Pm); N = norm(world_dir(Nm))
        elif name == 'clip_plane': out((6, 3), [0], [dot4(P + [1.0], sc.clip)])
        else: raise Exception('no reference for ' + name)
    return o


def close(a, b):
    return abs(a - b) <= 1e-4 * max(1.0, abs(a), abs(b))


def compare(ref, got):
    """Mismatching outputs as strings."""
    bad = []
    for key in sorted(set(ref) | set(got)):
        if key not in got: bad.append('%s missing' % (key,)); continue
        if key not in ref: bad.append('%s unexpected' % (key,)); continue
        for k in range(4):
            if ref[key][k] is not None and not close(ref[key][k], got[key][k]):
                bad.append('%s%s' % ({(4, 0): 'oPos', (4, 1): 'oFog', (4, 2): 'oPts'}.get(key, ('oD%d' if key[0] == 5 else 'oT%d') % key[1]), '.' + 'xyzw'[k]))
    return bad


def validate(tokens):
    """vs.1.x operand rules: one constant and one input register per instruction."""
    problems = []
    for line in vsdis.dis(tokens):
        ops = [x.strip().lstrip('-').split('.')[0] for x in line.split(' ', 1)[1].split(',')[1:]]
        if len({x for x in ops if x.startswith('c')}) > 1: problems.append('two constants: ' + line)
        if len({x for x in ops if x.startswith('v')}) > 1: problems.append('two inputs: ' + line)
    return problems


def run_dump(binary, shader_dir, cases, scratch, mode):
    script = os.path.join(scratch, 'fixcheck_script.txt')
    with open(script, 'w') as f:
        for i, (vtype, names, info) in enumerate(cases):
            f.write('new\nreserve 0 %d\ndecl %s\necho CASE %d\nlink %s\n' % (88 if vtype == 7 else 22, DECL[vtype], i, ' '.join(names)))
            for cn in CONSTS:
                for k in range(4): f.write('slot %s %d\n' % (cn, k))
    outp = subprocess.run([binary, '--' + mode, shader_dir, script], capture_output=True, text=True, check=True).stdout
    res = {}; cur = None
    for line in outp.splitlines():
        p = line.split()
        if not p: continue
        if p[0] == 'CASE': cur = int(p[1]); res[cur] = {'tokens': None, 'slots': {}, 'fallback': None, 'uninit': []}
        elif p[0] == 'UNINIT': res[cur]['uninit'].append(line[7:])
        elif p[0] == 'TOKENS': res[cur]['tokens'] = [int(x, 16) for x in p[1:]]
        elif p[0] == 'FALLBACK': res[cur]['fallback'] = line
        elif p[0] == 'SLOT':
            reg = int(p[5].split('=')[1].split('(')[0])
            res[cur]['slots'].setdefault(p[1], []).append(reg)
        elif p[0] == '#' and not line.startswith('# applied'): print(line)
    for r in res.values():  # trim trailing -1 (no such instance)
        for k, s in r['slots'].items():
            while s and s[-1] < 0: s.pop()
    return res


CONSTS = ['c_fog_constant', 'c_point_light_position', 'c_point_light_attenuation', 'c_point_light_diffuse',
          'c_point_light_specular', 'c_directional_light_direction', 'c_directional_light_diffuse',
          'c_directional_light_specular', 'c_spot_light_position', 'c_spot_light_direction', 'c_spot_light_attenuation',
          'c_spot_light_diffuse', 'c_spot_light_specular', 'c_spot_light_falloff', 'c_dot3_light_direction',
          'c_sun_direction', 'c_sun_color', 'c_f_hg', 'c_beta1_plus_beta2', 'c_one_over_beta1_plus_beta2',
          'c_beta_dash_1', 'c_beta_dash_2', 'c_terrain_reflectance', 'c_term_multpliers',
          'c_texture_coordinate_transformation1_1', 'c_texture_coordinate_transformation1_2', 'c_clip_plane']


def main():
    shader_dir, binary = sys.argv[1], sys.argv[2]
    samples = int(sys.argv[3]) if len(sys.argv) > 3 else 4
    scratch = os.environ.get('TMPDIR', '/tmp')
    frags, version = nvlink.load(shader_dir)
    cases = [(vtype, list(names), info) for (vtype, names), info in gen.corpus(3).items()]
    dumps = {m: run_dump(binary, shader_dir, cases, scratch, m) for m in ('exact', 'fixed')}
    fixed = dumps['fixed']
    stats = {'orig': {}, 'fixed': {}}
    linked = {'orig': 0, 'fixed': 0}; fallbacks = 0; rule_problems = []
    uninit = {'orig': 0, 'fixed': 0}; wrong = {'orig': 0, 'fixed': 0}; scatter_changed = 0
    examples = {'orig': {}, 'fixed': {}}
    both = tokens_changed = outputs_changed = 0
    static_uninit = {'orig': {}, 'fixed': {}}; outs_static = {'orig': 0, 'fixed': 0}
    for i, (vtype, names, info) in enumerate(cases):
        toks, slots = nvlink.link(frags, version, names, 88 if vtype == 7 else 22)
        assert toks == dumps['exact'][i]['tokens'], 'C++ exact link differs from nvlink.py'
        progs = {'orig': (toks, slots if toks else None), 'fixed': (fixed[i]['tokens'], fixed[i]['slots'])}
        for which, mode in (('orig', 'exact'), ('fixed', 'fixed')):
            for u in dumps[mode][i]['uninit']:
                static_uninit[which][u] = static_uninit[which].get(u, 0) + 1
            if dumps[mode][i]['uninit']: outs_static[which] += 1
        if fixed[i]['fallback']: fallbacks += 1
        if fixed[i]['tokens']: rule_problems += validate(fixed[i]['tokens'])
        rnd = random.Random(1000 + i)
        outs = {}
        for s in range(samples):
            v = inputs(DECL[vtype], rnd)
            seed = rnd.random()
            for which, (t, sl) in progs.items():
                if t is None: continue
                sc = Scene(random.Random(seed), info[4], vtype)
                c = sc.upload(sl, vtype)
                g1 = vsrun.run(t, v, c, [[random.Random(seed + 1).uniform(-9, 9) for _ in range(4)] for _ in range(12)])
                g2 = vsrun.run(t, v, c, [[random.Random(seed + 2 + r).uniform(-9, 9) for _ in range(4)] for r in range(12)])
                if compare(g1, g2): outs.setdefault(which, set()).add('uninit')
                outs.setdefault(which + '_all', []).append(g1)
                if 'scattering' in names:
                    outs.setdefault(which + '_out', []).append(g1); continue
                bad = compare(reference(names, v, sc, c), g1)
                if bad:
                    outs.setdefault(which, set()).add('wrong')
                    for b in bad:
                        stats[which][b] = stats[which].get(b, 0) + 1
                        examples[which].setdefault(b, (vtype, names))
        for which in ('orig', 'fixed'):
            if progs[which][0] is not None: linked[which] += 1
            if 'uninit' in outs.get(which, ()): uninit[which] += 1
            if 'wrong' in outs.get(which, ()): wrong[which] += 1
        if progs['orig'][0] and progs['fixed'][0]:
            both += 1
            if progs['orig'][0] != progs['fixed'][0]: tokens_changed += 1
            if any(compare(a, b) for a, b in zip(outs['orig_all'], outs['fixed_all'])): outputs_changed += 1
        if 'scattering' in names and outs.get('orig_out') and outs.get('fixed_out'):
            for a, b in zip(outs['orig_out'], outs['fixed_out']):
                diff = compare(a, b)
                # fix 7 is the only one that reaches scattering lists: oTn.y of linear texture fog = 0
                if 'calculate_linear_normal_fog_factor' in names:
                    diff = [x for x in diff if not (x in ('oT1.y', 'oT2.y') and b[(6, int(x[2]))][1] == 0.0)]
                if diff: scatter_changed += 1; break
    print('%d game lists, %d samples each' % (len(cases), samples))
    for which in ('orig', 'fixed'):
        print('%-5s: %d linked, %d lists differ from the reference, %d lists change with the garbage in r0..r11, '
              '%d lists read a temporary before writing it' % (which, linked[which], wrong[which], uninit[which], outs_static[which]))
        for u, n in sorted(static_uninit[which].items(), key=lambda x: -x[1]):
            print('        read before written in %4d lists: %s' % (n, u))
        for b, n in sorted(stats[which].items(), key=lambda x: -x[1])[:25]:
            print('        %-8s wrong in %4d samples  e.g. type %d: %s' % (b, n, examples[which][b][0], ' '.join(examples[which][b][1])))
    print('of the %d lists both link: %d get different tokens, %d compute different outputs' % (both, tokens_changed, outputs_changed))
    print('fixed: %d lists fell back to the original program, %d scattering lists changed, vs.1.x rule problems: %d'
          % (fallbacks, scatter_changed, len(rule_problems)))
    for p in rule_problems[:5]: print('   ', p)
    return 0 if wrong['fixed'] == 0 and uninit['fixed'] == 0 and outs_static['fixed'] == 0 and scatter_changed == 0 and not rule_problems else 1


if __name__ == '__main__':
    sys.exit(main())
