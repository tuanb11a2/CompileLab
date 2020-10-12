/* Scanner
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
int ln = 0;
int cn = 0;
int read = 1;

/***************************************************************/

Token* getToken(void) {
  Token *token = makeToken(TK_NONE, lineNo, colNo);
  if (currentChar == EOF || state == 37)
    return makeToken(TK_EOF, lineNo, colNo);
  
  switch(state){
    case 0:
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

        //CHAR_PLUS 12
        case CHAR_PLUS:
          state = 12;
          free(token);
          token = makeToken(SB_PLUS,lineNo,colNo);
          readChar();
          break;

        //CHAR_MINUS 13
        case CHAR_MINUS:
          state = 13;
          free(token);
          token = makeToken(SB_MINUS,lineNo,colNo);
          readChar();
          break;

        //CHAR_TIMES 14
        case CHAR_TIMES:
          state = 14;
          free(token);
          token = makeToken(SB_TIMES,lineNo,colNo);
          readChar();
          break;

        //CHAR_MINUS 15
        case CHAR_SLASH:
          state = 15;
          free(token);
          token = makeToken(SB_SLASH,lineNo,colNo);
          readChar();
          break;

        //CHAR_EQ 16
        case CHAR_EQ:
          state = 16;
          free(token);
          token = makeToken(SB_EQ,lineNo,colNo);
          readChar();
          break;

        //CHAR_COMMA 17
        case CHAR_COMMA:
          state = 17;
          free(token);
          token = makeToken(SB_COMMA,lineNo,colNo);
          readChar();
          break;

        //CHAR_SEMICOLON 18
        case CHAR_SEMICOLON:
          state = 18;
          free(token);
          token = makeToken(SB_SEMICOLON,lineNo,colNo);
          readChar();
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

        case CHAR_UNKNOWN:
          state = 38;
          error(ERR_INVALIDSYMBOL,lineNo,colNo);
          return token;

        //CHAR_RPAR 39
        case CHAR_RPAR:
          state = 39;
          free(token);
          token = makeToken(SB_RPAR,lineNo,colNo);
          readChar();
          break;
      }
      return token;
      break;
      
    //CHAR_SPACE 1
    case 1:
      while(charCodes[currentChar] == CHAR_SPACE && currentChar != EOF){
        readChar();
      }
      state = 0;
      return token;
      break;

    //CHAR_LPAR 2 
    case 2:
      readChar();
      if (currentChar == EOF)
      {
        state = 37;
        return token;
      }
      if(charCodes[currentChar] == CHAR_PERIOD){
        //SB_LSEL 6
        state = 6;
        token = makeToken(SB_LSEL, token->lineNo, token->colNo);
        readChar();
        return token;
      }else if(charCodes[currentChar] == CHAR_TIMES){
        state = 3;
      }else{
        //SB_LPAR 7
        state = 7;
        token = makeToken(SB_LPAR, token->lineNo, token->colNo);
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
          state = 38;
          error(ERR_ENDOFCOMMENT,token->lineNo,token->colNo);
          return token;
          break;
        }else{
          readChar();
        }
      }

    //CHAR_TIMES to check comment 4
    case 4:
      readChar();
      while(1){
        if(charCodes[currentChar] == CHAR_RPAR){
          state = 5;
          readChar();
          return token;
          break;
        }else if (currentChar == EOF){
          error(ERR_ENDOFCOMMENT,token->lineNo,token->colNo);
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
      break;

    //CHAR_LETTER 8
    case 8:
      token->string[0] = (char)currentChar;
      int count = 1;
      readChar();
      while(charCodes[currentChar] == CHAR_DIGIT || charCodes[currentChar] == CHAR_LETTER){
        if(count <= MAX_IDENT_LEN){
          token->string[count] = (char)currentChar;
          readChar();
          count++;
        }else{
          state = 38;
          error(ERR_IDENTTOOLONG,token->lineNo,token->colNo);
          return token;
          break;
        }
      }
      state = 9;
    
    //TK_IDENT 9
    case 9:
      token->string[count] = '\0';
      token->tokenType = checkKeyword(token->string);
      if(token->tokenType == TK_NONE){
        token->tokenType = TK_IDENT;
      }
      state = 0;
      return token;
      break;

    //CHAR_DIGIT 10
    case 10:
      token->string[0] = (char)currentChar;
      int num = 1;
      readChar();
      while (charCodes[currentChar] == CHAR_DIGIT) {
	      if (num >= MAX_IDENT_LEN) {
		      error(ERR_IDENTTOOLONG, token->lineNo, token->colNo);
          state = 38;
          return token;
          break;
	      }else{
          token->string[num] = currentChar;
          num++;
          readChar();
        }
      }
      state = 11;

    //TK_NUMBER
    case 11:
      token->tokenType = TK_NUMBER;
      token->string[num] = '\0';
      //Number overflow check
      //Overflow occurs then number that convert string will change to negative
      if(atoi(token->string) < 0){
        state = 38;
        printf("%d-%d:Number is too large!\n",token->lineNo,token->colNo);
        exit(-1);
      }else{
        token->value = atoi(token->string);
      }
      state = 0;
      return token;
      break;

    //CHAR_PERIOD 19
    case 19:
      token = makeToken(SB_RSEL,lineNo,colNo);
      readChar();
	    if(charCodes[currentChar] == CHAR_RPAR){
        //CHAR_RSEL 20
        state = 20;
        readChar();
        return token;
	    }else{
        //CHAR_PERIOD 21
		    state = 21;
        token = makeToken(SB_PERIOD,token->lineNo,token->colNo);
        return token;
	    };

    //CHAR_LT 22 
    case 22:
      token = makeToken(SB_GE, lineNo, colNo);
  	  readChar();
	    if(charCodes[currentChar] == CHAR_EQ){
        //CHAR_GEQ 23
        state = 23;
        readChar();
        return token;
	    }else{
        //CHAR_GT 24
        state = 24;
        token = makeToken(SB_GT, token->lineNo, token->colNo);
        return token;
        break;
	    }
      

    //CHAR_LT 25
    case 25:
      token = makeToken(SB_LE, lineNo, colNo);
  	  readChar();
	    if(charCodes[currentChar] == CHAR_EQ){
        //CHAR_LEQ 26
        state = 26;
        readChar();
        return token;
	    }else{
        //CHAR_LT 27
        state = 27;
        token = makeToken(SB_LT, token->lineNo, token->colNo);
        return token;
	    }

    //CHAR_EXCLAIMATION 28
    case 28:
      token = makeToken(SB_NEQ, lineNo, colNo);
  	  readChar();
	    if(charCodes[currentChar] == CHAR_EQ){
        //CHAR_NEQ 29
        state = 29;
        readChar();
        return token;
	    }else{
        //ERROR 30
        state = 30;
        error(ERR_INVALIDSYMBOL, token->lineNo, token->colNo);
        return token;
	    };

    //CHAR_COLON 31
    case 31:
      token = makeToken(SB_ASSIGN, lineNo, colNo);
      readChar();
      if(charCodes[currentChar] == CHAR_EQ){
        //TK_ASSIGN 32
        state = 32;
        readChar();
        return token;
	    }else{
        //CHAR_COLON 33
        state = 33;
        token = makeToken(SB_COLON, token->lineNo, token->colNo);
        return token;
	    }

    //CHAR_SINGLEQUOTE 34
    case 34:
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
          token->string[0]=(char)currentChar;
          token->string[1]='\0';
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
          //TK_CHAR 36
          state = 36;
          token->tokenType = TK_CHAR;
          readChar();
          return token;
        }else{
          error(ERR_INVALIDCHARCONSTANT, token->lineNo, token->colNo);
          state = 38;
          return token;
        }
      }
      break;

    default:
      state = 0;
      return token;
      break;
  }}
/******************************************************************/

void printToken(Token *token) {

  if(token->tokenType != TK_NONE){
  printf("%d-%d:", token->lineNo, token->colNo);
  }

  switch (token->tokenType) {
  case TK_NONE: //printf("TK_NONE\n");
   break;
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

  state = 0;
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



