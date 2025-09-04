# AI Coding Agent Instructions for CPU-Design-in-Logisim

## Project Overview
This repository contains a complete working computer built in Logisim, inspired by the Nand2Tetris course. The computer is designed to simulate a Hack-style architecture, including a custom CPU, memory-mapped I/O, and compatibility with Hack assembly programs.

### Key Components
- **Hardware**: Contains the logic circuits for the CPU, memory, and combinational/sequential logic.
  - `Hardware/CPU/CPU.circ`: The main CPU design.
  - `Hardware/Combinational-Logic/`: Contains primary gates and multi-way gates.
  - `Hardware/Sequential-Logic/`: Includes flip-flops and memory circuits.
- **Software**: Contains tools and programs for the computer.
  - `Software/Compiler/HackComp.c`: A C-based compiler for Hack assembly.
- **Logisim-Modified-version**: Custom version of Logisim tailored for Hack-specific features.

## Developer Workflows

### Setting Up the Project
1. Clone the repository:
   ```bash
   git clone https://github.com/Goku144/CPU-Design-in-Logisim.git
   ```
2. Open `computer.circ` in the custom Logisim version provided in `Logisim-Modified-version/Windows-Program-Exec/`.
3. Load the CPU library:
   - Go to `Project → Load Library → Logisim Library…`
   - Select `Hardware/CPU/CPU.circ`.

### Running Simulations
1. Load a program into ROM (e.g., `Pong.hex` or a custom `.hack` file).
2. Run the simulation in Logisim to observe the computer's behavior.

### Building and Testing the Compiler
- The compiler (`HackComp.c`) translates Hack assembly to machine code.
- Compile the C program using a C compiler (e.g., GCC):
  ```bash
  gcc -o HackComp HackComp.c
  ```
- Run the compiler to generate `.hack` files:
  ```bash
  ./HackComp input.asm output.hack
  ```

## Project-Specific Conventions
- **Layered Abstraction**: The design follows a bottom-up approach:
  1. Elementary gates (e.g., NAND, AND, OR).
  2. Combinational circuits (e.g., multiplexers, ALU).
  3. Sequential circuits (e.g., registers, memory).
  4. Full CPU.
- **Memory-Mapped I/O**:
  - Screen starts at `0x4000`.
  - Keyboard starts at `0x6000`.
- **Custom Logisim Features**: Modified to support Hack-specific memory-mapped devices.

## Integration Points
- **Logisim**: The primary tool for simulating the hardware.
- **Hack Assembly Programs**: Used to test the CPU and memory-mapped I/O.
- **C Compiler**: Converts Hack assembly to machine code.

## Examples
- Example programs like `Pong` and custom `.hack` files can be loaded into the ROM for simulation.
- Use `Software/Compiler/HackComp.c` to create new `.hack` files from `.asm` files.

## Notes for AI Agents
- Focus on maintaining the layered abstraction approach when modifying hardware components.
- Ensure compatibility with Hack assembly programs when updating the compiler or CPU design.
- Document any changes to the memory-mapped I/O addresses or Logisim features.

For more details, refer to the `README.md` file.
