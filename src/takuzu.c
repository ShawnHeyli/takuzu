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

#include "grid.h"

software_info sw = {
    .mode = NONE,
    .output_file = NULL,  // Can't be initialized to stdout because it is not a
                          // constant, but it is initialized in main
                          // (which is called before any use)

    .grid = NULL,
    .grid_size = 0,
    .percentage_fill = 20,

    .all = false,
    .unique = false,
    .verbose = false,
};

t_mode mode = MODE_FIRST;

int main(int argc, char *argv[]) {
  sw.output_file = stdout;
  t_grid grid;
  sw.grid = &grid;

  srand(time(NULL));
  parse_args(argc, argv);

  if (sw.mode == SOLVER) {
    if (sw.verbose) {
      fprintf(sw.output_file, "Solver mode detected\n");
    }
    if (argv[optind] == NULL) {
      errx(EXIT_FAILURE, "no input file to solve!");
    }

    file_parser(sw.grid, argv[optind]);

    if (sw.verbose) {
      fprintf(sw.output_file, "Parsed grid:\n");
      grid_print(sw.grid, sw.output_file);
    }

    if (!grid_solver(sw.grid, mode)) {
      return EXIT_FAILURE;
    }
  } else if (sw.mode == GENERATOR) {
    if (sw.verbose) {
      fprintf(sw.output_file, "Generator mode detected\n");
    }

    grid_allocate(sw.grid, sw.grid_size);

    if (sw.unique) {
      fprintf(sw.output_file, "Unique mode detected\n");
      generate_unique_grid(sw.grid, sw.percentage_fill);
    } else {
      generate_grid(sw.grid, sw.percentage_fill);
    }

    fprintf(sw.output_file, "Generated grid:\n");
    grid_print(sw.grid, sw.output_file);
  }

  grid_free(sw.grid);
  return EXIT_SUCCESS;
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
void grid_free(t_grid *g) {
  if (g == NULL || g->grid == NULL) {
    return;
  }

  for (int i = 0; i < g->size; i++) {
    free(g->grid[i]);
  }
  free(g->grid);
}

// Prints a grid to sw.output_file
// (stdout if no -o option has been given)
void grid_print(const t_grid *g, FILE *fd) {
  for (int i = 0; i < g->size; i++) {
    for (int j = 0; j < g->size; j++) {
      fprintf(fd, "%c", g->grid[i][j]);
    }
    fprintf(fd, "\n");
  }
}
// Check if a character is a valid takuzu grid character
bool check_char(char c) {
  if (c == '0' || c == '1' || c == '_') {
    return true;
  }
  return false;
}

// The parser must be able to read a grid of size 4 to 64 with only one scan
// of the file. The scanner has to be as robust as possible when a user
// provides an incorrect grid
//    file.A meaningful error message must be issued in these situations,
//    all dynamically allocated memory has to be freed and the program has to
//        stop and return EXIT_FAILURE
void file_parser(t_grid *grid, char *filename) {
  if (sw.verbose) {
    fprintf(sw.output_file, "Parsing file: %s\n", filename);
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
      {"number", required_argument, 0, 'N'},
      {"unique", no_argument, 0, 'u'},
      {"verbose", no_argument, 0, 'v'},
      {"help", no_argument, 0, 'h'},
      {0, 0, 0, 0}};

  int opt;

  while ((opt = getopt_long(argc, argv, "ag:N:o:uvh", parse_structure, NULL)) !=
         -1) {
    switch (opt) {
      case 'a':
        if (sw.mode == GENERATOR) {
          errx(EXIT_FAILURE, "ERRROR -> invalid option combination!");
        }

        sw.mode = SOLVER;
        sw.all = true;
        mode = MODE_ALL;
        break;

      case 'g':
        if (sw.mode == SOLVER) {
          errx(EXIT_FAILURE, "ERROR -> invalid option combination!");
        }

        sw.mode = GENERATOR;
        if (optarg != NULL) {
          int size = atoi(optarg);
          if (is_valid_size(size) == false) {
            fprintf(stderr,
                    "ERROR -> Invalid size, %d is not in the valid sizes %d, "
                    "%d, %d, %d, %d!\n",
                    size, 4, 8, 16, 32, 64);
            exit(EXIT_FAILURE);
          }
          sw.grid_size = size;
        } else {
          // Default size
          sw.grid_size = 8;
        }
        break;

      case 'N':
        if (sw.mode == SOLVER) {
          errx(EXIT_FAILURE, "ERROR -> invalid option combination!");
        }
        // We don't set the mode to GENERATOR so that it will error out if
        // there is no -g option
        sw.percentage_fill = atoi(optarg);
        break;

      case 'o': {
        FILE *fd = fopen(optarg, "w");
        if (fd == NULL) {
          fclose(fd);
          switch (errno) {
            case EACCES:
              fprintf(stderr, "ERROR -> cannot create file %s!\n", optarg);
              exit(EXIT_FAILURE);
              break;
            default:
              fprintf(stderr, "ERROR -> Unknow error with '%s'!\n", optarg);
              exit(EXIT_FAILURE);
          }
        } else {
          sw.output_file = fd;
        }
        break;

        case 'u':
          if (sw.mode == SOLVER) {
            errx(EXIT_FAILURE, "ERROR -> invalid option combination!");
          }

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
          errx(EXIT_FAILURE, "ERROR -> invalid option!");
          break;
      }
    }
  }

  // Incompatibility checks
  if (argv[optind] == NULL && sw.mode == SOLVER) {
    errx(EXIT_FAILURE, "ERROR -> no input file to solve!");
  } else if (argv[optind] != NULL && sw.mode == GENERATOR) {
    errx(EXIT_FAILURE, "ERROR -> invalid option combination!");
  } else if (sw.unique && (sw.mode != GENERATOR)) {
    errx(EXIT_FAILURE, "ERROR -> invalid option combination!");
  }

  // Meaning only a file has been given
  if (argv[optind] != NULL && sw.mode == NONE) {
    sw.mode = SOLVER;
  }

  // Failsafe
  if (sw.mode == NONE) {
    errx(EXIT_FAILURE, "ERROR -> no mode selected!");
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