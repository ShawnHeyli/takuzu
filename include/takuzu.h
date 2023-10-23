#ifndef TAKUZU_H
#define TAKUZU_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define ERROR_BUFFER_SIZE 128
#define MIN_GRID_SIZE 4
#define MAX_GRID_SIZE 64

/* ---------------------------------------------------------------------------------------------
 */
// ----------------------------------- STRUCTURES AND ENUMS
// ------------------------------------ //
/* ---------------------------------------------------------------------------------------------
 */

typedef enum { SOLVER, GENERATOR } modes;

typedef struct {
  modes mode;  // software mode

  uint8_t grid_size_tg;  // size of grid to generate
  bool uniq_sol_grid;    // generate grid with unique solution

  bool all_sol;     // search for all solutions
  FILE *in_grid_s;  // stream that contains grid to solve

  FILE *out_s;   // stream for print found solutions or grid generate
  bool verbose;  // verbose output

} software_infos;

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

/* ---------------------------------------------------------------------------------------------
 */
// ----------------------------- DECLARATION OF MAIN UTIL FUNCTIONS
// ---------------------------- //
/* ---------------------------------------------------------------------------------------------
 */

void usage();

void init_software_infos(software_infos *);

void grid_allocate(t_grid *g, int size);
void grid_free(t_grid *g);
void print_grid(t_grid *g, FILE *fd);
bool check_char(const char c);

void file_parser(t_grid *grid, char *filename);

#endif /* TAKUZU_H */
