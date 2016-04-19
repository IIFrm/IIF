#!/bin/bash
red="\033[33;\x1b[31m"
green="\033[33;\x1b[32m"
yellow="\033[33;\x1b[33m"
blue="\033[33;\x1b[34m"
white="\033[0m"
#bold=$(tput bold)
#normal=$(tput sgr0)
bold="\033[1m"
normal="\033[0m"


if [ $# -lt 1 ]
then
	echo "./run_iterative.sh needs more parameters"
	echo "./run_iterative.sh cofig_prefix"
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
path_cfg=$dir_cfg""$file_cfg
file_cpp=$prefix".cpp"
path_cpp=$dir_test""$file_cpp
file_var=$prefix".var"
path_var=$dir_temp""$file_var
file_inv=$prefix".inv"
path_inv=$dir_temp""$file_inv
file_cnt=$prefix".cnt"
path_cnt=$dir_temp""$file_cnt
file_cnt_lib=$prefix".cntlib"
path_cnt_lib=$dir_temp""$file_cnt_lib

file_dataset=$prefix".ds"
path_dataset=$dir_temp""$file_dataset

rm -f $path_cnt
rm -f $path_dataset
rm -f $path_cnt_lib

##########################################################################
# BEGINNING 
##########################################################################
cd tools
./make_tools.sh
cd ..

./build_project.sh $1 $path_cnt $path_dataset

iteration=1
echo -e $blue"Running the project to generate invariant candidiate..."$white
while [ $iteration -le 32 ]; do
	echo -n -e $green$bold"--------------------------------------------- Iteration "
	echo -n -e $iteration
	echo -e " --------------------------------------------------------"$normal$white
	##########################################################################
	# Run the target to get Invariant Candidates
	##########################################################################
	cd build
	./$prefix
	ret=$?
	if [ $ret -ne 0 ]; then
		echo -e $red$bold"can not get an invariant candidate, read log file to find out more."$normal$white
		exit 1
	fi
	cd ..


	#**********************************************************************************************
	# verification phase
	#**********************************************************************************************
	./verify.sh $1
	if [ $? -eq 0 ]; then
		#echo -n -e $green$bold"------------------------------------------------------------- Iteration "
		#echo -e " Done -------------------------------------------------------------------"$normal$white
		exit 0
	else
		iteration=$(($iteration+1))
	fi
	cat $dir_temp"svm.ds" > $path_dataset
done
exit $?
