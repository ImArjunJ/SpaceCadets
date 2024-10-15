#include <Windows.h>

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

struct BareBonesVars
{
    std::uint32_t Index[26] = {0}; // Supports variables A-Z, initialized to 0
};

enum CommandType
{
    Clear,
    Incr,
    Decr,
    WhileBegin,
    WhileEnd
};

struct Command
{
    CommandType Type;
    char Var; // Single letter variable associated with the command
};

std::vector<Command> ParseBareBones(const std::string& Input)
{
    std::vector<Command> Commands = {};
    size_t Position = 0;

    while (Position < Input.size())
    {
        // Skip whitespace and semicolons
        while (Position < Input.size() && (Input[Position] == ' ' || Input[Position] == ';' || Input[Position] == '\n'))
        {
            Position++;
        }

        if (Position >= Input.size())
            break;

        // Parse the command
        if (Input.substr(Position, 5) == "clear")
        {
            Position += 6; // Move past "clear "
            char Var = Input[Position];
            Commands.push_back({Clear, Var});
            Position++;
        }
        else if (Input.substr(Position, 4) == "incr")
        {
            Position += 5; // Move past "incr "
            char Var = Input[Position];
            Commands.push_back({Incr, Var});
            Position++;
        }
        else if (Input.substr(Position, 4) == "decr")
        {
            Position += 5; // Move past "decr "
            char Var = Input[Position];
            Commands.push_back({Decr, Var});
            Position++;
        }
        else if (Input.substr(Position, 5) == "while")
        {
            Position += 6; // Move past "while "
            char Var = Input[Position];
            Commands.push_back({WhileBegin, Var});
            Position += 14; // Move past "name not 0 do"
        }
        else if (Input.substr(Position, 3) == "end")
        {
            Commands.push_back({WhileEnd, '\0'});
            Position += 3; // Move past "end"
        }
    }
    return Commands;
}

void GenerateMachineCode(void* Buffer, const std::vector<Command>& Commands, BareBonesVars* Variables)
{
    std::uint8_t* FunctionPtr = static_cast<std::uint8_t*>(Buffer);
    std::vector<std::uint8_t*> LoopStack = {}; // Stack to manage loop jumps

    for (const auto& Command : Commands)
    {
        // Calculate the offset in the BareBonesVars struct (each variable is represented as Index[0] -> Index[25] for A-Z)
        const std::uint32_t IndexOffset = offsetof(BareBonesVars, Index) + sizeof(std::uint32_t) * (Command.Var - 'A'); // 'A' becomes 0, 'B' becomes 1, etc.

        switch (Command.Type)
        {
        case Clear:
        {
            // clear name => mov dword ptr [rcx+offset], 0
            const std::uint8_t ClearCmd[] = {0xC7, 0x41, (std::uint8_t) IndexOffset, 0x00, 0x00, 0x00, 0x00}; // mov [rcx+offset], 0
            std::memcpy(FunctionPtr, ClearCmd, sizeof(ClearCmd));
            FunctionPtr += sizeof(ClearCmd);
            break;
        }
        case Incr:
        {
            // incr name => add dword ptr [rcx+offset], 1
            const std::uint8_t IncrCmd[] = {0x83, 0x41, (std::uint8_t) IndexOffset, 0x01}; // add [rcx+offset], 1
            std::memcpy(FunctionPtr, IncrCmd, sizeof(IncrCmd));
            FunctionPtr += sizeof(IncrCmd);
            break;
        }
        case Decr:
        {
            // decr name => sub dword ptr [rcx+offset], 1
            const std::uint8_t DecrCmd[] = {0x83, 0x69, (std::uint8_t) IndexOffset, 0x01}; // sub [rcx+offset], 1
            std::memcpy(FunctionPtr, DecrCmd, sizeof(DecrCmd));
            FunctionPtr += sizeof(DecrCmd);
            break;
        }
        case WhileBegin:
        {
            // while name not 0 => cmp dword ptr [rcx+offset], 0; jz end
            const std::uint8_t CmpCmd[] = {0x83, 0x79, (std::uint8_t) IndexOffset, 0x00}; // cmp [rcx+offset], 0
            std::memcpy(FunctionPtr, CmpCmd, sizeof(CmpCmd));
            FunctionPtr += sizeof(CmpCmd);

            const std::uint8_t JzPlaceholder[] = {0x0F, 0x84, 0x00, 0x00, 0x00, 0x00}; // jz (jump to be filled later)
            LoopStack.push_back(FunctionPtr);                                          // Save the address to fill in the jump later
            std::memcpy(FunctionPtr, JzPlaceholder, sizeof(JzPlaceholder));
            FunctionPtr += sizeof(JzPlaceholder);
            break;
        }
        case WhileEnd:
        {
            // end => jmp back to the beginning of the loop
            std::uint8_t JmpBack[] = {0xE9, 0x00, 0x00, 0x00, 0x00}; // jmp back
            long loopStart = (long) (LoopStack.back() - FunctionPtr - 5);
            std::memcpy(JmpBack + 1, &loopStart, sizeof(std::uint32_t));
            std::memcpy(FunctionPtr, JmpBack, sizeof(JmpBack));
            FunctionPtr += sizeof(JmpBack);

            // Fill in the forward jump (jz) from the while statement
            const long ForwardJump = (long) (FunctionPtr - LoopStack.back() - 6);
            std::memcpy(LoopStack.back() + 2, &ForwardJump, sizeof(std::uint32_t));
            LoopStack.pop_back();
            break;
        }
        }
    }

    // Add a ret instruction to return from the function
    std::uint8_t RetInstr[] = {0xC3};
    std::memcpy(FunctionPtr, RetInstr, sizeof(RetInstr));
}

int main()
{
    std::cout << "Bare Bones interpreter with raw JIT machine code execution (Windows)\n";

    // 4096 is the page size on Windows, so we allocate a single page
    void* ExecutableBuffer = VirtualAlloc(NULL, 4096, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

    if (!ExecutableBuffer)
    {
        std::cerr << "Failed to allocate executable memory.\n";
        return EXIT_FAILURE;
    }

    const std::string Input = R"(
        clear X;
        incr X;
        incr X;
        clear Y;
        incr Y;
        incr Y;
        incr Y;
        clear Z;
        while X not 0 do;
        clear W;
        while Y not 0 do;
            incr Z;
            incr W;
            decr Y;
        end;
        while W not 0 do;
            incr Y;
            decr W;
        end;
        decr X;
        end;
    )";

    const std::vector<Command> Commands = ParseBareBones(Input);

    BareBonesVars Variables; // Supports variables A-Z

    GenerateMachineCode(ExecutableBuffer, Commands, &Variables);

    // Execute the generated machine code
    using CodeFuncT = void (*)(BareBonesVars*);
    const CodeFuncT Function = reinterpret_cast<CodeFuncT>(ExecutableBuffer);
    Function(&Variables);

    // Print the final values of all variables
    for (char Variable = 'A'; Variable <= 'Z'; ++Variable)
    {
        if (Variables.Index[Variable - 'A'] != 0)
        {
            std::cout << "Value " << Variable << ": " << Variables.Index[Variable - 'A'] << "\n";
        }
    }

    VirtualFree(ExecutableBuffer, 0, MEM_RELEASE);
    return EXIT_SUCCESS;
}
