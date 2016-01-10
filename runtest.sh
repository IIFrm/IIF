#!/bin/bash
if [ $# -lt 1 ]
then
	echo "./test.sh needs more parameters"
	echo "./test.sh cofig_filename"
	echo "try it again..."
	exit 1
fi

filename=$1
cfgname=$filename".cfg"
cppname=$filename".cpp"
cfgfile="./test/"$cfgname
cppfile="./test/"$cppname

g++ cfg2cpp.cpp -o cfg2cpp
./cfg2cpp $cfgfile $cppfile
VARS=$?
rm ./cfg2cpp

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

rm $cmakefile
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
