#include <preproc.h>

/******************************************************************************
	ЛЕКСИЧЕСКИЙ АНАЛИЗАТОР
Вход:
	имя файла-программы (программа во внешнем представлении)
Выход:
	таблица лексем (программа во внутреннем представлении)
	в форме односвязного списка
Алгоритм:
	файл загружается целиком,
	разбивается на лексемы,
	разделенные пробелами, концом строки или табуляцией
	выделенная лексема сравнивается с шаблоном внешнего представления
		при совпадении лексема добавляется в конец списка как терминал
		при несовпадении добавляется в конец списка как
							символ пользователя
	тип строки (идентификатор, число) будет
	определяться на этапе семантического анализа
*******************************************************************************/


// ключевые слова
static struct {
	Term term;
	char *name;
	} keyTab[] = {
		{T_PROG,"program"},{T_END,"end"},
		{T_IF,"if"},{T_THEN,"then"},{T_ELSE,"else"}, {T_ENDIF,"endif"},
		{T_WHILE,"while"},{T_DO,"do"},{T_ENDWHILE,"endwhile"},
		{T_ASSIGN,":="},
		{T_PRINT,"print"},{T_INPUT,"input"},{T_COMMA,","},
		{T_EQ,"="},{T_NEQ,"#"},{T_LT,"<"},{T_LE,"<="},
		{T_GT,">"},{T_GE,">="},
		{T_PLUS,"+"},{T_MINUS,"-"},{T_OR,"or"},
		{T_MUL,"*"},{T_DIV,"/"},{T_AND,"and"},
		{T_OPEN,"("},{T_CLOSE,")"},
		{T_NOT,"not"},
		{T_EMPTY,""}
};
// поиск лексемы по шаблону
char * sym_string(Term t)
{
//	puts("SYNSTR");
	for(int i=0; keyTab[i].term != T_EMPTY; i++) {
//		printf("k = %d t = %d\n",keyTab[i].term,t);
		if(keyTab[i].term == t) return keyTab[i].name;
	}
//	puts("SYNSTR1");
	return 0;

}

// таблица лексем и вспомогательно - хвост списка
static Lex *lexTab, *tail;


// добавление в конец спика
static void add_queue(Term t)
{
	Lex *n = malloc(sizeof(Lex));
	n->term = t;
	n->str[0]=0;
	n->next = 0;
	if(tail) tail->next = n; else lexTab = n;
	tail = n;
}
void lexan(char*fname,Lex**lextab)
{

	int i;
	char str[TOKENLEN+1]; // буфер для строк
	FILE *fp=fopen(fname, "rb");
	if(!fp) serror("Error by opening file\n");
	// начинаем разбор
	tail = 0;

	char c = getc(fp);	
start:
	if( c == EOF) goto end;
	switch(c) {
	case ' ':case '\t': case '\r': case '\n': // игнорируем
		break;
	case '=': add_queue(T_EQ);break;
	case '#': add_queue(T_NEQ);break;
	case ',': add_queue(T_COMMA);break;
	case '+': add_queue(T_PLUS);break;
	case '-': add_queue(T_MINUS);break;
	case '*': add_queue(T_MUL);break;
	case '/': add_queue(T_DIV);break;
	case '(': add_queue(T_OPEN);break;
	case ')': add_queue(T_CLOSE);break;
	case '<':
		c = getc(fp);
		if(c == '=') add_queue(T_LE);
		else {add_queue(T_LT);goto start;}
		break;
	case '>':
		c = getc(fp);
		if(c == '=') add_queue(T_GE);
		else {add_queue(T_GT);goto start;}
		break;
	case ':':
		c = getc(fp);
		if(c == '=') add_queue(T_ASSIGN);
		else serror("Lexan: expected :=");
		break;
	default: // числа, имена и ключевые слова
		
		if(isalpha(c)) {
			str[TOKENLEN]=0;
			for(i=0; isalnum(c) && i<TOKENLEN; i++) {
				str[i] = c; c = getc(fp);
			}
			str[i]=0;
			//printf("str = %s\n",str);
			for(i=0; keyTab[i].term != T_EMPTY; i++) {
				if( !strcmp(str,keyTab[i].name )) {
					//printf("str1 = %s %s\n",str,keyTab[i].name);
					add_queue(keyTab[i].term);
					strcpy(tail->str,str);
					goto start;
				}
			}
			add_queue(T_IDENT);
			strcpy(tail->str,str);
			goto start;
		}
		for(i=0; isdigit(c) && i<TOKENLEN; i++) {
			str[i] = c; c = getc(fp);
		}
		str[i]=0;
		//printf("str = %s\n",str);
		//printf("c1 = %c\n",c);
		//exit(0);
		add_queue(T_NUMBER);
		strcpy(tail->str,str);
		goto start;
		
	}
	
	c = getc(fp); goto start;
end:
	fclose(fp);
	*lextab = lexTab;
}
/*
Lex* nextsym(void)
{
	sym = lexTab[curlex].code;
	strncpy(name,lexTab[curlex].str,STRLEN);
	curlex++;
}
*/

