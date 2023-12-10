#ifndef GRID_H
#define GRID_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "takuzu.h"

void grid_copy(t_grid *gs, t_grid *gd);
void set_cell(int i, int j, t_grid *g, char v);
char get_cell(int i, int j, t_grid *g);

bool is_row_consistent(int row, t_grid *g);
bool is_col_consistent(int col, t_grid *g);

bool is_row_empty(int row, t_grid *g);
bool is_col_empty(int row, t_grid *g);
bool is_full(t_grid *g);

bool is_consistent(t_grid *g);
bool is_valid(t_grid *g);

bool apply_heuristic1(t_grid *g);
bool sub_heuristic1_rows(t_grid *g);
bool sub_heuristic1_cols(t_grid *g);
bool apply_heuristic2(t_grid *g);
bool sub_heuristic2_rows(t_grid *g);
bool sub_heuristic2_cols(t_grid *g);

void apply_heuristics(t_grid *g);
void generate_grid(t_grid *g, int percentage_fill);
#endif