# 1 "xy0.c"
# 1 "<built-in>"
# 1 "<command-line>"
# 1 "xy0.c"
int nondet();

int main (){

 int x = 0;
 int y = 0;

  while (nondet()){
    x += 1;
    y += 1;
  }

  while (x > 0){
    x = x - 1;
    y = y - 1;
  }

  if (y <= -1)
    goto ERROR;

  return 1;

  ERROR:;
}
