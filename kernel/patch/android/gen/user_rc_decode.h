#pragma once
/* Thuần C, không phụ thuộc linux headers hay VLA. */

#include "patch/android/gen/user_rc_obf.h"  /* #define USER_RC_OBF_LEN, user_rc_obf[] */

/* Đồng bộ công thức với gen_user_rc_obf.py */
static inline unsigned char _kp_key_rc(unsigned int i) {
    return (unsigned char)((0xA7 ^ ((i * 73u + 17u) & 0xFFu)) & 0xFFu);
}

/* Giải mã template RC vào buffer tĩnh; trả về độ dài, *out trỏ buffer (không free). */
static inline unsigned int deobf_user_rc_get(char **out)
{
    static char buf[USER_RC_OBF_LEN + 1];
    unsigned int i;
    for (i = 0; i < USER_RC_OBF_LEN; ++i) {
        buf[i] = (char)(user_rc_obf[i] ^ _kp_key_rc(i));
    }
    buf[USER_RC_OBF_LEN] = '\0';
    *out = buf;
    return USER_RC_OBF_LEN;
}
