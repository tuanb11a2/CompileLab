/* Scanner
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "reader.h"
#include "charcode.h"
#include "token.h"
#include "error.h"


extern int lineNo;
extern int colNo;
extern int currentChar;

extern CharCode charCodes[];
int state = 0;
int read = 0;

/***************************************************************/

Token* getToken(void) {
  Token *token = makeToken(TK_NONE, lineNo, colNo);
  int count = 0;
  if (state == 37 ||currentChar == EOF){
    return makeToken(TK_EOF, lineNo, colNo);
  }
  
  switch(state){
    case 0:
      if(read == 1){
        readChar();
      }else read = 0;
      
      if(currentChar == EOF){
        state = 37;
        return makeToken(TK_EOF,lineNo,colNo);
      }
      token = makeToken(TK_NONE,lineNo,colNo);
      switch(charCodes[currentChar]){
        case CHAR_SPACE: 
          state = 1;
          break;

        case CHAR_LPAR:
          state = 2;
          break;

        case CHAR_LETTER: 
          state = 8;
          break;

        case CHAR_DIGIT:
          state = 10;
          break;

        case CHAR_PLUS:
          state = 12;
          break;
  
        case CHAR_MINUS:
          state = 13;
          break;

        case CHAR_TIMES:
          state = 14;
          break;

        case CHAR_SLASH:
          state = 15;
          break;
        
        case CHAR_EQ:
          state = 16;
          break;

        case CHAR_COMMA:
          state = 17;
          break;

        case CHAR_SEMICOLON:
          state = 18;
          break;

        case CHAR_PERIOD:
          state = 19;
          break;

        case CHAR_GT:
          state = 22;
          break;

        case CHAR_LT:
          state = 25;
          break;

        case CHAR_EXCLAIMATION:
          state = 28;
          break;

        case CHAR_COLON:
          state = 31;
          break;

        case CHAR_SINGLEQUOTE:
          state = 34;
          break;

        case CHAR_RPAR:
          state = 39;
          break;

        default:
          error(ERR_INVALIDSYMBOL, lineNo, colNo);
          state = 38;
          return token;
      }
      return token;
      break;
      
    //CHAR_SPACE 1
    case 1:
      while(charCodes[currentChar] == CHAR_SPACE && currentChar != EOF){
        readChar();
      }
      read = 0;
      state = 0;
      return token;

    //CHAR_LPAR 2 
    case 2:
      token = makeToken(SB_LSEL, lineNo, colNo);
      readChar();
      if(charCodes[currentChar] == CHAR_PERIOD){
        state = 6;
        return token;
      }else if(charCodes[currentChar] == CHAR_TIMES){
        state = 3;
        return token;
      }else{
        state = 7;
        return token;
      }

    //skip comment 3
    case 3:
      readChar();
      while(1){
        if(charCodes[currentChar] == CHAR_TIMES){
          state = 4;
          return token;
          break;
        }else if(currentChar == EOF){
          error(ERR_ENDOFCOMMENT,lineNo,colNo);
          state = 38;
          return token;
          break;
        }else{
          readChar();
        }
      }
      return token;
      

    //CHAR_TIMES to check comment 4
    case 4:
      readChar();
      while(1){
        if(charCodes[currentChar] == CHAR_RPAR){
          state = 5;
          return token;
          break;
        }else if (currentChar == EOF){
          error(ERR_ENDOFCOMMENT,lineNo,colNo);
          return token;
          break;
        }else if(charCodes[currentChar] == CHAR_TIMES){
          readChar();
        }else{
          state = 3;
          return token;
          break;
        }
      }
      return token;
      

    //Found comment, read next char 5
    case 5:
      read = 1;
      state = 0;
      return token;

    //LSEL 6
    case 6:
		  read = 1;
      state = 0;
		  return token;  
    
    //TK_LPAR 7
    case 7:
		  token = makeToken(SB_LPAR, token->lineNo, token->colNo);
      read = 0;
      state = 0;
		  return token;

    //CHAR_LETTER 8
    case 8:
      token = makeToken(TK_IDENT,lineNo,colNo);
      while(charCodes[currentChar] == CHAR_DIGIT || charCodes[currentChar] == CHAR_LETTER){
        if(count <= MAX_IDENT_LEN){
          token->string[count] = currentChar;
          readChar();
          count++;
        }else{
          error(ERR_IDENTTOOLONG,token->lineNo,token->colNo);
          state = 38;
          return token;
          break;
        }
      }
      token->string[count] = '\0';
      state = 9;
      return token;
    
    //TK_IDENT
    case 9:
      token->tokenType = checkKeyword(token->string);
      if(token->tokenType == TK_NONE){
        token->tokenType = TK_IDENT;
      }
      count = 0;
      state = 0;
      read = 1;
      return token;

    //CHAR_DIGIT
    case 10:
      token = makeToken(TK_NUMBER,lineNo,colNo);
      while (charCodes[currentChar] == CHAR_DIGIT) {
	      if (count > MAX_IDENT_LEN) {
		      error(ERR_IDENTTOOLONG, token->lineNo, token->colNo);
          state = 38;
          return token;
          break;
	      }
        token->string[count] = currentChar;
        count++;
        readChar();
      }
      token->string[count] = '\0';
      state = 11;
      return token;

    //TK_NUMBER
    case 11:
      //Number overflow check
      //Overflow occurs then number that convert string will change to negative
      if(atoi(token->string) < 0){
        printf("%d-%d:Number is too large!\n",token->lineNo,token->colNo);
        exit(-1);
      }else{
        token->value = atoi(token->string);
      }
      state = 0;
      count = 0;
      read = 1;
      return token;

    //CHAR_PLUS 12
    case 12:
      token = makeToken(SB_PLUS, lineNo, colNo);
      read = 1;
      state = 0;
      return token;
      
    //CHAR_MINUS 13
    case 13:
  	  token = makeToken(SB_MINUS, lineNo, colNo);
      read = 1;
      state = 0;
      return token;
      
    //CHAR_TIMES 14
    case 14:
  	  token = makeToken(SB_TIMES, lineNo, colNo);
      read = 1;
      state = 0;
      return token;

    //CHAR_SLASH 15
    case 15:
  	  token = makeToken(SB_SLASH, lineNo, colNo);
      read = 1;
      state = 0;
      return token;

    //CHAR_EQ 16
    case 16:
      token = makeToken(SB_EQ, lineNo, colNo);
      read = 1;
      state = 0;
      return token;

    //CHAR_COMMA 17
    case 17:
      token = makeToken(SB_COMMA, lineNo, colNo);
      read = 1;
      state = 0;
      return token;

    //CHAR_SEMICOLON 18
    case 18:
      token = makeToken(SB_SEMICOLON, lineNo, colNo);
      read = 1;
      state = 0;
      return token;

    //CHAR_PERIOD 19
    case 19:
      token = makeToken(SB_RSEL, lineNo, colNo);
      readChar();
	    if(charCodes[currentChar] == CHAR_RPAR){
        state = 20;
        return token;
	    }else{
		    state = 21;
        return token;
	    };

    //CHAR_RSEL 20
    case 20:
      read = 1;
      state = 0;
      return token;

    //CHAR_PERIOD 21
    case 21:
      token = makeToken(SB_PERIOD, token->lineNo, token->colNo);
      state = 0;
      read = 0;
      return token;

    //CHAR_LT 22 
    case 22:
      token = makeToken(SB_GE, lineNo, colNo);
  	  readChar();
	    if(charCodes[currentChar] == CHAR_EQ){
        state = 23;
        return token;
	    }else{
        state = 24;
        return token;
	    }
      
    //CHAR_GEQ 23
    case 23:
		  read = 1;
      state = 0;
		  return token;

    //CHAR_GT 24
    case 24:
		  token = makeToken(SB_GT, token->lineNo, token->colNo);
		  state = 0;
      read = 0;
      return token;

    //CHAR_LT 25
    case 25:
      token = makeToken(SB_LE, lineNo, colNo);
  	  readChar();
	    if(charCodes[currentChar] == CHAR_EQ){
        state = 26;
        return token;
	    }else{
        state = 27;
        return token;
	    }

    //CHAR_LEQ 26
    case 26:
		  read = 1;
      state = 0;
		  return token;

    //CHAR_LT 27
    case 27:
		  token = makeToken(SB_LT, token->lineNo, token->colNo);
		  state = 0;
      read = 0;
      return token; 

    //CHAR_EXCLAIMATION 28
    case 28:
      token = makeToken(SB_NEQ, lineNo, colNo);
  	  readChar();
	    if(charCodes[currentChar] == CHAR_EQ){
        state = 29;
        return token;
	    }else{
        state = 30;
        return token;
	    }

    //CHAR_NEQ 29
    case 29:
		  read = 1;
      state = 0;
		  return token;
      
    //ERROR 30
    case 30:
      state = 0;
      read = 0;
      error(ERR_INVALIDSYMBOL, token->lineNo, token->colNo);
      return token;

    //CHAR_COLON 31
    case 31:
      token = makeToken(SB_ASSIGN, lineNo, colNo);
      readChar();
      if(charCodes[currentChar] == CHAR_EQ){
        state = 32;
        return token;
	    }else{
        state = 33;
        return token;
	    }

    //TK_ASSIGN 32
    case 32:
		  read = 1;
      state = 0;
		  return token;
      
    //CHAR_COLON 33
    case 33:
		  token = makeToken(SB_COLON, token->lineNo, token->colNo);
		  state = 0;
      read = 0;
      return token;

    //CHAR_SINGLEQUOTE 34
    case 34:
      token = makeToken(TK_NONE,lineNo,colNo);
      readChar();

      if(currentChar == EOF){
        state = 37;
        return token;
      }else{
        if(charCodes[currentChar] == CHAR_UNKNOWN){
          error(ERR_INVALIDCHARCONSTANT,token->lineNo,token->colNo);
          state = 38;
          return token;
        }else{
          state = 35;
          token->string[0] = currentChar;
          token->string[1] = '\0';
        }
      }
      
    //Read const char 35
    case 35:
      readChar();

      if(currentChar == EOF){
        state = 37;
        return token;
      }else{
        if(charCodes[currentChar] == CHAR_SINGLEQUOTE){
          state = 36;
          return token;
        }else{
          error(ERR_INVALIDCHARCONSTANT, token->lineNo, token->colNo);
          state = 38;
          return token;
        }
      }

    //TK_CHAR 36
    case 36:
      token = makeToken(TK_CHAR,token->lineNo,token->colNo);
      read = 1;
      state = 0;
      return token;

    //CHAR_EOF
    case 37:
      token = makeToken(TK_EOF,token->lineNo,token->colNo);
      return token;

    //ERROR!
    case 38:
      //printf("\nError!\n");
      return token;
      
    //CHAR_RPAR
    case 39:
      token = makeToken(SB_RPAR, lineNo, colNo);
      read = 1;
      state = 0;
      return token;

    default:
      return token;
  }}
/******************************************************************/

void printToken(Token *token) {

  printf("%d-%d:", token->lineNo, token->colNo);

  switch (token->tokenType) {
  case TK_NONE: printf("TK_NONE\n"); break;
  case TK_IDENT: printf("TK_IDENT(%s)\n", token->string); break;
  case TK_NUMBER: printf("TK_NUMBER(%s)\n", token->string); break;
  case TK_CHAR: printf("TK_CHAR(\'%s\')\n", token->string); break;
  case TK_EOF: printf("TK_EOF\n"); break;

  case KW_PROGRAM: printf("KW_PROGRAM\n"); break;
  case KW_CONST: printf("KW_CONST\n"); break;
  case KW_TYPE: printf("KW_TYPE\n"); break;
  case KW_VAR: printf("KW_VAR\n"); break;
  case KW_INTEGER: printf("KW_INTEGER\n"); break;
  case KW_CHAR: printf("KW_CHAR\n"); break;
  case KW_ARRAY: printf("KW_ARRAY\n"); break;
  case KW_OF: printf("KW_OF\n"); break;
  case KW_FUNCTION: printf("KW_FUNCTION\n"); break;
  case KW_PROCEDURE: printf("KW_PROCEDURE\n"); break;
  case KW_BEGIN: printf("KW_BEGIN\n"); break;
  case KW_END: printf("KW_END\n"); break;
  case KW_CALL: printf("KW_CALL\n"); break;
  case KW_IF: printf("KW_IF\n"); break;
  case KW_THEN: printf("KW_THEN\n"); break;
  case KW_ELSE: printf("KW_ELSE\n"); break;
  case KW_WHILE: printf("KW_WHILE\n"); break;
  case KW_DO: printf("KW_DO\n"); break;
  case KW_FOR: printf("KW_FOR\n"); break;
  case KW_TO: printf("KW_TO\n"); break;

  case SB_SEMICOLON: printf("SB_SEMICOLON\n"); break;
  case SB_COLON: printf("SB_COLON\n"); break;
  case SB_PERIOD: printf("SB_PERIOD\n"); break;
  case SB_COMMA: printf("SB_COMMA\n"); break;
  case SB_ASSIGN: printf("SB_ASSIGN\n"); break;
  case SB_EQ: printf("SB_EQ\n"); break;
  case SB_NEQ: printf("SB_NEQ\n"); break;
  case SB_LT: printf("SB_LT\n"); break;
  case SB_LE: printf("SB_LE\n"); break;
  case SB_GT: printf("SB_GT\n"); break;
  case SB_GE: printf("SB_GE\n"); break;
  case SB_PLUS: printf("SB_PLUS\n"); break;
  case SB_MINUS: printf("SB_MINUS\n"); break;
  case SB_TIMES: printf("SB_TIMES\n"); break;
  case SB_SLASH: printf("SB_SLASH\n"); break;
  case SB_LPAR: printf("SB_LPAR\n"); break;
  case SB_RPAR: printf("SB_RPAR\n"); break;
  case SB_LSEL: printf("SB_LSEL\n"); break;
  case SB_RSEL: printf("SB_RSEL\n"); break;
  }
}

int scan(char *fileName) {
  Token *token;

  if (openInputStream(fileName) == IO_ERROR)
    return IO_ERROR;

  token = getToken();
  while (token->tokenType != TK_EOF) {
    printToken(token);
    free(token);
    token = getToken();
  }

  free(token);
  closeInputStream();
  return IO_SUCCESS;
}

/******************************************************************/

int main(int argc, char *argv[]) {
  if (argc <= 1) {
    printf("scanner: no input file.\n");
    return -1;
  }

  if (scan(argv[1]) == IO_ERROR) {
    printf("Can\'t read input file!\n");
    return -1;
  }

  return 0;
}



