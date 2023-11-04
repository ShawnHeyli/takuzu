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
void grid_copy(t_grid *src, t_grid *dst) {
  dst->size = src->size;
  dst->grid = malloc(dst->size * sizeof(char *));
  for (int i = 0; i < dst->size; i++) {
    dst->grid[i] = malloc(dst->size * sizeof(char));
  }

  for (int i = 0; i < dst->size; i++) {
    for (int j = 0; j < dst->size; j++) {
      dst->grid[i][j] = src->grid[i][j];
    }
  }
}

// Returns true if the sell is cell was set, false otherwise
bool set_cell(int i, int j, t_grid *g, char v) {
  if (check_char(v)) {
    fprintf(stderr,
            "Error: tried to insert an invalid character ('%c') in a grid \n",
            v);
    exit(EXIT_FAILURE);
  }
  if (get_cell(i, j, g) == v) {
    return false;
  } else {
    g->grid[i][j] = v;
    return true;
  }
}

// the returned char should already be valid (checked in set_cell and
// file_parser)
char get_cell(int i, int j, t_grid *g) { return g->grid[i][j]; }

char *get_row(int row, t_grid *g) {
  char *line = malloc(g->size * sizeof(char));
  for (int j = 0; j < g->size; j++) {
    line[j] = g->grid[row][j];
  }
  return line;
}

char *get_col(int col, t_grid *g) {
  char *line = malloc(g->size * sizeof(char));
  for (int i = 0; i < g->size; i++) {
    line[i] = g->grid[i][col];
  }
  return line;
}

bool is_row_consistent(int row, t_grid *g) {
  char *line = get_row(row, g);
  if (strstr(line, "000") || strstr(line, "111")) {
    free(line);
    return false;
  }
  free(line);
  return true;
}

bool is_col_consistent(int col, t_grid *g) {
  char *line = get_col(col, g);
  if (strstr(line, "000") || strstr(line, "111")) {
    free(line);
    return false;
  }
  free(line);
  return true;
}

// Implement a function that returns true if a grid is consistent, i.e.
// meets the constraints of the Takuzu game : a.no identical lines /
//    colums b.no more than two consecutive zeros and ones in rows and
//    columns
//        .This function should work even if the grid is not fully filled
//        .The function will return false if the grid is not consistent,
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
    if (strchr(get_row(i, g), '_') || strchr(get_col(i, g), '_')) {
      return false;
    }
  }
  return true;
}

// returns true if a grid is full (no empty cells) and meets all the
// constraints of the Takuzu
bool is_valid(t_grid *g) { return is_consistent(g) && is_grid_full(g); }

// Heuristic 1 : If a row (respectively column) has two consecutive zeroes, the
// cells before/after must be ones. The same heuristics applies to ones as well.
bool apply_heuristic1(t_grid *g) {
  bool changed = false;
  for (int i = 0; i < g->size; i++) {
    char *row = get_row(i, g);
    char *col = get_col(i, g);

    // The || is to not overwrite the value of changed
    changed = sub_heuristic1(g, row) || changed;
    changed = sub_heuristic1(g, col) || changed;
    // for (int j = 0; j < g->size - 2; j++) {
    //   // Check for consecutive zeros
    //   if (row[j] == '0' && row[j + 1] == '0') {
    //     // Change cell after the two consecutive zeros
    //     if (j > 0 && row[j - 1] == '_') {
    //       set_cell(i, j - 1, g, '1');
    //       changed = true;
    //     }
    //     // Change cell before the two consecutive zeros
    //     if (j < g->size - 3 && row[j + 2] == '_') {
    //       set_cell(i, j + 2, g, '1');
    //       changed = true;
    //     }
    //   }
    //   if (col[j] == '0' && col[j + 1] == '0') {
    //     if (j > 0 && col[j - 1] == '_') {
    //       set_cell(j - 1, i, g, '1');
    //       changed = true;
    //     }
    //     if (j < g->size - 3 && col[j + 2] == '_') {
    //       set_cell(j + 2, i, g, '1');
    //       changed = true;
    //     }
    //   }
    //   // Check for consecutive ones
    //   if (row[j] == '1' && row[j + 1] == '1') {
    //     // Change cell after the two consecutive ones
    //     if (j > 0 && row[j - 1] == '_') {
    //       set_cell(i, j - 1, g, '0');
    //       changed = true;
    //     }
    //     // Change cell before the two consecutive ones
    //     if (j < g->size - 3 && row[j + 2] == '_') {
    //       set_cell(i, j + 2, g, '0');
    //       changed = true;
    //     }
    //   }
    //   if (col[j] == '1' && col[j + 1] == '1') {
    //     if (j > 0 && col[j - 1] == '_') {
    //       set_cell(j - 1, i, g, '0');
    //       changed = true;
    //     }
    //     if (j < g->size - 3 && col[j + 2] == '_') {
    //       set_cell(j + 2, i, g, '0');
    //       changed = true;
    //     }
    //   }
    // }
    free(row);
    free(col);
  }
  return changed;
}

// heuristic 1 agnostic to row/column
bool sub_heuristic1(t_grid *g, char *line) {
  bool changed = false;
  for (int j = 0; j < g->size - 2; j++) {
    // Check for consecutive zeros
    if (line[j] == '0' && line[j + 1] == '0') {
      // Change cell after the two consecutive zeros
      if (j > 0 && line[j - 1] == '_') {
        line[j - 1] = '1';
        changed = true;
      }
      // Change cell before the two consecutive zeros
      if (j < g->size - 3 && line[j + 2] == '_') {
        line[j + 2] = '1';
        changed = true;
      }
    }
    // Check for consecutive ones
    if (line[j] == '1' && line[j + 1] == '1') {
      // Change cell after the two consecutive ones
      if (j > 0 && line[j - 1] == '_') {
        line[j - 1] = '0';
        changed = true;
      }
      // Change cell before the two consecutive ones
      if (j < g->size - 3 && line[j + 2] == '_') {
        line[j + 2] = '0';
        changed = true;
      }
    }
  }
  return changed;
}

// Heuristic 2 : If a row (respectively column) has all its zeros filled, the
// remaining empty cells are ones. The same heuristics applies to ones
bool apply_heuristic2(t_grid *g) {
  bool changed = false;
  for (int i = 0; i < g->size; i++) {
    char *row = get_row(i, g);
    char *col = get_col(i, g);

    changed = sub_heuristic2(g, row) || changed;
    changed = sub_heuristic2(g, col) || changed;

    free(row);
    free(col);
  }
  return changed;
}

// heuristic 2 agnostic to row/col
bool sub_heuristic2(t_grid *g, char *line) {
  int zeros = 0;
  int ones = 0;
  bool changed = false;
  for (int j = 0; j < g->size; j++) {
    if (line[j] == '0') {
      zeros++;
    } else if (line[j] == '1') {
      ones++;
    }
  }
  if (zeros == g->size / 2) {
    for (int j = 0; j < g->size; j++) {
      if (line[j] == '_') {
        line[j] = '1';
        changed = true;
      }
    }
  }
  if (ones == g->size / 2) {
    for (int j = 0; j < g->size; j++) {
      if (line[j] == '_') {
        line[j] = '0';
        changed = true;
      }
    }
  }
  return changed;
}

void apply_heuristics(t_grid *g) {
  bool changed = true;
  while (changed) {
    changed = apply_heuristic1(g) || apply_heuristic2(g);
  }
}

void generate_grid(t_grid *g, long number_cells_fill) {
  long original_number_cells_fill = number_cells_fill;
  grid_allocate(g, g->size);
  // fill the grid with n 0 and 1 at random
  while (number_cells_fill > 0) {
    int i = rand() % g->size;
    int j = rand() % g->size;
    if (get_cell(i, j, g) == '_') {
      // If a cell is already filled, we retry somewhere else
      if (rand() % 2 == 0) {
        if (set_cell(i, j, g, '0')) {
          number_cells_fill--;
        }
      } else if (rand() % 2 == 1) {
        if (set_cell(i, j, g, '1')) {
          number_cells_fill--;
        }
      }
    }
  }
  if (!is_consistent(g)) {
    puts("Generated grid is not consistent, generating another one...");
    grid_free(g);
    generate_grid(g, original_number_cells_fill);
  }
}