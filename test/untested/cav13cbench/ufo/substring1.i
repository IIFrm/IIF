# 1 "substring1.c"
# 1 "<built-in>"
# 1 "<command-line>"
# 1 "substring1.c"
void main () {
int i, j;
int from;
int to;
int k;

if (!(k >= 0)) return;
if (!(k <= 100)) return;

if (!(from >= 0)) return;
if (!(from <= k)) return;

i = from;
j = 0;

while (i < k) {
i++;
j++;
}

if (j >= 101)
  goto ERROR;

  return;

ERROR:;

}
