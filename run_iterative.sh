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
file_cpp=$prefix".cpp"
file_var=$prefix".var"
file_inv=$prefix".inv"
path_cfg=$dir_cfg""$file_cfg
path_cpp=$dir_test""$file_cpp
path_var=$dir_temp""$file_var
path_inv=$dir_temp""$file_inv
prefix_path_inv=$dir_temp""$prefix
path_cnt=$dir_temp""$prefix".cnt"

file_dataset=$prefix".ds"
path_dataset=$dir_temp""$file_dataset

rm -f $path_cnt
rm -f $path_dataset

##########################################################################
# BEGINNING 
##########################################################################
cd tools
./make_tools.sh
cd ..

./build_project.sh $1 $path_cnt $path_dataset

iteration=1
while [ $iteration -lt 32 ]; do
echo -n -e $green$bold"------------------------------------------------------------- Iteration "
echo -n -e $iteration
echo -e " ------------------------------------------------------------------------"$normal$white
##########################################################################
# Run the target to get Invariant Candidates
##########################################################################
cd build
echo -e $blue"Running the project to generate invariant candidiate..."$white
./$prefix
ret=$?
if [ $ret -ne 0 ]; then
	echo -e $red$bold"can not separate using default paramater"$normal$white
	#echo "try more parameters to get a perfect classifier"
	exit 1
fi
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
echo -e $green$bold"[Done]"$white


#**********************************************************************************************
# verification phase
#**********************************************************************************************
./verify.sh $1
if [ $? -eq 0 ]; then
	echo -n -e $green$bold"------------------------------------------------------------- Iteration "
	echo -e " Done -------------------------------------------------------------------"$normal$white
	exit 0
else
	iteration=$(($iteration+1))
fi
done
exit $?
