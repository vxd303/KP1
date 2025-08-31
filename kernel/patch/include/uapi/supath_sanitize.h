#pragma once
#include "patch/include/uapi/scdefs.h"   // đã trả SUPERCMD/SU_PATH bằng getter

/* Xoá mọi define cũ (nếu có) và ép về getter runtime */
#ifdef SUPERCMD
# undef SUPERCMD
#endif
#ifdef SU_PATH
# undef SU_PATH
#endif
#define SUPERCMD (KP_SUPERCMD())
#define SU_PATH  (KP_SU_PATH())
