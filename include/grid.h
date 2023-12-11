#ifndef GRID_H
#define GRID_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "takuzu.h"

typedef struct {
  int row;
  int column;
  char choice;
} choice_t;

typedef enum { MODE_FIRST, MODE_ALL } t_mode;
extern t_mode mode;

void grid_copy(t_grid *gs, t_grid *gd);
void set_cell(int i, int j, t_grid *g, char v);
char get_cell(int i, int j, t_grid *g);

bool is_grid_full(t_grid *g);

bool is_row_empty(int i, t_grid *g);
bool is_col_empty(int j, t_grid *g);

bool is_consistent(t_grid *g);
bool is_valid(t_grid *g);

choice_t grid_choice(t_grid *grid);
void grid_choice_apply(t_grid *grid, const choice_t choice);
void grid_choice_remove(t_grid *grid, const choice_t choice);
void grid_choice_print(const choice_t choice, FILE *fd);

void add_solution(t_grid *grid, t_grid **solutions, int *nb_solutions);
void free_solutions(t_grid **solutions, int nb_solutions);

t_grid *grid_solver(t_grid *grid, const t_mode mode);
void sub_grid_solver(t_grid *grid, t_grid *solution, t_grid **solutions,
                     int *nb_solutions);

bool apply_heuristic1(t_grid *g);
bool sub_heuristic1_rows(t_grid *g);
bool sub_heuristic1_cols(t_grid *g);
bool apply_heuristic2(t_grid *g);
bool sub_heuristic2_rows(t_grid *g);
bool sub_heuristic2_cols(t_grid *g);

void apply_heuristics(t_grid *g);
void generate_grid(t_grid *g, int percentage_fill);
#endif