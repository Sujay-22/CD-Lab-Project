#ifndef REGISTER_ALLOC_H
#define REGISTER_ALLOC_H

#define MAX_VARS 100
#define MAX_STATEMENTS 100
#define MAX_NAME 64

extern int parse_error;

void begin_statement(const char *lhs);
void add_current_use(const char *name);
void finish_statement(void);
void run_register_allocation(void);

#endif
