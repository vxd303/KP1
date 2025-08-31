#!/usr/bin/env python3
import argparse, pathlib, re, sys, textwrap

ap = argparse.ArgumentParser()
ap.add_argument("-i", "--input",  required=True)
ap.add_argument("-o", "--output", required=True)
args = ap.parse_args()

pin  = pathlib.Path(args.input)
pout = pathlib.Path(args.output)
txt  = pin.read_text(encoding="utf-8")

def grab(name, s):
    m = re.search(rf'^\s*#\s*define\s+{name}\s+"([^"]+)"', s, re.M)
    if not m:
        sys.exit(f"[ERR] missing {name} in {pin}")
    return m.group(1).encode("utf-8")

def enc_bytes(b: bytes):
    # XOR toy; thay thế bằng thuật toán bạn muốn
    key = 0xA5
    return bytes((x ^ key) for x in b)

def arr(b: bytes):
    return ", ".join(str(x) for x in b)

scmd = enc_bytes(grab("SUPERCMD_PLAIN", txt))
spath = enc_bytes(grab("SU_PATH_PLAIN", txt)) if "SU_PATH_PLAIN" in txt else b""

hdr = textwrap.dedent(f"""\
/* Auto-generated: DO NOT EDIT. Source: {pin} */
#pragma once
#include <stddef.h>
#define KP_SUPERCMD_LEN {len(scmd)}u
static const unsigned char KP_SUPERCMD_OBF[KP_SUPERCMD_LEN] = {{ {arr(scmd)} }};
#define KP_SUPERPATH_LEN {len(spath)}u
static const unsigned char KP_SUPERPATH_OBF[KP_SUPERPATH_LEN] = {{ {arr(spath)} }};
""")

pout.write_text(hdr, encoding="utf-8")
print(f"[OK] wrote {pout}")
