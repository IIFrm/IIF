#include <klee/klee.h>

int main()
 {
int x;
klee_make_symbolic(&x, sizeof(x), "x");
klee_assume((x>=0) && (x < 50));
x = x + 1;
klee_Massert(x >= 0);
return 0;
}
