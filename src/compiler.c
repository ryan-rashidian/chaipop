/*
 * File: compiler.c
 *
 * Bytecode compiler for plain-text instruction files
 *
 * Licensed under the MIT License. Copyright (c) 2026 Ryan A. Rashidian
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "instructions.h"

#define MAX_INSTR_STR_LEN 10
#define INIT_OFFSETS_CAPACITY 64

// To match plain-text strings with valid instructions during parsing
static const char *instr_map[INSTR_COUNT] = {
    "NOP",
    "PUSH",
    "POP",
    "ADD",
    "SUB",
    "MUL",
    "EQ",
    "LT",
    "GT",
    "AND",
    "OR",
    "XOR",
    "JMP",
    "JMPZ",
    "JMPNZ",
    "DUP",
    "SWAP",
    "HALT"
};

static FILE *fp_in = NULL, *fp_out = NULL;

static struct {
    size_t *data;
    size_t n;
    size_t capacity;
} offsets;

static size_t program_size = 0;

static void terminate_compiler(void)
{
    if (fp_in)  fclose(fp_in);
    if (fp_out) fclose(fp_out);

    if (offsets.data) free(offsets.data);

    exit(EXIT_FAILURE);
}

static void init_offsets_array(void)
{
    offsets.data = malloc(INIT_OFFSETS_CAPACITY * sizeof(size_t));
    if (offsets.data == NULL) {
        fprintf(stderr, "Error: Failed to alloc memory for compiler\n");
        terminate_compiler();
    }

    offsets.n = 0;
    offsets.capacity = INIT_OFFSETS_CAPACITY;
}

static void increase_offsets_array_capacity(void)
{
    size_t *tmp = realloc(offsets.data, offsets.capacity * 2);
    if (tmp == NULL) {
        fprintf(stderr, "Error: Failed to realloc memory for compiler\n");
        terminate_compiler();
    }

    offsets.data = tmp;
    offsets.capacity *= 2;
}

static uint8_t get_instruction(const char *instr_str)
{
    uint8_t instr = INSTR_NOP;
    bool instr_valid = false;

    for (uint8_t i = 0; i < INSTR_COUNT; i++) {
        if (strncmp(instr_str, instr_map[i], MAX_INSTR_STR_LEN) == 0) {
            instr_valid = true;
            instr = i;
            break;
        }
    }

    if (!instr_valid) {
        fprintf(stderr, "Error: Invalid instruction: %s\n", instr_str);
        terminate_compiler();
    }

    return instr;
}

static int16_t get_operand(uint8_t instr)
{
    int16_t operand;
    int ret;

    ret = fscanf(fp_in, " %hd", &operand);
    if (ret != 1) {
        fprintf(stderr, "Error: Instruction %2X missing operand\n", instr);
        terminate_compiler();
    }

    return operand;
}

// Instruction offsets are primarily needed for jump instructions in order to
// change operand values at compile time to match bytecode spacing.
// Alternatively, a fixed instruction size could be used to avoid this step,
// but that method wastes space with padding.
static void calculate_offsets(void)
{
    char instr_str[MAX_INSTR_STR_LEN];

    while (fscanf(fp_in, " %s", instr_str) == 1) {
        if (offsets.n >= offsets.capacity) increase_offsets_array_capacity();

        uint8_t instr = get_instruction(instr_str);
        offsets.data[offsets.n++] = program_size++;

        if (instr == INSTR_PUSH || instr == INSTR_JMP ||
            instr == INSTR_JMPZ || instr == INSTR_JMPNZ
        ) {
            (void)get_operand(instr);
            program_size += sizeof(int16_t);
        }
    }

    rewind(fp_in);
}

static void parse_instructions(void)
{
    char instr_str[MAX_INSTR_STR_LEN];
    size_t n = 0;

    while (fscanf(fp_in, " %s", instr_str) == 1) {
        uint8_t instr = get_instruction(instr_str);
        fwrite(&instr, sizeof(uint8_t), 1, fp_out);

        if (instr == INSTR_PUSH) {
            int16_t operand = get_operand(instr);
            fwrite(&operand, sizeof(int16_t), 1, fp_out);
        }

        if (instr==INSTR_JMP || instr==INSTR_JMPZ || instr==INSTR_JMPNZ) {
            int16_t operand = get_operand(instr);

            size_t offset_instr = offsets.data[n];
            size_t offset_dest = offsets.data[n+operand];
            int16_t jump = offset_dest - offset_instr;

            fwrite(&jump, sizeof(int16_t), 1, fp_out);
        }

        n++;
    }
}

// Compiler entry point
void compile(const char *file_in, const char *file_out)
{
    if ((fp_in = fopen(file_in, "rb")) == NULL) {
        fprintf(stderr, "Error: Cannot open file: %s", file_in);
        terminate_compiler();
        return;
    }
    if ((fp_out = fopen(file_out, "wb")) == NULL) {
        fprintf(stderr, "Error: Cannot create file: %s", file_out);
        terminate_compiler();
        return;
    }

    init_offsets_array();

    // First pass to allocate meta-data
    calculate_offsets();
    // Second and final pass to output bytecode
    parse_instructions();

    fclose(fp_in);
    fclose(fp_out);
}

