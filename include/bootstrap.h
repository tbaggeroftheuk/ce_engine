#ifndef BOOTSTRAP_H
#define BOOTSTRAP_H

#include <SDL2/SDL.h>
#include <stdbool.h>

void ce_bootstrap(void);

void ce_exit(void);
void ce_exit_debug(void);
void ce_exit_global(void);

#endif // BOOTSTRAP_H