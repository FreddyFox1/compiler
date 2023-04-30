#include <preproc.h>

ASTnode * node_create(ASTtype type)
{
	ASTnode *n = malloc(sizeof(ASTnode));
	n->type  = type;
	n->left = n->right = 0;
	return n;
}

Ident * ident_create(Ident**idents,char*name)
{
	Ident * id;
	static int numer = 0;
	// ищем, нет ли уже такого имени
	for(id = *idents; id; id = id->next) {
//		printf("id->name = %s name= %s\n",id->name,name);
		if(!strcmp(id->name,name)) return id;
	}
	id = malloc(sizeof(Ident));
	id->numer = numer++;
	strncpy(id->name,name,TOKENLEN);
//	printf("id->name = %s numer= %d\n",id->name,id->numer);
	id->next = *idents;
	*idents = id;
	return id;
}
Number * number_create(Number** numbers,char*str)
{
	Number * num;
	int number = (int)strtol(str,0,10);
	// ищем, нет ли уже такой константы
	for(num = *numbers; num; num = num->next) {
		if(number == num->value) return num;
	}
	num = malloc(sizeof(Number));
	num->value = number;
	num->next = *numbers;
	*numbers = num;
	return num;
}
