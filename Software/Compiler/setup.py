#!/usr/bin/env python3
"""
Setup script for the Hack Assembler (Windows + MSYS2/mingw64).

What it does:
- Verifies MSYS2 is installed (default C:\msys64 or MSYS2_DIR env).
- Installs required packages (idempotent): gcc, make (mingw64).
- Invokes your Makefiles to build the project.
- Ensures the executable is at: Software/Compiler/bin/HackAssembler.exe
  (Moves/renames legacy outputs like HackComp(.exe) if needed.)

What it does NOT do:
- It does not run the assembler. Use bin\run.bat for that.

Usage (from repo root, e.g., CPU-Design-in-Logisim):
    python .\Software\Compiler\setup_env.py
Optional:
    set MSYS2_DIR=D:\msys64
"""

import os
import sys
import subprocess
import pathlib
from shutil import move

MSYS_DEFAULT = r"C:\msys64"
REQUIRED_PKGS = ["mingw-w64-x86_64-gcc", "mingw-w64-x86_64-make", "make"]

def fail(msg: str) -> None:
    print(f"[ERR] {msg}", file=sys.stderr)
    sys.exit(1)

def win_to_msys(p: pathlib.Path) -> str:
    p = p.resolve()
    # Convert "C:\path\to\dir" -> "/c/path/to/dir"
    return f"/{p.drive[0].lower()}{str(p).replace('\\','/')[2:]}"

def bash_call(msys_dir: pathlib.Path, cmd: str) -> None:
    bash = msys_dir / "usr" / "bin" / "bash.exe"
    if not bash.is_file():
        fail(f"bash.exe not found at {bash}. Install MSYS2 or set MSYS2_DIR.")
    full = f'export MSYSTEM=MINGW64; source /etc/profile; set -e; {cmd}'
    subprocess.check_call([str(bash), "-lc", full])

def ensure_deps(msys_dir: pathlib.Path) -> None:
    # Idempotent installs: --needed skips packages already present
    print("[STEP] Refreshing MSYS2 package database …")
    bash_call(msys_dir, "pacman -Sy --noconfirm")
    pkgs = " ".join(REQUIRED_PKGS)
    print(f"[STEP] Installing toolchain (idempotent): {pkgs}")
    bash_call(msys_dir, f"pacman -S --noconfirm --needed {pkgs}")

def do_build(msys_dir: pathlib.Path, compiler_dir: pathlib.Path) -> None:
    c_ms = win_to_msys(compiler_dir)
    print("[STEP] Building (lib then assembler) …")
    cmd = rf'''
        cd "{c_ms}";
        if command -v mingw32-make >/dev/null 2>&1; then M=mingw32-make; else M=make; fi;
        $M -C lib -j
        $M -j
    '''
    bash_call(msys_dir, cmd)

def normalize_binary(compiler_dir: pathlib.Path) -> pathlib.Path:
    """
    Place final exe in: Software/Compiler/bin/HackAssembler.exe
    Accepts legacy names and locations, and moves/renames as needed.
    """
    bin_dir = compiler_dir / "bin"
    bin_dir.mkdir(parents=True, exist_ok=True)

    targets = [
        compiler_dir / "HackAssembler.exe",
        compiler_dir / "HackAssembler",
        compiler_dir / "HackComp.exe",
        compiler_dir / "HackComp",
        bin_dir / "HackAssembler.exe",
        bin_dir / "HackAssembler",
        bin_dir / "HackComp.exe",
        bin_dir / "HackComp",
    ]

    found = None
    for cand in targets:
        if cand.exists():
            found = cand
            break

    if not found:
        fail("Could not find the assembler binary after build. "
             "Check your Makefile output name/paths.")

    # Decide final target
    final = bin_dir / "HackAssembler.exe"
    if found.resolve() == final.resolve():
        print(f"[STEP] Binary is ready: {final}")
        return final

    # If it's a non-.exe, still normalize name to .exe for Windows UX
    print(f"[STEP] Moving/renaming '{found.name}' -> '{final.name}'")
    try:
        # If target exists, replace it
        if final.exists():
            final.unlink()
        move(str(found), str(final))
    except Exception as e:
        fail(f"Failed to move '{found}' -> '{final}': {e}")

    if not final.exists():
        fail("Move/rename succeeded but final file not found. Permissions?")
    print(f"[STEP] Binary is ready: {final}")
    return final

def main() -> None:
    # Resolve paths relative to repo root (user runs this from repo root ideally)
    proj_root = pathlib.Path(".").resolve()
    compiler_dir = proj_root / "Software" / "Compiler"

    if not compiler_dir.exists():
        fail(f"Compiler directory not found: {compiler_dir}")

    msys_dir = pathlib.Path(os.environ.get("MSYS2_DIR", MSYS_DEFAULT))
    if not msys_dir.exists():
        fail(f"MSYS2 not found at {msys_dir}. Install MSYS2 or set MSYS2_DIR.")

    ensure_deps(msys_dir)
    do_build(msys_dir, compiler_dir)
    normalize_binary(compiler_dir)

    print("[OK] Setup complete. You can now run: Software\\Compiler\\bin\\run.bat")

if __name__ == "__main__":
    try:
        main()
    except subprocess.CalledProcessError as e:
        fail(f"Command failed with exit code {e.returncode}")
