#include<stdio.h>
#include<time.h>
#include<stdlib.h>
#include<iostream>
using namespace std;


int x;
int y;
bool passP = false;
bool passQ = false;

int main (){
  x = -50;
  while (x < 0){
     x = x + y;
     y++;
  }

  if (y < 0)
    goto ERROR;

  return 1;

ERROR:;


}
