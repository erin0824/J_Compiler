#include <stdio.h>
#include <ctype.h>

// save the caller's return address and the frame pointer
void prologue(FILE *f);
// pops local variables and restores pointers
void epilogue(FILE *f);
