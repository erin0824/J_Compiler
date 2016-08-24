#include "toasm.h"
#include <stdio.h>
#include <ctype.h>
#include <string.h>

void prologue(FILE *f) { // prologue invoked by every function call
  char* stackptr;
  char* frameptr;
  char* retaddr;
  char* updateFP;

  retaddr = "STR R7, R6, #-2 ;; saves caller's ret addr\n";
  frameptr = "STR R5, R6, #-3 ;; saves caller's frame ptr\n";
  stackptr = "ADD R6, R6, #-3 ;; updates stack ptr\n";
  updateFP = "ADD R5, R6, #0 ;; creates/updates frame ptr\n";

  fputs(retaddr, f);
  fputs(frameptr, f);
  fputs(stackptr, f);
  fputs(updateFP, f);
}

void epilogue(FILE *f) { // epilogue that follows every function call
  char* loadret;
  char* popvars;
  char* restoreSP;
  char* storeret;
  char* restoreFP;
  char* restorePC;
  char* jumpret;

  loadret = "LDR R7, R6, #0 ;; load return value\n";
  popvars = "ADD R6, R5, #0 ;;  pop local vars\n";
  restoreSP = "ADD R6, R6, #3 ;; restore SP\n";
  storeret = "STR R7, R6, #-1 ;; store ret val\n";
  restoreFP = "LDR R5, R6, #-3 ;; restore FP\n";
  restorePC = "LDR R7, R6, #-2 ;; R7 = pow's RA (3F)\n";
  jumpret = "RET ;; jump back\n\n";

  fputs(loadret, f);
  fputs(popvars, f);
  fputs(restoreSP, f);
  fputs(storeret, f);
  fputs(restoreFP, f);
  fputs(restorePC, f);
  fputs(jumpret, f);
}
