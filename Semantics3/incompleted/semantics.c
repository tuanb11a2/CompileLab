/* 
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdlib.h>
#include <string.h>
#include "semantics.h"
#include "error.h"

extern SymTab* symtab;
extern Token* currentToken;

Object* lookupObject(char *name) {
  // TODO
  Object* tmpObj;
  ObjectNode* tmp;
  Scope* tmpScope = symtab->currentScope;
  
  while (tmpScope != NULL)
  { 
    tmpObj = findObject(tmpScope->objList,name);
    
    if(tmpObj != NULL){
      return tmpObj;
    }
    
    tmpScope = tmpScope->outer;
  }
  
  //Check for the case of global scope
  tmpObj = findObject(symtab->globalObjectList,name);
  if(tmpObj != NULL){
    return tmpObj;
  }

  return NULL;
}

void checkFreshIdent(char *name) {
  // TODO
  Object* obj = findObject(symtab->currentScope->objList,name);
  if(obj != NULL){
    error(ERR_DUPLICATE_IDENT,currentToken->lineNo,currentToken->colNo);
  }
}

Object* checkDeclaredIdent(char* name) {
  // TODO
  Object* ident = lookupObject(name);
  if(ident == NULL){
    error(ERR_UNDECLARED_IDENT,currentToken->lineNo,currentToken->colNo);
  }

  return ident;
}

Object* checkDeclaredConstant(char* name) {
  // TODO
  Object* constant = lookupObject(name);
  if(constant != NULL && constant->kind == OBJ_CONSTANT ){
    return constant;
  }else if(constant == NULL){
    error(ERR_UNDECLARED_CONSTANT,currentToken->lineNo,currentToken->colNo);
  }else{
    error(ERR_INVALID_CONSTANT,currentToken->lineNo,currentToken->colNo);
  }
}

Object* checkDeclaredType(char* name) {
  // TODO
  Object* type = lookupObject(name);
  if(type != NULL && type->kind == OBJ_TYPE ){
    return type;
  }else if(type == NULL){
    error(ERR_UNDECLARED_TYPE,currentToken->lineNo,currentToken->colNo);
  }else{
    error(ERR_INVALID_TYPE,currentToken->lineNo,currentToken->colNo);
  }
}

Object* checkDeclaredVariable(char* name) {
  // TODO
  Object* variable = lookupObject(name);
  if(variable != NULL && variable->kind == OBJ_VARIABLE){
    return variable;
  }else if(variable == NULL){
    error(ERR_UNDECLARED_VARIABLE,currentToken->lineNo,currentToken->colNo);
  }else{
    error(ERR_INVALID_VARIABLE,currentToken->lineNo,currentToken->colNo);
  }
}

Object* checkDeclaredFunction(char* name) {
  // TODO
  Object* function = lookupObject(name);
  if(function != NULL && function->kind == OBJ_FUNCTION){
    return function;
  }else if(function == NULL){
    error(ERR_UNDECLARED_FUNCTION,currentToken->lineNo,currentToken->colNo);
  }else{
    error(ERR_INVALID_FUNCTION,currentToken->lineNo,currentToken->colNo);
  }
}

Object* checkDeclaredProcedure(char* name) {
  // TODO
  Object* procedure = lookupObject(name);
  if(procedure != NULL && procedure->kind == OBJ_PROCEDURE){
    return procedure;
  }else if(procedure == NULL){
    error(ERR_UNDECLARED_PROCEDURE,currentToken->lineNo,currentToken->colNo);
  }
}

Object* checkDeclaredLValueIdent(char* name) {
  // TODO
  Object* ident = lookupObject(name);
  if(ident != NULL && (ident->kind == OBJ_PARAMETER || ident->kind == OBJ_FUNCTION || ident->kind == OBJ_VARIABLE)){
    return ident;
  }else if(ident == NULL){
    error(ERR_UNDECLARED_IDENT,currentToken->lineNo,currentToken->colNo);
  }else{
    error(ERR_INVALID_IDENT,currentToken->lineNo,currentToken->colNo);
  }

}

