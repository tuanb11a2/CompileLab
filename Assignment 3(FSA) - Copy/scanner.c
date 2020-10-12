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
int state = 0;
int read = 1;   // the next time state = 0 should we read char or not?

extern CharCode charCodes[];

/***************************************************************/
/******************************************************************/
Token* getToke(void) {
    Token *token = makeToken(TK_NONE, lineNo, colNo);   // default token
    int ln = lineNo, cn = colNo;
    if(state == 37|| currentChar == EOF){               // eof
        token->tokenType=TK_EOF;
        return token;
    }
    switch(state){
        case 0:
            if(read == 1) readChar();
            else read = 0;
            if(currentChar == EOF){ //in case we did read char  // eof
                state = 37;
                token->tokenType=TK_EOF;
                return token;
            }
            token = makeToken(TK_NONE, lineNo, colNo);   // default token
            ln = lineNo; cn = colNo;
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
                    token->tokenType=SB_PLUS;
                    read = 1;
                    break;
                case CHAR_MINUS:
                    state = 13;
                    token->tokenType=SB_MINUS;
                    read = 1;
                    break;
                case CHAR_TIMES:
                    state = 14;
                    token->tokenType=SB_TIMES;
                    read = 1;
                    break;
                case CHAR_SLASH:
                    state = 15;
                    token->tokenType=SB_SLASH;
                    read = 1;
                    break;
                case CHAR_EQ:
                    state = 16;
                    token->tokenType=SB_EQ;
                    read = 1;
                    break;
                case CHAR_COMMA:
                    state = 17;
                    token->tokenType=SB_COMMA;
                    read = 1;
                    break;
                case CHAR_SEMICOLON:
                    state = 18;
                    token->tokenType=SB_SEMICOLON;
                    read = 1;
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
                    error(ERR_INVALIDSYMBOL, lineNo, colNo);
                    return token;
                case CHAR_RPAR:
                    state = 39;
                    token->tokenType=SB_RPAR;
                    read = 1;
                    break;
            }
            return token;
            break;

        case 1:
            while(1){
                readChar();
                if((currentChar != EOF)&&(charCodes[currentChar] != CHAR_SPACE))
                    break;      // then end the loop and return
                else if(currentChar == EOF){
                    state = 37;
                    return token;
                    break;
                }
              }
              state = 0;
              read = 0; // we should not do the readchar() in the next loop
              return token;
              break;

        case 2: // comment
            readChar();
            if(currentChar == EOF){
                state = 37;
                return token;
            }
            switch(charCodes[currentChar]){
                case CHAR_PERIOD:
                    state = 6;
                    read = 1;
                    token->tokenType = SB_LSEL;
                    return token;
                    break;
                case CHAR_TIMES:
                    state = 3;
                    return token;
                    break;
                default:
                    state = 7;
                    read = 0;
                    token->tokenType=SB_LPAR;
                    return token;
                    break;
            }
            break;

        case 3:
            readChar();     // the char after 1st '*'
            while(1){
                if(currentChar == EOF){
                    state = 37;
                    error(ERR_ENDOFCOMMENT, token->lineNo, token->colNo);
                    return token;
                    break;
                }
                else if(charCodes[currentChar]==CHAR_TIMES){
                    state = 4;
                    return token;
                    break;
                }
                readChar();
            }
            break;

        case 4:
            readChar();     // the char after 2nd '*'
                if(currentChar == EOF){
                    state = 37;
                    error(ERR_ENDOFCOMMENT, token->lineNo, token->colNo);
                    return token;
                    break;
                }
                else if(charCodes[currentChar]==CHAR_TIMES){
                    state = 4;
                    return token;
                    break;
                }
                else if(charCodes[currentChar]==CHAR_RPAR){
                    state = 5;
                    return token;
                    break;
                }
                else{
                    state = 3;
                    return token;
                    break;
                }
            break;

        case 5:
            state  = 0;
            read = 1;
            return token;
            break;

        case 8: // read ident
            token->string[0]=(char)currentChar;
            int identLength = 1;
            readChar();

            while ((currentChar != EOF) && ((charCodes[currentChar] == CHAR_LETTER) || (charCodes[currentChar] == CHAR_DIGIT))) {
                if (identLength <= MAX_IDENT_LEN) token->string[identLength++] = (char)currentChar;
                readChar();
              }
            if (identLength > MAX_IDENT_LEN) {
                error(ERR_IDENTTOOLONG, token->lineNo, token->colNo);
                return token;
              }

        case 9:
            token->string[identLength] = '\0';
              token->tokenType = checkKeyword(token->string);

              if (token->tokenType == TK_NONE)
                token->tokenType = TK_IDENT;
                read = 0;
                state = 0;
              return token;
            break;

        case 10:// read number
            token->tokenType = TK_NUMBER;
            int count = 0;

              while ((currentChar != EOF) && (charCodes[currentChar] == CHAR_DIGIT)) {
                token->string[count++] = (char)currentChar;
                readChar();
              }

        case 11:
            token->string[count] = '\0';
              token->value = atoi(token->string);
              read = 0;
              state = 0;
              return token;
            break;

        case 19:
            readChar();
            if(currentChar != EOF){
                if(charCodes[currentChar]==CHAR_RPAR){
                    token->tokenType = SB_RSEL;
                    read = 1;
                    state = 20;
                    return token;
                }
                else{
                    token->tokenType = SB_PERIOD;
                    read = 0;
                    state = 21;
                    return token;
                }
            }
            else{
                state = 37;
                return token;
            }
            break;

        case 22:
            readChar();
            if(currentChar != EOF){
                if(charCodes[currentChar]==CHAR_EQ){
                    token->tokenType = SB_GE;
                    read = 1;
                    state = 23;
                    return token;
                }
                else{
                    token->tokenType = SB_GT;
                    read = 0;
                    state = 24;
                    return token;
                }
            }
            else{
                state = 37;
                return token;
            }
            break;

        case 25:
            readChar();
            if(currentChar != EOF){
                if(charCodes[currentChar]==CHAR_EQ){
                    token->tokenType = SB_LE;
                    read = 1;
                    state = 26;
                    return token;
                }
                else{
                    token->tokenType = SB_LT;
                    read = 0;
                    state = 27;
                    return token;
                }
            }
            else{
                state = 37;
                return token;
            }
            break;

        case 28:
            readChar();
            if(currentChar != EOF){
                if(charCodes[currentChar]==CHAR_EQ){
                    token->tokenType = SB_NEQ;
                    read = 1;
                    state = 29;
                    return token;
                }
                else{
                    error(ERR_INVALIDSYMBOL, lineNo, colNo);
                    state = 30;
                    return token;
                }
            }
            else{
                state = 37;
                return token;
            }
            break;

        case 31:
            readChar();
            if(currentChar != EOF){
                if(charCodes[currentChar]==CHAR_EQ){
                    token->tokenType = SB_ASSIGN;
                    read = 1;
                    state = 32;
                    return token;
                }
                else{
                    token->tokenType = SB_COLON;
                    read = 0;
                    state = 33;
                    return token;
                }
            }
            else{
                state = 37;
                return token;
            }
            break;


        case 34:// read const char
            readChar();

            if(currentChar != EOF){
                if(charCodes[currentChar]==CHAR_UNKNOWN){
                    error(ERR_INVALIDCHARCONSTANT, token->lineNo, token->colNo);
                    state = 38;
                    return token;
                }
                else{
                    state = 35;
                    token->string[0]=(char)currentChar;
                    token->string[1]='\0';
                }
            }
            else{
                state = 37;
                return token;
            }

        case 35:
            readChar();
            if(currentChar != EOF){
                if(charCodes[currentChar]==CHAR_SINGLEQUOTE){
                    state = 36;
                    token->tokenType=TK_CHAR;
                }
                else{
                    error(ERR_INVALIDCHARCONSTANT, token->lineNo, token->colNo);
                    state = 38;
                    return token;
                }
            }
            else{
                state = 37;
                return token;
            }

        case 36:
            read = 1;
            state = 0;
            return token;
            break;

        default:
            state = 0;  // reset
            //read = 1;   // continue to read the next char
            return token;
            break;
    }
}

/******************************************************************/

    void printToken(Token *token) {

        if(token->tokenType!=TK_NONE)
      printf("%d-%d:", token->lineNo, token->colNo);

      switch (token->tokenType) {
      case TK_NONE: printf(""); break;
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

/******************************************************************/

    // Read input file
    int scan(char *fileName) {
      if (openInputStream(fileName) == IO_ERROR)
        return IO_ERROR;

        state = 0;                  // initiate state
        read = 0;                   // because we have already read in the openInputStream func
        Token *token = getToke();  // 1st token

      while (token->tokenType != TK_EOF) {
        printToken(token);
        free(token);

        token = getToke();
      }

      free(token);
      closeInputStream();
      return IO_SUCCESS;
    }

/******************************************************************/

// scanner (path to file kpl)

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



