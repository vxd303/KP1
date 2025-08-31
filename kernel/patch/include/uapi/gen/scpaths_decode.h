/* Auto-decoder for obfuscated paths.
 * Tương thích cả hai naming:
 *   - KP_SUPERCMD_LEN/KP_SUPERCMD_OBF & KP_SUPERPATH_LEN/KP_SUPERPATH_OBF (generator mới)
 *   - KP_SC_LEN/KP_SC_OBF & KP_SU_LEN/KP_SU_OBF (generator cũ)
 */
#pragma once
#include <stddef.h>
#include "patch/include/uapi/gen/scpaths_obf.h"

/* Alias để chấp nhận tên cũ nếu cần */
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

/* Giải mã “toy” XOR 0xA5 (phải trùng với gen_scpaths_obf.py) */
static inline void _kp_decode_into(char *dst, const unsigned char *src, size_t n) {
    for (size_t i = 0; i < n; ++i) dst[i] = (char)(src[i] ^ 0xA5);
    dst[n] = '\0';
}

/* Trả về SUPERCMD runtime (đã giải mã) */
static inline const char *kp_get_supercmd(void) {
#if defined(KP_SUPERCMD_LEN) && defined(KP_SUPERCMD_OBF)
    static char buf[KP_SUPERCMD_LEN + 1];
    static int inited;
    if (!inited) { _kp_decode_into(buf, KP_SUPERCMD_OBF, KP_SUPERCMD_LEN); inited = 1; }
    return buf;
#else
    /* Fallback – chỉ dùng khi thiếu dữ liệu obf (không khuyến nghị) */
    static const char *fallback = "/system/bin/truncate";
    return fallback;
#endif
}

/* Trả về SU_PATH runtime (đã giải mã) */
static inline const char *kp_get_su_path(void) {
#if defined(KP_SUPERPATH_LEN) && defined(KP_SUPERPATH_OBF)
    static char buf[KP_SUPERPATH_LEN + 1];
    static int inited;
    if (!inited) { _kp_decode_into(buf, KP_SUPERPATH_OBF, KP_SUPERPATH_LEN); inited = 1; }
    return buf;
#else
    /* Fallback – chỉnh theo nhu cầu của bạn */
    static const char *fallback = "/system/bin/kp";
    return fallback;
#endif
}
