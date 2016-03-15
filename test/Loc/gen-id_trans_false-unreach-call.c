// from FreePastry source, file Id.java

int main() {
  int idBitLength, material_length, nlen;
  int j, k;

  //  nlen = __VERIFIER_nondet_int();
  //idBitLength = __VERIFIER_nondet_int();
  // material_length = __VERIFIER_nondet_int();
   assume( nlen  ==  idBitLength / 32 );

  for (j = 0; (j < idBitLength / 8) && (j < material_length); j++) {
    // assert( 0 <= j);
    // assert( j < material_length );
    // assert( 0 <= j/4 );
    // assert( j/4 < nlen);
  }

  return 0;
}
