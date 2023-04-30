/******************************************************************************
	ГЕНЕРАТОР КОДА
Вход:
	абстрактное синтаксическое дерево (бинарное дерево)
	и таблицы имен и констант в форме односвязных списков
Выход:
	программа для целевой архитектуры (виртуальной машины)
	в форме массива, сохраняемого в файл
Алгоритм:
*******************************************************************************/
#include <postproc.h>

static Code c[PROGSIZE];
static Code *cmd = c;
static void savecode(int command)
{
	*cmd = command;
//	printf(" com = %d\n",command);
	cmd++;
	if(cmd-c>PROGSIZE) serror("To big program");
}
static void savecode2(int command)
{
	*((int*)cmd) = command;
	cmd += sizeof(int);
	if(cmd-c>PROGSIZE) serror("To big program");
}
static void codegen(ASTnode *n)
{
	Code *p1, *p2,*p;
	int num1,num2;
	ASTnode*t;
	printf("AST %d\n",n->type);
	switch(n->type) {
	case AST_EXIT:
		savecode(I_HALT);
		break;
	case AST_SEQ:
//		puts("QQ1");
		codegen(n->left);
//		puts("QQ2");
		if(n->right) codegen(n->right);
//		puts("QQ3");
		break;
	case AST_VAR:
		//printf("id = %d\n",n->id->numer);
		savecode(I_LOAD); savecode2(n->id->numer);
//		puts("QQ");
		break;
	case AST_CONST:
		savecode(I_PUSH); savecode2(n->num->value);
		break;
	case AST_AND:
		codegen(n->left);
		savecode(I_JZ);
		p1 = cmd; savecode2(0); // сюда пишем адрес перехода
		codegen(n->right);
		*((int*)p1) = cmd - p1;
		break;
	case AST_OR:
		codegen(n->left); 	savecode(I_JNZ);
		p1 = cmd; savecode2(0); // сюда пишем адрес перехода
		codegen(n->right);
		*((int*)p1) = cmd - p1;
		break;
	case AST_NOT:
		savecode(I_NOT);
		codegen(n->left);
		break;
	case AST_LT:
		codegen(n->left); codegen(n->right); savecode(I_LT);
		break;
	case AST_LE:
		codegen(n->left); codegen(n->right); savecode(I_LE);
		break;
	case AST_GT:
		codegen(n->left); codegen(n->right); savecode(I_GT);
		break;
	case AST_GE:
		codegen(n->left); codegen(n->right); savecode(I_GE);
		break;
	case AST_NE:
		codegen(n->left); codegen(n->right); savecode(I_NE);
		break;
	case AST_NEG:
		codegen(n->left); savecode(I_NEG);
		break;
	case AST_EQ:
		codegen(n->left); codegen(n->right); savecode(I_EQ);
		break;
	case AST_SUM:
		codegen(n->left); codegen(n->right); savecode(I_ADD);
		break;
	case AST_SUB:
		codegen(n->left); codegen(n->right); savecode(I_SUB);
		break;
	case AST_MUL:
		codegen(n->left); codegen(n->right); savecode(I_MUL);
		break;
	case AST_DIV:
		codegen(n->left); codegen(n->right); savecode(I_DIV);
		break;
	case AST_ASSIGN:
		codegen(n->right);
		savecode(I_SAVE);
		savecode2(n->left->id->numer);
		break;
	case AST_IF:
		codegen(n->left); savecode(I_JZ);
		p1 = cmd; savecode2(0); // сюда запишем относительный переход
		codegen(n->right->left);
		if(n->right->right==NULL) *((int*)p1) = cmd - (p1-1); // нет else
		else {
			savecode(I_JMP);
			p2 = cmd; savecode2(0);
			*((int*)p1) = cmd - (p1-1);
			codegen(n->right->right);
			*((int*)p2) = cmd - (p2-1);
		}
		break;
	case AST_WHILE:
		p1 = cmd; //  номер команды начала цикла
		codegen(n->left);
		savecode(I_JZ);
		p2 = cmd; savecode2(0); // дырка для адреса перехода
		codegen(n->right);
		savecode(I_JMP);
		//printf("back = %d p1 = %p  cmd = %p\n",p1-cmd,p1,cmd);
		savecode2(p1 - (cmd - 1));
		//printf("for = %d  p2 = %p  cmd = %p\n",cmd-p2,p2,cmd);
		*((int*)p2) = cmd - (p2-1);
		break;
	case AST_DOWHILE:
		p1 = cmd;
		codegen(n->right); codegen(n->left);
		savecode(I_JNZ);
		savecode2(p1-(cmd-1));
		break;
	case AST_WRITE:
		t = n->left;
		while(1) {
			codegen(t->left); // поместить в стек аргумент
			savecode(I_WRITE);
			if(t->right) { // еще аргумент
				savecode(I_WRITEC);
				savecode(' ');
				t = t->right;
			} else { // переводим строку
				savecode(I_WRITEC);
				savecode('\n');
				break;
			}
		}
		break;
	case AST_READ:
		t = n->left;
		while(1) {
			// поместить в стек номер переменной
			savecode(I_READ);
			savecode2(t->left->id->numer);
			
			if(t->right) { // еще аргумент
				savecode(I_WRITEC);
				savecode(' ');
				t = t->right;
			} else { // переводим строку
				savecode(I_WRITEC);
				savecode('\n');
				break;
			}
		}
		break;
	
	}
}
static int numer(int pc)
{
	return *(int*)(c + pc);
}
static void printcode(void)
{
//	printf("I = %d %d %d %d %d | %d %d %d %d %d\n",c[0],c[1],c[2],c[3],c[4],
//	c[5],c[6],c[7],c[8],c[9]);
//	exit(0);
	int i,N=cmd-c;
	printf("N = %d\n",N);
	for(i=0;i<N;) switch(c[i++]) {
	case I_HALT: printf("%d:\t%s\n",i,"HALT");break;
	case I_ADD: printf("%d:\t%s\n",i,"ADD");break;
	case I_SUB: printf("%d:\t%s\n",i,"SUB");break;
	case I_MUL: printf("%d:\t%s\n",i,"MUL");break;
	case I_DIV: printf("%d:\t%s\n",i,"DIV");break;
	case I_NEG: printf("%d:\t%s\n",i,"NEG");break;
	case I_NOT: printf("%d:\t%s\n",i,"NOT");break;
	case I_LT: printf("%d:\t%s\n",i,"LT");break;
	case I_LE: printf("%d:\t%s\n",i,"LE");break;
	case I_GT: printf("%d:\t%s\n",i,"GT");break;
	case I_GE: printf("%d:\t%s\n",i,"GE");break;
	case I_EQ: printf("%d:\t%s\n",i,"EQ");break;
	case I_NE: printf("%d:\t%s\n",i,"NE");break;
	case I_LOAD: printf("%d:\t%s\t%d\n",i,"LOAD",numer(i));
		i+=sizeof(int);break;
	case I_SAVE: printf("%d:\t%s\t%d\n",i,"SAVE",numer(i));
		i+=sizeof(int);break;
	case I_PUSH: printf("%d:\t%s\t%d\n",i,"PUSH",numer(i));
		i+=sizeof(int);break;
	case I_JMP: printf("%d:\t%s\t%d\n",i,"JMP",numer(i));
		i+=sizeof(int);break;
	case I_JZ: printf("%d:\t%s\t%d\n",i,"JZ",numer(i));
		i+=sizeof(int);break;
	case I_JNZ: printf("%d:\t%s\t%d\n",i,"JNZ",numer(i));
		i+=sizeof(int);break;
	case I_READ: printf("%d:\t%s\t%d\n",i,"READ",numer(i));
		i+=sizeof(int);break;
	case I_WRITE: printf("%d:\t%s\n",i,"WRITE");break;
	case I_WRITEC: printf("%d:\t%s\t%x\n",i,"WRITEC",c[i]);break;
	}
}
void generator(ASTnode*ast,char*nameprog)
{
	codegen(ast);
	savecode(I_HALT);
	FILE *fp = fopen(nameprog,"w+");
	
	fwrite(c,sizeof(char),cmd-c,fp);
	fclose(fp);
	printcode();
	
}