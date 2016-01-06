#include <klee/klee.h>

int main()
{
    int x, y;
    klee_make_symbolic(&x, sizeof(x), "x");
    klee_make_symbolic(&y, sizeof(y), "y");
    klee_assume(x + y >= 0);	// invariant
    klee_assume(x >= 0);    // loop condition
    //while(x>0)
    x--;
    //y++;
    klee_assert(x + y >= 0);	// invariant
}
