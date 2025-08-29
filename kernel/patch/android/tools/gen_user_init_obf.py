#!/usr/bin/env python3
import argparse, pathlib, sys

def key(i: int) -> int:
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
/* Auto-generated: DO NOT EDIT. Source: {p_in} */
#define USER_INIT_OBF_LEN {len(enc)}u
static const unsigned char user_init_obf[USER_INIT_OBF_LEN] = {{ {arr} }};
"""
    p_out.parent.mkdir(parents=True, exist_ok=True)
    p_out.write_text(hdr)
    print(f"[OK] Wrote {p_out} ({len(enc)} bytes)")

if __name__ == "__main__":
    sys.exit(main())
