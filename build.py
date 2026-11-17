"""Build The Adventures of Batman and Robin from this disassembly and check it is byte-identical to the original.

    python build.py path/to/your/dump.bin     # or set BATMANROBIN_ROM, or put batmanrobin.bin here

Needs Python 3.11+ and the Macro Assembler AS (Flamewing's fork, as the Sonic disassemblies
use): the Windows binaries are in tools/as/ (GPL v2, see tools/as/AS.txt); elsewhere put
`asl` and `p2bin` on PATH (https://github.com/Clownacy/asl-releases) or set AS_DIR.

The disassembly still copies the spans nobody has decoded yet (`incrom`) from your dump, so
the build needs it: it is checked, copied here as batmanrobin.bin (gitignored, never commit it),
and the rebuilt ROM must match it byte for byte. Exit 0 only then.
"""
from __future__ import annotations

import hashlib
import os
import shutil
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent
MAIN = "batmanrobin.asm"
ROM_FILE = "batmanrobin.bin"
SHA1 = "1da4b3fc92a6572e70d748ebad346aeb291f89f3"
ENV = "BATMANROBIN_ROM"


def sha1(path: Path) -> str:
    return hashlib.sha1(path.read_bytes()).hexdigest()


def tool(name: str) -> str | None:
    exe = name + (".exe" if sys.platform == "win32" else "")
    for d in filter(None, (os.environ.get("AS_DIR"), str(ROOT / "tools" / "as" / "Windows-x86")
                           if sys.platform == "win32" else None)):
        if (Path(d) / exe).is_file():
            return str(Path(d) / exe)
    return shutil.which(exe) or (shutil.which("asl") if name == "asw" else None)


def rom(argv: list[str]) -> Path | None:
    here = ROOT / ROM_FILE
    src = Path(argv[0]) if argv else Path(os.environ[ENV]) if os.environ.get(ENV) else here
    if not src.is_file():
        print(f"FAIL: no ROM: pass your dump's path, set {ENV}, or copy it here as {ROM_FILE}")
        return None
    if sha1(src) != SHA1:
        print(f"FAIL: {src} is not the expected dump (SHA-1 {sha1(src)}, want {SHA1})")
        return None
    if src.resolve() != here.resolve():
        shutil.copyfile(src, here)
    return here


def main(argv: list[str]) -> int:
    if not rom(argv):
        return 1
    asw, p2bin = tool("asw"), tool("p2bin")
    if not asw or not p2bin:
        print("FAIL: the assembler is missing (asw/asl and p2bin; see the top of build.py)")
        return 1
    (ROOT / "build").mkdir(exist_ok=True)
    stem = Path(MAIN).stem
    obj, out = f"build/{stem}.p", f"build/{stem}.built.bin"
    r = subprocess.run([asw, "-xx", "-n", "-q", "-A", "-L", "-U", "-i", ".", "-olist", f"build/{stem}.lst",
                        "-o", obj, MAIN], cwd=ROOT, capture_output=True, text=True, stdin=subprocess.DEVNULL)
    if r.returncode:
        print("FAIL: assembler error\n" + (r.stdout + r.stderr)[-3000:])
        return 1
    r = subprocess.run([p2bin, obj, out], cwd=ROOT, capture_output=True, text=True, stdin=subprocess.DEVNULL)
    if r.returncode or not (ROOT / out).is_file():
        print("FAIL: p2bin\n" + r.stdout + r.stderr)
        return 1
    built = (ROOT / out).read_bytes()
    if hashlib.sha1(built).hexdigest() != SHA1:
        orig = (ROOT / ROM_FILE).read_bytes()
        diff = next((i for i in range(min(len(built), len(orig))) if built[i] != orig[i]), min(len(built), len(orig)))
        print(f"FAIL: MISMATCH at ${diff:06X}; sizes {len(built)} vs {len(orig)}")
        return 1
    print(f"OK: byte-identical ({len(built)} bytes) -> {out}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
