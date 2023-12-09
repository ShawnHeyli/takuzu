#include "takuzu.h"

#include <err.h>
#include <errno.h>
#include <getopt.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

software_info sw;

int main(int argc, char *argv[]) {
  init_software_infos(&sw);
  t_grid grid;
  sw.grid = &grid;

  parse_args(argc, argv);

  if (sw.mode == SOLVER) {
    if (sw.verbose) {
      printf("Solver mode detected\n");
    }
    if (argv[optind] == NULL) {
      errx(EXIT_FAILURE, "no input file to solve!");
    }
    file_parser(sw.grid, argv[optind]);
  }

  if (sw.mode == GENERATOR) {
    if (sw.verbose) {
      printf("Generator mode detected\n");
      printf("Generating grid of size %d\n", sw.grid_size);
    }
    grid_allocate(sw.grid, sw.grid_size);
    // grid_generate(sw.grid, sw.grid_size, sw.unique);
  }

  grid_free(sw.grid);
}

// Helper function
int is_valid_size(int size) {
  if (size == 4 || size == 8 || size == 16 || size == 32 || size == 64) {
    return true;
  }
  return false;
}

// Allocate memory for a t_grid structure
// Will exit the program if the size is invalid
void grid_allocate(t_grid *g, int size) {
  if (is_valid_size(size) == false) {
    errx(EXIT_FAILURE, "invalid grid size for allocation !");
  }

  g->size = size;
  g->grid = calloc(g->size, sizeof(char *));
  for (int i = 0; i < g->size; i++) {
    g->grid[i] = calloc(g->size, sizeof(char));
  }

  // Initialize grid with '_'
  for (int i = 0; i < g->size; i++) {
    for (int j = 0; j < g->size; j++) {
      g->grid[i][j] = '_';
    }
  }
}

// Free memory allocated for a t_grid structure
void grid_free(const t_grid *g) {
  for (int i = 0; i < g->size; i++) {
    free(g->grid[i]);
  }
  free(g->grid);
}

// Prints a grid to sw.output_file
// (stdout if no -o option has been given)
void grid_print(const t_grid *g, FILE *fd) {
  for (int i = 0; i < g->size; i++) {
    // Ignore comment lines
    if (g->grid[i][0] == '#') {
      continue;
    }
    for (int j = 0; j < g->size; j++) {
      fprintf(fd, "%c", g->grid[i][j]);
    }
    fprintf(fd, "\n");
  }
}
// Check if a character is a valid takuzu grid character
bool check_char(const char c) {
  if (c == '0' || c == '1' || c == '_') {
    return true;
  }
  return false;
}

void init_software_infos(software_info *si) {
  si->mode = SOLVER;
  si->output_file = stdout;

  si->grid = NULL;
  si->grid_size = 0;

  si->all = false;
  si->unique = false;
  si->verbose = false;
}

// The parser must be able to read a grid of size 4 to 64 with only one scan of
// the file. The scanner has
// to be as robust as possible when a user provides an incorrect grid
//    file.A meaningful error message must be issued in these situations,
//    all dynamically allocated memory has to be freed and the program has to
//        stop and return EXIT_FAILURE
void file_parser(t_grid *grid, char *filename) {
  if (sw.verbose) {
    printf("Parsing file: %s\n", filename);
  }

  FILE *fd = fopen(filename, "r");
  if (fd == NULL) {
    switch (errno) {
      case ENOENT:
        fprintf(stderr, "ERROR -> file '%s' not found!\n", filename);
        exit(EXIT_FAILURE);
        break;
      case EACCES:
        fprintf(stderr, "ERROR -> file '%s' not accessible!\n", filename);
        exit(EXIT_FAILURE);
        break;
      default:
        fprintf(stderr, "ERROR -> Unknow error with '%s'!\n", filename);
        exit(EXIT_FAILURE);
    }
  }

  char read;
  char line[MAX_GRID_SIZE];
  int lineSize = 0;

  // Read the first line to get the grid size
  while ((read = fgetc(fd)) != '\n') {
    switch (read) {
      case '#':
        while ((read = fgetc(fd)) != '\n') {
          if (read == EOF) {
            fclose(fd);
            fprintf(stderr, "ERROR -> empty file!\n");
            exit(EXIT_FAILURE);
          }
        }
        break;

      case EOF:
        fclose(fd);
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
          fclose(fd);
          fprintf(stderr, "ERROR -> invalid character!\n");
          exit(EXIT_FAILURE);
        }
        break;
    }
  }

  if (is_valid_size(lineSize) == false) {
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
        grid_free(grid);
        fclose(fd);
        fprintf(stderr, "ERROR -> inconsistent number of char by row!\n");
        exit(EXIT_FAILURE);
      }
      currentRow++;
      currentColumn = 0;
    } else {
      // too much chars
      if (currentColumn == lineSize) {
        grid_free(grid);
        fclose(fd);
        fprintf(stderr, "ERROR -> inconsistent number of char by row!\n");
        exit(EXIT_FAILURE);
      }
      if (check_char(read)) {
        grid->grid[currentRow][currentColumn] = read;
        currentColumn++;
      } else if (read == ' ' || read == '\t') {
        // ignore and skip
      } else {
        grid_free(grid);
        fclose(fd);
        fprintf(stderr, "ERROR -> invalid character!\n");
        exit(EXIT_FAILURE);
      }
    }
  }
  fclose(fd);
}

void parse_args(int argc, char **argv) {
  static struct option parse_structure[] = {
      {"all", no_argument, 0, 'a'},
      {"generate", optional_argument, 0, 'g'},
      {"output", required_argument, 0, 'o'},
      {"unique", no_argument, 0, 'u'},
      {"verbose", no_argument, 0, 'v'},
      {"help", no_argument, 0, 'h'},
      {0, 0, 0, 0}};

  int opt;
  while ((opt = getopt_long(argc, argv, "ag:o:uvh", parse_structure, NULL)) !=
         -1) {
    switch (opt) {
      case 'a':
        if (sw.mode == GENERATOR) {
          errx(EXIT_FAILURE, "invalid option combination!");
        }

        sw.mode = SOLVER;
        sw.all = true;
        break;

      case 'g':
        if (sw.mode == SOLVER) {
          errx(EXIT_FAILURE, "invalid option combination!");
        }

        sw.mode = GENERATOR;
        if (optarg != NULL) {
          sw.grid_size = atoi(optarg);
          if (sw.grid_size < MIN_GRID_SIZE || sw.grid_size > MAX_GRID_SIZE) {
            errx(EXIT_FAILURE, "invalid grid size!");
          }
        } else {
          sw.grid_size = 8;
        }
        break;

      case 'o':
        // Check if file is accessible
        if (access(optarg, F_OK) != -1) {
          sw.output_file = fopen(optarg, "w");
        } else {
          errx(EXIT_FAILURE, "cannot open file!");
        }
        break;

      case 'u':
        if (sw.mode == SOLVER) {
          errx(EXIT_FAILURE, "invalid option combination!");
        }

        sw.mode = GENERATOR;
        sw.unique = true;
        break;

      case 'v':
        sw.verbose = true;
        break;

      case 'h':
        usage();
        exit(EXIT_SUCCESS);
        break;

      default:
        errx(EXIT_FAILURE, "invalid option!");
        break;
    }
  }
}

void usage() {
  printf("Usage: takuzu [-a|-o FILE|-v|-h] FILE\n");
  printf("       takuzu -g[SIZE] [-u|-o FILE|-v|-h]\n");
  printf("Solve or generate takuzu grids of size: 4, 8 16, 32, 64\n");
  printf("  -a, --all               search for all possible solutions\n");
  printf("  -g[N], --generate[=N]   generate a grid of size NxN (default:8)\n");
  printf("  -o FILE, --output FILE  write output to FILE\n");
  printf("  -u, --unique            generate a grid with unique solution\n");
  printf("  -v, --verbose           verbose output\n");
  printf("  -h, --help              display this help and exit\n");
}