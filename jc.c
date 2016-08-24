#include "token.h"
#include "toasm.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#define MAX_FILENAME 100 * sizeof(char)
#define MAX_LABEL_LENGTH 100

// stack to keep track of if & endif tokens, and else tokens, if any
struct struct_stack {
  char label[MAX_LABEL_LENGTH];
  struct struct_stack *next;
};
struct struct_stack *head;
char popped[MAX_LABEL_LENGTH];
bool err;

void pop() {
  struct struct_stack *curr;
  if (head == NULL) {
    err = true;
    fprintf(stderr, "Stack is empty\n");
  } else {
    strcpy(popped, head -> label);
    curr = head -> next;
    free(head);
    head = curr;
    err = false;
  }
}

void peek() {
  if (head == NULL) {
    return;
  } else {
    printf("next label: %s\n", head -> label);
  }
}

void push(char* pushed) {
  int pushlen;
  struct struct_stack *curr = malloc(sizeof(struct struct_stack));
  if (curr == NULL) {
    fprintf(stderr, "OUT OF MEMORY\n");
    exit(1);
  }
  if (head == NULL) {
    strcpy(curr -> label, pushed);
    curr -> next = NULL;
    head = curr;
  } else {
    strcpy(curr -> label, pushed);
    curr -> next = head;
    head = curr;
  }
}

int main(int argc, char **argv) {
  FILE *in;
  FILE *out;
  char inname[MAX_FILENAME];
  char outname[MAX_FILENAME];
  token theToken;
  int returnval, num_label, offset, label, ifcount;
  int i;
  char *p, *next_label, *q, *t;
  char args[MAX_TOKEN_LENGTH];
  int count;
  char s[MAX_TOKEN_LENGTH];
  char *first;
  char *second;
  char *falign;
  char *code;
  char *main;
  int id; // keeps track of if ident was called after defun
  int c, hc;
  int defun_count, return_count, endif_count, else_no; // error indicators
  // tags to be printed in asm file
  falign = ".FALIGN\n";
  code = ".CODE\n\n";
  main = "main\n";

  // initialize all counters to 0
  defun_count = 0;
  return_count = 0;
  endif_count = 0;
  ifcount = 0;

  in = fopen(argv[1], "r");

  for (i = 0; i < MAX_FILENAME; i++) {
    if (argv[1][i] != '.') {
      inname[i] = argv[1][i];
    } else {
      inname[i] = '\0';
      break;
    }
  }

  if (in == NULL) {
    fprintf(stderr, "Input file could not be opened\n");
    return -1;
  }

  for (i = 0; i < MAX_FILENAME; i++) {
    if (argv[1][i] != '.') {
      outname[i] = argv[1][i];
    } else {
      outname[i] = '.';
      outname[i + 1]= 'a';
      outname[i + 2]= 's';
      outname[i + 3]= 'm';
      outname[i + 4]= '\0';
      break;
    }
  }

  out = fopen(outname, "w");

  if (out == NULL) {
    fprintf(stderr, "Output file could not be opened\n");
    return -1;
  }

  // print code label
  fputs(code, out);

  // while token is valid and file ptr not EOF
  while(returnval != -1 && returnval != 2) {
    returnval = read_token(&theToken, in);
    if (returnval == 0) {
      if (theToken.type == LITERAL) {
        if (theToken.literal_value < -32767 || theToken.literal_value > 32767) {
          fprintf(stderr,"Error: literal value is not within valid range\n");
          return 4;
        }
        c = theToken.literal_value & 0xFF;
        hc = theToken.literal_value >> 8;
        hc = hc & 0xFF;
        sprintf(s, "CONST R7, 0x%x\n", c);
        fputs(s, out);
        sprintf(s, "HICONST R7 0x%x\n", hc);
        fputs(s, out);
        fputs("STR R7, R6 #-1\n", out);
        fputs("ADD R6, R6, #-1\n", out);
      }

      if (theToken.type == PLUS || theToken.type == MINUS ||
        theToken.type == DIV || theToken.type == MOD ||
        theToken.type == AND || theToken.type == OR ||
        theToken.type == LT || theToken.type == LE ||
        theToken.type == EQ || theToken.type == GE ||
        theToken.type == GT || theToken.type == SWAP ||
        theToken.type == MUL) { // if operator requires two args
          fputs("LDR R0, R6, #0 ;; load in top value into R0\n", out);
          fputs("ADD R6, R6, #1 ;; update stack ptr\n", out);
          fputs("LDR R1, R6, #0 ;; load in bottom value into R1\n", out);
          fputs("ADD R6, R6, #1 ;; update stack ptr\n", out);
        }

        if (theToken.type  == PLUS) {
          fputs("ADD R0, R0, R1 ;; add the two values\n", out);
          fputs("STR R0, R6, #-1 ;; store result\n", out);
          fputs("ADD R6, R6, #-1 ;; update stack ptr\n", out);
        } else if (theToken.type == MINUS) {
          fputs("SUB R0, R0, R1 ;; subtract the two values\n", out);
          fputs("STR R0, R6, #-1 ;; store result\n", out);
          fputs("ADD R6, R6, #-1 ;; update stack ptr\n", out);
        } else if (theToken.type == MUL) {
          fputs("MUL R0, R0, R1 ;; multiply the two values\n", out);
          fputs("STR R0, R6, #-1 ;; store result\n", out);
          fputs("ADD R6, R6, #-1 ;; update stack ptr\n", out);
        } else if (theToken.type == DIV) {
          fputs ("DIV R0, R0, R1 ;; divide the two values\n",out);
          fputs("STR R0, R6, #-1 ;; store result\n", out);
          fputs("ADD R6, R6, #-1\n", out);
        } else if (theToken.type == MOD) {
          fputs ("MOD R0, R0, R1 ;; mod the two values\n",out);
          fputs("STR R0, R6, #-1 ;; store result\n", out);
          fputs("ADD R6, R6, #-1\n", out);
        } else if (theToken.type == AND) {
          fputs ("AND R0, R0, R1 ;; and the two values\n", out);
          fputs("STR R0, R6, #-1 ;; store result\n", out);
          fputs("ADD R6, R6, #-1\n", out);
        } else if (theToken.type == OR) {
          fputs("OR R0, R0, R1 ;; and the two values\n", out);
          fputs("STR R0, R6, #-1 ;; store result\n", out);
          fputs("ADD R6, R6, #-1 ;; update stack ptr\n", out);
        } else if (theToken.type == NOT) {
          fputs("LDR R0, R6, #0 ;; load val into R0\n", out);
          fputs ("NOT R0, R0 ;; perform NOT\n", out);
          fputs("STR R0, R6, #0 ;; store into original slot\n", out);
        } else if (theToken.type == LT) {
          num_label++;
          fputs("CONST R2, #1 ;; initialize to true\n", out);
          fputs("CMP R0, R1\n", out);
          sprintf(s, "BRn %s_end_lt%d ;; if <, then jump\n",
            inname, num_label);
          fputs(s, out);
          fputs("CONST R2, #0 ;; change to false\n", out);
          sprintf(s, "%s_end_lt%d\n", inname, num_label);
          fputs(s, out);
          fputs("STR R2, R6, #-1 ;; store result\n", out);
          fputs("ADD R6, R6 #-1 ;; increment stackptr\n", out);
        } else if (theToken.type == LE) {
          num_label++;
          fputs("CONST R2, #1 ;; initialize to true\n", out);
          fputs("CMP R0, R1 ;; compare the two registers\n", out);
          sprintf(s, "BRnz %s_end_le%d ;; if <= then jump\n",
            inname, num_label);
          fputs(s, out);
          fputs("CONST R2, #0 ;; change to false\n", out);
          sprintf(s, "%s_end_le%d\n", inname, num_label);
          fputs(s, out);
          fputs("STR R2, R6, #-1 ;; store result\n", out);
          fputs("ADD R6, R6 #-1 ;; increment stackptr\n", out);
        } else if (theToken.type == EQ) {
          num_label++;
          fputs("CONST R2, #1 ;; initialize to true\n", out);
          fputs("CMP R0, R1 ;; compare the two registers\n", out);
          sprintf(s, "BRz %s_end_eq%d ;; if ==, jump\n", inname, num_label);
          fputs(s, out);
          fputs("CONST R2, #0 ;; change to false\n", out);
          sprintf(s, "%s_end_eq%d\n", inname, num_label);
          fputs(s, out);
          fputs("STR R2, R6, #-1 ;; store result\n", out);
          fputs("ADD R6, R6 #-1 ;; increment stackptr\n", out);
        } else if (theToken.type == GE) {
          num_label++;
          fputs("CONST R2, #1 ;; initialize to true\n", out);
          fputs("CMP R0, R1 ;; compare the two registers\n", out);
          sprintf(s, "BRzp %s_end_ge%d ;; if >=, jump\n", inname, num_label);
          fputs(s, out);
          fputs("CONST R2, #0 ;; change to false\n", out);
          sprintf(s, "%s_end_ge%d\n", inname, num_label);
          fputs(s, out);
          fputs("STR R2, R6, #-1 ;; store result\n", out);
          fputs("ADD R6, R6 #-1 ;; increment stackptr\n", out);
        } else if (theToken.type == GT) {
          num_label++;
          fputs("CONST R2, #1 ;; initialize to true\n", out);
          fputs("CMP R0, R1 ;; compare the two registers\n", out);
          sprintf(s, "BRp %s_end_gt%d ;; if > then jump\n", inname, num_label);
          fputs(s, out);
          fputs("CONST R2, #0 ;; change to false\n", out);
          sprintf(s, "%s_end_gt%d\n", inname, num_label);
          fputs(s, out);
          fputs("STR R2, R6, #-1 ;; store result\n", out);
          fputs("ADD R6, R6 #-1 ;; increment stackptr\n", out);
        } else if (theToken.type == DROP) {
          fputs("ADD R6, R6 #1 ;; update stackptr down one for drop\n", out);
        } else if (theToken.type == DUP) {
          fputs("LDR R0, R6, #0 ;;  copy val into R0\n", out);
          fputs("ADD R6, R6 #-1 ;; increment stackptr\n", out);
          fputs("STR R0, R6, #0 ;; store in one slot above\n",out);
        } else if (theToken.type == SWAP) {
          fputs("ADD R6, R6 #-1 ;; move stackptr up one\n", out);
          fputs("STR R0, R6, #0 ;; store top val on bottom\n", out);
          fputs("ADD R6, R6 #-1 ;; move stackptr up one\n", out);
          fputs("STR R1, R6 #0 ;; store bottom val on top\n", out);
        } else if (theToken.type == ROT) {
          fputs("LDR R0, R6, #0 ;; load top val\n", out);
          fputs("ADD R6, R6, #1 ;; move stackptr down\n", out);
          fputs("LDR R1, R6, #0 ;; load middle val\n", out);
          fputs("ADD R6, R6, #1 ;; move stackptr down\n", out);
          fputs("LDR R2, R6, #0 ;; load top val\n", out);
          fputs("STR R1, R6, #0 ;; store top val on bottom\n", out);
          fputs("ADD R6, R6 #-1 ;; update stackptr\n", out);
          fputs("STR R0, R6, #0 ;; store top val in middle\n", out);
          fputs("ADD R6, R6 #-1 ;; update stackptr\n", out);
          fputs("STR R2, R6, #0 ;; store bottom val on top\n", out);
        } else if (theToken.type == ARG) {
          offset = 2 + theToken.arg_no; // arg#, located 2 + # below R6
          sprintf(s, "LDR R0, R5, #%d\n", offset);
          fputs(s, out);
          fputs("STR R0, R6, #-1\n", out);
          fputs("ADD R6, R6, #-1\n", out);
        } else if (theToken.type == DEFUN) {
          defun_count++;
          fputs(falign, out);
          id = 1; // checks defun was called
        } else if (theToken.type == IF) {
          ifcount++;
          sprintf(s, "%s_endif%d\n", inname, ifcount);
          push(s);
          fputs("LDR R2, R6, #0 ;; load val at top of stack\n", out);
          fputs("CONST R3, #0\n", out);
          fputs("CMP R2, R3 ;; compare with 0\n", out);
          sprintf(s, "BRz %s_else%d ;;;  jump to else if false\n",
            inname, ifcount);
          fputs(s, out);
          fputs("ADD R6, R6, #1 ;; calling if consumes top value\n", out);
        } else if (theToken.type == ELSE) {
          pop();
          sscanf(popped, "%*[^0123456789]%d", &else_no);
          push(popped);
          sprintf(s, "JMP %s_endif%d ;;  jump to endif after blockA if true\n",
            inname, else_no);
          fputs(s, out);
          sprintf(s, "%s_else%d\n", inname, else_no);
          fputs(s, out);
          fputs("ADD R6, R6, #1 ;; calling if consumes top value\n", out);
        } else if (theToken.type == ENDIF) {
          endif_count++;
          pop();
          fputs(popped, out);
        } else if (theToken.type == RETURN) {
          return_count++;
          epilogue(out);
        } else if (theToken.type == IDENT) {
          strcpy(s, theToken.str);
          if(isalpha(s[0]) != 0) {
            if (id == 0) { // func invoked
              sprintf(s, "JSR %s\n", theToken.str);
              fputs(s, out);
              fputs("ADD R6, R6 #-1\n", out);
            } else { // func being established
              sprintf(s, "%s\n", theToken.str);
              fputs(s, out);
              prologue(out);
              id = 0; // reset
            }
          }
        }
      }
    }
    if (theToken.type == BROKEN_TOKEN) {
      fputs("Error: Found broken token\n", out);
      return 1;
    }
    if (defun_count != return_count) {
      fputs("Error: Every function must end with return statement\n", out);
      return 2;
    }
    if (endif_count != ifcount) {
      fputs("Error: Every if statement must conclude with endif\n", out);
      return 3;
    }

    fclose(in);
    fclose(out);
    return 0;
}
