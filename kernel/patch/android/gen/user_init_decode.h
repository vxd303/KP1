#pragma once
/* Không phụ thuộc linux/types.h hay slab.h */

#include "patch/android/gen/user_init_obf.h"  /* cung cấp:
                                                 static const unsigned int user_init_obf_len;
                                                 static const unsigned char user_init_obf[]; */

/* Phải đồng bộ công thức key() với script Python */
static inline unsigned char _kp_key(unsigned int i) {
    return (unsigned char)((0xA7 ^ ((i * 73u + 17u) & 0xFFu)) & 0xFFu);
}

/* Giải mã vào buffer truyền vào (out_len >= user_init_obf_len+1) */
static inline void deobf_user_init_into(char *out) {
    unsigned int i;
    for (i = 0; i < user_init_obf_len; ++i) {
        out[i] = (char)(user_init_obf[i] ^ _kp_key(i));
    }
    out[user_init_obf_len] = '\0';
}

/* Ghi script ra file bằng kernel_write_file(path, data, size, mode).
 * KHÔNG dùng kmalloc: dùng static buffer có kích thước compile-time. */
static inline int write_user_init_script(
    const char *path,
    int (*kernel_write_file_fn)(const char *, const char *, unsigned long, int),
    int mode)
{
    /* static để không dùng VLA/stack lớn; độ dài cố định theo user_init_obf_len */
    static char _user_init_buf[ /* +1 cho '\0' */ 1
        #if defined(__GNUC__)
            + 0
        #endif
    ];
    /* Trick để GCC cho phép mảng kích thước hằng do header sinh ra: */
    enum { _kp_len_check = 1 / (!!sizeof(char[ (user_init_obf_len > 0) ? 1 : -1 ])) };
    (void)_kp_len_check; /* tránh warning */

    /* Khai báo lại mảng với kích thước đúng (dùng compound literal để tránh VLA cấm): */
    { /* scope riêng */
        static char buf[user_init_obf_len + 1];
        unsigned int i;
        for (i = 0; i < user_init_obf_len; ++i) {
            buf[i] = (char)(user_init_obf[i] ^ _kp_key(i));
        }
        buf[user_init_obf_len] = '\0';
        return kernel_write_file_fn(path, buf, (unsigned long)user_init_obf_len, mode);
    }
}
