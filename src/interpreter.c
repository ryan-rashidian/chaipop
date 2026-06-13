/*
 * File: interpreter.c
 *
 * Bytecode Interpreter
 *
 * Licensed under the MIT License. Copyright (c) 2026 Ryan A. Rashidian
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "instructions.h"

#define MAX_STACK_SIZE 1024

void op_nop(void);
void op_push(void);
void op_pop(void);
void op_add(void);
void op_sub(void);
void op_mul(void);
void op_eq(void);
void op_lt(void);
void op_gt(void);
void op_and(void);
void op_or(void);
void op_xor(void);
void op_jmp(void);
void op_jmpz(void);
void op_jmpnz(void);
void op_dup(void);
void op_swap(void);
void op_halt(void);

typedef void (*Op)(void);

// Function pointer lookup table
static const Op dispatch_table[INSTR_COUNT] = {
    op_nop,
    op_push,
    op_pop,
    op_add,
    op_sub,
    op_mul,
    op_eq,
    op_lt,
    op_gt,
    op_and,
    op_or,
    op_xor,
    op_jmp,
    op_jmpz,
    op_jmpnz,
    op_dup,
    op_swap,
    op_halt,
};

static bool opt_view_flag = false;

static struct VM {
    int16_t stack[MAX_STACK_SIZE];
    int sp;
    struct {
        char  *data;
        size_t size;
        int counter;
        int counter_update;
    } prog;
} vm = { 0 };

static void program_destroy(void);

static void terminate_interpreter(void)
{
    program_destroy();
    exit(EXIT_FAILURE);
}

//-----------------------------------------------------------------------------
// Program handling functions
//-----------------------------------------------------------------------------

static void program_init(void)
{
    if (vm.prog.size == 0) {
        fprintf(stderr, "Error: Trying to interpret empty file\n");
        terminate_interpreter();
    }

    vm.prog.data = malloc((vm.prog.size + 1) * sizeof(char));
    if (vm.prog.data == NULL) {
        fprintf(stderr, "Error: Failed to alloc memory for program\n");
        terminate_interpreter();
    }

    vm.prog.counter = 0;
    vm.prog.counter_update = 1;
}

static void program_destroy(void)
{
    if (vm.prog.data != NULL) free(vm.prog.data);
}

static void stack_print(uint8_t opcode)
{
    if (vm.sp > 0) {
        printf("OP %2X | PC %3d:  ", opcode, vm.prog.counter);
        for (int i = 0; i < vm.sp; i++) {
            printf("[%5d] ", vm.stack[i]);
        }
        printf("\n");
    }
}

// Count bytes for allocation first
static void program_count(FILE *fp)
{
    int ch;

    while ((ch = fgetc(fp)) != EOF) {
        vm.prog.size++;
    }

    rewind(fp);
}

static void program_read(FILE *fp)
{
    int ch;
    size_t i = 0;

    while ((ch = fgetc(fp)) != EOF) {
        vm.prog.data[i++] = ch;
    }

    vm.prog.data[i] = INSTR_HALT;
}

static void interpreter_loop(void)
{
    uint8_t instr = INSTR_NOP;

    for (;;) {
        if (vm.prog.counter < 0 || vm.prog.counter >= (int)vm.prog.size) {
            terminate_interpreter();
        }

        instr = vm.prog.data[vm.prog.counter];
        dispatch_table[instr](); // Call Op routine from table

        if (opt_view_flag) {
            stack_print(instr);
        }

        vm.prog.counter += vm.prog.counter_update;
        vm.prog.counter_update = 1;
    }
}

// Interpreter entry point
void interpret(const char *file_path, bool opt_view)
{
    FILE *fp;

    if ((fp = fopen(file_path, "rb")) == NULL) {
        fprintf(stderr, "Error: failed to open file: %s\n", file_path);
        exit(EXIT_FAILURE);
    }

    program_count(fp);
    program_init();
    program_read(fp);
    fclose(fp);

    opt_view_flag = opt_view;

    interpreter_loop();
}

//-----------------------------------------------------------------------------
// Op routine definitions
//-----------------------------------------------------------------------------

void op_nop(void)
{
    ;
}

void op_push(void)
{
    if (vm.sp >= MAX_STACK_SIZE) {
        fprintf(stderr, "Error: Stack overflow\n");
        terminate_interpreter();
    }

    size_t counter_update = sizeof(int16_t) + sizeof(uint8_t);

    if (vm.prog.counter + counter_update >= vm.prog.size) {
        fprintf(stderr, "Error: Missing operand\n");
        terminate_interpreter();
    }

    vm.stack[vm.sp++] = (int16_t)vm.prog.data[vm.prog.counter + 1];

    vm.prog.counter_update = counter_update;
}

void op_pop(void)
{
    if (vm.sp <= 0) {
        fprintf(stderr, "Error: Stack underflow\n");
        terminate_interpreter();
    }

    vm.sp--;
}

void op_add(void)
{
    op_pop();
    int16_t b = vm.stack[vm.sp];
    op_pop();
    int16_t a = vm.stack[vm.sp];

    vm.stack[vm.sp++] = a + b;
}

void op_sub(void)
{
    op_pop();
    int16_t b = vm.stack[vm.sp];
    op_pop();
    int16_t a = vm.stack[vm.sp];

    vm.stack[vm.sp++] = a - b;
}

void op_mul(void)
{
    op_pop();
    int16_t b = vm.stack[vm.sp];
    op_pop();
    int16_t a = vm.stack[vm.sp];

    vm.stack[vm.sp++] = a * b;
}

void op_eq(void)
{
    op_pop();
    int16_t b = vm.stack[vm.sp];
    op_pop();
    int16_t a = vm.stack[vm.sp];

    vm.stack[vm.sp++] = (a == b) ? 1 : 0;
}

void op_lt(void)
{
    op_pop();
    int16_t b = vm.stack[vm.sp];
    op_pop();
    int16_t a = vm.stack[vm.sp];

    vm.stack[vm.sp++] = (a < b) ? 1 : 0;
}

void op_gt(void)
{
    op_pop();
    int16_t b = vm.stack[vm.sp];
    op_pop();
    int16_t a = vm.stack[vm.sp];

    vm.stack[vm.sp++] = (a > b) ? 1 : 0;
}

void op_and(void)
{
    op_pop();
    int16_t b = vm.stack[vm.sp];
    op_pop();
    int16_t a = vm.stack[vm.sp];

    vm.stack[vm.sp++] = ((a != 0) && (b != 0)) ? 1 : 0;
}

void op_or(void)
{
    op_pop();
    int16_t b = vm.stack[vm.sp];
    op_pop();
    int16_t a = vm.stack[vm.sp];

    vm.stack[vm.sp++] = ((a != 0) || (b != 0)) ? 1 : 0;
}

void op_xor(void)
{
    op_pop();
    int16_t b = vm.stack[vm.sp];
    op_pop();
    int16_t a = vm.stack[vm.sp];

    vm.stack[vm.sp++] = ((a != 0) ^ (b != 0)) ? 1 : 0;
}

static void jump_to(int16_t counter_update)
{
    if (vm.prog.counter + counter_update < 0 ||
        vm.prog.counter + counter_update >= (int)vm.prog.size) {
        fprintf(stderr, "Error: Program counter out of bounds\n");
        terminate_interpreter();
    }

    vm.prog.counter_update = counter_update;
}

void op_jmp(void)
{
    if (vm.prog.counter + sizeof(int16_t) >= vm.prog.size) {
        fprintf(stderr, "Error: Missing operand\n");
        terminate_interpreter();
    }

    int16_t counter_update = (int16_t)vm.prog.data[vm.prog.counter+1];

    jump_to(counter_update);
}

void op_jmpz(void)
{
    op_pop();
    int16_t value = vm.stack[vm.sp];

    if (value == 0) {
        if (vm.prog.counter + sizeof(int16_t) >= vm.prog.size) {
            fprintf(stderr, "Error: Missing operand\n");
            terminate_interpreter();
        }

        int16_t counter_update = (int16_t)vm.prog.data[vm.prog.counter+1];

        jump_to(counter_update);
    } else {
        vm.prog.counter_update = sizeof(int16_t) + sizeof(uint8_t);
    }
}

void op_jmpnz(void)
{
    op_pop();
    int16_t value = vm.stack[vm.sp];

    if (value != 0) {
        if (vm.prog.counter + sizeof(int16_t) >= vm.prog.size) {
            fprintf(stderr, "Error: Missing operand\n");
            terminate_interpreter();
        }

        int16_t counter_update = (int16_t)vm.prog.data[vm.prog.counter+1];

        jump_to(counter_update);
    } else {
        vm.prog.counter_update = sizeof(int16_t) + sizeof(uint8_t);
    }
}

void op_dup(void)
{
    if (vm.sp <= 0) {
        fprintf(stderr, "Error: Stack underflow\n");
        terminate_interpreter();
    } else if (vm.sp >= MAX_STACK_SIZE) {
        fprintf(stderr, "Error: Stack overflow\n");
        terminate_interpreter();
    }

    int16_t operand = vm.stack[vm.sp-1];
    vm.stack[vm.sp++] = operand;
}

void op_swap(void)
{
    if (vm.sp < 2) {
        fprintf(stderr, "Error: Stack underflow\n");
        terminate_interpreter();
    }

    int16_t tmp_operand = vm.stack[vm.sp-1];
    vm.stack[vm.sp-1] = vm.stack[vm.sp-2];
    vm.stack[vm.sp-2] = tmp_operand;
}

void op_halt(void)
{
    program_destroy();
    exit(EXIT_SUCCESS);
}

