#include <stdio.h>
#include <string.h>
#include "register_alloc.h"

#define AVAILABLE_REGISTERS 3

typedef struct {
    int def;
    int uses[MAX_VARS];
    int use_count;
} Statement;

static char variable_names[MAX_VARS][MAX_NAME];
static int variable_count = 0;
static Statement statements[MAX_STATEMENTS];
static int statement_count = 0;
static int current_def = -1;
static int current_uses[MAX_VARS];
static int current_use_count = 0;
static int interference[MAX_VARS][MAX_VARS];

int parse_error = 0;

static int get_variable_id(const char *name)
{
    int i;

    for (i = 0; i < variable_count; i++) {
        if (strcmp(variable_names[i], name) == 0) {
            return i;
        }
    }

    if (variable_count >= MAX_VARS) {
        fprintf(stderr, "Error: maximum of %d variables exceeded.\n", MAX_VARS);
        parse_error = 1;
        return -1;
    }

    if (strlen(name) >= MAX_NAME) {
        fprintf(stderr, "Error: identifier '%s' is too long (maximum %d characters).\n",
                name, MAX_NAME - 1);
        parse_error = 1;
        return -1;
    }

    strcpy(variable_names[variable_count], name);
    return variable_count++;
}

void begin_statement(const char *lhs)
{
    int i;
    current_def = get_variable_id(lhs);
    current_use_count = 0;
    for (i = 0; i < MAX_VARS; i++) {
        current_uses[i] = 0;
    }
}

void add_current_use(const char *name)
{
    int id = get_variable_id(name);

    if (id < 0) {
        return;
    }

    if (!current_uses[id]) {
        current_uses[id] = 1;
        current_use_count++;
    }
}

void finish_statement(void)
{
    int i;

    if (current_def < 0) {
        parse_error = 1;
        return;
    }
    if (statement_count >= MAX_STATEMENTS) {
        fprintf(stderr, "Error: maximum of %d statements exceeded.\n", MAX_STATEMENTS);
        parse_error = 1;
        return;
    }

    statements[statement_count].def = current_def;
    statements[statement_count].use_count = 0;
    for (i = 0; i < variable_count; i++) {
        if (current_uses[i]) {
            statements[statement_count].uses[statements[statement_count].use_count++] = i;
        }
    }
    statement_count++;
}

static void add_interference_edge(int a, int b)
{
    if (a >= 0 && b >= 0 && a != b) {
        interference[a][b] = 1;
        interference[b][a] = 1;
    }
}

static void build_interference_graph(void)
{
    int live[MAX_VARS] = {0};
    int live_in[MAX_STATEMENTS][MAX_VARS] = {{0}};
    int live_out[MAX_STATEMENTS][MAX_VARS] = {{0}};
    int i, j, k;

    memset(interference, 0, sizeof(interference));

    /* For this mini-project, the final assignment's left-hand variable is
       treated as the result that remains live at the end of the basic block. */
    if (statement_count > 0) {
        live[statements[statement_count - 1].def] = 1;
    }

    /* A single reverse pass is sufficient for a straight-line basic block. */
    for (i = statement_count - 1; i >= 0; i--) {
        int def = statements[i].def;

        for (j = 0; j < variable_count; j++) {
            live_out[i][j] = live[j];
        }

        /* A newly defined value must not overwrite any value live after it. */
        for (j = 0; j < variable_count; j++) {
            if (live_out[i][j]) {
                add_interference_edge(def, j);
            }
        }

        /* live-in = uses union (live-out minus the defined variable). */
        for (j = 0; j < variable_count; j++) {
            live_in[i][j] = live_out[i][j] && j != def;
        }
        for (j = 0; j < statements[i].use_count; j++) {
            live_in[i][statements[i].uses[j]] = 1;
        }

        /* Values simultaneously live before an instruction need distinct registers. */
        for (j = 0; j < variable_count; j++) {
            if (!live_in[i][j]) {
                continue;
            }
            for (k = j + 1; k < variable_count; k++) {
                if (live_in[i][k]) {
                    add_interference_edge(j, k);
                }
            }
        }

        for (j = 0; j < variable_count; j++) {
            live[j] = live_in[i][j];
        }
    }
}

static void colour_graph(int colour[MAX_VARS])
{
    int v, n, r;

    for (v = 0; v < variable_count; v++) {
        colour[v] = -1;
    }

    for (v = 0; v < variable_count; v++) {
        int unavailable[AVAILABLE_REGISTERS] = {0};

        for (n = 0; n < variable_count; n++) {
            if (interference[v][n] && colour[n] >= 0) {
                unavailable[colour[n]] = 1;
            }
        }

        for (r = 0; r < AVAILABLE_REGISTERS; r++) {
            if (!unavailable[r]) {
                colour[v] = r;
                break;
            }
        }
    }
}

void run_register_allocation(void)
{
    int colours[MAX_VARS];
    int i, j, registers_used = 0;

    if (statement_count == 0) {
        printf("No assignment statements were entered.\n");
        return;
    }

    build_interference_graph();
    colour_graph(colours);

    printf("\nRegister Allocation Using Graph Colouring\n");
    printf("==========================================\n");
    printf("Statements parsed: %d\n", statement_count);
    printf("Available registers: %d\n", AVAILABLE_REGISTERS);
    printf("Assumption: the last assignment's left-hand variable is the program result.\n");

    printf("\nInterference edges:\n");
    {
        int found_edge = 0;
        for (i = 0; i < variable_count; i++) {
            for (j = i + 1; j < variable_count; j++) {
                if (interference[i][j]) {
                    printf("%s -- %s\n", variable_names[i], variable_names[j]);
                    found_edge = 1;
                }
            }
        }
        if (!found_edge) {
            printf("(no interference edges)\n");
        }
    }

    printf("\nRegister assignment:\n");
    printf("%-16s %s\n", "Variable", "Register");
    for (i = 0; i < variable_count; i++) {
        if (colours[i] < 0) {
            printf("%-16s %s\n", variable_names[i], "Spill required");
        } else {
            printf("%-16s R%d\n", variable_names[i], colours[i] + 1);
            if (colours[i] + 1 > registers_used) {
                registers_used = colours[i] + 1;
            }
        }
    }

    printf("\nRegisters used: %d\n", registers_used);
    printf("Note: variables connected by an interference edge must use different registers.\n");
}
