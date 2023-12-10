#ifndef TAKUZU_H
#define TAKUZU_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define MIN_GRID_SIZE 4
#define MAX_GRID_SIZE 64

// NONE is the default mode to better handle incompatible options in parse_args
typedef enum { NONE, SOLVER, GENERATOR } modes;

typedef struct {
  int size;     // Number of elements in a row
  char **grid;  // Pointer to the grid
} t_grid;

typedef struct {
  modes mode;         // solver or generator
  FILE *output_file;  // output file

  t_grid *grid;         // grid to solve
  t_grid *solution;     // solution grid
  int grid_size;        // size of the grid
  int percentage_fill;  // percentage of the grid to fill (GENERATOR mode)

  bool all;      // all solutions
  bool unique;   // unique solution
  bool verbose;  // verbose output

} software_info;

extern software_info sw;

void usage();
void parse_args(int argc, char **argv);

void grid_allocate(t_grid *g, int size);
void grid_free(const t_grid *g);
void grid_print(const t_grid *g, FILE *fd);
void file_parser(t_grid *grid, char *filename);
bool check_char(char c);

#endif /* TAKUZU_H */
