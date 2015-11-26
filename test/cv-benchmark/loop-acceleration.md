#sv-benchmarks *c*

##loop-acceleration


###array_false-unreach-call1.call, array_true-unreach-call1.c
```
  int A[2048];
  int i;
  for (i = 0; i < 1024; i++) {
    A[i] = i;
  }
  __VERIFIER_assert(A[1023] != 1023);
```
for all m < i, A[m] = m 



###array_false-unreach-call2.call, array_true-unreach-call2.c
```
  int A[SZ];
  int B[SZ];
  int i;
  int tmp;

  for (i = 0; i < SZ; i++) {
    tmp = A[i];
    B[i] = tmp;
  }
  __VERIFIER_assert(A[SZ/2] != B[SZ/2]);
```
for all m < i, A[m] = B[m]



###array_false-unreach-call3.call, array_true-unreach-call3.c
```
  int A[N];
  int i;

  for (i = 0; A[i] != 0 && i < N; i++) {
  }
  __VERIFIER_assert(i <= N / 2);
```
i <= N




###array_true-unreach-call4.c
```
  int A[N];
  int i;

  A[N-1] = 0;

  for (i = 0; A[i] != 0; i++) {
  }
  __VERIFIER_assert(i <= N);
```
i <= N - 1



###const_false-unreach-call1.c
```
  unsigned int x = 1;
  unsigned int y = 0;

  while (y < 1024) {
    x = 0;
    y++;
  }
  __VERIFIER_assert(x == 1);
```
y != 0 => x = 0

x * y = 0



###diamond_false-unreach-call1.c
```
  unsigned int x = 0;
  unsigned int y = __VERIFIER_nondet_uint();

  while (x < 99) {
    if (y % 2 == 0) {
      x++;
    } else {
      x += 2;
    }
  }
  __VERIFIER_assert((x % 2) == (y % 2));
```
y:odd => x:even
y:even => x <= 99




###diamond_true-unreach-call1.c
```
  unsigned int x = 0;
  unsigned int y = __VERIFIER_nondet_uint();

  while (x < 99) {
    if (y % 2 == 0) {
      x += 2;
    } else {
      x++;
    }
  }
  __VERIFIER_assert((x % 2) == (y % 2));
```
y:even => x:even
y:odd => x <= 99



###diamond_true-unreach-call2.c
```
  unsigned int x = 0;
  unsigned int y = __VERIFIER_nondet_uint();

  while (x < 99) {
    if (y % 2 == 0) x += 2;
    else x++;

    if (y % 2 == 0) x += 2;
    else x -= 2;

    if (y % 2 == 0) x += 2;
    else x += 2;

    if (y % 2 == 0) x += 2;
    else x -= 2;

    if (y % 2 == 0) x += 2;
    else x += 2;

    if (y % 2 == 0) x += 2;
    else x -= 4;

    if (y % 2 == 0) x += 2;
    else x += 4;

    if (y % 2 == 0) x += 2;
    else x += 2;

    if (y % 2 == 0) x += 2;
    else x -= 4;

    if (y % 2 == 0) x += 2;
    else x -= 4;
  }

  __VERIFIER_assert((x % 2) == (y % 2));
```
y:even => x:even
y:odd => ???


###function_false-unreach-call1.c, function_true-unreach-call1.c
```
unsigned int f(unsigned int z) {
  return z + 2;
}

  unsigned int x = 0;

  while (x < 0x0fffffff) {
    x = f(x);
  }

  __VERIFIER_assert(x % 2);
```
x%2=0


###multivar_false-unreach-call1
```
  unsigned int x = __VERIFIER_nondet_uint();
  unsigned int y = x + 1;

  while (x < 1024) {
    x++;
    y++;
  }
  __VERIFIER_assert(x == y);
```
x + 1 = y


###multivar_true-unreach-call1
```
  unsigned int x = __VERIFIER_nondet_uint();
  unsigned int y = x;

  while (x < 1024) {
    x++;
    y++;
  }
  __VERIFIER_assert(x == y);
```
x = y


###nested_true-unreach-call1
```
  unsigned int x = 0;
  unsigned int y = 0;

  while (x < 0x0fffffff) {
    y = 0;

    while (y < 10) {
      y++;
    }

    x++;
  }

  __VERIFIER_assert(x % 2);
```
x <= 0x0fffffff


###overflow_true-unreach-call1.c
```
  unsigned int x = 10;

  while (x >= 10) {
    x += 2;
  }

  __VERIFIER_assert(!(x % 2));
```
x >= 10 \/ x = 1??(overflow)



###phases_false-unreach-call...
###simple_false-unreach-call...
###underapprox_false-unreach-call...