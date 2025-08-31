#pragma once

/* ĐỪNG include scdefs.h ở đây để tránh kéo theo uid_t/struct...
   Chỉ cần decode getters là đủ. */
#ifndef __ASSEMBLER__  /* để .S không dính header này */
#include "patch/include/uapi/gen/scpaths_decode.h"  // kp_get_supercmd(), kp_get_su_path()
#ifdef KP_STRICT_SUPERCMD
#  if __builtin_constant_p(SUPERCMD)
#    error "SUPERCMD is constant in this TU (plaintext leak)"
#  endif
#endif

/* Xoá mọi định nghĩa cũ nếu có, ép về getter runtime */
#ifdef SUPERCMD
# undef SUPERCMD
#endif
#ifdef SU_PATH
# undef SU_PATH
#endif

/* Dùng getters – không tạo literal */
#define KP_SUPERCMD() kp_get_supercmd()
#define KP_SU_PATH()  kp_get_su_path()

#define SUPERCMD (KP_SUPERCMD())
#define SU_PATH  (KP_SU_PATH())

#endif /* !__ASSEMBLER__ */
