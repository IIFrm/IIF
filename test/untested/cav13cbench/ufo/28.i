# 1 "28.c"
# 1 "<built-in>"
# 1 "<command-line>"
# 1 "28.c"
int unknown1(){
    int x; return x;
}

int unknown2();
int unknown3();
int unknown4();






void main() {
  int x=0;
  int y=0;
  int n = 0;
  while(unknown1()) {
      x++;
      y++;
  }
  while(x <= n - 1 || x >= n + 1) {
      x--;
      y--;
  }
  if(x != n)
    return;
  if(y != n){
   goto ERROR; ERROR:;
  }
}
