#include <comp.h>
/******************************************************************************
	СЕМАНТИЧЕСКИЙ АНАЛИЗАТОР
Вход:
	абстрактное синтаксическое дерево (бинарное дерево)
	и таблицы имен и констант в форме односвязных списков
Выход:
	Сообщение при ошибке несоответствия типа
Алгоритм:
	Обход дерева
*******************************************************************************/
typedef enum {
	S_STAT=0,S_LOG,S_INT
} S_TYPE;

S_TYPE seman(ASTnode *n)
{
	S_TYPE type;
	code *p1, *p2;
	switch(n->type) {
	case AST_START:
		if(!n->left) serror("Empty program");
		if(seman(n->left) != S_STAT) 
			serror("Statement expected");
		break;
	case AST_SEQ:
		if(!n->left) serror("Empty statements sequence");
		if(seman(n->left) != S_STAT) 
			serror("Statement expected");
		if(n->right) if(seman(n->right) != S_STAT) 
					serror("Statement expected");
		type = S_STAT;
		break;
	case AST_IF:
		if(seman(n->left) != S_BOOL) serror("Expected logical expression");
		if(seman(n->right) != S_STAT) 
			serror("Statement expected");
		type = S_STAT;
		break;
	case AST_WHILE:
		if(seman(n->left) != S_BOOL) serror("Expected logical expression");
		if(seman(n->right) != S_STAT) 
			serror("Statement expected");
		type = S_STAT;
		break;
	case AST_DOWHILE:
		break;
	case AST_READ:case AST_WRITE:
	case AST_ADD:
		if(seman(n->left) != S_INT) serror();
		if(seman(n->right) != S_INT) serror();
	}
	return type;
}
