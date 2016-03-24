#!/bin/bash
red="\033[33;\x1b[31m"
green="\033[33;\x1b[32m"
yellow="\033[33;\x1b[33m"
blue="\033[33;\x1b[34m"
white="\033[0m"
bold="\033[1m"
normal="\033[0m"

dir_src="./src/"
dir_bin="./bin/"

function handle_exit() {
	exit_code=$1
	if [ $exit_code -eq 0 ]; then
		echo -e $green$bold"[PASS]"$white
	else
		echo -e $red$bold"[FAIL]Compiling Error. (error code: "$status")"$white
		exit $status
	fi
	return 0
}


echo -n -e "[ 33%] "$blue"Compiling cfg2test"$white
g++ $dir_src"cfg2test.cpp" -std=c++11 -o $dir_bin"cfg2test"
handle_exit $?

echo -n -e "[ 66%] "$blue"Compiling cfg2verif"$white
g++ $dir_src"cfg2verif.cpp" -std=c++11 -o $dir_bin"cfg2verif"
handle_exit $?

echo -n -e "[100%] "$blue"Compiling model_parse"$white
g++ $dir_src"model_parser.cpp" -std=c++11 -o $dir_bin"model_parser"
handle_exit $?

exit 0

