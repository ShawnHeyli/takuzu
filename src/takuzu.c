#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

bool isSolver = false;
bool verbose = false;

int main(int argc, char* argv[]) {
  bool all = false;
  bool unique = false;

  char* output = NULL;

  static struct option long_options[] = {
      {"all", no_argument, 0, 'a'},
      {"generate", required_argument, 0, 'g'},
      {"ouput", required_argument, 0, 'o'},
      {"unique", no_argument, 0, 'u'},
      {"verbose", no_argument, 0, 'v'},
      {"help", no_argument, 0, 'h'},
      {0, 0, 0, 0}};
  int c = getopt_long(argc, argv, "agouvh", long_options, NULL);

  while (c != -1) {
    switch (c) {
      case 'a':
        all = true;
        break;
      case 'g':
        isSolver = false;
        break;
      case 'o':
        output = optarg;
        break;
      case 'u':
        unique = true;
        break;
      case 'v':
        verbose = true;
        break;
      case 'h':
        puts(
            "Usage: takuzu [-a|-o FILE|-v|-h] FILE...\n \
                takuzu -g[SIZE] [-u|-o FILE|-v|-h]\n \
                Solve or generate takuzu grids of size: 4, 8 16, 32, 64\n \
                -a, --all search for all possible solutions\n \
                -g[N], --generate[=N] generate a grid of size NxN (default:8)\n \
                -o FILE, --output FILE write output to FILE\n \
                -u, --unique generate a grid with unique solution\n \
                -v, --verbose verbose output\n \
                -h, --help display this help and exit");
        exit(EXIT_SUCCESS);
      default:
        printf("?? getopt returned character code 0%o ??\n", c);
    }
  }

  return EXIT_SUCCESS;
}