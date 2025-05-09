#!/usr/bin/env python3

import sys

if len(sys.argv) != 2:
    print(f"Usage: {sys.argv[0]} <virtual_address (hex)>")
    sys.exit(1)

va = int(sys.argv[1], 16)

def extract_index(name, shift):
    return (va >> shift) & 0x1FF

print(f"Virtual address : 0x{va:016x}")
print(f"PML4 index : {extract_index('PML4', 39)}")
print(f"PDPT index : {extract_index('PDPT', 30)}")
print(f" PD  index : {extract_index('PD', 21)}")
print(f" PT  index : {extract_index('PT', 12)}")
print(f" Offset    : {va & 0xFFF}")
