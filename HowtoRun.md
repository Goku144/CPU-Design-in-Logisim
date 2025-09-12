# How to Run the Hack Assembler (Windows)

**Binary:** `Software/Compiler/bin/HackAssembler.exe`

## CLI

```text
HackAssembler <input.asm> ([-o <name>] | [-h <name>] | [-b <name>])
```

### Options

* `-o <name>` → writes to `..\.HACK\<name>.hack`
* `-h <name>` → writes to `..\.HEX\<name>.hex`
* `-b <name>` → writes **both**:

  * `..\.HACK\<name>.hack`
  * `..\.HEX\<name>.hex`

> **Important:** `<name>` is a **base name only** (no folders).
> Don’t pass paths like `..\.HACK\Name` or `..\some\path\Name` — the program already prepends the right folder.
> Extensions are auto-added (case-insensitive). If you pass them (e.g., `Name.hack`), they won’t be duplicated.

## Examples

```bat
cd .\Software\Assembler

:: Assemble Pong to HACK (.hack text)
.\bin\HackAssembler ..\.ASM\Pong.asm -o Pong

:: Convert to HEX (.hex text)
.\bin\HackAssembler ..\.ASM\Pong.asm -h Pong

:: Do both at once
.\bin\HackAssembler ..\.ASM\Pong.asm -b Pong
```

**This produces:**

```
..\.HACK\Pong.hack
..\.HEX\Pong.hex
```

> The tool creates `..\.HACK\` and `..\.HEX\` automatically if they don’t exist.

## Common Pitfalls

* **Using a path in `<name>`**
  `-o ..\.HACK\Pong` ⇒ results in `..\.HACK\..\.HACK\Pong.hack` (wrong).
  Use `-o Pong` instead.

* **Old (deprecated) usage**
  Previous docs showed: `HackAssembler <input.asm> <output.hack>`
  Now you must use one of `-o`, `-h`, or `-b` as above.