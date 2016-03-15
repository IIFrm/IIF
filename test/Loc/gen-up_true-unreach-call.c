
int main() {
  int n;
  int i = 0;
  int k = 0;
  //n = __VERIFIER_nondet_int();
  while( i < n ) {
    //assert (i=k && i<n)
	i++;
	k++;
  }
  int j = 0;
  while( j < n ) {
    //assert (k > 0);
    j++;
    k--;
  }
}
