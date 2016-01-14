#!/bin/bash
function findSMT4Z3(){
	echo "in findSMT4Z3 funtion..."
	filename="failAssert0000";
	n=9
	i=1
	tmpfile="result"
	while [ $i -lt $n ]; do
		echo -n "processing "$filename""$i".smt2-->"
		if [ ! -f $filename""$i".smt2" ]; then
			echo  "No such file."
			return $((Si-1))
		fi
		#echo -n "processing "$filename""$i".smt2-->"
		z3 $filename""$i".smt2" > $tmpfile""$i
		read result < $tmpfile""$i
		echo "result=="$result
		if [ $result == "unsat" ]; then
			i=$(($i+1))
		else
			echo "NOT A VALID INVARIVANT..."
			return 255
		fi
	done
	return 0
}


if [ $# -lt 1 ]
then
	echo "./test.sh needs more parameters"
	echo "./test.sh cofig_filename"
	echo "try it again..."
	exit 1
fi

blue="\033[33;\x1b[34m"
white="\033[0m"

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



echo -n -e $blue"Converting the given config file to a valid cplusplus file..."$white
g++ cfg2test.cpp -o cfg2test
./cfg2test $cfgfile $cppfile $varfile $invfile
VARS=$?
echo -e $blue"[DONE]"$white
rm ./cfg2test
#exit $VARS




echo -n -e $blue"Generating CMakeLists file for further construction..."$white
cmakefile="./CMakeLists.txt"
echo "cmake_minimum_required (VERSION 2.8)" > $cmakefile
echo "set(VARS "$VARS")" >> $cmakefile
cat ./cmake.base >> $cmakefile
echo "add_executable("$filename" "$cppfile" \${DIR_SRCS} \${HEADER})" >> $cmakefile
echo "target_link_libraries("$filename" \${Z3_LIBRARY})" >> $cmakefile
echo -e $blue"[DONE]"$white




echo -e $blue"Build the project..."$white
cd build
#rm -rf *
cmake ..
make $filename
echo -e $blue"Running the project to generate invariant candidiate..."$white
./$filename
cd ..
echo -e $blue"Invariant file is located at "$invfile""$white
cat $invfile
echo ""





echo -n -e $blue"Generating a new config file contains the invariant candidate..."$white
tmpcfg="tmp/tmp.cfg"
cp $cfgfile $tmpcfg
echo -n "invariant=" >> $tmpcfg
cat $invfile >> $tmpcfg
echo -e $blue"[Done]"$white





echo -n -e $blue"Generating three C files to do the verification by KLEE"$white
verfname=$filename".c"
verffile="tmp/"$verfname
g++ cfg2verf.cpp -o cfg2verf
./cfg2verf $tmpcfg $verffile
rm ./cfg2verf
echo -e $blue"[Done]"$white




verfc1=$filename"_klee1.c"
verfc2=$filename"_klee2.c"
verfc3=$filename"_klee3.c"
verfo1=$filename"_klee1.o"
verfo2=$filename"_klee2.o"
verfo3=$filename"_klee3.o"






cd tmp
rm -rf klee-*
echo -e $blue"Compiling the C files and Run KLEE..."$white
echo -e $blue"Compiling the C files and Run KLEE...1"$white
llvm-gcc --emit-llvm -c -g $verfc1
klee -write-smt2s $verfo1 
ret=$?
findSMT4Z3
ret=$?
if [ $ret -ne 0 ]; then
	echo "PRECONDITION. stop here..."
	exit $ret
fi


echo -e $blue"Compiling the C files and Run KLEE...2"$white
llvm-gcc --emit-llvm -c -g $verfc2
klee -write-smt2s $verfo2 
ret=$?
findSMT4Z3
ret=$?
if [ $ret -ne 0 ]; then
	echo "INDUCTIVE FAILED. stop here..."
	exit $ret
fi


echo -e $blue"Compiling the C files and Run KLEE...3"$white
llvm-gcc --emit-llvm -c -g $verfc3
klee -write-smt2s $verfo3 
ret=$?
findSMT4Z3
ret=$?
if [ $ret -ne 0 ]; then
	echo "POSTCONDITION FAILED. stop here..."
	exit $ret
fi
#if [ $ret -ne 2 ]
#then
#	echo "POST COUNTER EXAMPLE..."
#	exit $ret
#fi


# more analysis here...

#rm $verfc1 $verfc2 $verfc3
#rm $verfo1 $verfo2 $verfo3
echo -e $blue"[DONE]"$white
cd ..


#rm $tmpcfg
rm $cmakefile
#rm $varfile
#rm $invfile
echo -e $blue"END HERE..."$white
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
