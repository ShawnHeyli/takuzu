#include "grid.h"

#include <errno.h>
#include <getopt.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "takuzu.h"

// we consider that the destination grid is already allocated
void grid_copy(t_grid *gs, t_grid *gd) {
  gd->size = gs->size;
  gd->grid = malloc(gd->size * sizeof(char *));
  for (int i = 0; i < gd->size; i++) {
    gd->grid[i] = malloc(gd->size * sizeof(char));
  }

  for (int i = 0; i < gd->size; i++) {
    for (int j = 0; j < gd->size; j++) {
      gd->grid[i][j] = gs->grid[i][j];
    }
  }
}

char get_cell(int i, int j, t_grid *g) {
  // Out of bounds
  if (i < 0 || i >= g->size || j < 0 || j >= g->size) {
    fprintf(stderr,
            "ERROR -> tried to get a cell out of bounds (%d, %d) in a grid "
            "of size %d\n",
            i, j, g->size);
    exit(EXIT_FAILURE);
  }
  return g->grid[i][j];
}

void set_cell(int i, int j, t_grid *g, char v) {
  // Invalid character
  if (!check_char(v)) {
    fprintf(stderr,
            "ERROR -> tried to insert an invalid character ('%c') in a grid \n",
            v);
    exit(EXIT_FAILURE);
  }
  // Out of bounds
  if (i < 0 || i >= g->size || j < 0 || j >= g->size) {
    fprintf(stderr,
            "ERROR -> tried to insert a cell out of bounds (%d, %d) in a grid "
            "of size %d\n",
            i, j, g->size);
    exit(EXIT_FAILURE);
  }

  g->grid[i][j] = v;
}

bool is_row_empty(int i, t_grid *g) {
  for (int j = 0; j < g->size; j++) {
    if (get_cell(i, j, g) != '_') {
      return false;
    }
  }
  return true;
}

bool is_col_empty(int j, t_grid *g) {
  for (int i = 0; i < g->size; i++) {
    if (get_cell(i, j, g) != '_') {
      return false;
    }
  }
  return true;
}

bool is_row_full(int i, t_grid *g) {
  for (int j = 0; j < g->size; j++) {
    if (get_cell(i, j, g) == '_') {
      return false;
    }
  }
  return true;
}

bool is_col_full(int j, t_grid *g) {
  for (int i = 0; i < g->size; i++) {
    if (get_cell(i, j, g) == '_') {
      return false;
    }
  }
  return true;
}

// a.no identical lines / columns (only check full lines / columns)
// b.no more than three consecutive zeros and ones in rows and columns.
bool is_consistent(t_grid *g) {
  if (sw.verbose) {
    printf("Checking consistency...\n");
  }

  // check for identical rows
  for (int i = 0; i < g->size; i++) {
    if (!is_row_full(i, g)) {
      continue;
    }
    for (int j = i + 1; j < g->size; j++) {
      bool identical = true;
      for (int k = 0; k < g->size; k++) {
        if (get_cell(i, k, g) != get_cell(j, k, g)) {
          identical = false;
          break;
        }
      }
      if (identical) {
        if (sw.verbose) {
          printf("Grid is not consistent : rows %d and %d are identical\n", i,
                 j);
        }
        return false;
      }
    }
  }

  // check for identical columns
  for (int j = 0; j < g->size; j++) {
    if (!is_col_full(j, g)) {
      continue;
    }
    for (int i = j + 1; i < g->size; i++) {
      bool identical = true;
      for (int k = 0; k < g->size; k++) {
        if (get_cell(k, i, g) != get_cell(k, j, g)) {
          identical = false;
          break;
        }
      }
      if (identical) {
        if (sw.verbose) {
          printf("Grid is not consistent : columns %d and %d are identical\n",
                 i, j);
        }
        return false;
      }
    }
  }

  // check for more than three consecutive zeros and ones in rows and columns
  for (int i = 0; i < g->size; i++) {
    int zeros = 0;
    int ones = 0;
    for (int j = 0; j < g->size; j++) {
      if (get_cell(i, j, g) == '0') {
        zeros++;
        ones = 0;
      } else if (get_cell(i, j, g) == '1') {
        ones++;
        zeros = 0;
      } else {
        zeros = 0;
        ones = 0;
      }
      if (zeros > 2 || ones > 2) {
        if (sw.verbose) {
          printf(
              "Grid is not consistent : more than three consecutive zeros "
              "and ones in rows\n");
        }
        return false;
      }
    }
  }

  for (int j = 0; j < g->size; j++) {
    int zeros = 0;
    int ones = 0;
    for (int i = 0; i < g->size; i++) {
      if (get_cell(i, j, g) == '0') {
        zeros++;
        ones = 0;
      } else if (get_cell(i, j, g) == '1') {
        ones++;
        zeros = 0;
      } else {
        zeros = 0;
        ones = 0;
      }
      if (zeros > 2 || ones > 2) {
        if (sw.verbose) {
          printf(
              "Grid is not consistent : more than three consecutive zeros "
              "and ones in columns\n");
        }
        return false;
      }
    }
  }

  if (sw.verbose) {
    printf("Grid is consistent\n");
  }
  return true;
}

bool is_grid_full(t_grid *g) {
  for (int i = 0; i < g->size; i++) {
    for (int j = 0; j < g->size; j++) {
      if (get_cell(i, j, g) == '_') {
        return false;
      }
    }
  }
  return true;
}

// returns true if a grid is full (no empty cells) and meets all the
// constraints of the Takuzu
bool is_valid(t_grid *g) {
  if (sw.verbose) {
    printf("Checking validity...\n");
  }
  return is_consistent(g) && is_grid_full(g);
}

void grid_choice_apply(t_grid *grid, const choice_t choice) {
  if (sw.verbose) {
    printf("Choice => (%d,%d)=%c\n", choice.row, choice.column, choice.choice);
  }
  set_cell(choice.row, choice.column, grid, choice.choice);
}

void grid_choice_remove(t_grid *grid, const choice_t choice) {
  set_cell(choice.row, choice.column, grid, '_');
}

void grid_choice_print(const choice_t choice, FILE *fd) {
  fprintf(fd, "Choice : (%d, %d) = %c\n", choice.row, choice.column,
          choice.choice);
}

// returns a random choice from a grid, the choice is always valid (the cell is
// empty) otherwise, we error out
choice_t grid_choice(t_grid *grid) {
  if (is_grid_full(grid)) {
    fprintf(stderr, "ERROR -> tried to get a choice from a full grid\n");
    exit(EXIT_FAILURE);
  }
  choice_t choice;
  choice.row = rand() % grid->size;
  choice.column = rand() % grid->size;
  choice.choice = rand() % 2 == 0 ? '0' : '1';
  if (get_cell(choice.row, choice.column, grid) != '_') {
    return grid_choice(grid);
  }
  return choice;
}

void add_solution(t_grid *grid, t_grid **solutions, int *nb_solutions) {
  if (sw.verbose) {
    printf("Adding solution...\n");
  }
  solutions[*nb_solutions] = malloc(sizeof(t_grid));
  grid_allocate(solutions[*nb_solutions], grid->size);
  grid_copy(grid, solutions[*nb_solutions]);
  (*nb_solutions)++;
}

void list_init(list_t *l) { l->head = NULL; }

void list_add(list_t *l, choice_t choice) {
  node_t *new_node = malloc(sizeof(node_t));
  new_node->choice = choice;
  new_node->next = l->head;
  l->head = new_node;
}

void list_remove(list_t *l) {
  if (l->head == NULL) {
    return;
  }
  node_t *tmp = l->head;
  l->head = l->head->next;
  free(tmp);
}

void list_print(list_t *l, FILE *fd) {
  node_t *tmp = l->head;
  while (tmp != NULL) {
    grid_choice_print(tmp->choice, fd);
    tmp = tmp->next;
  }
}

// Solve a grid using backtracking, and stack of choices
// Solve a grid using backtracking, and stack of choices
t_grid *grid_solver(t_grid *grid, const t_mode mode) {
  if (sw.verbose) {
    printf("Solving grid...\n");
  }

  t_grid *solution = malloc(sizeof(t_grid));
  grid_copy(grid, solution);

  // We don't do this on grid because this leads to segfault in main
  // when we try to solution and grid that both point to the same grid
  if (is_valid(solution)) {
    if (sw.verbose) {
      printf("Grid is already solved\n");
    }
    return solution;
  }

  t_grid **solutions = malloc(sizeof(t_grid *));
  int nb_solutions = 0;

  sub_grid_solver(grid, solution, solutions, &nb_solutions);

  if (nb_solutions == 0) {
    if (sw.verbose) {
      printf("No solution found\n");
    }
    free(solutions);
    return NULL;
  }

  if (mode == MODE_FIRST) {
    if (sw.verbose) {
      printf("Returning first solution\n");
    }
    t_grid *first_solution = solutions[0];
    free(solution);
    free(solutions);
    return first_solution;
  }

  free(solution);
  free(solutions);
  return NULL;
}

// Recursive function to solve a grid
void sub_grid_solver(t_grid *grid, t_grid *solution, t_grid **solutions,
                     int *nb_solutions) {
  if (!is_consistent(grid) || is_grid_full(grid) || *nb_solutions > 1) {
    return;
  }

  if (is_valid(grid)) {
    add_solution(grid, solutions, nb_solutions);
    return;
  }

  apply_heuristics(grid);

  if (is_valid(grid)) {
    add_solution(grid, solutions, nb_solutions);
    return;
  }

  if (is_grid_full(grid)) {
    return;
  }

  t_grid grid_1, grid_2;
  choice_t choice = grid_choice(grid);

  grid_allocate(&grid_1, grid->size);
  grid_copy(grid, &grid_1);
  choice.choice = '0';
  grid_choice_apply(&grid_1, choice);
  sub_grid_solver(&grid_1, solution, solutions, nb_solutions);
  grid_free(&grid_1);

  grid_allocate(&grid_2, grid->size);
  grid_copy(grid, &grid_2);
  choice.choice = '1';
  grid_choice_apply(&grid_2, choice);
  sub_grid_solver(&grid_2, solution, solutions, nb_solutions);
  grid_free(&grid_2);
}

// Heuristic 1 : If a row (respectively column) has two consecutive
// zeroes, the cells before/after must be ones. The same heuristics
// applies to ones as well.
bool apply_heuristic1(t_grid *g) {
  if (sw.verbose) {
    printf("Applying heuristic 1...\n");
  }

  bool changed = false;
  changed = sub_heuristic1_rows(g) || changed;
  changed = sub_heuristic1_cols(g) || changed;

  if (sw.verbose && changed) {
    printf("Heuristic 1 has modified the grid\n");
  }
  return changed;
}

bool sub_heuristic1_rows(t_grid *g) {
  bool changed = false;
  for (int i = 0; i < g->size; i++) {
    for (int j = 0; j < g->size - 2; j++) {
      // if two consecutive zeros
      if (get_cell(i, j, g) == '0' && get_cell(i, j + 1, g) == '0') {
        // if the cell after is empty, we fill it with a one
        if (get_cell(i, j + 2, g) == '_') {
          if (sw.verbose) {
            printf("Cell (%d, %d) => 1\n", i, j + 2);
          }
          set_cell(i, j + 2, g, '1');
          changed = true;
        }  // if the cell before is empty, we fill it with a one
        else if (j > 0 && get_cell(i, j - 1, g) == '_') {
          if (sw.verbose) {
            printf("Cell (%d, %d) => 1\n", i, j - 1);
          }
          set_cell(i, j - 1, g, '1');
          changed = true;
        }
      }
      // same thing for ones
      else if (get_cell(i, j, g) == '1' && get_cell(i, j + 1, g) == '1') {
        // if the cell after is empty, we fill it with a zero
        if (get_cell(i, j + 2, g) == '_') {
          if (sw.verbose) {
            printf("Cell (%d, %d) => 0\n", i, j + 2);
          }
          set_cell(i, j + 2, g, '0');
          changed = true;
        }  // if the cell before is empty, we fill it with a zero
        else if (j > 0 && get_cell(i, j - 1, g) == '_') {
          if (sw.verbose) {
            printf("Cell (%d, %d) => 0\n", i, j - 1);
          }
          set_cell(i, j - 1, g, '0');
          changed = true;
        }
      }
    }
  }
  return changed;
}

bool sub_heuristic1_cols(t_grid *g) {
  bool changed = false;
  for (int j = 0; j < g->size; j++) {
    for (int i = 0; i < g->size - 2; i++) {
      // if two consecutive zeros
      if (get_cell(i, j, g) == '0' && get_cell(i + 1, j, g) == '0') {
        // if the cell after is empty, we fill it with a one
        if (get_cell(i + 2, j, g) == '_') {
          if (sw.verbose) {
            printf("Cell (%d, %d) => 1\n", i + 2, j);
          }
          set_cell(i + 2, j, g, '1');
          changed = true;
        }  // if the cell before is empty, we fill it with a one
        else if (i > 0 && get_cell(i - 1, j, g) == '_') {
          if (sw.verbose) {
            printf("Cell (%d, %d) => 1\n", i - 1, j);
          }
          set_cell(i - 1, j, g, '1');
          changed = true;
        }
      }
      // same thing for ones
      else if (get_cell(i, j, g) == '1' && get_cell(i + 1, j, g) == '1') {
        // if the cell after is empty, we fill it with a zero
        if (get_cell(i + 2, j, g) == '_') {
          if (sw.verbose) {
            printf("Cell (%d, %d) => 0\n", i + 2, j);
          }
          set_cell(i + 2, j, g, '0');
          changed = true;
        }  // if the cell before is empty, we fill it with a zero
        else if (i > 0 && get_cell(i - 1, j, g) == '_') {
          if (sw.verbose) {
            printf("Cell (%d, %d) => 0\n", i - 1, j);
          }
          set_cell(i - 1, j, g, '0');
          changed = true;
        }
      }
    }
  }
  return changed;
}

// Heuristic 2 : If a row (respectively column) has all its zeros
// filled, the remaining empty cells are ones. The same heuristics
// applies to ones
bool apply_heuristic2(t_grid *g) {
  if (sw.verbose) {
    printf("Applying heuristic 2...\n");
  }
  bool changed = false;

  changed = sub_heuristic2_rows(g) || changed;
  changed = sub_heuristic2_cols(g) || changed;

  if (sw.verbose && changed) {
    printf("Heuristic 2 applied\n");
  }
  return changed;
}
// If a row(respectively column) has all its zeros filled,
//     the remaining empty cells are ones.The same
//     heuristics applies to ones.
bool sub_heuristic2_rows(t_grid *g) {
  bool changed = false;
  for (int i = 0; i < g->size; i++) {
    int zeros = 0;
    int ones = 0;
    for (int j = 0; j < g->size; j++) {
      if (get_cell(i, j, g) == '0') {
        zeros++;
      } else if (get_cell(i, j, g) == '1') {
        ones++;
      }
    }
    if (zeros == g->size / 2) {
      for (int j = 0; j < g->size; j++) {
        if (get_cell(i, j, g) == '_') {
          if (sw.verbose) {
            printf("Cell (%d, %d) => 1\n", i, j);
          }
          set_cell(i, j, g, '1');
          changed = true;
        }
      }
    } else if (ones == g->size / 2) {
      for (int j = 0; j < g->size; j++) {
        if (get_cell(i, j, g) == '_') {
          if (sw.verbose) {
            printf("Cell (%d, %d) => 0\n", i, j);
          }
          set_cell(i, j, g, '0');
          changed = true;
        }
      }
    }
  }
  return changed;
}

bool sub_heuristic2_cols(t_grid *g) {
  bool changed = false;
  for (int j = 0; j < g->size; j++) {
    int zeros = 0;
    int ones = 0;
    for (int i = 0; i < g->size; i++) {
      if (get_cell(i, j, g) == '0') {
        zeros++;
      } else if (get_cell(i, j, g) == '1') {
        ones++;
      }
    }
    if (zeros == g->size / 2) {
      for (int i = 0; i < g->size; i++) {
        if (get_cell(i, j, g) == '_') {
          if (sw.verbose) {
            printf("Cell (%d, %d) => 1\n", i, j);
          }
          set_cell(i, j, g, '1');
          changed = true;
        }
      }
    } else if (ones == g->size / 2) {
      for (int i = 0; i < g->size; i++) {
        if (get_cell(i, j, g) == '_') {
          if (sw.verbose) {
            printf("Cell (%d, %d) => 0\n", i, j);
          }
          set_cell(i, j, g, '0');
          changed = true;
        }
      }
    }
  }
  return changed;
}

void apply_heuristics(t_grid *g) {
  if (sw.verbose) {
    printf("Applying heuristics...\n");
  }
  // apply heuristics until guess are exhausted

  while (apply_heuristic1(g) || apply_heuristic2(g)) {
    if (sw.verbose) {
      printf("New grid :\n");
      grid_print(g, stdout);
    }
  }
}

void generate_grid(t_grid *g, int percentage_fill) {
  if (sw.verbose) {
    printf("Generating grid of size %d\n", g->size);
  }

  grid_allocate(g, sw.grid_size);
  // get the number of cells to fill from percentage
  int cells_fill = (((g->size * g->size) * percentage_fill) / 100);
  // fill the grid with n 0 and 1 at random
  while (cells_fill > 0) {
    int i = rand() % g->size;
    int j = rand() % g->size;
    // Rand was inserted in the ifs but behaved weirdly, it's doing fine
    // here
    int random = rand() % 2;
    // If a cell is already filled, we retry somewhere else
    if (get_cell(i, j, g) == '_') {
      if (random == 0) {
        set_cell(i, j, g, '0');
      } else if (random % 2 == 1) {
        set_cell(i, j, g, '1');
      }
      cells_fill--;
    }
  }

  if (!is_consistent(g)) {
    if (sw.verbose) {
      printf("Grid is not consistent, retrying...\n");
    }
    grid_free(g);
    generate_grid(g, percentage_fill);
  }
}