#pragma once
#include "patch/include/uapi/gen/scpaths_obf.h"

/* size_t without <stddef.h> */
#ifndef _KP_SIZE_T_DEFINED_
#  ifndef __SIZE_TYPE__
#    define __SIZE_TYPE__ unsigned long
#  endif
typedef __SIZE_TYPE__ size_t;
#define _KP_SIZE_T_DEFINED_
#endif

/* Alias tên cũ ↔ mới (nếu cần) */
#if !defined(KP_SUPERCMD_LEN) && defined(KP_SC_LEN)
#  define KP_SUPERCMD_LEN KP_SC_LEN
#endif
#if !defined(KP_SUPERPATH_LEN) && defined(KP_SU_LEN)
#  define KP_SUPERPATH_LEN KP_SU_LEN
#endif

static inline void _kp_decode_into(char *dst, const unsigned char *src, size_t n) {
    for (size_t i = 0; i < n; ++i) dst[i] = (char)(src[i] ^ 0xA5);
    dst[n] = '\0';
}

/* Nếu muốn cứng rắn, bật #error khi thiếu obf ở build release */
#ifndef KP_DECODE_ALLOW_FALLBACK
#  define KP_DECODE_ALLOW_FALLBACK 1   /* đặt 0 để cấm fallback */
#endif

static inline const char *kp_get_supercmd(void) {
#ifdef KP_SUPERCMD_LEN
    extern const unsigned char KP_SUPERCMD_OBF[KP_SUPERCMD_LEN];
    static char buf[KP_SUPERCMD_LEN + 1];
    static int inited;
    if (!inited) { _kp_decode_into(buf, KP_SUPERCMD_OBF, KP_SUPERCMD_LEN); inited = 1; }
    return buf;
#else
  #if KP_DECODE_ALLOW_FALLBACK
    static const char *fallback = "/system/bin/truncate"; /* dev-only */
    return fallback;
  #else
    #error "Missing obfuscated SUPERCMD (KP_SUPERCMD_LEN). Did you generate scpaths_obf.h?"
  #endif
#endif
}

static inline const char *kp_get_su_path(void) {
#ifdef KP_SUPERPATH_LEN
    extern const unsigned char KP_SUPERPATH_OBF[KP_SUPERPATH_LEN];
    static char buf[KP_SUPERPATH_LEN + 1];
    static int inited;
    if (!inited) { _kp_decode_into(buf, KP_SUPERPATH_OBF, KP_SUPERPATH_LEN); inited = 1; }
    return buf;
#else
  #if KP_DECODE_ALLOW_FALLBACK
    static const char *fallback = "/system/bin/kp"; /* dev-only */
    return fallback;
  #else
    #error "Missing obfuscated SU_PATH (KP_SUPERPATH_LEN). Did you generate scpaths_obf.h?"
  #endif
#endif
}
