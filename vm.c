#include <vm.h>
#include <stdio.h>
#include <stdlib.h>
static Code code[PROGSIZE];
static int memory[MEMSIZE];
static int numer(int pc)
{
	return *(int*)(code + pc);
}
void PRINT(int pc,int sp)
{
	Code *c = code;
	switch(c[pc]) {
	case I_HALT: puts("HALT");break;
	case I_ADD: puts("ADD");break;
	case I_SUB: puts("SUB");break;
	case I_MUL: puts("MUL");break;
	case I_DIV: puts("DIV");break;
	case I_NEG: puts("NEG");break;
	case I_NOT: puts("NOT");break;
	case I_LT: puts("LT");break;
	case I_LE: puts("LE");break;
	case I_GT: puts("GT");break;
	case I_GE: puts("GE");break;
	case I_EQ: puts("EQ");break;
	case I_NE: puts("NE");break;
	case I_LOAD: printf("LOAD %d\n",numer(pc+1));break;
	case I_SAVE: printf("SAVE %d\n",numer(pc+1));break;
	case I_PUSH: printf("PUSH %d\n",numer(pc+1));break;
	case I_JMP: printf("JMP %d\n",numer(pc+1));break;
	case I_JZ: printf("JZ %d\n",numer(pc+1));break;
	case I_JNZ: printf("JNZ %d\n",numer(pc+1));break;
	case I_READ: printf("READ %d\n",numer(pc+1));break;
	case I_WRITE: puts("WRITE");break;
	case I_WRITEC: printf("WRITEC %x\n",c[pc+1]);break;
	}
	printf("pc = %d sp = %d\n",pc,sp);
	for(int i=0;i<=sp;i++) printf("stack = %d\n",memory[i]);
}

void execute(int*DS,int*SS)
{
//	int *stack=memory,*ds = memory + MEMSIZE-1;
	int sp=-1,pc=0;
start:
//	PRINT(pc,sp);
	switch(code[pc]) {
	case I_HALT:
		break;
	
	case I_ADD:
		SS[sp-1] += SS[sp]; sp--;pc++;
		goto start;
	case I_SUB:
		SS[sp-1] -= SS[sp]; sp--;pc++;
		goto start;
	case I_MUL:
		SS[sp-1] *= SS[sp]; sp--;pc++;
		goto start;
	case I_DIV:
		SS[sp-1] /= SS[sp]; sp--;pc++;
		goto start;
	case I_NEG:
		SS[sp] = -SS[sp];pc++;
		goto start;
	case I_NOT:
		SS[sp] = !SS[sp];pc++;
		goto start;
	case I_LT:
		SS[sp-1] = SS[sp-1] < SS[sp];sp--;pc++;
		goto start;
	case I_LE:
		SS[sp-1] = SS[sp-1] <= SS[sp];sp--;pc++;
		goto start;
	case I_GT:
		SS[sp-1] = SS[sp-1] > SS[sp];sp--;pc++;
		goto start;
	case I_GE:
		SS[sp-1] = SS[sp-1] >= SS[sp];sp--;pc++;
		goto start;
	case I_EQ:
		SS[sp-1] = SS[sp-1] == SS[sp];sp--;pc++;
		goto start;
	case I_NE:
		SS[sp-1] = SS[sp-1] != SS[sp];sp--;pc++;
		goto start;
	case I_LOAD:
		sp++;pc++;
		SS[sp] = DS[ -numer(pc)  ]; pc+=sizeof(int);
		goto start;
	case I_SAVE:
		pc++;
		DS[ -numer(pc) ] = SS[sp]; sp--; pc += sizeof(int);
		goto start;
	case I_PUSH:
		sp++;pc++; SS[sp] = numer(pc); pc += sizeof(int);
		goto start;
	case I_JMP:
		pc += numer(pc+1);
		goto start;
	case I_JZ:
		if(SS[sp] == 0) pc += numer(pc+1); else pc += 1+sizeof(int);
		sp--;
		goto start;
	case I_JNZ:
		if(SS[sp] != 0) pc += numer(pc+1); else pc+=1+sizeof(int);
		sp--;
		goto start;
	case I_READ:
		putchar('?');scanf("%d",& DS[ -SS[sp] ]);
		while( getchar() != '\n');
		sp++;pc++;
		goto start;
	case I_WRITE:
		printf("%d",SS[sp]);sp--;pc++;
		goto start;
	case I_WRITEC:
		printf("%c",code[pc+1]);pc+=2;
		goto start;
	
	default:
		printf("VM: code incorrect\n");exit(0);
	}
}
int main(int argc,char**argv)
{

	FILE *fp = fopen("p3.cod","rb");
	
	int codesize=0;
	//code = (Code*)memory;
	while( !feof(fp) ) {
		code[codesize++] = fgetc(fp); // код -- один байт
		if(codesize == PROGSIZE*sizeof(int)) {
			puts("Too big program");exit(0);
		}
	}
	
	int*DS = memory + MEMSIZE-1;
	int*SS = memory;
	execute(DS,SS);
	
}	
