# chaipop

Stack-based virtual machine in C.

**This project is for fun and learning purposes.**

## Build

A Makefile in the project's root directory is provided for building with gcc.

```bash
cd chaipop/
make
```

## ISA

| Category | Instructions |
| --- | --- |
| Stack | `PUSH <operand>`, `POP`, `DUP`, `SWAP` |
| Arithmetic | `ADD`, `SUB`, `MUL` |
| Logic & Comparison | `EQ`, `LT`, `GT`, `AND`, `OR`, `XOR` |
| Jumps | `JMP <operand>`, `JMPZ <operand>`, `JMPNZ <operand>` |
| End Program | `HALT` |

## Using the VM

You can interact with the VM by supplying the name of a text file as an argument on the command line, and it will attempt to interpret that file as instructions.

The `POP` instruction currently doubles as a print statement.

An example program that computes `n!` is given to demonstrate. `n` is simply the value given with the first `PUSH` instruction of the program.

```bash
./chaipop factorial.cpop
```

The `--view` option prints a formatted view of the stack after each instruction is executed.

```bash
./chaipop factorial.cpop --view
```

The most important rules are:
- Operands must be signed 16 bit integer values.
- Instructions are read by the interpreter one-per-line. Adding multiple instructions on a single line will result in an error.
- The VM interpreter will automatically place a single `HALT` instruction at the end of a program.
- Jump instructions require an operand that will set the program counter backward, or forward relative to the jump instruction's location in the program. Negative integer values set it backward, and positive integer values set it forward.

## To-Do / Ideas

- Programs should be translated to a simple stream of bytes (bytecode). 8 bit opcodes and 16 bit operands in a contiguous array. (currently wasting space with array of structs)
- Possibly have a mini tool-chain with bytecode compiler + interpreter. (`chaipop build <file> <out>`, `chaipop run <file>`)
- Dispatch table for operations instead of switch statement (just a lookup array with function pointers).
- More instructions

