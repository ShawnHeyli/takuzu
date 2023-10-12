#ifndef GRID_H
#define GRID_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "takuzu.h"

void grid_copy(t_grid *gs, t_grid *gd);
void set_cell(int i, int j, t_grid *g, char v);
char get_cell(int i, int j, t_grid *g);

bool is_consistent(t_grid *g);
bool is_valid(t_grid *g);
#endif