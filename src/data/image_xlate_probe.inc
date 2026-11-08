// Stand-in for image_xlate.inc while re/image/gen_xlate.py asks the compiler for the x64 layout that file is made
// from (JOAN_IMAGE_LAYOUT_PROBE: re/image/probe.py; never part of a build). Same X-macros, one identity run.
//   JOAN_IMAGE_X64_SIZE(size)   sizeof(layout::Image) on x64
//   JOAN_XLATE(va, off)         a run: original addresses va.. (up to the next run) are at off + (address - va)
// before including. Runs are sorted by va; the first starts at the image (0x9cf000, offset 0).
#ifndef JOAN_IMAGE_X64_SIZE
#define JOAN_IMAGE_X64_SIZE(size)
#define JOAN_XLATE_UNDEF_SIZE
#endif
#ifndef JOAN_XLATE
#define JOAN_XLATE(va, off)
#define JOAN_XLATE_UNDEF_RUN
#endif
JOAN_IMAGE_X64_SIZE(0xa1000u)
JOAN_XLATE(0x9cf000u, 0x0u)
#ifdef JOAN_XLATE_UNDEF_SIZE
#undef JOAN_IMAGE_X64_SIZE
#undef JOAN_XLATE_UNDEF_SIZE
#endif
#ifdef JOAN_XLATE_UNDEF_RUN
#undef JOAN_XLATE
#undef JOAN_XLATE_UNDEF_RUN
#endif
