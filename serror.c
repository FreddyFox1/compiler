#include <preproc.h>
// вывод сообщения об ошибке и выход
void serror(char*s)
{
	printf("%s\n",s);printf("\n");
	exit(0);
}
