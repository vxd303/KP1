#pragma once
#include <linux/slab.h>
#include <linux/types.h>
#include <linux/errno.h>

#include "patch/android/gen/user_init_obf.h"

/* CÙNG công thức với script Python gen_user_init_obf.py */
static inline u8 _kp_key(size_t i)
{
    return (u8)((0xA7 ^ ((i * 73 + 17) & 0xFF)) & 0xFF);
}

static inline void deobf_user_init(char *out)
{
    size_t i;
    for (i = 0; i < user_init_obf_len; ++i)
        out[i] = (char)(user_init_obf[i] ^ _kp_key(i));
    out[user_init_obf_len] = '\0';
}

/* kernel_write_file_fn: int (*)(const char*, const char*, size_t, int mode) */
static inline int write_user_init_script(const char *path,
    int (*kernel_write_file_fn)(const char *, const char *, size_t, int),
    int mode)
{
    int ret;
    char *buf;

    if (!kernel_write_file_fn) return -EINVAL;

    buf = kmalloc(user_init_obf_len + 1, GFP_KERNEL);
    if (!buf) return -ENOMEM;

    deobf_user_init(buf);
    ret = kernel_write_file_fn(path, buf, user_init_obf_len, mode);
    kfree(buf);
    return ret;
}
