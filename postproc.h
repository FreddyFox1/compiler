#ifndef _POSTPROC_H_
#define _POSTPROC_H_

/*------------------------------ ПОСТПРОЦЕССОР ---------------------------------
Состоит из сементического анализатора и генератора кода
На выходе получаем код виртуальной машины
------------------------------------------------------------------------------*/
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <vm.h>


// Типы узлов дерева
typedef enum {
AST_EXIT=10,
// операторы
AST_SEQ,AST_IF,AST_WHILE,AST_DOWHILE,
AST_READ,AST_WRITE,
AST_ASSIGN,
// листья дерева -- переменные и константы
AST_VAR,AST_CONST,
// бинарные логические операции
AST_AND,AST_OR,
// унарная логическая
AST_NOT,
// бинарные операции отношения
AST_LT,AST_LE,AST_GT,AST_GE,AST_NE,AST_EQ,
// унарная арифметическая операция
AST_NEG,
// бинарные арифметические операции
AST_MUL,AST_SUM,AST_SUB,AST_DIV,
} ASTtype;

typedef struct ast_tag ASTnode;
typedef struct id_tag Ident;
typedef struct num_tag Number;

// абстрактное синаксическое дерево
struct ast_tag {
	ASTtype type;
	union {
		ASTnode *left;
		Ident * id;
		Number* num;
	};
	ASTnode *right;
};
//  максимальная длина любой строки -- имя переменнй, число и т.д.
#define TOKENLEN  80
// таблица идентификаторов
struct id_tag {
	char name[TOKENLEN+1];
	int numer;
	Ident*next;
};
// таблица констант
struct num_tag {
	int value;
	Number*next;
};

void serror(char*s);
ASTnode * node_create(ASTtype type);
Ident* ident_create(Ident**id,char*str);
Number* number_create(Number**num,char*str);
void generator(ASTnode*ast,char*nameprog);
#endif