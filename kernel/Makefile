ifndef TARGET_COMPILE
    $(error TARGET_COMPILE not set)
endif

TARGET=kpimg

CC = $(TARGET_COMPILE)gcc
LD = $(TARGET_COMPILE)ld
AS = $(TARGET_COMPILE)as
OBJCOPY = $(TARGET_COMPILE)objcopy

CFLAGS += -Wall -fno-builtin -std=gnu11 -nostdinc
CFLAGS += -g
CFLAGS += -include patch/include/uapi/supath_sanitize.h

ifdef DEBUG
	CFLAGS += -DDEBUG -DMAP_DEBUG -g
endif

ifdef ANDROID
	CFLAGS += -DANDROID
endif

INCLUDE := -I. -Iinclude -Ipatch/include -Ilinux -Ilinux/include -Ilinux/arch/arm64/include -Ilinux/tools/arch/arm64/include
INCLUDE += -Ipatch/android -Ipatch/android/gen
INCLUDE += -Ipatch/include/uapi/gen

BASE_SRCS += base/setup.c 
BASE_SRCS += base/setup1.S
BASE_SRCS += base/cache.S
BASE_SRCS += base/tlsf.c
BASE_SRCS += base/start.c 
BASE_SRCS += base/map.c 
BASE_SRCS += base/map1.S 
BASE_SRCS += base/hook.c 
BASE_SRCS += base/fphook.c 
BASE_SRCS += base/hmem.c 
BASE_SRCS += base/predata.c 
BASE_SRCS += base/symbol.c 
BASE_SRCS += base/baselib.c 
BASE_SRCS += base/sha256.c 

BASE_SRCS += $(wildcard patch/*.c)
BASE_SRCS += $(wildcard patch/common/*.c)
BASE_SRCS += $(wildcard patch/module/*.c)
BASE_SRCS += $(wildcard patch/ksyms/*.c)

ifdef ANDROID
	BASE_SRCS += $(wildcard patch/android/*.c)
	BASE_SRCS += $(wildcard patch/android/gen/*.c)
endif

SRCS += $(BASE_SRCS)
SRCS += $(LINUX_SRCS)

OBJS := $(SRCS:.c=.o)
OBJS := $(OBJS:.S=.o)

# --- Obfuscation: paths ---
GEN_TOOL  := patch/android/tools/gen_user_init_obf.py
GEN_PLAIN := patch/android/gen/user_init_plain.sh
GEN_HEADR := patch/android/gen/user_init_obf.h

# --- Obfuscation (RC): paths ---
GEN_TOOL_RC  := patch/android/tools/gen_user_rc_obf.py
GEN_PLAIN_RC := patch/android/gen/user_rc_plain.txt
GEN_HEADR_RC := patch/android/gen/user_rc_obf.h

# --- Obfuscation (paths) ---
GEN_TOOL_PATHS  := patch/tools/gen_scpaths_obf.py
GEN_PLAIN_PATHS := patch/tools/data/scpaths_plain.txt
GEN_HEADR_PATHS := patch/include/uapi/gen/scpaths_obf.h

# Tạo header trước khi build target chính
all: hdr $(GEN_HEADR) $(GEN_HEADR_RC) $(GEN_HEADR_PATHS) ${TARGET}

${TARGET}: ${TARGET}.elf
	${OBJCOPY} -O binary -S $^ $@

${TARGET}.elf: ${OBJS}
	${LD} -nostdlib -static -no-pie -Tkpimg.lds -o $@ $^

# Bảo đảm header đã sinh trước khi compile .c/.S
%.o: %.c | $(GEN_HEADR) $(GEN_HEADR_RC) $(GEN_HEADR_PATHS)
	${CC} $(CFLAGS) $(INCLUDE) -c -O2 -o $@ $<

%.o: %.S | $(GEN_HEADR) $(GEN_HEADR_RC)
	${CC} $(CFLAGS) $(INCLUDE) -c -o $@ $<

.PHONY: hdr
hdr:
	cp -Rf patch/include/uapi ../user/
	cp -f ../version ../user/
	cp -f include/preset.h ../tools/

# --- generate obfuscated header ---
$(GEN_HEADR_RC): $(GEN_PLAIN_RC) $(GEN_TOOL_RC)
	@echo "  GEN     $@"
	@python3 $(GEN_TOOL_RC) -i $(GEN_PLAIN_RC) -o $(GEN_HEADR_RC)

# --- generate obfuscated header ---
$(GEN_HEADR): $(GEN_PLAIN) $(GEN_TOOL)
	@echo "  GEN     $@"
	@python3 $(GEN_TOOL) -i $(GEN_PLAIN) -o $(GEN_HEADR)

$(GEN_HEADR_PATHS): $(GEN_PLAIN_PATHS) $(GEN_TOOL_PATHS)
	@echo "  GEN     $@"
	@python3 $(GEN_TOOL_PATHS) -i $(GEN_PLAIN_PATHS) -o $(GEN_HEADR_PATHS)

.PHONY: clean
clean:
	rm -rf *.elf
	rm -rf kpimg
	find . -name *.o | xargs rm -f
	rm -f $(GEN_HEADR)
	rm -f $(GEN_HEADR_RC)
	rm -f $(GEN_HEADR_PATHS)
.PHONY: pp-sucompat
pp-sucompat:
	@$(CC) $(CFLAGS) $(INCLUDE) -DANDROID \
		-include patch/include/uapi/supath_sanitize.h \
		-E patch/common/sucompat.c | grep -n '"/system/bin/tail"' || true
.PHONY: why-tail asm-sucompat

why-tail:
	@echo "[pp] preprocess sucompat.c and grep literal"
	@$(CC) $(CFLAGS) $(INCLUDE) -DANDROID \
		-include patch/include/uapi/supath_sanitize.h \
		-E patch/common/sucompat.c | grep -n '"/system/bin/tail"' || true
	@echo "[inc] include tree (tìm ai include plaintext)"
	@$(CC) $(CFLAGS) $(INCLUDE) -DANDROID \
		-include patch/include/uapi/supath_sanitize.h \
		-H -c patch/common/sucompat.c -o /dev/null 2>&1 | sed -n '/scpaths_/p' || true
	@echo "[ro] readelf .rodata (xem offset string)"
	@readelf -p .rodata patch/common/sucompat.o | sed -n '/tail/p' || true

asm-sucompat:
	@echo "[S] sinh assembly có chú thích dòng"
	@$(CC) $(CFLAGS) $(INCLUDE) -DANDROID \
		-include patch/include/uapi/supath_sanitize.h \
		-S -fverbose-asm -g0 patch/common/sucompat.c -o /tmp/sucompat.s
	@grep -n '/system/bin/tail' /tmp/sucompat.s || true
