#pragma once
#ifndef __ASSEMBLER__
#include "patch/include/uapi/gen/scpaths_decode.h"

/* Map về getter runtime nếu chưa có */
#ifndef SUPERCMD
#  define KP_SUPERCMD() kp_get_supercmd()
#  define SUPERCMD      (KP_SUPERCMD())
#endif
#ifndef SU_PATH
#  define KP_SU_PATH()  kp_get_su_path()
#  define SU_PATH       (KP_SU_PATH())
#endif

/* ---- Tripwire: FAIL compile nếu SUPERCMD là hằng compile-time ---- */
#ifdef KP_STRICT_SUPERCMD
  /* C11: _Static_assert, chạy ở compile phase (không phải preprocessor) */
  _Static_assert(!__builtin_constant_p(SUPERCMD),
                 "SUPERCMD is constant in this TU (plaintext leak)");
#endif

#endif /* __ASSEMBLER__ */
