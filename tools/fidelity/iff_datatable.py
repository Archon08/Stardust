#!/usr/bin/env python3
# SWG IFF DATATABLE (DTII) reader -> rows as dicts. IFF chunk headers big-endian; cell values little-endian.
import struct, sys, json, os

class Reader:
    def __init__(self, b): self.b=b; self.p=0
    def tag(self): t=self.b[self.p:self.p+4]; self.p+=4; return t
    def u32be(self): v=struct.unpack_from(">I",self.b,self.p)[0]; self.p+=4; return v
    def chunk(self):
        tag=self.tag(); size=self.u32be(); data=self.b[self.p:self.p+size]; self.p+=size
        # SWG IFF: no even-padding
        return tag,data

def walk(b):
    """Return dict of leaf-chunk-tag -> data, descending through FORM chunks; collect COLS/TYPE/ROWS."""
    found={}
    def rec(buf):
        r=Reader(buf)
        while r.p < len(buf):
            try: tag,data=r.chunk()
            except struct.error: break
            if tag==b"FORM":
                rec(data[4:])  # skip the 4-byte form-type, recurse into sub-chunks
            else:
                found[tag]=data
    rec(b)
    return found

def cstrings(buf, n=None):
    out=[]; i=0
    while i < len(buf):
        j=buf.find(b"\x00", i)
        if j<0: break
        s=buf[i:j].decode("latin-1"); out.append(s); i=j+1
        if n and len(out)>=n: break
    return out

def parse_datatable(path):
    b=open(path,"rb").read()
    ch=walk(b)
    if b"COLS" not in ch or b"ROWS" not in ch:
        raise ValueError(f"no DATATABLE chunks in {path} (have {[t.decode('latin-1') for t in ch]})")
    cols_buf=ch[b"COLS"]; ncols=struct.unpack_from("<I",cols_buf,0)[0]
    colnames=cstrings(cols_buf[4:], ncols)
    types_buf=ch[b"TYPE"]; coltypes=cstrings(types_buf, ncols)
    rows_buf=ch[b"ROWS"]; nrows=struct.unpack_from("<I",rows_buf,0)[0]
    r=Reader(rows_buf); r.p=4
    def read_cell(tp):
        c=tp[0].lower() if tp else 'i'
        if c in ('s',):  # string
            j=rows_buf.find(b"\x00", r.p); s=rows_buf[r.p:j].decode("latin-1"); r.p=j+1; return s
        elif c=='f':
            v=struct.unpack_from("<f",rows_buf,r.p)[0]; r.p+=4; return round(v,6)
        else:  # i,b,e,h,z -> 4-byte int
            v=struct.unpack_from("<i",rows_buf,r.p)[0]; r.p+=4; return v
    rows=[]
    for _ in range(nrows):
        row={}
        for ci in range(ncols):
            row[colnames[ci]] = read_cell(coltypes[ci] if ci<len(coltypes) else 'i')
        rows.append(row)
    return {"columns":colnames,"types":coltypes,"nrows":nrows,"rows":rows}

if __name__=="__main__":
    path=sys.argv[1]
    dt=parse_datatable(path)
    if len(sys.argv)>2 and sys.argv[2]=="json":
        print(json.dumps(dt,indent=1))
    else:
        print(f"{os.path.basename(path)}: {dt['nrows']} rows x {len(dt['columns'])} cols")
        print("COLS:", dt["columns"])
        print("TYPES:", dt["types"])
        for row in dt["rows"][:6]:
            print(row)

