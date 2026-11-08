# Test corpus for the C++ linker (src/render/nvlink_test.cpp), stage 3 N3.
#
#   python3 gen_n3.py script  > n3.txt               harness script: the 1810 game lists (as the game
#                                                     links them) + random lists with random
#                                                     reservations / optimize flag
#   harness/nvl nvlinker.dll SHADER n3.txt > n3_dll.txt    ground truth (the real DLL)
#   python3 gen_n3.py model SHADER n3.txt > n3_model.txt  the same transcript from nvlink.py
#   nvlink_test SHADER n3_dll.txt / n3_model.txt          the C++ linker against either
#
# Each case is `new` (fresh linker, the seven files), an `echo CASE ...` line naming the optimize
# flag and the reservation, `link`, then `slot` for every constant name x instances 0..3.
import os, random, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import gen, nvlink, nvo

CONSTS = ['c_fog_constant', 'c_point_light_position', 'c_point_light_attenuation', 'c_point_light_diffuse',
          'c_point_light_specular', 'c_directional_light_direction', 'c_directional_light_diffuse',
          'c_directional_light_specular', 'c_spot_light_position', 'c_spot_light_direction', 'c_spot_light_attenuation',
          'c_spot_light_diffuse', 'c_spot_light_specular', 'c_spot_light_falloff', 'c_dot3_light_direction',
          'c_sun_direction', 'c_sun_color', 'c_f_hg', 'c_beta1_plus_beta2', 'c_one_over_beta1_plus_beta2',
          'c_beta_dash_1', 'c_beta_dash_2', 'c_terrain_reflectance', 'c_term_multpliers',
          'c_texture_coordinate_transformation1_1', 'c_texture_coordinate_transformation1_2', 'c_clip_plane']
DECL = {1: gen.DECL1, 2: gen.DECL2, 5: gen.DECL5, 7: gen.DECL7}


def cases(nrandom=3000, seed=20261009):
    out = []  # (optimize, reserve (lo, hi) or None, decl, names)
    for (vtype, names), info in gen.corpus(3).items():
        out.append((1, (0, 88 if vtype == 7 else 22), DECL[vtype], list(names)))
    # the game lists once more without the optimizer
    for (vtype, names), info in list(gen.corpus(1).items())[::3]:
        out.append((0, (0, 88 if vtype == 7 else 22), DECL[vtype], list(names)))
    return out, random.Random(seed)


def random_cases(names, rng, n):
    out = []
    for _ in range(n):
        length = rng.choice([1, 2, 3, 5, 8, 12, 16, 20, 28])
        lst = [rng.choice(names) for _ in range(rng.randint(1, length))]
        opt = 0 if rng.random() < 0.2 else 1
        r = rng.random()
        if r < 0.3: res = (0, 22)
        elif r < 0.45: res = (0, 88)
        elif r < 0.6: res = None
        else:
            lo = rng.randint(0, 100); res = (lo, lo + rng.randint(0, 60))
        out.append((opt, res, gen.DECL1, lst))
    return out


def all_cases(shader_dir):
    frags, _ = nvlink.load(shader_dir)
    names = []
    for f in nvlink.FILES:
        _, _, fl = nvo.fragments(os.path.join(shader_dir, f))
        names += [n for n, _ in fl]
    base, rng = cases()
    return base + random_cases(names, rng, 3000)


def write_script(shader_dir):
    last_opt = 1
    for i, (opt, res, decl, names) in enumerate(all_cases(shader_dir)):
        if opt != last_opt:
            print('create 100 %d' % opt); last_opt = opt
        print('new')
        if res: print('reserve %d %d' % res)
        print('decl ' + decl)
        print('echo CASE %d opt=%d reserve=%s' % (i, opt, '%d,%d' % res if res else 'none'))
        print('link ' + ' '.join(names))
        for c in CONSTS:
            for k in range(4): print('slot %s %d' % (c, k))


HR = {'out of constants': 0x80040401, 'out of temporaries': 0x80040403}


def write_model(shader_dir, script):
    """The transcript the harness would print, computed with nvlink.py."""
    frags, version = nvlink.load(shader_dir)
    fid = {}; sid = {}; n = 0
    for f in nvlink.FILES:
        _, syms, fl = nvo.fragments(os.path.join(shader_dir, f))
        for name, _ in fl:
            n += 1; fid.setdefault(name, n)
        for s in syms: sid.setdefault(s, 125 + len(sid))
    opt = 1; res = None
    for line in open(script):
        p = line.split()
        if not p: continue
        if p[0] == 'create': opt = int(p[2])
        elif p[0] == 'new': res = None; last = None
        elif p[0] == 'reserve': res = (int(p[1]), int(p[2]))
        elif p[0] == 'echo': print(' '.join(p[1:]))
        elif p[0] == 'link':
            names = p[1:]
            print('LINK ' + ' '.join('%s=%d' % (x, fid[x]) for x in names))
            lo, hi = res if res else (1, 0)
            toks, slots = nvlink.link(frags, version, names, hi, reserve_lo=lo, optimize=bool(opt))
            if toks is None:
                hr = 0x80040400 if slots.startswith('too many') else HR[slots]
                print('CreateShader hr=%x handle=0' % hr); last = None
            else:
                print('CreateShader hr=0 handle=1000')
                print('TOKENS ' + ' '.join('%08x' % t for t in toks)); last = slots
        elif p[0] == 'slot':
            name, k = p[1], int(p[2])
            if last is None:
                print('SLOT %s %d id=%x(hr 0) reg=-2(hr 80004005)' % (name, k, sid[name]))
            else:
                s = last.get(name, [])
                print('SLOT %s %d id=%x(hr 0) reg=%d(hr 0)' % (name, k, sid[name], s[k] if k < len(s) else -1))


if __name__ == '__main__':
    if sys.argv[1] == 'script': write_script(sys.argv[2] if len(sys.argv) > 2 else os.environ.get('SHADER', 'SHADER'))
    elif sys.argv[1] == 'model': write_model(sys.argv[2], sys.argv[3])
