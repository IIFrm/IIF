#!/bin/bash
file=$1
#remove more space
sed -i 's/\ \ /\ /g' $file
sed -i 's/(\ /(/g' $file
sed -i 's/\ )/)/g' $file
#sed -i 's/)\ (/)(/g' $file
sed -i 's/\n\n/\n/g' $file

sed -i 's/\?//g' $file

sed -i 's/(set-logic/;(set-logic/g' $file
#sed -i '1d' $file
sed -i 's/(Array\ (_\ BitVec\ 32)\ (_\ BitVec\ 8)))/\ Int)/g' $file

# remove (_ bv12 32)
sed -i 's/(_\ bv\([0-9]*\)\ 32)/\1/g' $file

#replace bvslt/bvsle with <= operator
sed -i 's/bvslt/</g' $file
sed -i 's/bvsgt/>/g' $file
sed -i 's/bvsle/<=/g' $file
sed -i 's/bvsge/>=/g' $file

# replace bv*** with +-*/
sed -i 's/bvadd/+/g' $file
sed -i 's/bvsub/-/g' $file
sed -i 's/bvmul/*/g' $file
sed -i 's/bvdiv/\//g' $file

# (bvxor -1 m) == (- -1 m)
sed -i 's/(bvxor\ 4294967295\ /(-\ -1\ /g' $file

sed -i 's/\([2-4][0-9]\{9\}\)/(-\ \1\ 4294967296)/g' $file
#sed -i 's/(+\ 4294967295\ \([a-zA-Z_][a-zA-Z0-9_]*\))/(-\ \1 1)/g' $file
#sed -i 's/(+\ 4294967294\ \([a-zA-Z_][a-zA-Z0-9_]*\))/(-\ \1 2)/g' $file
#sed -i 's/(+\ 4294967293\ \([a-zA-Z_][a-zA-Z0-9_]*\))/(-\ \1 3)/g' $file
#sed -i 's/(+\ 4294967292\ \([a-zA-Z_][a-zA-Z0-9_]*\))/(-\ \1 4)/g' $file
#sed -i 's/(+\ 4294967291\ \([a-zA-Z_][a-zA-Z0-9_]*\))/(-\ \1 5)/g' $file
#sed -i 's/(+\ 4294967290\ \([a-zA-Z_][a-zA-Z0-9_]*\))/(-\ \1 6)/g' $file
#sed -i 's/(+\ 4294967289\ \([a-zA-Z_][a-zA-Z0-9_]*\))/(-\ \1 7)/g' $file
#sed -i 's/(+\ 4294967288\ \([a-zA-Z_][a-zA-Z0-9_]*\))/(-\ \1 8)/g' $file
#sed -i 's/(+\ 4294967287\ \([a-zA-Z_][a-zA-Z0-9_]*\))/(-\ \1 10)/g' $file
#sed -i 's/(+\ 4294967286\ \([a-zA-Z_][a-zA-Z0-9_]*\))/(-\ \1 11)/g' $file
#sed -i 's/(+\ 4294967285\ \([a-zA-Z_][a-zA-Z0-9_]*\))/(-\ \1 12)/g' $file
#sed -i 's/(+\ 4294967284\ \([a-zA-Z_][a-zA-Z0-9_]*\))/(-\ \1 13)/g' $file
#sed -i 's/(+\ 4294967283\ \([a-zA-Z_][a-zA-Z0-9_]*\))/(-\ \1 14)/g' $file
#sed -i 's/(+\ 4294967282\ \([a-zA-Z_][a-zA-Z0-9_]*\))/(-\ \1 15)/g' $file
#sed -i 's/(+\ 4294967281\ \([a-zA-Z_][a-zA-Z0-9_]*\))/(-\ \1 16)/g' $file
#sed -i 's/(+\ 4294967280\ \([a-zA-Z_][a-zA-Z0-9_]*\))/(-\ \1 17)/g' $file
#sed -i 's/(+\ 4294967279\ \([a-zA-Z_][a-zA-Z0-9_]*\))/(-\ \1 18)/g' $file
#sed -i 's/(+\ 4294967278\ \([a-zA-Z_][a-zA-Z0-9_]*\))/(-\ \1 19)/g' $file
#sed -i 's/(+\ 4294967277\ \([a-zA-Z_][a-zA-Z0-9_]*\))/(-\ \1 20)/g' $file
#sed -i 's/(+\ 4294967276\ \([a-zA-Z_][a-zA-Z0-9_]*\))/(-\ \1 21)/g' $file
#sed -i 's/(+\ 4294967275\ \([a-zA-Z_][a-zA-Z0-9_]*\))/(-\ \1 22)/g' $file
#sed -i 's/(+\ 4294967274\ \([a-zA-Z_][a-zA-Z0-9_]*\))/(-\ \1 23)/g' $file
#sed -i 's/(+\ 4294967273\ \([a-zA-Z_][a-zA-Z0-9_]*\))/(-\ \1 24)/g' $file
#sed -i 's/(+\ 4294967272\ \([a-zA-Z_][a-zA-Z0-9_]*\))/(-\ \1 25)/g' $file
#sed -i 's/(+\ 4294967271\ \([a-zA-Z_][a-zA-Z0-9_]*\))/(-\ \1 26)/g' $file
#sed -i 's/(+\ 4294967270\ \([a-zA-Z_][a-zA-Z0-9_]*\))/(-\ \1 27)/g' $file
#sed -i 's/(+\ 4294967269\ \([a-zA-Z_][a-zA-Z0-9_]*\))/(-\ \1 28)/g' $file
#sed -i 's/(+\ 4294967268\ \([a-zA-Z_][a-zA-Z0-9_]*\))/(-\ \1 29)/g' $file
#sed -i 's/(+\ 4294967267\ \([a-zA-Z_][a-zA-Z0-9_]*\))/(-\ \1 30)/g' $file
#sed -i 's/(+\ 4294967266\ \([a-zA-Z_][a-zA-Z0-9_]*\))/(-\ \1 31)/g' $file
#sed -i 's/(+\ 4294967265\ \([a-zA-Z_][a-zA-Z0-9_]*\))/(-\ \1 32)/g' $file

#replace concat of bitvector vector with int 
sed -i 's/(concat\ (select\ \([a-zA-Z][a-zA-A0-9_]*\)\ 3)\ (concat\ (select\ \([a-zA-Z][a-zA-A0-9_]*\)\ 2)\ (concat\ (select\ \([a-zA-Z][a-zA-A0-9_]*\)\ 1)\ (select\ \([a-zA-Z][a-zA-A0-9_]*\)\ 0))))/\1\ /g' $file

# remove (check-sat)
sed -i 's/(check-sat)/;(check-sat)/g' $file
# remove (get-model)
sed -i 's/(get-model)/;(get-model)/g' $file
# remove (exit)
sed -i 's/(exit)/;(exit)/g' $file

#remove more space
sed -i 's/\ \ /\ /g' $file
sed -i 's/(\ /(/g' $file
sed -i 's/\ )/)/g' $file
#sed -i 's/)\ (/)(/g' $file
sed -i 's/\n\n/\n/g' $file
