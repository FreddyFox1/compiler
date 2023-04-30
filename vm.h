#ifndef _VM_H_
#define _VM_H_

#define PROGSIZE 8000
#define MEMSIZE 10000

// команды процессора
typedef enum {
I_HALT=100,
I_ADD,I_SUB,I_MUL,I_DIV,I_NEG,	// арифметические
I_NOT, 		// логические
I_LT,I_LE,I_GT,I_GE,I_EQ,I_NE,	// отношения
I_LOAD,I_SAVE,			// запись-чтение памяти
I_PUSH,				// поместить в стек
I_JMP,I_JZ,I_JNZ,		// переходы
I_READ,I_WRITE,I_WRITEC		// ввод-вывод
} ICODE;

typedef unsigned char Code;
#endif
