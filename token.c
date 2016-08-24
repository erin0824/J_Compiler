#include "token.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int read_token (token *theToken, FILE *theFile) {
  char *p;
  char *s;
  char *first;
  char *second;
  char args[MAX_TOKEN_LENGTH];
  char arg[MAX_TOKEN_LENGTH];
  int i, x, y;
  int label;
  char* firstarg;
  char* secondarg;
  int valone, valtwo;
  int num_label;
  int test, sext;
  char *parsedchar;

    i = 0;
    if (test == EOF) {
      return -1;
    }
    if (feof(theFile)) {
      return -1;
    }
    do {
      test = fgetc(theFile);
      if (test == EOF) {
        break;
      }
      if (test == ';') {
        while (test != '\n') {
          test = fgetc(theFile);
        }
        test = fgetc(theFile);
      }
      if (test != '\n' && test != ' ' && test != '\t' && test != ';'
        && test != EOF) {
        args[i] = test;
        i++;
      }
    } while (test != '\n' && test != ' ' && test != '\t' && test != EOF);
    args[i] = '\0';

    strcpy(theToken -> str, args);

    for (i = 0; i < strlen(theToken -> str); i++) {
      if (isalpha(theToken -> str[i]) != 0 &&
      isdigit(theToken -> str[i]) != 0 &&
      theToken -> str[i] != '_' && theToken -> str[i] != '-') {
        theToken -> type = BROKEN_TOKEN;
        fprintf(stderr, "broken token");
        return 2;
      }
    }

    first = &(theToken -> str[0]);
    if (strcmp(first, " ") == 0) {
      return 499;
    }

    first = &(theToken -> str[0]);
    second = &(theToken -> str[1]);
    if ((strncmp(theToken -> str, "0x", 2) == 0)) {
      // for (x = 2; i < strlen(theToken -> str); x++) {
      //   if (!isdigit(theToken -> str[x])) {
      //     fprintf(stderr, "Invalid hex literal value");
      //     return 3;
      //   }
      // }
      if(theToken -> str[2] == '1') {
        theToken -> str[0] = '1';
        theToken -> str[1] = '1';
      } else {
        theToken -> str[0] = '0';
        theToken -> str[1] = '0';
      }
      theToken -> type = LITERAL;
      sscanf(theToken -> str, "%x", &(theToken -> literal_value));
    } else if (isdigit(theToken -> str[0]))  {
      // for (y = 1; i < strlen(theToken -> str) ; y++) {
      //   if (isdigit(theToken -> str[y]) != 0) {
      //     fprintf(stderr, "Invalid decimal literal value");
      //     return 3;
      //   }
      // }
      sscanf(theToken -> str, "%d", &(theToken -> literal_value));
      theToken -> type = LITERAL;
      return 0;
    } else if ((strncmp(theToken -> str, "-1", 2) == 0) ||
        (strncmp(theToken -> str, "-2", 2) == 0) ||
        (strncmp(theToken -> str, "-3", 2) == 0) ||
        (strncmp(theToken -> str, "-4", 2) == 0) ||
        (strncmp(theToken -> str, "-5", 2) == 0) ||
        (strncmp(theToken -> str, "-6", 2) == 0) ||
        (strncmp(theToken -> str, "-7", 2) == 0) ||
        (strncmp(theToken -> str, "-8", 2) == 0) ||
        (strncmp(theToken -> str, "-9", 2) == 0)) {
          theToken -> str[0] = '0';
          theToken -> type = LITERAL;
          sscanf(theToken -> str, "%d", &(sext));
          theToken -> literal_value = 0 - sext;
          return 0;
    } else if (strcmp(theToken -> str, "defun") == 0) {
      theToken -> type = DEFUN;
      return 0;
    } else if (strcmp(theToken -> str, "return") == 0) {
      theToken -> type = RETURN;
      return 0;
    } else if (strcmp(theToken -> str, "+") == 0) {
      theToken -> type = PLUS;
      return 0;
    } else if (strcmp(theToken -> str, "-") == 0) {
      theToken -> type = MINUS;
      return 0;
    } else if (strcmp(theToken -> str, "*") == 0) {
      theToken -> type = MUL;
      return 0;
    } else if (strcmp(theToken -> str, "/") == 0) {
      theToken -> type = DIV;
      return 0;
    } else if (strcmp(theToken -> str, "%") == 0) {
      theToken -> type = MOD;
      return 0;
    } else if (strcmp(theToken -> str, "and") == 0) {
      theToken -> type = AND;
      return 0;
    } else if (strcmp(theToken -> str, "or") == 0) {
      theToken -> type = OR;
      return 0;
    } else if (strcmp(theToken -> str, "!") == 0) {
      theToken -> type = NOT;
      return 0;
    } else if (strcmp(theToken -> str, "lt") == 0) {
      theToken -> type = LT;
      return 0;
    } else if (strcmp(theToken -> str, "le") == 0) {
      theToken -> type = LE;
      return 0;
    } else if (strcmp(theToken -> str, "eq") == 0) {
      theToken -> type = EQ;
      return 0;
    } else if (strcmp(theToken -> str, "ge") == 0) {
      theToken -> type = GE;
      return 0;
    } else if (strcmp(theToken -> str, "gt") == 0) {
      theToken -> type = GT;
      return 0;
    } else if (strcmp(theToken -> str, "if") == 0) {
      theToken -> type = IF;
      return 0;
    } else if (strcmp(theToken -> str, "else") == 0) {
      theToken -> type = ELSE;
      return 0;
    } else if (strcmp(theToken -> str, "endif") == 0) {
      theToken -> type = ENDIF;
      return 0;
    } else if (strcmp(theToken -> str, "drop") == 0) {
      theToken -> type = DROP;
      return 0;
    } else if (strcmp(theToken -> str, "dup") == 0) {
      theToken -> type = DUP;
      return 0;
    } else if (strcmp(theToken -> str, "swap") == 0) {
      theToken -> type = SWAP;
      return 0;
    } else if (strcmp(theToken -> str, "rot") == 0) {
      theToken -> type = ROT;
      return 0;
    } else if (strncmp(theToken -> str, "arg", 3) == 0) {
      theToken -> type = ARG;
      // printf("string in arg %s", theToken->str);
      theToken -> str[0] = '0';
      theToken -> str[1] = '0';
      theToken -> str[2] = '0';
      sscanf(theToken -> str, "%d", &theToken -> arg_no);
      // printf("arg number in token.c :%d\n", theToken -> arg_no);
      return 0;
    } else {
        theToken -> type = IDENT;
        return 0;
    }
    if (isspace(test) == 0) {
      return 499;
    }
    if (test == '\n') {
      return 501;
    }
    return 1;
}


const char *token_type_to_string (int type) {
  switch(type) {
    case DEFUN:
    return "DEFUN";

    case IDENT:
    return "IDENT";

    case RETURN:
    return "RETURN";

    case PLUS:
    return "PLUS";

    case MINUS:
    return "MINUS";

    case MUL:
    return "MUL";

    case DIV:
    return "DIV";

    case MOD:
    return "MOD";

    case AND:
    return "AND";

    case OR:
    return "OR";

    case NOT:
    return "NOT";

    case LT:
    return "LT";

    case LE:
    return "LE";

    case EQ:
    return "EQ";

    case GE:
    return "GE";

    case GT:
    return "GT";

    case IF:
    return "IF";

    case ELSE:
    return "ELSE";

    case ENDIF:
    return "ENDIF";

    case DROP:
    return "DROP";

    case DUP:
    return "DUP";

    case SWAP:
    return "SWAP";

    case ROT:
    return "ROT";

    case ARG:
    return "ARG";

    case LITERAL:
    return "LITERAL";

    default:
    return "BROKEN_TOKEN";
    break;
  }
}

void print_token (token *theToken) {
  const char *p_tok;

  p_tok = token_type_to_string(theToken -> type);
  printf("%s\n", p_tok);
}
