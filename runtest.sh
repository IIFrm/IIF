#!/bin/bash
red="\033[33;\x1b[31m"
green="\033[33;\x1b[32m"
yellow="\033[33;\x1b[33m"
blue="\033[33;\x1b[34m"
white="\033[0m"

function func_findSmtForZ3(){
	#echo "in func_findSmtForZ3 funtion..."
	smtname="failAssert0000";
	n=99
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

function func_numOfInvCands(){
	inv_prefix=$1
	#echo "in func_numOfInvCands funtion..."
	n=99
	i=0
	tmpfile="result"
	while [ $i -lt $n ]; do
		if [ ! -f $inv_prefix"_"$i".inv" ]; then
			#echo  "No such file."
			return $i 
		else
			i=$(($i+1))
		fi
	done
	return $n
}


if [ $# -lt 1 ]
then
	echo "./test.sh needs more parameters"
	echo "./test.sh cofig_prefix"
	echo "try it again..."
	exit 1
fi

dir_cfg="cfg/"
dir_test="test/"
dir_temp="tmp/"


prefix=$1
mkdir -p tmp
file_cfg=$prefix".cfg"
file_cpp=$prefix".cpp"
file_var=$prefix".var"
file_inv=$prefix".inv"
path_cfg=$dir_cfg""$file_cfg
path_cpp=$dir_test""$file_cpp
path_var=$dir_temp""$file_var
path_inv=$dir_temp""$file_inv
prefix_path_inv=$dir_temp""$prefix

file_verf=$prefix".c"
path_verf=$dir_temp""$file_verf
file_c1_verf=$prefix"_klee1.c"
file_c2_verf=$prefix"_klee2.c"
file_c3_verf=$prefix"_klee3.c"
file_o1_verf=$prefix"_klee1.o"
file_o2_verf=$prefix"_klee2.o"
file_o3_verf=$prefix"_klee3.o"


#**********************************************************************************************
# Learning phase
#**********************************************************************************************
##########################################################################
# Prepare the target loop program
##########################################################################
echo -n -e $blue"Converting the given config file to a valid cplusplus file..."$white
g++ cfg2test.cpp -o cfg2test
ret=$?
if [ $ret -ne 0 ]; then
	echo "cfg2test.cpp compiling error, stop here."
	exit $ret 
fi

if [ $# -ge 2 ]; then
	if [ $# -ge 3 ]; then
		./cfg2test $path_cfg $path_cpp $path_var $prefix_path_inv $2 $3
	else
		./cfg2test $path_cfg $path_cpp $path_var $prefix_path_inv $2
	fi
else
	./cfg2test $path_cfg $path_cpp $path_var $prefix_path_inv
fi
Nv=$?
rm ./cfg2test


##########################################################################
# Generate CMakeLists from cmake.base and Nv value
##########################################################################
echo -n -e $blue"Generating CMakeLists file for further construction..."$white
cmakefile="./CMakeLists.txt"
echo "cmake_minimum_required (VERSION 2.8)" > $cmakefile
echo "set(Nv "$Nv")" >> $cmakefile
cat ./cmake.base >> $cmakefile
echo "add_executable("$prefix" "$path_cpp" \${DIR_SRCS} \${HEADER})" >> $cmakefile
echo "target_link_libraries("$prefix" \${Z3_LIBRARY})" >> $cmakefile
echo -e $blue"[DONE]"$white



##########################################################################
# Build the project
##########################################################################
echo -e $blue"Build the project..."$white
cd build
#rm -rf *
cmake ..
make $prefix
if [ $? -ne 0 ]; then
	echo "make error, contact developer to fix project source code first..."
	exit 1
fi

##########################################################################
# Run the target to get Invariant Candidates
##########################################################################
echo -e $blue"Running the project to generate invariant candidiate..."$white
./$prefix
ret=$?
if [ $ret -ne 0 ]; then
	echo "can not separate using default paramater"
	echo "try more parameters to get a perfect classifier"
	exit 1
fi


# change to project home dir
cd ..

##########################################################################
# From inv files to prepare for verification step
##########################################################################
multi_candidates=1
if [ -f $path_inv ]; then
	echo -e $blue"Invariant file is located at "$path_inv""$white
	multi_candidates=0
	cat $path_inv
	echo ""
else
	echo -e $blue"Invariant file is located at "$prefix_path_inv"_**.inv"$white
	func_numOfInvCands $prefix_path_inv
	numInvCands=$?
	#echo "numInvCands="$numInvCands
	i=0
	while [ $i -lt $numInvCands ]; do
		echo -n $i"-->"
		cat $prefix_path_inv"_"$i".inv"
		echo ""
		i=$(($i+1))
	done
	echo ""
fi

if [ $multi_candidates -lt 0 ]; then
	path_inv=$prefix_path_inv"_0.inv"
fi

##########################################################################
# Generating a new config file contains the invariant candidate...
##########################################################################
echo -n -e $blue"Generating a new config file contains the invariant candidate..."$white
path_tmp_cfg="tmp/tmp.cfg"
cp $path_cfg $path_tmp_cfg
echo -n "invariant=" >> $path_tmp_cfg
cat $path_inv >> $path_tmp_cfg
echo -e $blue"[Done]"$white









#**********************************************************************************************
# verification phase
#**********************************************************************************************
##########################################################################
# Generate C files to verify using cfg file and inv file
##########################################################################
echo -n -e $blue"Generating three C files to do the verification by KLEE"$white
g++ cfg2verf.cpp -o cfg2verf
ret=$?
if [ $ret -ne 0 ]
then
	echo "cfg2verf.cpp compiling error, stop here."
	exit $ret 
fi
./cfg2verf $path_tmp_cfg $path_verf
rm ./cfg2verf
echo -e $blue"[Done]"$white



##########################################################################
# File preparation for verificattion
##########################################################################
cd $dir_temp
mkdir -p $prefix"_klee1" $prefix"_klee2" $prefix"_klee3"
mv $file_c1_verf $prefix"_klee1"
mv $file_c2_verf $prefix"_klee2"
mv $file_c3_verf $prefix"_klee3"


##########################################################################
# Verify Precondition=>Invariant
##########################################################################
cd $prefix"_klee1" 
rm -rf klee-*
rm -rf *.smt2
echo -e $blue"Compiling the C files and Run KLEE..."$white
echo -e $blue"Compiling the C files and Run KLEE...1"$white
llvm-gcc --emit-llvm -c -g $file_c1_verf
klee -write-smt2s $file_o1_verf
ret=$?
func_findSmtForZ3
ret=$?
if [ $ret -ne 0 ]; then
	echo -n -e $red">>>NOT A VALID INVARIVANT..."
	echo -e "Reason: Property I (precondition ==> invariant) FAILED. stop here..."$white
	exit $ret
fi
cd ..


##########################################################################
# Verify Invariant & loop_condition ==LOOP==> Invariant
##########################################################################
cd $prefix"_klee2" 
rm -rf klee-*
rm -rf *.smt2
echo -e $blue"Compiling the C files and Run KLEE..."$white
echo -e $blue"Compiling the C files and Run KLEE...1"$white
llvm-gcc --emit-llvm -c -g $file_c2_verf
klee -write-smt2s $file_o2_verf
ret=$?
func_findSmtForZ3
ret=$?
if [ $ret -ne 0 ]; then
	echo -n -e $red">>>NOT A VALID INVARIVANT..."
	echo -e "Reason: Property II (invariant && loopcondition =S=> invariant) FAILED. stop here..."$white
	exit $ret
fi
cd ..


##########################################################################
# Verify Invariant & !loopcondition => postcondition
##########################################################################
cd $prefix"_klee3" 
rm -rf klee-*
rm -rf *.smt2
echo -e $blue"Compiling the C files and Run KLEE...3"$white
llvm-gcc --emit-llvm -c -g $file_c3_verf
klee -write-smt2s $file_o3_verf
ret=$?
func_findSmtForZ3
ret=$?
if [ $ret -ne 0 ]; then
	echo -n -e $red">>>NOT A VALID INVARIVANT..."
	echo -e "Reason: Property III (invariant && ~loopcondition ==> postcondition) FAILED. stop here..."$white
	exit $ret
fi
cd ..
