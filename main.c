#include <stdio.h>
#include <stdlib.h>
#include "register_alloc.h"

int yyparse(void);

int main(void)
{
    int result;

    printf("Register Allocation Using Graph Colouring\n");
    printf("Enter one assignment per line; finish each statement with ';'.\n");
    printf("Example: a = b + c;\n");
    printf("Finish input with Ctrl+D (Linux/macOS) or Ctrl+Z then Enter (Windows).\n");

    result = yyparse();
    if (result != 0 || parse_error) {
        fprintf(stderr, "Input could not be processed. Check the syntax and try again.\n");
        return EXIT_FAILURE;
    }

    run_register_allocation();
    return EXIT_SUCCESS;
}
