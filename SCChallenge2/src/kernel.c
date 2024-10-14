#include "barebones.h"
#include "terminal.h"

void kernel_main()
{
    initialize_memory_pool();
    clear_screen();
    print_string_color("Welcome to BareBones OS!\n", TXT_LIGHT_BLUE);

    // const char* program =
    //     "clear X;\n"
    //     "incr X;\n"
    //     "incr X;\n"
    //     "clear Y;\n"
    //     "incr Y;\n"
    //     "incr Y;\n"
    //     "incr Y;\n"
    //     "clear Z;\n"
    //     "while X not 0 do;\n"
    //     "   clear W;\n"
    //     "   while Y not 0 do;\n"
    //     "      incr Z;\n"
    //     "      incr W;\n"
    //     "      decr Y;\n"
    //     "   end;\n"
    //     "   while W not 0 do;\n"
    //     "      incr Y;\n"
    //     "      decr W;\n"
    //     "   end;\n"
    //     "   decr X;\n"
    //     "end;\n";

    char* program =
        "clear X;\n"
        "incr X;\n"
        "incr X;\n"
        "incr X;\n"
        "while X not 0 do;\n"
        "   decr X;\n"
        "end;\n";

    print_string_color("Executing Program:\n", TXT_LIGHT_GREEN);
    print_string_color(program, TXT_LIGHT_RED);
    print_string("------------\n");
    interpret(program);
}
