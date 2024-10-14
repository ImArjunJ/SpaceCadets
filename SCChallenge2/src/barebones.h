#ifndef BAREBONES_H
#define BAREBONES_H

#define MAX_VAR_NAME 100
#define INITIAL_VAR_CAPACITY 100
#define INITIAL_INSTR_CAPACITY 100
#define INITIAL_STACK_CAPACITY 100

typedef enum
{
    CLEAR,
    INCR,
    DECR,
    WHILE,
    END
} InstrType;

typedef struct
{
    char name[MAX_VAR_NAME];
    int value;
} Variable;

typedef struct
{
    InstrType type;
    char var_name[MAX_VAR_NAME]; // For CLEAR, INCR, DECR, WHILE
    int jump;                    // For WHILE and END, index to jump to
} Instruction;

typedef struct
{
    Variable* vars;
    int count;
    int capacity;
} VarList;

typedef struct
{
    Instruction* instrs;
    int count;
    int capacity;
} InstrList;

typedef struct
{
    int* data;
    int top;
    int capacity;
} Stack;

void interpret(const char* program);

#endif // BAREBONES_H