#ifndef TAKUZU_H
#define TAKUZU_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define ERROR_BUFFER_SIZE 128
#define MIN_GRID_SIZE 4
#define MAX_GRID_SIZE 64

typedef enum { SOLVER, GENERATOR } modes;

// Comment lines: all characters in a line that follow a ‘#’ are ignored until
// ‘\n’ or EO
// A row has an even number of significant characters (from 4 to 64) and end
// with an ‘\n’ or EOF
// Consistent number of char by row and column Characters ‘0’, ‘1’
// and ‘_’ denote respectively a cell filled with a 0, a cell filled-in with a 1
// and an empty cell Supported separators are '\t' and ' '
typedef struct {
  int size;     // Number of elements in a row
  char **grid;  // Pointer to the grid
} t_grid;

typedef struct {
  modes mode;         // solver or generator
  FILE *output_file;  // output file

  t_grid *grid;   // grid to solve
  int grid_size;  // size of the grid

  bool all;      // all solutions
  bool unique;   // unique solution
  bool verbose;  // verbose output

} software_info;

void usage();
void parse_args(int argc, char **argv);

void grid_allocate(t_grid *g, int size);
void grid_free(const t_grid *g);
void init_software_infos(software_info *si);
void grid_print(const t_grid *g, FILE *fd);
void file_parser(t_grid *grid, char *filename);
bool check_char(const char c);

#endif /* TAKUZU_H */
