/* Scanner
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdio.h>
#include <stdlib.h>

#include "reader.h"
#include "charcode.h"
#include "token.h"
#include "error.h"


extern int lineNo;
extern int colNo;
extern int currentChar;

extern CharCode charCodes[];

/***************************************************************/

void skipBlank() {
  int tmp;
  tmp = readChar();
  while(charCodes[tmp] == CHAR_SPACE){
    tmp = readChar();
  }
}

void skipComment() {
  int tmp;
  tmp = readChar();
}

Token* readIdentKeyword(void) {
  // TODO
}

Token* readNumber(void) {
  // TODO
}

Token* readConstChar(void) {
  // TODO
}

Token* getToken(void) {
  Token *token;
  int ln, cn, tmp;

  if (currentChar == EOF) 
    return makeToken(TK_EOF, lineNo, colNo);

  switch (charCodes[currentChar]) {
  case CHAR_SPACE: skipBlank(); return getToken();
  
  case CHAR_LETTER: return readIdentKeyword();
  
  case CHAR_DIGIT: return readNumber();
  
  case CHAR_PLUS: 
    token = makeToken(SB_PLUS, lineNo, colNo);
    readChar(); 
    return token;
    
  case CHAR_MINUS:
  	token = makeToken(SB_MINUS, lineNo, colNo);
    readChar(); 
    return token;
    
  case CHAR_TIMES:
  	token = makeToken(SB_TIMES, lineNo, colNo);
    readChar(); 
    return token;
    
  case CHAR_SLASH:
  	token = makeToken(SB_SLASH, lineNo, colNo);
    readChar(); 
    return token;
    
  case CHAR_LT:
  	tmp = readChar();
	if(charCodes[tmp] == CHAR_EQ){
    colNo = colNo - 1;
		token = makeToken(SB_LE, lineNo, colNo);
		readChar();
		return token;
	}else{
    colNo = colNo - 1;
		token = makeToken(SB_LT, lineNo, colNo);
		return token;
    colNo = colNo + 1;
	};
	
  case CHAR_GT:
  	tmp = readChar();
	    if(charCodes[tmp] == CHAR_EQ){
        colNo = colNo - 1;
		    token = makeToken(SB_GE, lineNo, colNo);
		    readChar();
		    return token;
	    }else{
        colNo = colNo - 1;
		    token = makeToken(SB_GT, lineNo, colNo);
		    return token;
        colNo = colNo + 1;
	    };
  
  case CHAR_EXCLAIMATION:
  	tmp = readChar();
	    if(charCodes[tmp] == CHAR_EQ){
        colNo = colNo - 1;
		    token = makeToken(SB_NEQ, lineNo, colNo);
		    readChar();
		    return token;
	    }else{
        colNo = colNo - 1;
		    token = makeToken(TK_NONE, lineNo, colNo);
        error(ERR_INVALIDSYMBOL, lineNo, colNo); 
        return token;
        colNo = colNo + 1;
	    };
  
  case CHAR_COMMA:
    token = makeToken(SB_COMMA, lineNo, colNo);
    readChar(); 
    return token;

  case CHAR_PERIOD:
    tmp = readChar();
	  if(charCodes[tmp] == CHAR_RPAR){
      colNo = colNo - 1;
		  token = makeToken(SB_RPAR, lineNo, colNo);
		  readChar();
		  return token;
	  }else{
      colNo = colNo - 1;
		  token = makeToken(SB_PERIOD, lineNo, colNo);
		  return token;
      colNo = colNo + 1;
	  };

  case CHAR_COLON:
    tmp = readChar();
    if(charCodes[tmp] == CHAR_EQ){
        colNo = colNo - 1;
		    token = makeToken(SB_ASSIGN, lineNo, colNo);
		    readChar();
		    return token;
	    }else{
        colNo = colNo - 1;
		    token = makeToken(SB_COLON, lineNo, colNo);
		    return token;
        colNo = colNo + 1;
	    };

  case CHAR_SEMICOLON:
    token = makeToken(SB_SEMICOLON, lineNo, colNo);
    readChar(); 
    return token;
  
  case CHAR_SINGLEQUOTE:
    return readConstChar();


  //Not done
  case CHAR_LPAR:
    tmp = readChar();
    if(charCodes[tmp] == CHAR_PERIOD){
      colNo = colNo - 1;
		  token = makeToken(SB_LSEL, lineNo, colNo);
		  readChar();
		  return token;
    }else if(charCodes[tmp] == CHAR_TIMES){
    }else{
      colNo = colNo - 1;
		  token = makeToken(SB_LPAR, lineNo, colNo);
		  readChar();
		  return token;
    }

  case CHAR_RPAR:
    token = makeToken(SB_RPAR, lineNo, colNo);
    readChar(); 
    return token;
  
  default:
    token = makeToken(TK_NONE, lineNo, colNo);
    error(ERR_INVALIDSYMBOL, lineNo, colNo);
    readChar(); 
    return token;
  }
}


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



