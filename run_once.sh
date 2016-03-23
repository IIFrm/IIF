#!/bin/bash
red="\033[33;\x1b[31m"
green="\033[33;\x1b[32m"
yellow="\033[33;\x1b[33m"
blue="\033[33;\x1b[34m"
white="\033[0m"

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
dir_tool="tools/bin/"


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
path_cnt=$dir_temp""$prefix".cnt"

file_verf=$prefix".c"
path_verf=$dir_temp""$file_verf
file_c1_verf=$prefix"_klee1.c"
file_c2_verf=$prefix"_klee2.c"
file_c3_verf=$prefix"_klee3.c"
file_o1_verf=$prefix"_klee1.o"
file_o2_verf=$prefix"_klee2.o"
file_o3_verf=$prefix"_klee3.o"



function func_findSmtForZ3(){
	smtname="failAssert0000";
	n=99
	i=1
	tmpfile="result"
	while [ $i -lt $n ]; do
		if [ ! -f $smtname""$i".smt2" ]; then
			#echo  "No such file."
			return 0
		fi
		path_smt2=$smtname""$i".smt2"
		path_model=$smtname""$i".model"
		echo -n "processing "$path_smt2" --> "
		## delete the last two lines, check-sat and exit
		sed '$d' -i  $smtname""$i".smt2"
		echo "(get-model)" >> $path_smt2
		echo "(exit)" >> $path_smt2

		echo -n -e $blue"Z3 solving"$white
		z3 $path_smt2 > $path_model
		
		"../../"$dir_tool"model_parser" "../../"$path_var $path_model "../../"$path_cnt
		result=$?
		if [ $result -eq 0 ]; then
			# unsat
			echo -e $green": unsat"$white
			i=$(($i+1))
		else
			echo -e $red": sat"$white
			echo "counter examples are stored in file "$path_cnt
			return 1
		fi
	done
	return 0
}


##########################################################################
# BEGINNING 
##########################################################################
cd tools
./make_tools.sh
cd ..

./build_project.sh $1

##########################################################################
# Run the target to get Invariant Candidates
##########################################################################
cd build
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
echo -e $blue"Invariant file is located at "$path_inv""$white
cat $path_inv
echo ""

##########################################################################
# Generating a new config file contains the invariant candidate...
##########################################################################
echo -n -e $blue"Generating a new config file contains the invariant candidate..."$white
path_tmp_cfg="tmp/tmp.cfg"
cp $path_cfg $path_tmp_cfg
echo -n "invariant=" >> $path_tmp_cfg
cat $path_inv >> $path_tmp_cfg
echo -e $green"[Done]"$white



#**********************************************************************************************
# verification phase
#**********************************************************************************************
##########################################################################
# Generate C files to verify using cfg file and inv file
##########################################################################
echo -n -e $blue"Generating three C files to do the verification by KLEE"$white
$dir_tool"cfg2verf" $path_tmp_cfg $path_verf
echo -e $green"[Done]"$white



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
