# Register Allocation Using Graph Colouring (C + Lex + Yacc)

## Project summary
This small Compiler Design mini-project uses **Lex/Flex** for tokenization, **Yacc/Bison** for parsing simple assignment statements, and **C** for liveness analysis, interference-graph construction, and greedy register allocation.

The supported input is a straight-line sequence of assignments containing identifiers, integer/decimal literals, `+`, `-`, `*`, `/`, parentheses, and semicolons. Example:

```text
a = b + c;
d = a * c;
e = d + b;
```

The final assignment's left-hand variable is treated as the program result. The program uses three available registers and reports a spill if greedy colouring cannot assign one. The graph is generated from statement-level liveness; this is a teaching-sized model, not a complete production compiler.

## Files
- `lexer.l` — Lex rules
- `parser.y` — Yacc grammar
- `register_alloc.c` — liveness, interference graph, greedy colouring
- `register_alloc.h` — shared declarations
- `main.c` — program entry point
- `sample_input.txt` — sample input
- `Project_Report.docx` — report for submission

## Build on Linux with Flex and Bison
Install the tools if needed, then run these commands from this directory:

```bash
bison -d -o y.tab.c parser.y
flex -o lex.yy.c lexer.l
gcc -Wall -Wextra -o register_alloc main.c register_alloc.c y.tab.c lex.yy.c
./register_alloc < sample_input.txt
```

For classic `lex` and `yacc`, use `yacc -d parser.y`, then `lex lexer.l`, and compile `y.tab.c`, `lex.yy.c`, `register_alloc.c`, and `main.c` together. The lexer defines `yywrap()`, so the build does not need an extra Lex library flag.

## Expected behavior
The output lists the interference edges, the register chosen for each variable, and the number of registers used. Exact variable colours depend on the graph and the greedy order.

## Scope and limitations
- One straight-line basic block only; no branches or loops.
- The input statements are parsed, but this does not generate machine code.
- The final assignment's left-hand variable is considered the observable result.
- Uses a single reverse liveness pass because there are no control-flow joins.
- Greedy colouring may use more registers than an optimal colouring; if no colour is available, the variable is reported as requiring a spill, but spill code is not generated.
