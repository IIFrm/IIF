#!/bin/bash
red="\033[33;\x1b[31m"
green="\033[33;\x1b[32m"
yellow="\033[33;\x1b[33m"
blue="\033[33;\x1b[34m"
white="\033[0m"

function findSMT4Z3(){
	#echo "in findSMT4Z3 funtion..."
	smtname="failAssert0000";
	n=9
	i=1
	tmpfile="result"
	while [ $i -lt $n ]; do
		if [ ! -f $smtname""$i".smt2" ]; then
			#echo  "No such file."
			return 0
		fi
		echo -n "processing "$smtname""$i".smt2 --> "
		z3 $smtname""$i".smt2" > $tmpfile""$i
		read result < $tmpfile""$i
		rm $tmpfile""$i
		if [ $result == "unsat" ]; then
			echo -e $green$result$white
			i=$(($i+1))
		else
			echo -e $red$result$white
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
ret=$?
if [ $ret -ne 0 ]
then
	echo "cfg2test.cpp compiling error, stop here."
	exit $ret 
fi

if [ $# -ge 2 ]
then
	./cfg2test $cfgfile $cppfile $varfile $invfile $2
else
	./cfg2test $cfgfile $cppfile $varfile $invfile
fi
Nv=$?
rm ./cfg2test
#exit $Nv


echo -n -e $blue"Generating CMakeLists file for further construction..."$white
cmakefile="./CMakeLists.txt"
echo "cmake_minimum_required (VERSION 2.8)" > $cmakefile
echo "set(Nv "$Nv")" >> $cmakefile
cat ./cmake.base >> $cmakefile
echo "add_executable("$filename" "$cppfile" \${DIR_SRCS} \${HEADER})" >> $cmakefile
echo "target_link_libraries("$filename" \${Z3_LIBRARY})" >> $cmakefile
echo -e $blue"[DONE]"$white




echo -e $blue"Build the project..."$white
cd build
#rm -rf *
cmake ..
make $filename
if [ $? -ne 0 ]
then
	echo "make error, contact developer to fix project source code first..."
	exit 1
fi

echo -e $blue"Running the project to generate invariant candidiate..."$white
./$filename
ret=$?
if [ $ret -ne 0 ]
then
	echo "can not separate using default paramater"
	echo "try more parameters to get a perfect classifier"
	exit 1
fi

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
ret=$?
if [ $ret -ne 0 ]
then
	echo "cfg2verf.cpp compiling error, stop here."
	exit $ret 
fi
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

mkdir -p $filename"_klee1" $filename"_klee2" $filename"_klee3"
mv $verfc1 $filename"_klee1"
mv $verfc2 $filename"_klee2"
mv $verfc3 $filename"_klee3"


cd $filename"_klee1" 
rm -rf klee-*
rm -rf *.smt2
echo -e $blue"Compiling the C files and Run KLEE..."$white
echo -e $blue"Compiling the C files and Run KLEE...1"$white
llvm-gcc --emit-llvm -c -g $verfc1
klee -write-smt2s $verfo1 
ret=$?
findSMT4Z3
ret=$?
if [ $ret -ne 0 ]; then
	echo -n -e $red">>>NOT A VALID INVARIVANT..."
	echo -e "Reason: Property I (precondition ==> invariant) FAILED. stop here..."$white
	exit $ret
fi
cd ..


cd $filename"_klee2" 
rm -rf klee-*
rm -rf *.smt2
echo -e $blue"Compiling the C files and Run KLEE...2"$white
llvm-gcc --emit-llvm -c -g $verfc2
klee -write-smt2s $verfo2 
ret=$?
findSMT4Z3
ret=$?
if [ $ret -ne 0 ]; then
	echo -n -e $red">>>NOT A VALID INVARIVANT..."
	echo -e "Reason: Property II (invariant && loopcondition =S=> invariant) FAILED. stop here..."$white
	exit $ret
fi
cd ..


cd $filename"_klee3" 
rm -rf klee-*
rm -rf *.smt2
echo -e $blue"Compiling the C files and Run KLEE...3"$white
llvm-gcc --emit-llvm -c -g $verfc3
klee -write-smt2s $verfo3 
ret=$?
findSMT4Z3
ret=$?
if [ $ret -ne 0 ]; then
	echo -n -e $red">>>NOT A VALID INVARIVANT..."
	echo -e "Reason: Property III (invariant && ~loopcondition ==> postcondition) FAILED. stop here..."$white
	exit $ret
fi
cd ..
#if [ $ret -ne 2 ]
#then
#	echo "POST COUNTER EXAMPLE..."
#	exit $ret
#fi


# more analysis here...

#rm $verfc1 $verfc2 $verfc3
#rm $verfo1 $verfo2 $verfo3
echo -e $yellow"[DONE]"$white
cd ..


#rm $tmpcfg
#rm $cmakefile
#rm $varfile
#rm $invfile
echo -e $yellow"END HERE..."$white
exit $Nv




