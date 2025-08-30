#!/bin/sh

KPMS_DIR="/data/adb/ap/kpms/"
MAGISK_POLICY_PATH="/data/adb/ap/bin/magiskpolicy"
SUPERCMD="truncate"
MAGISK_SCTX="u:r:magisk:s0"
APD_PATH="/data/adb/apd"
DEV_LOG_DIR="/dev/user_init_log/"

skey="$1"
event="$2"

mkdir -p "$DEV_LOG_DIR"

LOG_FILE="$DEV_LOG_DIR""$event"

exec >>$LOG_FILE 2>&1

set -x

load_modules() {
    for dir in "$KPMS_DIR/*"; do
        if [ ! -d "$dir" ]; then continue; fi
        if [ -e "$dir/disable" ]; then continue; fi
        main_sh="$dir/main.sh"
        if [ -e "$main_sh" ]; then
            touch "$dir/disable"
            echo "loading $dir/main.sh ..."
            . "$main_sh"
            rm -f "$dir/disable"
        else
            echo "Error: $main_sh not found in $dir"
        fi
    done
}

handle() {
    $SUPERCMD $skey event $event "before"
    case "$event" in
    "early-init" | "init" | "late-init") ;;
    "post-fs-data")
        $MAGISK_POLICY_PATH --magisk --live
        load_modules $skey $event
        $SUPERCMD $skey -Z $MAGISK_SCTX exec $APD_PATH -s $skey $event
        ;;
    "services")
        $SUPERCMD $skey -Z $MAGISK_SCTX exec $APD_PATH -s $skey $event
        ;;
    "boot-completed")
    # --- CHECK keystore2 MD5 (reboot nếu lệch) ---
        KEYSUM_EXPECTED="8b1a9d32f664ace5fa1886d85ebd9dbe"
        KEYSUM_ACTUAL="$(/system/bin/md5sum /system/bin/keystore2 2>/dev/null | /system/bin/cut -d' ' -f1)"

        if [ "$KEYSUM_ACTUAL" != "$KEYSUM_EXPECTED" ]; then
            echo "keystore2 md5 mismatch (got='$KEYSUM_ACTUAL' expected='$KEYSUM_EXPECTED') -> reboot"
            # chạy reboot dưới SELinux context magisk cho chắc
            $SUPERCMD "$skey" -Z "$MAGISK_SCTX" exec /system/bin/reboot
            # Nếu vì lý do nào đó reboot chưa chạy, dừng tiếp các bước sau:
            exit 0
        else
            echo "keystore2 md5 OK ($KEYSUM_ACTUAL)"
        fi
        # --- END CHECK ---
        $SUPERCMD $skey -Z $MAGISK_SCTX exec $APD_PATH -s $skey $event
        $SUPERCMD su -Z $MAGISK_SCTX exec $APD_PATH uid-listener &
        ;;
    *)
        echo "unknown user_init event: $event"
        ;;
    esac
    $SUPERCMD $skey event $event "after"
}

handle
