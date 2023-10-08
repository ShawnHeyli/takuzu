#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>

#define ERROR_BUFFER_SIZE 128
#define MIN_GIRD_SIZE 4
#define MAX_GIRD_SIZE 64
#define MIN_GIRD_SIZE_DOWN 3
#define MAX_GIRD_SIZE_UP 65



/* --------------------------------------------------------------------------------------------- */
// ----------------------------------- STRUCTURES AND ENUMS ------------------------------------ //
/* --------------------------------------------------------------------------------------------- */

typedef enum { SOLVER, GENERATOR } modes;

typedef struct {
  modes mode;            // software mode

  uint8_t gird_size_tg;  // size of gird to generate
  bool uniq_sol_gird;    // generate gird with unique solution
  
  bool all_sol;          // search for all solutions
  FILE* in_gird_s;       // stream that contains gird to solve
  
  FILE* out_s;           // stream for print found solutions or gird generate
  bool verbose;          // verbose output
  
} software_infos;



/* --------------------------------------------------------------------------------------------- */
// ----------------------------- DECLARATION OF MAIN UTIL FUNCTIONS ---------------------------- //
/* --------------------------------------------------------------------------------------------- */

static inline void usage (char *);

static inline void init_software_infos (software_infos *);



/* --------------------------------------------------------------------------------------------- */
// --------------------------------------- MAIN FUNCTION --------------------------------------- //
/* --------------------------------------------------------------------------------------------- */

int main(int argc,   char* argv[]) {
  int opt;
  software_infos sw_if;
  init_software_infos (&sw_if);

  char error_buffer[ERROR_BUFFER_SIZE];
  
  static struct option long_opts[] = {
      {"all", no_argument, 0, 'a'},
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
	  fprintf (stderr, "ERROR -> unauthorized -a, --all option in GENERATOR mode!\n");
	  exit (EXIT_FAILURE);
	}
	sw_if.all_sol = true;
	break;
      
      case 'u':
	if (sw_if.mode == SOLVER) {
	  fprintf (stderr, "ERROR -> unauthorized -u, --unique option in SOLVER mode!\n");
	  exit (EXIT_FAILURE);
	}
	sw_if.uniq_sol_gird = true;
	break;
      
      case 'g': // Option for generate gird of optarg*optarg size
	if (sw_if.all_sol) {
	  fprintf (stderr, "ERROR -> unauthorized -a, --all option in GENERATOR mode!\n");
	  exit (EXIT_FAILURE);
	}
	
	// Converting optarg (string) to long integer
	long gird_size = strtol (optarg, NULL, 10);
	if (gird_size == LONG_MAX || gird_size == LONG_MIN) {
	  snprintf (error_buffer, ERROR_BUFFER_SIZE, "ERROR -> '%s' ", optarg);
	  perror (error_buffer);
	  exit (EXIT_FAILURE);
	}

	// Check if given number is valid
	sw_if.gird_size_tg = MIN_GIRD_SIZE;
	bool valid_gird_size = false;
	while (sw_if.gird_size_tg < MAX_GIRD_SIZE_UP && gird_size >= sw_if.gird_size_tg) {
	  if (gird_size == sw_if.gird_size_tg) {
	    valid_gird_size = true;
	    break;
	  }
	  sw_if.gird_size_tg <<= 1;
	}
	if (!valid_gird_size) {
	  fprintf (stderr, "ERROR -> gird size not accepted!"
		   "Authorized : [4; 8; 16; 32; 64]\n");
	  exit (EXIT_FAILURE);
	}

	// TESTS SUCCEDED :: validate generator mode
	sw_if.mode = GENERATOR;
	break;
      
      case 'o':
	sw_if.out_s = fopen(optarg, "w");
	if (!sw_if.out_s) {
	  snprintf (error_buffer, ERROR_BUFFER_SIZE, "ERROR -> '%s' ", optarg);
	  perror (error_buffer);
	  exit (EXIT_FAILURE);
	}
	break;

      case 'v':
	sw_if.verbose = true;
	break;
	
      case 'h':
	usage (argv[0]);
    }
  }

  if (sw_if.mode == SOLVER) {
    if (!argv[optind]) {
      fprintf (stderr, "ERROR -> no input file to solve!\n");
      exit (EXIT_FAILURE);
    }
    
    sw_if.in_gird_s = fopen (argv[optind], "r");
    if (!sw_if.in_gird_s) {
      snprintf (error_buffer, ERROR_BUFFER_SIZE, "ERROR -> '%s' ", argv[optind]);
      perror (error_buffer);
      exit (EXIT_FAILURE);
    }
  }


  

  // CLOSING OPEN STREAMS
  
  if (sw_if.out_s != stdout) {
    fclose (sw_if.out_s);
  }

  if (sw_if.mode == SOLVER) {
    fclose (sw_if.in_gird_s);
  }

  return EXIT_SUCCESS;
}



/* --------------------------------------------------------------------------------------------- */
// ------------------------------- BODY OF MAIN UTIL FUNCTIONS  -------------------------------- //
/* --------------------------------------------------------------------------------------------- */

static inline void usage (char *prg_name) {
  printf ("Usage :%*c%s [-a|-o FILE|-v|-h] FILE...\n", 10, ' ', prg_name);
  printf ("%*c%s-g[SIZE] [-u|-o FILE|-v|-h]\n\n", 17, ' ', prg_name); 
  printf ("Solve or generate takuzu grids of size: 4, 8 16, 32, 64\n\n\
	  -a, --all search for all possible solutions\n\
	  -g[N], --generate[=N] generate a grid of size NxN (default:8)\n\
	  -o FILE, --output FILE write output to FILE\n\
	  -u, --unique generate a grid with unique solution\n\
	  -v, --verbose verbose output\n\
	  -h, --help display this help and exit\n");
  fflush (stdout);
  exit (EXIT_SUCCESS);
}

static inline void init_software_infos (software_infos *si) {
  si->mode = SOLVER;

  si->gird_size_tg = 0;
  si->uniq_sol_gird = false;
  
  si->all_sol = false;
  si->in_gird_s = NULL;
  
  si->out_s = stdout;
  si->verbose = false;
}
