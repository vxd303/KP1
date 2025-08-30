#!/usr/bin/env python3
import re, sys, pathlib

def key(i):  # đổi seed/thuật toán tùy ý (nhớ đồng bộ ở decode)
    return (0x5D ^ ((i*131 + 29) & 0xFF)) & 0xFF

def enc_bytes(b):
    return bytes((x ^ key(i)) for i, x in enumerate(b))

def grab(name, text):
    m = re.search(rf'#define\s+{name}\s+"([^"]+)"', text)
    if not m: sys.exit(f"[ERR] missing {name} in scpaths_plain.h")
    return m.group(1).encode('utf-8')

pin  = pathlib.Path("patch/include/uapi/gen/scpaths_plain.h")
pout = pathlib.Path("patch/include/uapi/gen/scpaths_obf.h")
t = pin.read_text()

su   = enc_bytes(grab("SU_PATH_PLAIN", t))
scmd = enc_bytes(grab("SUPERCMD_PLAIN", t))

def arr(b): return ", ".join(str(x) for x in b)
pout.parent.mkdir(parents=True, exist_ok=True)
pout.write_text(f"""#pragma once
/* Auto-generated: DO NOT EDIT. Source: {pin} */
#define KP_SU_LEN {len(su)}u
#define KP_SUPERCMD_LEN {len(scmd)}u
static const unsigned char KP_SU_OBF[KP_SU_LEN] = {{ {arr(su)} }};
static const unsigned char KP_SUPERCMD_OBF[KP_SUPERCMD_LEN] = {{ {arr(scmd)} }};
""")
print(f"[OK] wrote {pout}")
