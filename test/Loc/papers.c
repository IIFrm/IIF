synergy. fig 3

void foo(int y)
{
0: lock.state = U;
1: do { // assert (lock.state = L /\ x=y \/ lock.state = U /\ x!=y);
2: lock.state = L;
3: x = y;
4: if (*) {
5: lock.state = U;
6: y++;
}
7: } while (x != y)
8: if (lock.state != L)
9: error()
}


================
synergy. fig 9
void foo()
{
int x, y;
1: x = 0;
2: y = 0;
3: while (y >= 0) { // assert (y>=0);
4: y = y + x;
}
5: assert(false);
}

-- do not understand...
-- x does not change?
-- remove x=0, and get a bug trace.
-- never terminate --

======================
Program Analysis as Constraint Solving. PLDI08

Fig1.
PV1 (int y){
x:=−50;
while (x <0){ //assert (x <0 || y >0);
x:=x+y;
y++;
}
assert(y >0)
}

--fail--

------------------
Fig2
(0≤x≤51∧x=y)∨(x≥51∧y≥0∧x+y= 102).

V2(){
1. x:= 0;y:= 0;
2 while (true){
3 if(x≤50)
4  y++;
5 else
6  y--;
7 if (y <0)
8   break;
9 x++;
10}
11 assert(x= 102);
}

-- too many constraints on x and y initial values
-- run once, do not need to get a loop invariant
-- only get positive traces

----------------
Fig4a

WP1(int i,j){
assume i>=j;
x:=y:= 0;
while (x≤100){ // assert (x>=y & 0<=x<=100);
x:=x+i;
y:=y+j;
}
assert(x≥y);
}

-- x, y do not depend on the input.
-- program bugs?
-- further fix...