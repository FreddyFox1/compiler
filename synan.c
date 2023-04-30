#include <preproc.h>

//#define DEBUG
#ifdef DEBUG
#define PRINT(rule) do {\
	printf("%s\t",rule); \
	if(term != T_IDENT && term != T_NUMBER) printf("%s\n",sym_string(term));\
	else printf("%s\n",lexstr); \
} while(0)
#else
#define PRINT(rule) /* nop */
#endif

/******************************************************************************
	СИНТАКСИЧЕСКИЙ АНАЛИЗАТОР
Вход:
	таблица лексем (программа во внутреннем представлении)
Выход:
	абстрактное синтаксическое дерево (бинарное дерево)
	и таблицы имен и констант в форме односвязных списков
Алгоритм:
	Рекурсивный спуск для грамматики LL(1)
*******************************************************************************/

static Term term;
static char lexstr[TOKENLEN+1];
static Lex *lex;

static Ident * idents;
static Number * numbers;

static void nextterm(void)
{
//	puts("QQW");	
	lex = lex->next;
//	puts("QE");
//	printf("lex = %p\n",lex);
	if(!lex ) {term = 0;return;} // дошли до конца списка
//	printf("lex = %p str = %p term = %d\n",lex,lex->str,lex->term);
	term = lex->term;
//	puts("QW");
	strncpy(lexstr,lex->str,TOKENLEN);
//	puts("QW");
}

static void accept(Term t)
{
	
	
//	if(term ==11) {lex = lex->next;term=0;return;}
//	printf("term %d t %d %p\n",term,t,lex);
	if(term == t) {
		nextterm();
	} else {
		char msg[2*TOKENLEN];
		strcpy(msg,"ожидается ");strcat(msg,sym_string(t));
		serror(msg);
	}
}
// упреждающие объявления
static ASTnode *termin(void);
static ASTnode *factor(void);
static ASTnode *expr(void);
static ASTnode *sexpr(void);
static ASTnode *statements(void);
static ASTnode *statement(void);
static ASTnode *ifstatement(void);
static ASTnode *whilestatement(void);
static ASTnode *assign(void);
static ASTnode *print(void);
static ASTnode *input(void);


// program = 'program' statements 'end'.
static ASTnode*program(void)
{
	PRINT("program");
	ASTnode *n;
	accept(T_PROG);
	n  = statements();
	if(!n->left) serror("Expected statement in program");
	PRINT("program1");
	accept(T_END);
	return n;
}
// statements = statement {statement}.
static ASTnode*statements(void)
{
	PRINT("statements");
	ASTnode *t,*n,*n0;
	n = n0 = node_create(AST_SEQ);
	n->left = statement();
	if(!n->left) return n;
start:
	PRINT("statements1");
	
	t = statement();
	if(!t) goto end;
	n->right = node_create(AST_SEQ);
	n->right->left = t;
	n = n->right;
	goto start;
end:	
	return n0;
}
// statement = assign | ifstatement | whilestatement | print | input
ASTnode* statement(void)
{
	PRINT("statement");
	ASTnode*n=0;
	switch(term) {
	case T_IDENT: 
		n = assign();break;
	case T_IF:
		PRINT("statementif");
		n = ifstatement();
		PRINT("statementif1");
		break;
	case T_WHILE:
		n = whilestatement();break;
	case T_PRINT:
		n = print();break;
	case T_INPUT:
		n = input();break;
	case T_EMPTY: serror("Unexpected end of program");
	}
	PRINT("statement1");
	return n;
}
// ifstatement = 'if' expr 'then' statements ['else' statements] 'endif'.
static ASTnode * ifstatement(void)
{
	PRINT("if");
	ASTnode * n = node_create(AST_IF);
	nextterm();
	n->left = expr();
	if(!n->left) serror("Expected logical expression");
	PRINT("if1");
	accept(T_THEN);
	PRINT("if2");
	n->right = node_create(AST_SEQ);
	
	n->right->left = statements();
	if(!n->right->left) serror("Expected statement in if");
	PRINT("if3");
	if( term == T_ELSE ) {nextterm();n->right->right = statements();}
	PRINT("if4");
	accept(T_ENDIF);
	PRINT("if5");
	return n;
}

// whilestatement = 'while' expr 'do' expr statements 'endwhile'.

static ASTnode * whilestatement(void)
{
	PRINT("while");
	ASTnode * n = node_create(AST_WHILE);
	nextterm();
	n->left = expr();
	if(!n->left) serror("Expected logical expression");
	PRINT("while0,5");
	accept(T_DO);
	PRINT("while1");
	n->right = statements();
	if(!n->right) serror("Expected statement in while");
	PRINT("while2");
//	printf("nroght = %p\n",n->right);
	accept(T_ENDWHILE);
	return n;
}

ASTnode*assign(void)
{
	PRINT("assign");
	ASTnode *n;
	n = node_create(AST_ASSIGN);
	n->left = node_create(AST_VAR);
	n->left->id = ident_create(&idents,lexstr);
	nextterm();
	PRINT("assign1");
	accept(T_ASSIGN);
	n->right = expr();
	return n;
}
// print = 'print' ident { ',' ident }.

static ASTnode* print(void)
{
	PRINT("print");
	ASTnode *n,*t;
	n = node_create(AST_WRITE);
	nextterm();
	n->left = node_create(AST_SEQ);
	t = n->left;
	t->left = expr();
	while(term == T_COMMA) {
		nextterm();
		t->right = node_create(AST_SEQ);
		t = t->right;
		t->left = expr();
	}
	return n;

}

// input = 'input'  ident { ',' ident }.
static ASTnode* input(void)
{
	PRINT("input");
	ASTnode *n,*t;
	n = node_create(AST_READ);
	nextterm();
	n->left = node_create(AST_SEQ);
	t = n->left;
	t->left = expr();
	while(term == T_COMMA) {
		nextterm();
		t->right = node_create(AST_SEQ);
		t = t->right;
		t->left = expr();
	}

}


// expr = sexpr [ ( '=' | '#' | '<' | '<=' | '>' | '>=' ) sexpr ].

static ASTnode* expr(void)
{
	PRINT("expr");
	ASTnode *n,*t;

	
	n = sexpr();
	t = 0;
	PRINT("expr1");
	switch(term) {
	case T_EQ:
		t = node_create(AST_EQ); 
		break;
	case T_NEQ:
		t = node_create(AST_NE);
		break;
	case T_LT:
		t = node_create(AST_LT);
		break;
	case T_LE:
		t = node_create(AST_LE);
		break;
	case T_GT:
		t = node_create(AST_GT);
		break;
	case T_GE:
		t = node_create(AST_GE);
		break;
	}
	if(t) {
		nextterm();
		t->right = sexpr();
		t->left = n;
		n = t;
	}
	return n;
}
// sexpr = ['+'|'-'] term { ('+'| '-' | 'or' )  term }.

static ASTnode* sexpr(void)
{
	PRINT("sexpr");
	ASTnode *n,*t;
	switch(term) {
	case T_PLUS:
		nextterm();
		n = termin();
		break;
	case T_MINUS:
		n = node_create(AST_NEG);
		nextterm();
		n->left = termin();
		break;
	default:
		n = termin();
	}
//	if(!n) serror("Expected 	
	while(1) {
		switch(term) {
		case T_PLUS: t = node_create(AST_SUM);break;
		case T_MINUS: t = node_create(AST_SUB);break;
		case T_OR: t = node_create(AST_OR);break;
		default: return n;
		}
		t->left = n;
		nextterm();
		t->right = termin();
		n = t;
	}
	return 0;
}
// termin = factor { ('*' | '/' | 'and') factor}.

static ASTnode* termin(void)
{
	PRINT("termin");
	ASTnode *n,*t;
	n = factor();
	PRINT("termin1");
	while(1) {
		switch(term) {
		case T_MUL: t = node_create(AST_MUL);nextterm();break;
		case T_DIV: t = node_create(AST_DIV);nextterm();break;
		case T_AND: t = node_create(AST_AND);nextterm();break;
		default: return n;
		}
		t->left = n;
		n = t;
		PRINT("termin2");
		n->right = factor();
	}
	
	return 0;
}

// factor = ident | integer | '(' expr ')' | 'not' factor.
static ASTnode *factor(void)
{
	PRINT("factor");
	ASTnode *n=0,*t;
	switch( term ) {
	case T_IDENT:
		n = node_create(AST_VAR);
		n->id = ident_create(&idents,lexstr);
		nextterm();
		break;
	case T_NUMBER:
		n = node_create(AST_CONST);
		n->num = number_create(&numbers,lexstr);
		nextterm();
		break;
	case T_OPEN:
		nextterm();
		n = expr();
		accept(T_CLOSE);
		break;
	case T_NOT:
		nextterm();
		n = factor();
		break;
	case T_EMPTY: serror("Unexpected end of program");
	default:
		return 0;
	}
	
	return n;
}
void synan(Lex*lextab,ASTnode ** ast,Ident**id,Number**num)
{
	idents = 0;
	numbers = 0;
	lex = lextab;
	term = lex->term;
	strcpy(lexstr,lex->str);
	*ast = program();
	*id = idents;
	*num = numbers;
}
/*
static void printast(ASTnode*n)
{
	if(!n) return;
	switch(n->type) {
	case N_PROG:
		printf("PROG");
		printf("left:\n");printast(n->left);
		printf("right:\n");printast(n->right);
		break;
	}
}
void printAST(void)
{
	printast(AST);
}
*/