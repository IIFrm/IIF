/*************************************************************************
    > File Name: test.c
    > Author: Li Jiaying
    > Mail: lijiaying1989@gmail.com 
    > Created Time: 2015年10月04日 星期日 21时32分15秒
 ************************************************************************/

#include <stdio.h>

int main()
{
	int x = -2147483648;
	int y = -1073741825;
	int z = x + y;
	printf ("%d + %d = %d\n", x, y, z);
	return 0;
}
