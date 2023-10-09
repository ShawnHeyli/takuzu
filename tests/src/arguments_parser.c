#include "cu_test.h"
#include "arguments_parser.h"

#include <stdlib.h>

#define EXEC_PATH_SIZE 512
#define MIN_PATH_SIZE 128


/* --------------------------------------------------------------------------------------------- */
// ----------------------------- DECLARATION OF MAIN UTIL FUNCTIONS ---------------------------- //
/* --------------------------------------------------------------------------------------------- */
/* MAYBE CREATE LIB FOR ALL PROJECT WITH UTILS FUNCTIONS */
static inline short our_strncpy (char*, const char*, size_t);



/* --------------------------------------------------------------------------------------------- */
// ------------------------------------- GLOBAL VARIABLES -------------------------------------- //
/* --------------------------------------------------------------------------------------------- */

char exec_path[EXEC_PATH_SIZE];     // contain path where executable is stored
char *exec_basename;
size_t exec_bs_max_size;

// contain path where valid girds with unique solution are stored
char vu_gird_path[MIN_PATH_SIZE];
char *vu_gird_basename;
size_t vu_gird_bs_max_size;

char out_log_path[MIN_PATH_SIZE];
char err_log_path[MIN_PATH_SIZE];



/* --------------------------------------------------------------------------------------------- */
// ------------------------------------------- TESTS ------------------------------------------- //
/* --------------------------------------------------------------------------------------------- */

void single_help (CuTest* tc) {
  snprintf (exec_basename, exec_bs_max_size, "takuzu -h %s %s",\
	    out_log_path, err_log_path);
  CuAssertTrue (tc, system(exec_path) == 0);
}

void double_help (CuTest* tc) {
  snprintf (exec_basename, exec_bs_max_size, "takuzu --help %s %s",\
	    out_log_path, err_log_path);
  CuAssertTrue (tc, system(exec_path) == 0);
}

void verbose_mode (CuTest* tc) {
  snprintf (exec_basename, exec_bs_max_size, "takuzu -v %s %s",\
	    out_log_path, err_log_path);
  CuAssertTrue (tc, system(exec_path) != 0);
}

void failed_help (CuTest* tc) {
  snprintf (exec_basename, exec_bs_max_size, "takuzu --helping %s %s",\
	    out_log_path, err_log_path);
  CuAssertTrue (tc, system(exec_path) != 0);
}

 void classic_solve_with_wrong_file_name (CuTest* tc) {
  snprintf (exec_basename, exec_bs_max_size, "takuzu iDontExist %s %s",\
	    out_log_path, err_log_path);
  CuAssertTrue (tc, system(exec_path) != 0);
}

void classic_solve_with_good_file_name (CuTest* tc) {
  snprintf (exec_basename, exec_bs_max_size, "takuzu %s00.gird %s %s",\
	    vu_gird_path, out_log_path, err_log_path);
  CuAssertTrue (tc, system(exec_path) == 0);
}

void classic_solve_found_all (CuTest* tc) {
  snprintf (exec_basename, exec_bs_max_size, "takuzu %s00.gird -a %s %s",\
	    vu_gird_path, out_log_path, err_log_path);
  CuAssertTrue (tc, system(exec_path) == 0);
}

void generator_mode_4 (CuTest* tc) {
  snprintf (exec_basename, exec_bs_max_size, "takuzu -g 4 %s %s",\
	    out_log_path, err_log_path);
  CuAssertTrue (tc, system(exec_path) == 0);
}

void generator_mode_8 (CuTest* tc) {
  snprintf (exec_basename, exec_bs_max_size, "takuzu -g 8 %s %s",	\
	       out_log_path, err_log_path);
  CuAssertTrue (tc, system(exec_path) == 0);
}

void generator_mode_16 (CuTest* tc) {
  snprintf (exec_basename, exec_bs_max_size, "takuzu -g 16 %s %s", \
	    out_log_path, err_log_path);
  CuAssertTrue (tc, system(exec_path) == 0);
}

void generator_mode_32 (CuTest* tc) {
  snprintf (exec_basename, exec_bs_max_size, "takuzu -g 32 %s %s",	\
	    out_log_path, err_log_path);
  CuAssertTrue (tc, system(exec_path) == 0);
}

void generator_mode_64 (CuTest* tc) {
  snprintf (exec_basename, exec_bs_max_size, "takuzu -g 64 %s %s", \
	    out_log_path, err_log_path);
  CuAssertTrue (tc, system(exec_path) == 0);
}

void generator_mode_unique_sol (CuTest* tc) {
  snprintf (exec_basename, exec_bs_max_size, "takuzu -g 64 -u %s %s", \
	    out_log_path, err_log_path);
  CuAssertTrue (tc, system(exec_path) == 0);
}

void generator_mode_unique_sol_out (CuTest* tc) {
  snprintf (exec_basename, exec_bs_max_size, "takuzu -g 64 -u -o trash/tod %s %s", \
	    out_log_path, err_log_path);
  CuAssertTrue (tc, system(exec_path) == 0);
}

void generator_mode_failed_with_letters (CuTest* tc) {
  snprintf (exec_basename, exec_bs_max_size, "takuzu -g tod %s %s", \
	    out_log_path, err_log_path);
  CuAssertTrue (tc, system(exec_path) != 0);
}

void generator_mode_wrong_number_00 (CuTest* tc) {
  snprintf (exec_basename, exec_bs_max_size, "takuzu -g 6 %s %s", \
	    out_log_path, err_log_path);
  CuAssertTrue (tc, system(exec_path) != 0);
}

void generator_mode_wrong_number_01 (CuTest* tc) {
  snprintf (exec_basename, exec_bs_max_size, "takuzu -g 2 %s %s", \
	    out_log_path, err_log_path);
  CuAssertTrue (tc, system(exec_path) != 0);
}

void generator_mode_wrong_number_02 (CuTest* tc) {
  snprintf (exec_basename, exec_bs_max_size, "takuzu -g 128 %s %s", \
	    out_log_path, err_log_path);
  CuAssertTrue (tc, system(exec_path) != 0);
}

void generator_mode_number_overflow (CuTest* tc) {
  snprintf (exec_basename, exec_bs_max_size, \
	    "takuzu -g 6583247087320547504852243875257202 %s %s",\
	    out_log_path, err_log_path);
  CuAssertTrue (tc, system(exec_path) != 0);
}

void generator_mode_number_underflow (CuTest* tc) {
  snprintf (exec_basename, exec_bs_max_size, \
	    "takuzu -g \"-6283094758375487507584287503487502837450\" %s %s",\
	    out_log_path, err_log_path);
  CuAssertTrue (tc, system(exec_path) != 0);
}

void generator_mode_negative_number (CuTest* tc) {
  snprintf (exec_basename, exec_bs_max_size, \
	    "takuzu -g \"-62\" %s %s",\
	    out_log_path, err_log_path);
  CuAssertTrue (tc, system(exec_path) != 0);
}

void generator_mode_wrong_option_a_00 (CuTest* tc) {
  snprintf (exec_basename, exec_bs_max_size, "takuzu -g 32 -a %s %s",	\
	    out_log_path, err_log_path);
  CuAssertTrue (tc, system(exec_path) != 0);
}

void generator_mode_wrong_option_a_01 (CuTest* tc) {
  snprintf (exec_basename, exec_bs_max_size, "takuzu -a -g 32 %s %s",	\
	    out_log_path, err_log_path);
  CuAssertTrue (tc, system(exec_path) != 0);
}

void solver_mode_wrong_option_u_00 (CuTest* tc) {
   snprintf (exec_basename, exec_bs_max_size, "takuzu -u %s00.gird %s %s", \
	     vu_gird_path, out_log_path, err_log_path);
  CuAssertTrue (tc, system(exec_path) != 0);
}

void solver_mode_wrong_option_u_01 (CuTest* tc) {
   snprintf (exec_basename, exec_bs_max_size, "takuzu %s00.gird -u %s %s", \
	     vu_gird_path, out_log_path, err_log_path);
  CuAssertTrue (tc, system(exec_path) != 0);
}

void without_argument (CuTest* tc) {
  snprintf (exec_basename, exec_bs_max_size, "takuzu %s %s",\
	    out_log_path, err_log_path);
  CuAssertTrue (tc, system(exec_path) != 0);
}



/* --------------------------------------------------------------------------------------------- */
// ----------------------------- "MAIN" FUNCTION (call all tests) ------------------------------ //
/* --------------------------------------------------------------------------------------------- */

CuSuite* arg_parser_tests (char bin_path[], char vg_path[], char log_path[]){
  size_t next_char = our_strncpy (exec_path, bin_path, EXEC_PATH_SIZE);
  exec_basename = exec_path + next_char;
  exec_bs_max_size = EXEC_PATH_SIZE - next_char;

  next_char = our_strncpy (vu_gird_path, vg_path, MIN_PATH_SIZE);
  vu_gird_basename = vu_gird_path + next_char;
  vu_gird_bs_max_size = MIN_PATH_SIZE - next_char;

  snprintf (err_log_path, MIN_PATH_SIZE, ">>%s/err.log", log_path);
  snprintf (out_log_path, MIN_PATH_SIZE, "2>>%s/out.log", log_path);
  
  CuSuite* suite = CuSuiteNew ();
  SUITE_ADD_TEST (suite, single_help);
  SUITE_ADD_TEST (suite, double_help);
  SUITE_ADD_TEST (suite, verbose_mode);
  SUITE_ADD_TEST (suite, failed_help);
  SUITE_ADD_TEST (suite, classic_solve_with_wrong_file_name);
  SUITE_ADD_TEST (suite, classic_solve_with_good_file_name);
  SUITE_ADD_TEST (suite, classic_solve_found_all);
  SUITE_ADD_TEST (suite, generator_mode_4);
  SUITE_ADD_TEST (suite, generator_mode_8);
  SUITE_ADD_TEST (suite, generator_mode_16);
  SUITE_ADD_TEST (suite, generator_mode_32);
  SUITE_ADD_TEST (suite, generator_mode_64);
  SUITE_ADD_TEST (suite, generator_mode_unique_sol);
  SUITE_ADD_TEST (suite, generator_mode_unique_sol_out);
  SUITE_ADD_TEST (suite, generator_mode_failed_with_letters);
  SUITE_ADD_TEST (suite, generator_mode_wrong_number_00);
  SUITE_ADD_TEST (suite, generator_mode_wrong_number_01);
  SUITE_ADD_TEST (suite, generator_mode_wrong_number_02);
  SUITE_ADD_TEST (suite, generator_mode_number_overflow);
  SUITE_ADD_TEST (suite, generator_mode_number_underflow);
  SUITE_ADD_TEST (suite, generator_mode_negative_number);
  SUITE_ADD_TEST (suite, generator_mode_wrong_option_a_00);
  SUITE_ADD_TEST (suite, generator_mode_wrong_option_a_01);
  SUITE_ADD_TEST (suite, solver_mode_wrong_option_u_00);
  SUITE_ADD_TEST (suite, solver_mode_wrong_option_u_01);
  SUITE_ADD_TEST (suite, without_argument);
  
  return suite;
}



/* --------------------------------------------------------------------------------------------- */
// ------------------------------- BODY OF MAIN UTIL FUNCTIONS  -------------------------------- //
/* --------------------------------------------------------------------------------------------- */

static inline short our_strncpy (char* dest, const char* src, size_t size) {
  size_t i;
  for (i = 0; src[i] != '\0'; i++) {
    if (i < size) {
      dest[i] = src[i];
    } else {
      fprintf (stderr, "ERROR -> our_strncpy : src to long for dest!\n");
      exit (EXIT_FAILURE);
    }
  }
  
  return i;
}
