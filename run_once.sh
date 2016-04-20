#!/bin/bash
red="\e[31m"
green="\e[32m"
yellow="\e[33m"
blue="\e[34m"
bold="\e[1m"
normal="\e[0m"

if [ $# -lt 1 ]
then
	echo "./run_once.sh needs more parameters"
	echo "./run_once.sh cofig_prefix"
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
echo -e $blue"Running the project to generate invariant candidiate..."$normal
./$prefix
ret=$?
if [ $ret -ne 0 ]; then
	echo -e $red$bold"can not get an invariant candidate, read log file to find out more."$normal$normal
	#echo "can not separate using default paramater"
	#echo "try more parameters to get a perfect classifier"
	exit 1
fi
cd ..


#**********************************************************************************************
# verification phase
#**********************************************************************************************
./verify.sh $1

exit $?
