#include "cu_test.h"
#include "arguments_parser.h"

#ifdef ALL_TESTS
int main (void) {
  CuString *output = CuStringNew ();
  CuSuite *suite = CuSuiteNew ();

  /* NEED TO DEFINED BIN_PATH at the building time;
     with option -D (-DBIN_PATH="my_path") */
  CuSuiteAddSuite (suite, (CuSuite*) (arg_parser_tests(BIN_PATH, VUGIRD_PATH, LOG_PATH)));

  
  CuSuiteRun (suite);
  CuSuiteSummary (suite, output);
  CuSuiteDetails (suite, output);
  
  printf ("%s\n", output->buffer);
  
  CuStringDelete (output);
  CuSuiteDelete (suite);
  
  return 0;
}

#else

int main (void) {
  CuString *output = CuStringNew ();
  CuSuite *suite = CuSuiteNew ();

#ifdef ARG_PARSER
  CuSuiteAddSuite (suite,  arg_parser_tests (BIN_PATH, VUGIRD_PATH, LOG_PATH));
#endif
  
  CuSuiteRun (suite);
  CuSuiteSummary (suite, output);
  CuSuiteDetails (suite, output);
  
  printf ("%s\n", output->buffer);
  
  CuStringDelete (output);
  CuSuiteDelete (suite);
  
  return 0;
}
#endif
