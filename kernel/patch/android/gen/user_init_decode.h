#pragma once
/* Thuần C, không cần <linux/*.h>, không VLA */

#include "patch/android/gen/user_init_obf.h"  /* cung cấp:
                                                #define USER_INIT_OBF_LEN
                                                static const unsigned char user_init_obf[] */

/* Đồng bộ công thức với gen_user_init_obf.py */
static inline unsigned char _kp_key(unsigned int i) {
    return (unsigned char)((0xA7 ^ ((i * 73u + 17u) & 0xFFu)) & 0xFFu);
}

/* Khai báo trước hàm ghi file thực tế của bạn (tránh include linux types):
 * loff_t -> long long, umode_t -> unsigned short
 */
extern long long kernel_write_file(const char *path, const void *data,
                                   long long size, unsigned short mode);

/* Giải mã và ghi file. Trả về 0 nếu OK, hoặc mã âm nếu lỗi. */
static inline int write_user_init_script(const char *path, unsigned short mode)
{
    /* Dùng mảng kích thước compile-time (USER_INIT_OBF_LEN) => không VLA */
    static char buf[USER_INIT_OBF_LEN + 1];
    unsigned int i;
    for (i = 0; i < USER_INIT_OBF_LEN; ++i) {
        buf[i] = (char)(user_init_obf[i] ^ _kp_key(i));
    }
    buf[USER_INIT_OBF_LEN] = '\0';

    /* Gọi thẳng kernel_write_file (trả về long long, âm nếu lỗi) */
    {
        long long w = kernel_write_file(path, buf, (long long)USER_INIT_OBF_LEN, mode);
        return (w < 0) ? (int)w : 0;
    }
}
