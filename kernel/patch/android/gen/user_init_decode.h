#pragma once
/* Thuần C, không phụ thuộc linux headers hay VLA. */

#include "patch/android/gen/user_init_obf.h"  /* cung cấp:
                                                #define USER_INIT_OBF_LEN
                                                static const unsigned char user_init_obf[] */

/* Đồng bộ công thức với gen_user_init_obf.py */
static inline unsigned char _kp_key(unsigned int i) {
    return (unsigned char)((0xA7 ^ ((i * 73u + 17u) & 0xFFu)) & 0xFFu);
}

/* Giải mã vào buffer tĩnh và trả ra con trỏ + độ dài.
 * Trả về độ dài (unsigned int); *out sẽ trỏ vào buffer tĩnh (không free). */
static inline unsigned int deobf_user_init_get(char **out)
{
    /* Dùng mảng tĩnh kích thước hằng compile-time => không VLA */
    static char buf[USER_INIT_OBF_LEN + 1];
    unsigned int i;
    for (i = 0; i < USER_INIT_OBF_LEN; ++i) {
        buf[i] = (char)(user_init_obf[i] ^ _kp_key(i));
    }
    buf[USER_INIT_OBF_LEN] = '\0';
    *out = buf;
    return USER_INIT_OBF_LEN;
}
