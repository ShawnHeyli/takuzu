#include "takuzu.h"

#include <errno.h>
#include <getopt.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "grid.h"

/* ---------------------------------------------------------------------------------------------
 */
// --------------------------------------- MAIN FUNCTION
// --------------------------------------- //
/* ---------------------------------------------------------------------------------------------
 */

int main(int argc, char *argv[]) {
  int opt;
  software_infos sw_if;

  init_software_infos(&sw_if);

  char error_buffer[ERROR_BUFFER_SIZE];

  static struct option long_opts[] = {{"all", no_argument, 0, 'a'},
                                      {"generate", required_argument, 0, 'g'},
                                      {"ouput", required_argument, 0, 'o'},
                                      {"unique", no_argument, 0, 'u'},
                                      {"verbose", no_argument, 0, 'v'},
                                      {"help", no_argument, 0, 'h'},
                                      {0, 0, 0, 0}};

  while ((opt = getopt_long(argc, argv, "ag:o:uvh", long_opts, NULL)) != -1) {
    switch (opt) {
      case 'a':
        if (sw_if.mode == GENERATOR) {
          fprintf(
              stderr,
              "ERROR -> unauthorized -a, --all option in GENERATOR mode!\n");
          exit(EXIT_FAILURE);
        }
        sw_if.all_sol = true;

        // Take file as input
        if (!argv[optind]) {
          fprintf(stderr, "ERROR -> no input file to solve!\n");
          exit(EXIT_FAILURE);
        }
        t_grid g;
        file_parser(&g, argv[optind]);
        print_grid(&g, stdout);
        printf("consistent : %d\n", is_consistent(&g));
        // printf("valid : %d\n", is_valid(&g));
        grid_free(&g);
        break;

      case 'u':
        if (sw_if.mode == SOLVER) {
          fprintf(
              stderr,
              "ERROR -> unauthorized -u, --unique option in SOLVER mode!\n");
          exit(EXIT_FAILURE);
        }
        sw_if.uniq_sol_grid = true;
        break;

      case 'g':  // Option for generate grid of optarg*optarg size
        if (sw_if.all_sol) {
          fprintf(
              stderr,
              "ERROR -> unauthorized -a, --all option in GENERATOR mode!\n");
          exit(EXIT_FAILURE);
        }

        // Converting optarg (string) to long integer
        long gird_size = strtol(optarg, NULL, 10);
        if (gird_size == LONG_MAX || gird_size == LONG_MIN) {
          snprintf(error_buffer, ERROR_BUFFER_SIZE, "ERROR -> '%s' ", optarg);
          perror(error_buffer);
          exit(EXIT_FAILURE);
        }

        // Check if given number is valid
        sw_if.grid_size_tg = MIN_GRID_SIZE;
        bool valid_gird_size = false;
        while (sw_if.grid_size_tg < MAX_GRID_SIZE + 1 &&
               gird_size >= sw_if.grid_size_tg) {
          if (gird_size == sw_if.grid_size_tg) {
            valid_gird_size = true;
            break;
          }
          sw_if.grid_size_tg <<= 1;
        }
        if (!valid_gird_size) {
          fprintf(stderr,
                  "ERROR -> gird size not accepted!"
                  "Authorized : [4; 8; 16; 32; 64]\n");
          exit(EXIT_FAILURE);
        }

        // TESTS SUCCEDED :: validate generator mode
        sw_if.mode = GENERATOR;
        break;

      case 'o':
        sw_if.out_s = fopen(optarg, "r");
        if (sw_if.out_s == NULL) {
          snprintf(error_buffer, ERROR_BUFFER_SIZE, "ERROR -> '%s' ", optarg);
          perror(error_buffer);
          exit(EXIT_FAILURE);
        }
        break;

      case 'v':
        sw_if.verbose = true;
        break;

      case 'h':
        usage(argv[0]);
    }
  }

  if (sw_if.mode == SOLVER) {
    if (!argv[optind]) {
      fprintf(stderr, "ERROR -> no input file to solve!\n");
      exit(EXIT_FAILURE);
    }

    sw_if.in_grid_s = fopen(argv[optind], "r");
    if (!sw_if.in_grid_s) {
      snprintf(error_buffer, ERROR_BUFFER_SIZE, "ERROR -> '%s' ", argv[optind]);
      perror(error_buffer);
      exit(EXIT_FAILURE);
    }
  }

  // CLOSING OPEN STREAMS

  if (sw_if.out_s != stdout) {
    fclose(sw_if.out_s);
  }

  if (sw_if.mode == SOLVER) {
    fclose(sw_if.in_grid_s);
  }

  return EXIT_SUCCESS;
}

/* ---------------------------------------------------------------------------------------------
 */
// ------------------------------- BODY OF MAIN UTIL FUNCTIONS
// -------------------------------- //
/* ---------------------------------------------------------------------------------------------
 */

static inline void usage(char *prg_name) {
  printf("Usage :%*c%s [-a|-o FILE|-v|-h] FILE...\n", 10, ' ', prg_name);
  printf("%*c%s-g[SIZE] [-u|-o FILE|-v|-h]\n\n", 17, ' ', prg_name);
  printf(
      "Solve or generate takuzu grids of size: 4, 8 16, 32, 64\n\n\
	  -a, --all search for all possible solutions\n\
	  -g[N], --generate[=N] generate a grid of size NxN (default:8)\n\
	  -o FILE, --output FILE write output to FILE\n\
	  -u, --unique generate a grid with unique solution\n\
	  -v, --verbose verbose output\n\
	  -h, --help display this help and exit\n");
  fflush(stdout);
  exit(EXIT_SUCCESS);
}

static inline void init_software_infos(software_infos *si) {
  si->mode = SOLVER;

  si->grid_size_tg = 0;
  si->uniq_sol_grid = false;

  si->all_sol = false;
  si->in_grid_s = NULL;

  si->out_s = stdout;
  si->verbose = false;
}

// size : number of elements in the whole grid
void grid_allocate(t_grid *g, int size) {
  g->size = size;
  g->grid = malloc(g->size * sizeof(char *));
  for (int i = 0; i < g->size; i++) {
    g->grid[i] = malloc(g->size * sizeof(char));
  }

  // Initialize grid with '_'
  for (int i = 0; i < g->size; i++) {
    for (int j = 0; j < g->size; j++) {
      g->grid[i][j] = '_';
    }
  }
}

void grid_free(t_grid *g) {
  puts("grid_free");
  for (int i = 0; i < g->size; i++) {
    free(g->grid[i]);
  }
  free(g->grid);
}

// Example of valid grid:
// 0 _ _ _ _ _
// 0 1 1 _ _ _
// _ _ _ _ _ _
// _ _ 0 _ _ _
// 1 _ _ _ _ _
// _ 0 _ _ _ _
void print_grid(t_grid *g, FILE *fd) {
  puts("print_grid");
  for (int i = 0; i < g->size; i++) {
    for (int j = 0; j < g->size; j++) {
      fprintf(fd, "%c ", g->grid[i][j]);
    }
    fputc('\n', fd);
  }
}

bool check_char(const char c) {
  if (c != '0' && c != '1' && c != '_') {
    return false;
  }
  return true;
}

void file_parser(t_grid *grid, char *filename) {
  puts("file_parser");
  int allowed_sizes[5] = {4, 8, 16, 32, 64};

  FILE *fd = fopen(filename, "r");
  if (fd == NULL) {
    switch (errno) {
      case ENOENT:
        fprintf(stderr, "ERROR -> file '%s' not found!\n", filename);
        break;
      case EACCES:
        fprintf(stderr, "ERROR -> file '%s' not accessible!\n", filename);
        break;
      default:
        fprintf(stderr, "ERROR -> Unknow error with '%s'!\n", filename);
        exit(EXIT_FAILURE);
    }
  }

  char read;
  char line[MAX_GRID_SIZE];
  int lineSize = 0;

  // Read first line & skip comment lines along the way
  while ((read = fgetc(fd)) != '\n') {
    switch (read) {
      // Comment line
      case '#':
        while ((read = fgetc(fd)) != '\n') {
          // ignore and skip
        }
        break;

      case EOF:
        fprintf(stderr, "ERROR -> empty file!\n");
        exit(EXIT_FAILURE);
        break;

      // Start of the first line
      default:
        if (check_char(read)) {
          line[lineSize] = read;
          lineSize++;
        } else if (read == ' ' || read == '\t') {
          // ignore and skip
        } else {
          fprintf(stderr, "ERROR -> invalid character!\n");
          exit(EXIT_FAILURE);
        }
        break;
    }
  }

  // Check first line size is in allowed_sizes
  bool valid_size = false;
  for (int i = 0; i < 5; i++) {
    if (lineSize == allowed_sizes[i]) {
      valid_size = true;
      break;
    }
  }
  if (valid_size == false) {
    fprintf(stderr,
            "ERROR -> Invalid size, %d is not in the valid sizes %d, %d, %d, "
            "%d, %d!\n",
            lineSize, 4, 8, 16, 32, 64);
  }

  // At this point lineSize is equal to the length of the first line (and be a
  // valid size too)
  grid_allocate(grid, lineSize);

  for (int i = 0; i < lineSize; i++) {
    grid->grid[0][i] = line[i];
  }

  int currentRow = 1;  // 1 because we already read the first line
  int currentColumn = 0;

  while ((read = fgetc(fd)) != EOF) {
    if (read == '\n') {
      // not enough chars
      if (currentColumn != lineSize) {
        fprintf(stderr, "ERROR -> inconsistent number of char by row!\n");
        exit(EXIT_FAILURE);
      }
      currentRow++;
      currentColumn = 0;
    } else {
      // too much chars
      if (currentColumn == lineSize) {
        fprintf(stderr, "ERROR -> inconsistent number of char by row!\n");
        exit(EXIT_FAILURE);
      }
      if (check_char(read)) {
        grid->grid[currentRow][currentColumn] = read;
        currentColumn++;
      } else if (read == ' ' || read == '\t') {
        // ignore and skip
      } else {
        fprintf(stderr, "ERROR -> invalid character!\n");
        exit(EXIT_FAILURE);
      }
    }
  }
  fclose(fd);
}
