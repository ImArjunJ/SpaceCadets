#include "barebones.h"

#include "mem.h"
#include "terminal.h"
#include "utils.h"

void init_varlist(VarList* vl)
{
    vl->capacity = INITIAL_VAR_CAPACITY;
    vl->count = 0;
    vl->vars = (Variable*) mem_malloc(sizeof(Variable) * vl->capacity);
    if (!vl->vars)
    {
        print_string("Memory allocation failed for variables.\n");
        // Probably halt or something lol
        return;
    }

    for (int i = 0; i < vl->capacity; i++)
    {
        vl->vars[i].name[0] = '\0';
        vl->vars[i].value = 0;
    }
}

// Get variable index, add if not exists (lazy initialization)
int get_var_index(VarList* vl, const char* name)
{
    for (int i = 0; i < vl->count; i++)
    {
        if (util_strcmp(vl->vars[i].name, name) == 0)
            return i;
    }
    // Add new variable
    if (vl->count >= vl->capacity)
    {
        vl->capacity *= 2;
        vl->vars = (Variable*) mem_realloc(vl->vars, sizeof(Variable) * vl->capacity);
        if (!vl->vars)
        {
            print_string("Memory reallocation failed for variables.\n");
            // Probably halt or something lol
            return -1;
        }
        // Initialize new variables
        for (int i = vl->count; i < vl->capacity; i++)
        {
            vl->vars[i].name[0] = '\0';
            vl->vars[i].value = 0;
        }
    }
    // Add new variable
    util_strncpy(vl->vars[vl->count].name, name, MAX_VAR_NAME);
    vl->vars[vl->count].name[MAX_VAR_NAME - 1] = '\0'; // Ensure null-termination
    vl->vars[vl->count].value = 0;
    return vl->count++;
}

void set_var(VarList* vl, const char* name, int value)
{
    int idx = get_var_index(vl, name);
    if (idx != -1)
    {
        vl->vars[idx].value = value;
    }
    else
    {
        print_string("Error: Unable to set variable. Index invalid.\n");
    }
}

int get_var(VarList* vl, const char* name)
{
    int idx = get_var_index(vl, name);
    if (idx != -1)
    {
        return vl->vars[idx].value;
    }
    else
    {
        print_string("Error: Variable not found.\n");
        return 0; // Default value
    }
}

void print_vars(VarList* vl)
{
    for (int i = 0; i < vl->count; i++)
    {
        print_string_color(vl->vars[i].name, TXT_LIGHT_GREEN);
        print_string_color(" = ", TXT_WHITE);

        char buffer[32];
        util_itoa(vl->vars[i].value, buffer, 10);
        print_string_color(buffer, TXT_LIGHT_CYAN);
        print_string_color("\n", TXT_WHITE);
    }
}

void init_instrlist(InstrList* il)
{
    il->capacity = INITIAL_INSTR_CAPACITY;
    il->count = 0;
    il->instrs = (Instruction*) mem_malloc(sizeof(Instruction) * il->capacity);
    if (!il->instrs)
    {
        print_string("Memory allocation failed for instructions.\n");
        // Probably halt or something lol
        return;
    }
}

void add_instruction(InstrList* il, Instruction instr)
{
    if (il->count >= il->capacity)
    {
        il->capacity *= 2;
        il->instrs = (Instruction*) mem_realloc(il->instrs, sizeof(Instruction) * il->capacity);
        if (!il->instrs)
        {
            print_string("Memory reallocation failed for instructions.\n");
            // Probably halt or something lol
            return;
        }
    }
    il->instrs[il->count++] = instr;
}

void init_stack(Stack* s)
{
    s->capacity = INITIAL_STACK_CAPACITY;
    s->top = -1;
    s->data = (int*) mem_malloc(sizeof(int) * s->capacity);
    if (!s->data)
    {
        print_string("Memory allocation failed for stack.\n");
        return;
    }
}

void push_stack(Stack* s, int value)
{
    if (s->top + 1 >= s->capacity)
    {
        s->capacity *= 2;
        s->data = (int*) mem_realloc(s->data, sizeof(int) * s->capacity);
        if (!s->data)
        {
            print_string("Memory reallocation failed for stack.\n");
            // Probably halt or something lol
            return;
        }
    }
    s->data[++(s->top)] = value;
}

int pop_stack(Stack* s)
{
    if (s->top < 0)
    {
        print_string("Error: Pop from empty stack.\n");
        // Probably halt or something lol
        return -1;
    }
    return s->data[(s->top)--];
}

int is_empty(Stack* s)
{
    return s->top < 0;
}

void free_stack_memory(Stack* s)
{
    mem_free(s->data);
}

// Split by ';'
char** split_statements(const char* program, int* count)
{
    int cnt = 0;
    for (int i = 0; program[i]; i++)
    {
        if (program[i] == ';')
            cnt++;
    }
    *count = cnt;
    if (cnt == 0)
        return NULL;

    char** stmts = (char**) mem_malloc(sizeof(char*) * cnt);
    if (!stmts)
    {
        print_string("Memory allocation failed for statements.\n");
        // Probably halt or something lol
        return NULL;
    }

    // Duplicate program string to use util_strtok-like functionality
    char* prog_dup = util_strdup(program);
    if (!prog_dup)
    {
        print_string("Memory allocation failed for program duplication.\n");
        mem_free(stmts);
        return NULL;
    }

    int idx = 0;
    char* token = util_strtok(prog_dup, ";");
    while (token != NULL && idx < cnt)
    {
        // Trim leading and trailing whitespace for each statement
        char* trimmed = util_trim(token);
        stmts[idx++] = util_strdup(trimmed);
        token = util_strtok(NULL, ";");
    }

    mem_free(prog_dup);
    return stmts;
}

void interpret(const char* program)
{
    VarList vl;
    init_varlist(&vl);

    InstrList il;
    init_instrlist(&il);

    // Split program into statements
    int stmt_count = 0;
    char** statements = split_statements(program, &stmt_count);
    if (!statements && stmt_count > 0)
    {
        print_string_color("Error: Failed to split program into statements.\n", TXT_RED);
        return;
    }

    Stack loop_stack;
    init_stack(&loop_stack);

    // Parse statements into instructions
    for (int i = 0; i < stmt_count; i++)
    {
        char* stmt = util_trim(statements[i]);
        if (util_strlen(stmt) == 0)
        {
            mem_free(statements[i]);
            continue; // Skip empty statements
        }

        Instruction instr;
        util_memset(&instr, 0, sizeof(Instruction));

        if (util_strncmp(stmt, "clear", 5) == 0)
        {
            instr.type = CLEAR;
            util_sscanf(stmt, "clear %s", instr.var_name);
        }
        else if (util_strncmp(stmt, "incr", 4) == 0)
        {
            instr.type = INCR;
            util_sscanf(stmt, "incr %s", instr.var_name);
        }
        else if (util_strncmp(stmt, "decr", 4) == 0)
        {
            instr.type = DECR;
            util_sscanf(stmt, "decr %s", instr.var_name);
        }
        else if (util_strncmp(stmt, "while", 5) == 0)
        {
            instr.type = WHILE;
            util_sscanf(stmt, "while %s not 0 do", instr.var_name);
            // Push the current instruction index onto the stack
            push_stack(&loop_stack, il.count);
        }
        else if (util_strncmp(stmt, "end", 3) == 0)
        {
            instr.type = END;
            if (is_empty(&loop_stack))
            {
                print_string_color("Error: 'end' without matching 'while'\n", TXT_RED);
                // Free memory and exit
                for (int j = 0; j < stmt_count; j++) mem_free(statements[j]);
                mem_free(statements);
                free_stack_memory(&loop_stack);
                mem_free(il.instrs);
                mem_free(vl.vars);
                return;
            }
            int while_index = pop_stack(&loop_stack);
            // Set jump indices
            instr.jump = while_index;
            il.instrs[while_index].jump = il.count;
        }
        else
        {
            print_string_color("Error: Unknown statement '", TXT_RED);
            print_string_color(stmt, TXT_RED);
            print_string_color("'\n", TXT_RED);
            // Free memory and exit
            for (int j = 0; j < stmt_count; j++) mem_free(statements[j]);
            mem_free(statements);
            free_stack_memory(&loop_stack);
            mem_free(il.instrs);
            mem_free(vl.vars);
            return;
        }

        add_instruction(&il, instr);
        mem_free(statements[i]);
    }
    mem_free(statements);

    if (!is_empty(&loop_stack))
    {
        print_string_color("Error: 'while' without matching 'end'\n", TXT_RED);
        free_stack_memory(&loop_stack);
        mem_free(il.instrs);
        mem_free(vl.vars);
        return;
    }
    free_stack_memory(&loop_stack);

    // Execute the instructions
    int ip = 0; // Instruction pointer
    while (ip < il.count)
    {
        Instruction current = il.instrs[ip];
        switch (current.type)
        {
        case CLEAR:
#ifdef BBDEBUG
            print_string_color("Clearing variable: ", TXT_LIGHT_GREEN);
            print_string_color(current.var_name, TXT_LIGHT_GREEN);
            print_string_color(" = 0\n", TXT_WHITE);
#endif
            set_var(&vl, current.var_name, 0);
            ip++;
            break;
        case INCR:
        {
            int idx = get_var_index(&vl, current.var_name);
            if (idx != -1)
            {
                int old_val = vl.vars[idx].value;
                vl.vars[idx].value += 1;
                int new_val = vl.vars[idx].value;
#ifdef BBDEBUG
                print_string_color("Incrementing variable: ", TXT_LIGHT_GREEN);
                print_string_color(current.var_name, TXT_LIGHT_GREEN);
                print_string_color(" : ", TXT_WHITE);
                print_int(old_val);
                print_string_color(" -> ", TXT_WHITE);
                print_int(new_val);
                print_char('\n');
#endif
            }
            else
            {
                print_string_color("Error: Variable not found for 'incr'.\n", TXT_RED);
            }
        }
            ip++;
            break;
        case DECR:
        {
            int idx = get_var_index(&vl, current.var_name);
            if (idx != -1)
            {
                int old_val = vl.vars[idx].value;
                if (vl.vars[idx].value > 0)
                    vl.vars[idx].value -= 1;
                int new_val = vl.vars[idx].value;

#ifdef BBDEBUG
                print_string_color("Decrementing variable: ", TXT_LIGHT_GREEN);
                print_string_color(current.var_name, TXT_LIGHT_GREEN);
                print_string_color(" : ", TXT_WHITE);
                print_int(old_val);
                print_string_color(" -> ", TXT_WHITE);
                print_int(new_val);
                print_char('\n');
#endif
            }
            else
            {
                print_string_color("Error: Variable not found for 'decr'.\n", TXT_RED);
            }
        }
            ip++;
            break;
        case WHILE:
        {
            int val = get_var(&vl, current.var_name);
            if (val == 0)
            {
                ip = current.jump + 1; // Jump to after 'end'
            }
            else
            {
                ip++;
            }
        }
        break;
        case END:
        {
            // Go back to corresponding while to check condition
            int while_ip = current.jump;
            int val = get_var(&vl, il.instrs[while_ip].var_name);
            if (val != 0)
            {
                ip = while_ip + 1;
            }
            else
            {
                ip++;
            }
        }
        break;
        default:
            print_string_color("Error: Unknown instruction type.\n", TXT_RED);
            ip++;
            break;
        }
    }

    print_string_color("Program execution completed.\n", TXT_LIGHT_BLUE);
    print_string_color("------------\n", TXT_WHITE);
    print_string_color("Final variable values:\n", TXT_LIGHT_GREEN);
    print_vars(&vl);

    mem_free(il.instrs);
    mem_free(vl.vars);
}