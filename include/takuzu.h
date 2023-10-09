#ifndef TAKUZU_H
#define TAKUZU_H


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



#endif /* TAKUZU_H */
