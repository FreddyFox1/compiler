#ifndef _PREPROC_H_
#define _PREPROC_H_ 
/*------------------------------ ПРЕПРОЦЕССОР ---------------------------------
Состоит из лексического и синтаксического анализаторов
На выходе получаем AST и таблицы констант и id
------------------------------------------------------------------------------*/

#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <postproc.h> 


// внутреннее представление терминалов (лексем)
typedef enum {
T_PROG=10,T_END,		// program end
T_IF,T_THEN,T_ELSE,T_ENDIF,	// if then else endif
T_WHILE,T_DO,T_ENDWHILE,	// while do endwhile
T_ASSIGN,			// :=
T_PRINT,T_INPUT,T_COMMA,	// print input ,
T_EQ,T_NEQ,T_LT,T_LE,T_GT,T_GE, // = # < <= > >=
T_MINUS,T_PLUS,T_OR,	// - + or
T_MUL,T_DIV, T_AND,	// * / and
T_OPEN,T_CLOSE,		// ( )
T_NOT,		// not
T_NUMBER,T_IDENT,	// числа имена
T_EMPTY, // вспомогательная
} Term;

typedef struct lex_tag Lex;

//  максимальная длина любой строки -- имя переменнй, число и т.д.
#define TOKENLEN  80
void serror(char*err);

struct lex_tag {
	Term term;
	char str[TOKENLEN+1];
	Lex *next;
};

void serror(char*s);
char * sym_string(Term code);
void lexan(char*name,Lex**lexTab);
void synan(Lex*lextab,ASTnode ** ast,Ident**id,Number**num);

#endif