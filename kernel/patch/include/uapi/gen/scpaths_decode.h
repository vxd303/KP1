#pragma once
#include "patch/include/uapi/gen/scpaths_obf.h"

/* Đồng bộ với gen_scpaths_obf.py */
static inline unsigned char _kp_key_paths(unsigned int i) {
    return (unsigned char)((0x5D ^ ((i*131u + 29u) & 0xFFu)) & 0xFFu);
}
static inline void _kp_decode_into(char *dst, const unsigned char *src, unsigned int n) {
    for (unsigned int i = 0; i < n; ++i) dst[i] = (char)(src[i] ^ _kp_key_paths(i));
    dst[n] = '\0';
}

/* Getters trả về buffer tĩnh (decode 1 lần) */
static inline const char* kp_get_su_path(void) {
    static char buf[KP_SU_LEN + 1]; static int inited;
    if (!inited) { _kp_decode_into(buf, KP_SU_OBF, KP_SU_LEN); inited = 1; }
    return buf;
}
static inline const char* kp_get_supercmd(void) {
    static char buf[KP_SUPERCMD_LEN + 1]; static int inited;
    if (!inited) { _kp_decode_into(buf, KP_SUPERCMD_OBF, KP_SUPERCMD_LEN); inited = 1; }
    return buf;
}

/* (tuỳ chọn) expose chiều dài hằng nếu cần so sánh-length ở compile-time */
#define KP_HAS_LEN_MACROS 1
#define KP_SU_LEN_CONST        KP_SU_LEN
#define KP_SUPERCMD_LEN_CONST  KP_SUPERCMD_LEN
