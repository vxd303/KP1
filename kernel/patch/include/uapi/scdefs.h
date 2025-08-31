/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * KernelPatch UAPI: scdefs.h (đã rút gọn & ổn định)
 * - Ẩn SUPERCMD / SU_PATH bằng getter runtime (không literal trong binary)
 * - Đặt include-guard & hằng số sớm để mọi TU đều thấy
 */

#ifndef _KP_UAPI_SCDEF_H_
#define _KP_UAPI_SCDEF_H_

/* Dùng sớm ở nhiều TU */
#define SU_PATH_MAX_LEN 128

/* Getter runtime do generator tạo (kp_get_supercmd, kp_get_su_path) */
#include "patch/include/uapi/gen/scpaths_decode.h"

/* Hash tiện dụng (giữ nguyên theo bản của bạn) */
static inline long hash_key(const char *key)
{
    long hash = 1000000007;
    for (int i = 0; key[i]; i++) {
        hash = hash * 31 + key[i];
    }
    return hash;
}

/* Supercall IDs (giữ nguyên như bản gốc của bạn) */
#define SUPERCALL_HELLO_ECHO "hoa"

/* #define __NR_supercall __NR3264_truncate // 45 */
#define __NR_supercall 45

#define SUPERCALL_HELLO               0x1000
#define SUPERCALL_KLOG                0x1004

#define SUPERCALL_BUILD_TIME          0x1007
#define SUPERCALL_KERNELPATCH_VER     0x1008
#define SUPERCALL_KERNEL_VER          0x1009

#define SUPERCALL_SKEY_GET            0x100a
#define SUPERCALL_SKEY_SET            0x100b
#define SUPERCALL_SKEY_ROOT_ENABLE    0x100c

#define SUPERCALL_SU                  0x1010
#define SUPERCALL_SU_TASK             0x1011 /* syscall(__NR_gettid) */

#define SUPERCALL_KPM_LOAD            0x1020
#define SUPERCALL_KPM_UNLOAD          0x1021
#define SUPERCALL_KPM_CONTROL         0x1022

#define SUPERCALL_KPM_NUMS            0x1030
#define SUPERCALL_KPM_LIST            0x1031
#define SUPERCALL_KPM_INFO            0x1032

struct kernel_storage {
    void *data;
    int len;
};

#define SUPERCALL_KSTORAGE_ALLOC_GROUP  0x1040
#define SUPERCALL_KSTORAGE_WRITE        0x1041
#define SUPERCALL_KSTORAGE_READ         0x1042
#define SUPERCALL_KSTORAGE_LIST_IDS     0x1043
#define SUPERCALL_KSTORAGE_REMOVE       0x1044
#define SUPERCALL_KSTORAGE_REMOVE_GROUP 0x1045

#define KSTORAGE_SU_LIST_GROUP        0
#define KSTORAGE_EXCLUDE_LIST_GROUP   1
#define KSTORAGE_UNUSED_GROUP_2       2
#define KSTORAGE_UNUSED_GROUP_3       3

#define SUPERCALL_BOOTLOG             0x10fd
#define SUPERCALL_PANIC               0x10fe
#define SUPERCALL_TEST                0x10ff

#define SUPERCALL_KEY_MAX_LEN         0x40
#define SUPERCALL_SCONTEXT_LEN        0x60

struct su_profile {
    uid_t uid;
    uid_t to_uid;
    char scontext[SUPERCALL_SCONTEXT_LEN];
};

/* Đường dẫn & scontext mặc định theo nền tảng */
#ifdef ANDROID
  #define SH_PATH                        "/system/bin/sh"
  #define LEGACY_SU_PATH                 "/system/bin/xu"
  #define ECHO_PATH                      "/system/bin/echo"
  #define KERNELPATCH_DATA_DIR           "/data/adb/kp"
  #define KERNELPATCH_MODULE_DATA_DIR    KERNELPATCH_DATA_DIR "/modules"
  #define APD_PATH                       "/data/adb/apd"
  #define ALL_ALLOW_SCONTEXT             "u:r:kp:s0"
  #define ALL_ALLOW_SCONTEXT_MAGISK      "u:r:magisk:s0"
  #define ALL_ALLOW_SCONTEXT_KERNEL      "u:r:kernel:s0"
#else
  #define SH_PATH                        "/usr/bin/sh"
  #define ECHO_PATH                      "/usr/bin/echo"
  #define ALL_ALLOW_SCONTEXT             "u:r:kernel:s0"
#endif

/* ---- Ẩn literal: dùng getter runtime ở mọi nền tảng ---- */
static inline const char *KP_SUPERCMD(void) { return kp_get_supercmd(); }
static inline const char *KP_SU_PATH(void)  { return kp_get_su_path();  }

#define SUPERCMD (KP_SUPERCMD())
#define SU_PATH  (KP_SU_PATH())

/* Flag file */
#define SAFE_MODE_FLAG_FILE "/dev/.safe"

/* SU control calls */
#define SUPERCALL_SU_GRANT_UID         0x1100
#define SUPERCALL_SU_REVOKE_UID        0x1101
#define SUPERCALL_SU_NUMS              0x1102
#define SUPERCALL_SU_LIST              0x1103
#define SUPERCALL_SU_PROFILE           0x1104
#define SUPERCALL_SU_GET_ALLOW_SCTX    0x1105
#define SUPERCALL_SU_SET_ALLOW_SCTX    0x1106
#define SUPERCALL_SU_GET_PATH          0x1110
#define SUPERCALL_SU_RESET_PATH        0x1111
#define SUPERCALL_SU_GET_SAFEMODE      0x1112

#define SUPERCALL_MAX                  0x1200
#define SUPERCALL_RES_SUCCEED          0

#define SUPERCALL_HELLO_MAGIC          0x11581158

#endif /* _KP_UAPI_SCDEF_H_ */
