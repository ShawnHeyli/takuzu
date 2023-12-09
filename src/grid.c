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

char get_cell(int i, int j, t_grid *g) { return g->grid[i][j]; }

bool set_cell(int i, int j, t_grid *g, char v) {
  // Invalid character
  if (check_char(v)) {
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

  if (get_cell(i, j, g) == v) {
    return false;
  } else {
    g->grid[i][j] = v;
    return true;
  }
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