#!/bin/bash
if [ $# -lt 1 ]
then
	echo "./test.sh needs more parameters"
	echo "./test.sh cofig_filename"
	echo "try it again..."
	exit 1
fi

filename=$1
mkdir -p tmp
cfgname=$filename".cfg"
cppname=$filename".cpp"
varname=$filename".var"
invname=$filename".inv"
cfgfile="cfg/"$cfgname
cppfile="test/"$cppname
varfile="tmp/"$varname
invfile="tmp/"$invname

g++ cfg2test.cpp -o cfg2test
./cfg2test $cfgfile $cppfile $varfile $invfile
VARS=$?
rm ./cfg2test
#exit $VARS


cmakefile="./CMakeLists.txt"
echo "cmake_minimum_required (VERSION 2.8)" > $cmakefile
echo "set(VARS "$VARS")" >> $cmakefile
cat ./cmake.base >> $cmakefile
echo "add_executable("$filename" "$cppfile" \${DIR_SRCS} \${HEADER})" >> $cmakefile
echo "target_link_libraries("$filename" \${Z3_LIBRARY})" >> $cmakefile

cd build
#rm -rf *
cmake ..
make $filename
./$filename
cd ..
echo "invariant file is located at "$invfile


tmpcfg="tmp/tmp.cfg"
cp $cfgfile $tmpcfg
echo -n "invariant=" >> $tmpcfg
cat $invfile >> $tmpcfg



verfname=$filename".c"
verffile="tmp/"$verfname
g++ cfg2verf.cpp -o cfg2verf
./cfg2verf $tmpcfg $verffile
rm ./cfg2verf

verfc1=$filename"_klee1.c"
verfc2=$filename"_klee2.c"
verfc3=$filename"_klee3.c"
verfo1=$filename"_klee1.o"
verfo2=$filename"_klee2.o"
verfo3=$filename"_klee3.o"
cd tmp
rm -rf klee-*
llvm-gcc --emit-llvm -c -g $verfc1
klee $verfo1 
llvm-gcc --emit-llvm -c -g $verfc2
klee $verfo2 
llvm-gcc --emit-llvm -c -g $verfc3
klee $verfo3 

# more analysis here...

#rm $verfc1 $verfc2 $verfc3
#rm $verfo1 $verfo2 $verfo3
cd ..


#rm $tmpcfg
rm $cmakefile
#rm $varfile
#rm $invfile
exit $VARS


#K=1 
#TESTLOOP=60
#rm ./data/test
#touch ./data/test

#while [ "$K" -lt $TESTLOOP ]; do
#	random=$RANDOM
#	echo "running test case $K "
#	./exe_instrument $random  >> ./data/test
#	K=$(($K+1))
#done




cd ./data/
echo "************* start to run svm with default parameters on orignal sets************"
svm-train -t 0 -c $DOUBLE_MAX train
svm-predict train train.model train.predict
echo "************ finish running svm with default parameters on orignal sets***********"

#echo "************* start to run svm with default parameters on scaled sets*************"
#svm-scale -l -1 -u 1 train > train.scale
#svm-scale -l -1 -u 1 test > test.scale
#svm-train -t 0 train.scale
#svm-predict train.scale train.scale.model train.scale.predict
#echo "************ finish running svm with default parameters on scaled  sets***********"
cd ..
svm2plane -m data/train.model -t data/train
