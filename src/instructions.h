/*
 * File: instructions.h
 *
 * Instruction set for the VM
 *
 * Licensed under the MIT License. Copyright (c) 2026 Ryan A. Rashidian
 */

#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

enum {
    INSTR_NOP,
    INSTR_PUSH,
    INSTR_POP,
    INSTR_ADD,
    INSTR_SUB,
    INSTR_MUL,
    INSTR_EQ,
    INSTR_LT,
    INSTR_GT,
    INSTR_AND,
    INSTR_OR,
    INSTR_XOR,
    INSTR_JMP,
    INSTR_JMPZ,
    INSTR_JMPNZ,
    INSTR_DUP,
    INSTR_SWAP,
    INSTR_HALT,
    INSTR_COUNT
};

#endif

