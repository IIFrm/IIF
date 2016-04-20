#!/bin/bash
red="\e[31m"
green="\e[32m"
yellow="\e[33m"
blue="\e[34m"
bold="\e[1m"
normal="\e[0m"

dir_src="./src/"
dir_bin="./bin/"

function handle_exit() {
	exit_code=$1
	if [ $exit_code -eq 0 ]; then
		echo -e $green"[DONE]"$normal
	else
		echo -e $red$bold"[FAIL]Compiling Error. (error code: "$status")"$normal
		exit $status
	fi
	return 0
}


echo -n -e "[ 33%] "$blue"Compiling cfg2test..."$normal
g++ $dir_src"cfg2test.cpp" -std=c++11 -o $dir_bin"cfg2test"
handle_exit $?

echo -n -e "[ 66%] "$blue"Compiling cfg2verif..."$normal
g++ $dir_src"cfg2verif.cpp" -std=c++11 -o $dir_bin"cfg2verif"
handle_exit $?

echo -n -e "[100%] "$blue"Compiling model_parse..."$normal
g++ $dir_src"model_parser.cpp" -std=c++11 -o $dir_bin"model_parser"
handle_exit $?

exit 0

