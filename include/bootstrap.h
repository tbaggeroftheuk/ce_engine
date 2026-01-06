#ifndef BOOTSTRAP_H
#define BOOTSTRAP_H

#include <stdbool.h>

void ce_initialize(void);

void ce_exit(void);
void ce_exit_debug(void);
void ce_exit_global(void);

#endif // BOOTSTRAP_H