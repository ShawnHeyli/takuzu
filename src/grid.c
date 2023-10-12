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

void set_cell(int i, int j, t_grid *g, char v) {
  if (check_char(v)) {
    fprintf(stderr,
            "Error: tried to insert an invalid character in a grid %c\n", v);
    exit(EXIT_FAILURE);
  }
  g->grid[i][j] = v;
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
  if (strstr(line, "00") || strstr(line, "11")) {
    free(line);
    return false;
  }
  free(line);
  return true;
}

bool is_col_consistent(int col, t_grid *g) {
  char *line = get_col(col, g);
  if (strstr(line, "00") || strstr(line, "11")) {
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
//    and true otherwise.The signature will be as follows :
bool is_consistent(t_grid *g) {
  for (int i = 0; i < g->size; i++) {
    if (!is_row_consistent(i, g) || !is_col_consistent(i, g)) {
      return false;
    }
  }

  // check for identical rows
  for (int i = 0; i < g->size; i++) {
    char *row = get_row(i, g);
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

bool is_full(t_grid *g) {
  for (int i = 0; i < g->size; i++) {
    if (strchr(get_row(i, g), '_') || strchr(get_col(i, g), '_')) {
      return false;
    }
  }
  return true;
}

// returns true if a grid is full (no empty cells) and meets all the
// constraints of the Takuzu
bool is_valid(t_grid *g) { return is_consistent(g) && is_full(g); }