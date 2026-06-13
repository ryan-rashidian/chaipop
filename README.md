# chaipop

Stack-based virtual machine and mini tool-chain in C.

**This project is for fun and learning purposes.**

## Build

A Makefile in the project's root directory is provided for building with `gcc`.

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

chaipop works like a mini compiler/interpreter tool-chain, with a two step build/run process.

The `build` command converts a plain-text source file into bytecode, then outputs a file that can be read by the interpreter. (Currently outputs `out.bin` in the same directory that the build command is ran from. Optionally specifying the output file's name/path is a planned upgrade.)

The `run` command takes a bytecode file produced by the build command as input, and then attempts to read and execute the instructions.

An example [program](factorial.cpop) that computes `n!` is given to demonstrate. `n` is simply the value given with the first `PUSH` instruction of the program.

```bash
./chaipop build factorial.cpop
./chaipop run out.bin
```

You should be able to see a print out snapshot of the stack in your terminal, updated on each executed instruction. This view feature is not optional, and is currently the only way to get output/results from a program (this may change in the future).

The most important rules are:
- Values on the stack must be signed 16 bit integers.
- Instructions are read by the interpreter one-per-line. Adding multiple instructions on a single line will result in an error.
- The VM interpreter will automatically place a single `HALT` instruction at the end of a program.
- Jump instructions require an operand that will set the program counter backward, or forward relative to the jump instruction's location in the program. Negative values set it backward, and positive values set it forward.

## To-Do / Ideas

- Option to specify output file name for build command
- More instructions

