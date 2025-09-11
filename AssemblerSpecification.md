## Hack Assembly Language — Cheat Sheet

### 1) File Syntax

* One instruction or label per line.
* Whitespace is ignored.
* Comments start with `//` and continue to end of line.
* **Labels:** `(LABEL)` defines a ROM address equal to the next instruction’s address.
* **Symbols:** Can be predefined, label names, or variables introduced via `@symbol`.

**Valid symbol name regex:**
`^[A-Za-z_.$:][A-Za-z0-9_.$:]*$`

### 2) Instruction Types

There are only **two** instruction types:

#### A-instruction

```
@value          # decimal 0..32767
@symbol         # resolved to an address by the assembler
```

**Binary (16-bit):**
`0 v v v v v v v v v v v v v v v`
(15-bit value)

* If `@symbol` is:

  * A **predefined** symbol → use its fixed address.
  * A **label** → use its ROM address.
  * A **new variable** → allocate RAM starting at address **16** (R16, R17, …).

#### C-instruction

```
dest=comp;jump
```

Each part is optional:

* `dest=` may be omitted.
* `;jump` may be omitted.

**Binary (16-bit):**
`1 1 1 a c1 c2 c3 c4 c5 c6 d1 d2 d3 j1 j2 j3`

* `a c1..c6` encode the computation (`comp`)
* `d1 d2 d3` encode `dest`
* `j1 j2 j3` encode `jump`

### 3) dest / jump Codes

**dest (ddd)**

| dest | d1 d2 d3 |
| ---- | -------- |
| null | 0 0 0    |
| M    | 0 0 1    |
| D    | 0 1 0    |
| MD   | 0 1 1    |
| A    | 1 0 0    |
| AM   | 1 0 1    |
| AD   | 1 1 0    |
| AMD  | 1 1 1    |

**jump (jjj)**

| jump | j1 j2 j3 |
| ---- | -------- |
| null | 0 0 0    |
| JGT  | 0 0 1    |
| JEQ  | 0 1 0    |
| JGE  | 0 1 1    |
| JLT  | 1 0 0    |
| JNE  | 1 0 1    |
| JLE  | 1 1 0    |
| JMP  | 1 1 1    |

### 4) comp Codes (a c1..c6)

**When a = 0 (use A)**

| comp | a c1 c2 c3 c4 c5 c6 |
| ---- | ------------------- |
| 0    | 0 1 0 1 0 1 0       |
| 1    | 0 1 1 1 1 1 1       |
| -1   | 0 1 1 1 0 1 0       |
| D    | 0 0 0 1 1 0 0       |
| A    | 0 1 1 0 0 0 0       |
| !D   | 0 0 0 1 1 0 1       |
| !A   | 0 1 1 0 0 0 1       |
| -D   | 0 0 0 1 1 1 1       |
| -A   | 0 1 1 0 0 1 1       |
| D+1  | 0 0 0 1 1 1 1       |
| A+1  | 0 1 1 0 1 1 1       |
| D-1  | 0 0 0 1 1 1 0       |
| A-1  | 0 1 1 0 0 1 0       |
| D+A  | 0 0 0 0 0 1 0       |
| D-A  | 0 0 1 0 0 1 1       |
| A-D  | 0 0 0 0 1 1 1       |
| D\&A | 0 0 0 0 0 0 0       |
| D\|A | 0 0 1 0 1 0 1       |

**When a = 1 (use M)**

| comp | a c1 c2 c3 c4 c5 c6 |
| ---- | ------------------- |
| M    | 1 1 1 0 0 0 0       |
| !M   | 1 1 1 0 0 0 1       |
| -M   | 1 1 1 0 0 1 1       |
| M+1  | 1 1 1 0 1 1 1       |
| M-1  | 1 1 1 0 0 1 0       |
| D+M  | 1 0 0 0 0 1 0       |
| D-M  | 1 0 1 0 0 1 1       |
| M-D  | 1 0 0 0 1 1 1       |
| D\&M | 1 0 0 0 0 0 0       |
| D\|M | 1 0 1 0 1 0 1       |

> Full C-instruction word = `111` + `(a c1..c6)` + `ddd` + `jjj`.

### 5) Predefined Symbols

* **R0..R15** → 0..15
* **SP**=0, **LCL**=1, **ARG**=2, **THIS**=3, **THAT**=4
* **SCREEN**=16384, **KBD**=24576

### 6) Memory Map (RAM)

* 0..15 → general purpose (`R0..R15`)
* 16.. → available for **variables**
* 16384..24575 → **SCREEN**
* 24576 → **KBD**

### 7) Examples

**A-immediate**

```
@21        ; decimal
```

Binary: `0000000000010101`

**A-symbol (variable)**

```
@i         ; if first seen, assembler allocates RAM[16], then 17, ...
M=1
```

**C, no jump**

```
D=A        ; comp=A (a=0), dest=D
```

Binary: `111 0 110000 010 000` → `1110110000010000`

**C with jump**

```
0;JMP
```

Binary: `111 0 101010 000 111` → `1110101010000111`

**Label and use**

```
(LOOP)
0;JMP
@LOOP
```

---

## Notes

* Only **decimal** numbers are used in `@value`. Range: **0..32767**.
* The assembler resolves symbols in **two passes**: first collects labels (ROM addresses), then resolves A-instruction symbols (labels + predefined + variables).
* Variables start at **address 16** unless you predefine them.
