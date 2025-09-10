# How to Run the Hack Assembler (Windows)

The assembler binary is placed at:  
`Software/Compiler/bin/HackAssembler.exe`

## Quick Steps

1) **Run setup once (installs deps + builds)**
   - Open VS Code in the repo (`CPU-Design-in-Logisim`), press **Ctrl+J** to open the terminal.
   - Run:
     ```bat
     py -3 .\Software\Compiler\setup.py
     ```
     *(or `python .\Software\Compiler\setup.py`)*

2) **Assemble a file**
   - Go to the compiler folder:
     ```bat
     cd .\Software\Compiler
     ```
   - Run the assembler:
     ```bat
     .\bin\HackAssembler ..\.ASM\file.asm ..\.HACK\file.hack
     ```
   - **Rule:**  
     ```bat
     .\bin\HackAssembler <input.asm> <output.hack>
     ```
     Make sure the **output folder exists** (create it if needed).

### Examples
```bat
mkdir .\.HACK 2>nul
.\bin\HackAssembler ..\.ASM\Pong.asm .\.HACK\Pong.hack
