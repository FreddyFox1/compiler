#include <preproc.h>
#include <postproc.h>


// вспомогательная для вывода
static void printlex(Lex*lexTab)
{
	Lex*n;
	for(n = lexTab; n; n = n->next) {
		switch(n->term) {
		default: printf("Terminal: %s\n",sym_string(n->term));break;
		case T_NUMBER: printf("Number: %s\n",n->str);break;
		case T_IDENT: printf("Ident: %s\n",n->str);break;
		}
	}
}
int main(int argc,char**argv)
{
//	if(argc<2) {printf("Must be program name\n");exit(0);}
	

	Lex* lexTab;	
	lexan("p6.bas",&lexTab);
	printlex(lexTab);
//	exit(0);
/*	
	ASTnode *ast;
	Ident *idents;
	Number * numbers;
	synan(lexTab,&ast,&idents,&numbers);
	generator(ast,"p3.cod");
*/	
//	printAST();
	
	
}	
