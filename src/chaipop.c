/*
 * File: chaipop.c
 * Date: 2026-01-20
 *
 * Virtual Machine in C. (for fun/learning)
 * Program interpreter reads instructions from a text-file.
 * The VM then executes each instruction.
 *
 * Licensed under the MIT License. Copyright (c) 2026 Ryan A. Rashidian
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define VM_STACK_SIZE 256
#define INSTR_MAXLEN 10
#define NUM_INSTR 17

bool stack_view_option = false;

struct VM {
    int16_t stack[VM_STACK_SIZE];
    int sp;
} vm = {0};

static const char *instr_map[NUM_INSTR] = {
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

typedef enum {
    INSTR_PUSH  = 0,
    INSTR_POP   = 1,
    INSTR_ADD   = 2,
    INSTR_SUB   = 3,
    INSTR_MUL   = 4,
    INSTR_EQ    = 5,
    INSTR_LT    = 6,
    INSTR_GT    = 7,
    INSTR_AND   = 8,
    INSTR_OR    = 9,
    INSTR_XOR   = 10,
    INSTR_JMP   = 11,
    INSTR_JMPZ  = 12,
    INSTR_JMPNZ = 13,
    INSTR_DUP   = 14,
    INSTR_SWAP  = 15,
    INSTR_HALT  = 16
} Instr_Type;

typedef struct {
    Instr_Type type;
    int16_t value;
} Instr;

struct Program {
    size_t size;
    int pc;
    Instr program[];
} *prog;

void terminate(void);
void terminate_err(const char *message);
void prog_init(size_t size);
void prog_destroy(void);
static size_t  prog_count_instr(FILE *fp);
static int     prog_parse_value(FILE *fp);
static Instr   prog_parse_instr(FILE *fp);
void prog_read(char *filename);
void stack_print(Instr_Type opcode);
void op_push(int value);
int  op_pop(void);
void op_add(void);
void op_sub(void);
void op_mul(void);
void op_eq(void);
void op_lt(void);
void op_gt(void);
void op_and(void);
void op_or(void);
void op_xor(void);
void op_jmp(int value);
void op_jmpz(int value);
void op_jmpnz(int value);
void op_dup(void);
void op_swap(void);
void vm_loop(void);

void terminate(void)
{
    if (prog != NULL) {
        prog_destroy();
    }
    exit(EXIT_SUCCESS);
}

void terminate_err(const char *message)
{
    if (prog != NULL) {
        prog_destroy();
    }
    fprintf(stderr, "%s\n", message);
    exit(EXIT_FAILURE);
}

void prog_init(size_t size)
{
    prog = malloc(sizeof(struct Program) + size * sizeof(Instr));
    if (prog == NULL) {
        terminate_err("Error: failed to allocate memory for program");
    }
    prog->size = size;
    prog->pc = 0;
}

void prog_destroy(void)
{
    free(prog);
    prog = NULL;
}

static size_t prog_count_instr(FILE *fp)
{
    int ch;
    bool blank = true;
    size_t n_instr = 0;

    while ((ch = getc(fp)) != EOF) {
        if (ch == '\n' && !blank) {
            ++n_instr;
            blank = true;
        } else if (ch == '\n' && blank) {
            ;
        } else {
            blank = false;
        }
    }

    if (!blank) {
        ++n_instr;
    }

    return n_instr;
}

static int prog_parse_value(FILE *fp)
{
    int value;
    if (fscanf(fp, " %d", &value) == 1) {
        return value;
    } else {
        terminate_err("Error: missing operand");
    }
    return 0;
}

static Instr prog_parse_instr(FILE *fp)
{
    char instr_in[INSTR_MAXLEN+1];
    Instr new_instr;

    if (fscanf(fp, " %s", instr_in) == 1) {
        bool valid_instr = false;

        for (int i = 0; i < NUM_INSTR; i++) {
            if (strcmp(instr_in, instr_map[i]) == 0) {
                valid_instr = true;
                if (i == INSTR_PUSH || i == INSTR_JMP ||
                    i == INSTR_JMPZ || i == INSTR_JMPNZ
                ) {
                    int value = prog_parse_value(fp);
                    new_instr.value = value;
                }
                new_instr.type = i;
            }
        }

        if (!valid_instr) {
            terminate_err("Error: invalid instruction");
        }
    } else {
        terminate_err("Error: failed to read instruction");
    }
    
    return new_instr;
}

void prog_read(char *filename)
{
    FILE *fp;
    size_t n_instr;

    if ((fp = fopen(filename, "r")) == NULL) {
        terminate_err("Error: failed to open file");
    }

    n_instr = prog_count_instr(fp) + 1;
    rewind(fp);
    prog_init(n_instr);

    for (size_t i = 0; i < n_instr - 1; i++) {
        Instr new_instr = prog_parse_instr(fp);
        prog->program[i] = new_instr;
    }
    prog->program[n_instr-1].type = INSTR_HALT;

    fclose(fp);
}

void stack_print(Instr_Type opcode)
{
    if (vm.sp > 0) {
        printf("OP %2X | PC %3d:  ", opcode, prog->pc);
        for (int i = 0; i < vm.sp; i++) {
            printf("[%5d] ", vm.stack[i]);
        }
        printf("\n");
    }
}

void op_push(int value)
{
    if (vm.sp >= VM_STACK_SIZE) {
        terminate_err("Error: stack overflow");
    }
    vm.stack[vm.sp++] = value;
}

int op_pop(void)
{
    if (vm.sp <= 0) {
        terminate_err("Error: stack underflow");
    }
    return vm.stack[--vm.sp];
}

void op_add(void)
{
    int opnd_b = op_pop();
    int opnd_a = op_pop();
    op_push(opnd_a + opnd_b);
}

void op_sub(void)
{
    int opnd_b = op_pop();
    int opnd_a = op_pop();
    op_push(opnd_a - opnd_b);
}

void op_mul(void)
{
    int opnd_b = op_pop();
    int opnd_a = op_pop();
    op_push(opnd_a * opnd_b);
}

void op_eq(void)
{
    int opnd_b = op_pop();
    int opnd_a = op_pop();
    op_push(opnd_a == opnd_b ? 1 : 0);
}

void op_lt(void)
{
    int opnd_b = op_pop();
    int opnd_a = op_pop();
    op_push(opnd_a < opnd_b ? 1 : 0);
}

void op_gt(void)
{
    int opnd_b = op_pop();
    int opnd_a = op_pop();
    op_push(opnd_a > opnd_b ? 1 : 0);
}

void op_and(void)
{
    int opnd_b = op_pop();
    int opnd_a = op_pop();
    op_push(opnd_a != 0 && opnd_b != 0 ? 1 : 0);
}

void op_or(void)
{
    int opnd_b = op_pop();
    int opnd_a = op_pop();
    op_push(opnd_a != 0 || opnd_b != 0 ? 1 : 0);
}

void op_xor(void)
{
    int opnd_b = op_pop();
    int opnd_a = op_pop();
    op_push((opnd_a != 0) ^ (opnd_b != 0));
}

void op_jmp(int value)
{
    prog->pc += value - 1;
    if (prog->pc < 0 || prog->pc >= (int) prog->size) {
        terminate_err("Error: program counter out of bounds");
    }
}

void op_jmpz(int value)
{
    int opnd = op_pop();
    if (opnd == 0) {
        op_jmp(value);
    }
}

void op_jmpnz(int value)
{
    int opnd = op_pop();
    if (opnd != 0) {
        op_jmp(value);
    }
}

void op_dup(void)
{
    if (vm.sp <= 0) {
        terminate_err("Error: stack underflow");
    }
    int dup_opnd = vm.stack[vm.sp-1];
    op_push(dup_opnd);
}

void op_swap(void)
{
    if (vm.sp < 2) {
        terminate_err("Error: stack underflow");
    }
    int tmp_opnd = vm.stack[vm.sp-1];
    vm.stack[vm.sp-1] = vm.stack[vm.sp-2];
    vm.stack[vm.sp-2] = tmp_opnd;
}

void vm_loop(void)
{
    for (;;) {
        if (prog->pc < 0 || prog->pc >= (int) prog->size) {
            terminate_err("Error: program counter out of bounds");
        }
        Instr instr = prog->program[prog->pc++];

        switch (instr.type) {
            case INSTR_PUSH: {
                op_push(instr.value);
                break;
            }
            case INSTR_POP: {
                printf("%d\n", op_pop());
                break;
            }
            case INSTR_ADD: {
                op_add();
                break;
            }
            case INSTR_SUB: {
                op_sub();
                break;
            }
            case INSTR_MUL: {
                op_mul();
                break;
            }
            case INSTR_EQ: {
                op_eq();
                break;
            }
            case INSTR_LT: {
                op_lt();
                break;
            }
            case INSTR_GT: {
                op_gt();
                break;
            }
            case INSTR_AND: {
                op_and();
                break;
            }
            case INSTR_OR: {
                op_or();
                break;
            }
            case INSTR_XOR: {
                op_xor();
                break;
            }
            case INSTR_JMP: {
                op_jmp(instr.value);
                break;
            }
            case INSTR_JMPZ: {
                op_jmpz(instr.value);
                break;
            }
            case INSTR_JMPNZ: {
                op_jmpnz(instr.value);
                break;
            }
            case INSTR_DUP: {
                op_dup();
                break;
            }
            case INSTR_SWAP: {
                op_swap();
                break;
            }
            case INSTR_HALT: {
                terminate();
            }
            default: break;
        }

        if (stack_view_option) {
            stack_print(instr.type);
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        terminate_err("Usage: chaipop <filename>");
    }
    if (argc == 3) {
        if (strcmp(argv[2], "--view") == 0) {
            stack_view_option = true;
        }
    }

    prog_read(argv[1]);
    vm_loop();

    if (prog != NULL) {
        prog_destroy();
    }

    return 0;
}

