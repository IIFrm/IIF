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
file_var=$prefix".var"
path_var=$dir_temp""$file_var
path_cnt=$dir_temp""$prefix".cnt"

file_verif=$prefix".c"
path_verif=$dir_temp""$file_verif
file_c_verif=$prefix"_klee1.c"
file_c1_verif=$prefix"_klee1.c"
file_c2_verif=$prefix"_klee2.c"
file_c3_verif=$prefix"_klee3.c"
file_o_verif=$prefix"_klee1.o"


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
			echo -e $green": [unsat]"$white
			i=$(($i+1))
		else
			echo -e $red": [sat]"$white
			echo -e "counter examples are stored in file "$yellow$path_cnt$white
			return 1
		fi
	done
	return 0
}

function KleeVerify(){
	u=$1
	cd $prefix"_klee"$u 
	rm -rf klee-*
	rm -rf *.smt2
	echo -e $blue"Compiling the C files and Run KLEE..."$u$white
	llvm-gcc --emit-llvm -c -g $file_c_verif
	klee -write-smt2s $file_o_verif
	ret=$?
	func_findSmtForZ3
	ret=$?
	if [ $ret -ne 0 ]; then
		echo -n -e $red">>>NOT A VALID INVARIVANT..."
		if [ $u -eq 1 ]; then
			echo -e "Reason: Property I (precondition ==> invariant) FAILED. stop here..."$white
		else 
			if [ $u -eq 2 ]; then
				echo -e "Reason: Property II (invariant && loopcondition =S=> invariant) FAILED. stop here..."$white
			else 
				if [ $u -eq 3 ]; then
					echo -e "Reason: Property III (invariant && ~loopcondition ==> postcondition) FAILED. stop here..."$white
				fi
			fi
		fi
		exit $ret
	fi
	cd ..
	return 0
}


#**********************************************************************************************
# verification phase
#**********************************************************************************************
##########################################################################
# Generate C files to verify using cfg file and inv file
##########################################################################
path_tmp_cfg="tmp/tmp.cfg"
echo -n -e $blue"Generating three C files to do the verification by KLEE"$white
$dir_tool"cfg2verif" $path_tmp_cfg $path_verif
echo -e $green"[Done]"$white



##########################################################################
# File preparation for verificattion
##########################################################################
cd $dir_temp
mkdir -p $prefix"_klee1" $prefix"_klee2" $prefix"_klee3"
mv $file_c1_verif $prefix"_klee1/"$file_c_verif
mv $file_c2_verif $prefix"_klee2/"$file_c_verif
mv $file_c3_verif $prefix"_klee3/"$file_c_verif

KleeVerify 1
KleeVerify 2
KleeVerify 3
exit 0
