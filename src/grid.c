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

char *get_row(int row_index, t_grid *g) {
  char *row = malloc(g->size * sizeof(char));
  for (int j = 0; j < g->size; j++) {
    row[j] = g->grid[row_index][j];
  }
  return row;
}

char *get_col(int col_index, t_grid *g) {
  char *col = malloc(g->size * sizeof(char));
  for (int i = 0; i < g->size; i++) {
    col[i] = g->grid[i][col_index];
  }
  return col;
}

bool is_row_consistent(int row_index, t_grid *g) {
  char *row = get_row(row_index, g);
  if (strstr(row, "000") || strstr(row, "111")) {
    free(row);
    return false;
  }
  free(row);
  return true;
}

bool is_col_consistent(int col_index, t_grid *g) {
  char *col = get_col(col_index, g);
  if (strstr(col, "000") || strstr(col, "111")) {
    free(col);
    return false;
  }
  free(col);
  return true;
}

bool is_consistent(t_grid *g) {
  for (int i = 0; i < g->size; i++) {
    if (!is_row_consistent(i, g) || !is_col_consistent(i, g)) {
      return false;
    }
  }

  // check for identical rows
  // slip lines full of _ because they are empty and therefore not identical
  for (int i = 0; i < g->size; i++) {
    char *row = get_row(i, g);
    if (is_row_empty(i, g)) {
      free(row);
      continue;
    }
    for (int j = i + 1; j < g->size; j++) {
      char *row2 = get_row(j, g);
      if (strcmp(row, row2) == 0) {
        free(row);
        free(row2);
        return false;
      }
      free(row2);
    }
    free(row);
  }

  // check for identical cols
  for (int i = 0; i < g->size; i++) {
    char *col = get_col(i, g);
    if (is_col_empty(i, g)) {
      free(col);
      continue;
    }
    for (int j = i + 1; j < g->size; j++) {
      char *col2 = get_col(j, g);
      if (strcmp(col, col2) == 0) {
        free(col);
        free(col2);
        return false;
      }
      free(col2);
    }
    free(col);
  }

  return true;
}

bool is_row_empty(int row_index, t_grid *g) {
  char *row = get_row(row_index, g);
  if (strstr(row, "0") || strstr(row, "1")) {
    free(row);
    return false;
  }
  free(row);
  return true;
}

bool is_col_empty(int col_index, t_grid *g) {
  char *col = get_col(col_index, g);
  if (strstr(col, "0") || strstr(col, "1")) {
    free(col);
    return false;
  }
  free(col);
  return true;
}

bool is_grid_full(t_grid *g) {
  for (int i = 0; i < g->size; i++) {
    char *row = get_row(i, g);
    char *col = get_col(i, g);
    if (strstr(row, "_") || strstr(col, "_")) {
      free(row);
      free(col);
      return false;
    }
    free(row);
    free(col);
  }
  return true;
}

// returns true if a grid is full (no empty cells) and meets all the
// constraints of the Takuzu
bool is_valid(t_grid *g) { return is_consistent(g) && is_grid_full(g); }

// Heuristic 1 : If a row (respectively column) has two consecutive zeroes, the
// cells before/after must be ones. The same heuristics applies to ones as well.
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

// Heuristic 2 : If a row (respectively column) has all its zeros filled, the
// remaining empty cells are ones. The same heuristics applies to ones
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
  if (is_consistent(g)) {
    if (sw.verbose) {
      printf("Grid is consistent, applying heuristics...\n");
    }
    while (apply_heuristic1(g) || apply_heuristic2(g)) {
      if (sw.verbose) {
        printf("New grid :\n");
        grid_print(g, stdout);
      }
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
    // Rand was inserted in the ifs but behaved weirdly, it's doing fine here
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