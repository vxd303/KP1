#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Generate patch/android/gen/user_init_obf.h from patch/android/gen/user_init_plain.sh
XOR per-index to avoid plaintext in kernel image.
"""
import argparse, pathlib, sys

def key(i: int) -> int:
    # KHÓA ĐƠN GIẢN: đổi công thức/seed tùy ý bạn
    return (0xA7 ^ ((i * 73 + 17) & 0xFF)) & 0xFF

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("-i", "--input",  default="patch/android/gen/user_init_plain.sh")
    ap.add_argument("-o", "--output", default="patch/android/gen/user_init_obf.h")
    ns = ap.parse_args()

    p_in  = pathlib.Path(ns.input)
    p_out = pathlib.Path(ns.output)

    raw = p_in.read_bytes()
    enc = bytes((b ^ key(i)) for i, b in enumerate(raw))

    arr = ", ".join(str(b) for b in enc)
    hdr = f"""#pragma once
#include <linux/types.h>
/* Auto-generated: DO NOT EDIT.
 * Source: {p_in}
 */
static const size_t user_init_obf_len = {len(enc)};
static const u8 user_init_obf[{len(enc)}] = {{ {arr} }};
"""
    p_out.parent.mkdir(parents=True, exist_ok=True)
    p_out.write_text(hdr)
    print(f"[OK] Wrote {p_out} ({len(enc)} bytes)")

if __name__ == "__main__":
    sys.exit(main())
