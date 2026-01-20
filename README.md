# chaipop

**This project is for fun/learning purposes.**

Stack-based virtual machine and instruction interpreter in C.

## Build

A Makefile in the project's root directory is provided for building chaipop with gcc.

```bash
cd chaipop/
make
```

## ISA

- Stack: `PUSH <operand>`, `POP`, `DUP`, `SWAP`
- Arithmetic: `ADD`, `SUB`, `MUL`
- Logical & Comparison: `EQ`, `LT`, `GT`, `AND`, `OR`, `XOR`
- Jump & Conditional Jumps: `JMP <operand>`, `JMPZ <operand>`, `JMPNZ <operand>`
- End Program: `HALT`

## Using the VM

You can interact with the VM by supplying the name of a text file as an argument on the command line, and it will attempt to interpret that file as instructions.

The `POP` instruction currently doubles as a print statement, allowing some form of terminal output. More detailed ways of viewing the stack may be a future improvement.

An example program that computes `n!` is given to demonstrate. The first `PUSH` value represents `n`.:

```bash
./chaipop factorial.cpop
```

The most important rules are:
- Operands must be integer values.
- Instructions are read by the interpreter one-per-line. Adding multiple instructions on a single line will result in an error.
- The VM interpreter will automatically place a single `HALT` instruction at the end of a program.
- Jump instructions require an operand that will set the program counter backward, or forward relative to the jump instruction's location in the program. Negative integer values set it backward, and positive integer values set it forward.

