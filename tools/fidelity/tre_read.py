#!/usr/bin/env python3
# Minimal SWG .TRE (TreeFile v5 "EERT5000") reader: list names, extract by substring.
import struct, zlib, sys, os, re

def parse_tre(path):
    f = open(path, "rb")
    hdr = f.read(8)
    token, version = hdr[:4], hdr[4:8]
    if token != b"EERT":
        raise SystemExit(f"not a TRE (token={token!r}) {path}")
    # v5 header after the 8-byte magic:
    # numRecords, tocOffset, tocCompressor, tocCompressedSize,
    # nameCompressor, nameCompressedSize, nameUncompressedSize
    (numRecords, tocOffset, tocComp, tocCompSize,
     nameComp, nameCompSize, nameUncompSize) = struct.unpack("<7I", f.read(28))

    def maybe_decomp(comp, raw, uncomp_size=None):
        if comp == 2:
            return zlib.decompress(raw)
        return raw

    # records block
    f.seek(tocOffset)
    rec_raw = f.read(tocCompSize)
    rec_block = maybe_decomp(tocComp, rec_raw)
    if len(rec_block) != numRecords * 24:
        # try: maybe tocCompSize was uncompressed already
        if len(rec_raw) >= numRecords*24 and tocComp != 2:
            rec_block = rec_raw[:numRecords*24]
        else:
            raise SystemExit(f"TOC size mismatch: got {len(rec_block)} want {numRecords*24} (numRec={numRecords} tocComp={tocComp})")

    # name block follows records block
    f.seek(tocOffset + tocCompSize)
    name_raw = f.read(nameCompSize)
    name_block = maybe_decomp(nameComp, name_raw)

    records = []
    for i in range(numRecords):
        off = i*24
        crc, uncompSize, dataOffset, dataComp, dataCompSize, nameOff = struct.unpack_from("<6I", rec_block, off)
        # name: null-terminated string at nameOff in name_block
        end = name_block.find(b"\x00", nameOff)
        name = name_block[nameOff:end].decode("latin-1")
        records.append((name, dataOffset, dataComp, dataCompSize, uncompSize))
    return f, records

def main():
    path = sys.argv[1]
    mode = sys.argv[2] if len(sys.argv) > 2 else "list"
    pattern = sys.argv[3] if len(sys.argv) > 3 else None
    f, records = parse_tre(path)
    if mode == "list":
        for name, *_ in records:
            if pattern is None or re.search(pattern, name, re.I):
                print(name)
        sys.stderr.write(f"[{len(records)} records in {os.path.basename(path)}]\n")
    elif mode == "extract":
        destroot = sys.argv[4]
        n = 0
        for name, dataOffset, dataComp, dataCompSize, uncompSize in records:
            if pattern and not re.search(pattern, name, re.I):
                continue
            f.seek(dataOffset)
            raw = f.read(dataCompSize)
            data = zlib.decompress(raw) if dataComp == 2 else raw
            outp = os.path.join(destroot, name)
            os.makedirs(os.path.dirname(outp), exist_ok=True)
            open(outp, "wb").write(data)
            n += 1
            print(f"extracted {name} ({len(data)} bytes)")
        sys.stderr.write(f"[extracted {n} files]\n")

if __name__ == "__main__":
    main()
