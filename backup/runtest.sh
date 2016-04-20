#!/bin/bash
red="\e[31m"
green="\e[32m"
yellow="\e[33m"
blue="\e[34m"
white="\e[0m"

if [ $# -lt 1 ]
then
	echo "./test.sh needs more parameters"
	echo "./test.sh cofig_prefix"
	echo "try it again..."
	exit 1
fi

dir_test="test/"
dir_temp="./"


prefix=$1
file_var=$prefix".var"
file_smt2=$prefix".smt2"
file_model=$prefix".model"
path_var=$dir_temp""$file_var
path_smt2=$dir_temp""$file_smt2
path_model=$dir_temp""$file_model
path_cnt=$dir_temp""$prefix".cnt"


echo -e $blue"modifying smt2 file"$white
sed '$d' -i $path_smt2
echo "(get-model)" >> $path_smt2
echo "(exit)" >> $path_smt2

echo -e $blue"Z3 solving"$white
z3 $path_smt2 > $path_model

echo -e $blue"Compiling model_parse"$white
g++ model_parser.cpp -std=c++11 -o model_parser
./model_parser $path_var $path_model $path_cnt
status=$?
if [ $status -eq 0 ]; then
	echo "unsat"
else
	echo ""
	echo "sat."
	echo "counter-example is stored in "$path_cnt
fi
exit 0

