/* Auto-decoder for obfuscated paths (no std headers, works with -nostdinc) */
#pragma once
#include "patch/include/uapi/gen/scpaths_obf.h"

/* Provide size_t without <stddef.h> */
#ifndef _KP_SIZE_T_DEFINED_
#  ifndef __SIZE_TYPE__
#    define __SIZE_TYPE__ unsigned long
#  endif
typedef __SIZE_TYPE__ size_t;
#define _KP_SIZE_T_DEFINED_
#endif

/* Accept both old and new obf macro names */
#if !defined(KP_SUPERCMD_LEN) && defined(KP_SC_LEN)
#  define KP_SUPERCMD_LEN KP_SC_LEN
#endif
#if !defined(KP_SUPERCMD_OBF) && defined(KP_SC_OBF)
#  define KP_SUPERCMD_OBF KP_SC_OBF
#endif
#if !defined(KP_SUPERPATH_LEN) && defined(KP_SU_LEN)
#  define KP_SUPERPATH_LEN KP_SU_LEN
#endif
#if !defined(KP_SUPERPATH_OBF) && defined(KP_SU_OBF)
#  define KP_SUPERPATH_OBF KP_SU_OBF
#endif

/* XOR decode (must match your generator) */
static inline void _kp_decode_into(char *dst, const unsigned char *src, size_t n) {
    for (size_t i = 0; i < n; ++i) dst[i] = (char)(src[i] ^ 0xA5);
    dst[n] = '\0';
}

/* Runtime getters */
static inline const char *kp_get_supercmd(void) {
#if defined(KP_SUPERCMD_LEN) && defined(KP_SUPERCMD_OBF)
    static char buf[KP_SUPERCMD_LEN + 1];
    static int inited;
    if (!inited) { _kp_decode_into(buf, KP_SUPERCMD_OBF, KP_SUPERCMD_LEN); inited = 1; }
    return buf;
#else
    static const char *fallback = "/system/bin/truncate"; /* only if obf missing */
    return fallback;
#endif
}

static inline const char *kp_get_su_path(void) {
#if defined(KP_SUPERPATH_LEN) && defined(KP_SUPERPATH_OBF)
    static char buf[KP_SUPERPATH_LEN + 1];
    static int inited;
    if (!inited) { _kp_decode_into(buf, KP_SUPERPATH_OBF, KP_SUPERPATH_LEN); inited = 1; }
    return buf;
#else
    static const char *fallback = "/system/bin/kp"; /* adjust if needed */
    return fallback;
#endif
}
