#!/bin/bash

if [ $# -ge 2 ]
then
	file_path=$1
	input_num=$2
else
	echo "./test.sh needs more parameters"
	echo "./test.sh test_cpp_pathname input_num [MIN] [MAX]"
	echo "try it again..."
	exit 1
fi

if [ $# -ge 4 ] 
then
	MIN=$3
	MAX=$4
else
	MIN=-100
	MAX=100
fi

clear


test_cpp=${file_path}
test_out="./target.out"


mkdir -p ./data
rm -rf ./data/*
mkdir -p ./bin
make # > ./makeout
if [ $? -ne 0 ]
then
#	cat ./makeout
	echo "make command encounter errors..."
	echo "stop here."
#	rm ./makeout
	exit -1
fi
#rm ./makeout
g++ $test_cpp -o $test_out








#############################################################################################################################################
#############################################################################################################################################
######################################## REAL LEARNING STARTS HERE ##########################################################################
#############################################################################################################################################

NUM_ITER_BEGIN=`expr 4 \* $input_num`
NUM_ITER_AFTER=`expr 4 \* $input_num`
NUM_ITERS=32 #128
cond=0
prev=0 ## number of previous test cases
cur=0 ## number of cur run test cases after deletion
numfail=0 ###number of failed tests run...
rm -rf ./data/*

echo -n "" > ./data/points
echo -n "" > ./data/lines

echo "************************************** start **************************************************************"
echo -n " ..."
K=1 

while [ $K -le $NUM_ITERS ]; do 
	if [ $K -eq 1 ]
	then
		echo -n "" > ./data/train
		./bin/randgen $input_num $NUM_ITER_BEGIN $MIN $MAX > "./data/sample"${K}
	fi

	echo -n $K


	#############################################################################################################################################
	# run the program with all the samples
	echo -n "" >  "./data/train1_"${K}
	while IFS='' read -r line || [[ -n "$line" ]]; do
		echo "$line" | ./bin/rand2input > ./data/input
		if [ $? -eq 2 ]
		then
			echo -n "The input is: "
			echo "$line" | ./bin/rand2input
			exit -1;
		fi
		cat ./data/input |xargs  $test_out >> "./data/train1_"${K}
#		echo "$line" >> "./data/train1_"${K}
	done < "./data/sample"${K} 


	#############################################################################################################################################
	# do the marking for generated data
	#############################################################################################################################################
	./bin/markline  "./data/train1_"${K} > "./data/tempmark_"${K}	## do the marking for awk command..  output like -e '3,5d' -e '7,9d' 
	cond=$?


	#############################################################################################################################################
	# cut and convert the output file to the standard svm input data format
	# cond == 255[-1]  => there is '!' symbol. which means a bug in program. m0 |= Precondition, but mn |=\ Postcondition
	#############################################################################################################################################
	if [ $cond -eq 255 ]
	then
		echo "COUNTER EXAMPLE! ./bin/markline exit with -1."
		exit -1
	elif [ $cond -eq 0 ]		## there is no line need to delete...
	then						## echo "NO LINE TO REMOVE!"
		awk '{$1="";print $0}'  "./data/train1_"${K} | cut -c2- >>  ./data/train
	else					## we need to delete some lines, which have been marked by above output
		cat "./data/tempmark_"${K} | xargs sed "data/train1_"${K} >  "./data/train2_"${K}
		awk '{$1="";print $0}'  "./data/train2_"${K} | cut -c2- >> ./data/train
	fi
	# copy the data for plot use... for matlab
	cat ./data/train > "./data/points"${K} 


	#############################################################################################################################################
	########	./rSVM.sh
	#############################################################################################################################################
	DOUBLE_MAX=1.79769e+308
	cd ./data
	#echo "------------------- start to run SVM ----------------------"
	echo -n "...svm..."
	svm-train  -q -t 0 -c $DOUBLE_MAX train
	svm-predict -q train train.model train.predict
	#echo "------------------ finish running SVM ---------------------"
	#echo "------------ finish running SVM ----------"
	##echo 
	echo -n "" > svmline
	../bin/svm2plane -m train.model -t train -p svmline
	cond=$?
	if [ $cond -eq 254 ]  && [ $K -eq 1 ]
	then
		cd ..
		echo -n -e "\b\b\b\b\b\b\b\b\b\b"
		continue
	fi

	cat svmline >> lines
	cd ..


	#############################################################################################################################################
	# calculate the number of valid input
	prev=$cur
	cur=`cat ./data/train | wc -l`
	prev=`expr $cur - $prev`
	#############################################################################################################################################
	if [ $prev -eq 0 ]
	then 
		prev=`expr $NUM_ITER_AFTER \* 10`
		echo -n " +++"
		numfail=`expr $numfail + 1`
		#	else 
		#		prev=`expr $prev + $NUM_ITER_AFTER`
	elif [ $prev -ge $NUM_ITER_AFTER ]
	then
		prev=$NUM_ITER_AFTER
		echo -n " ..."
		numfail=0
	else
		prev=`expr $NUM_ITER_AFTER +  $NUM_ITER_AFTER - $prev`
		echo -n " ..."
		numfail=0
	fi

	if [ $prev -lt 0 ]
	then
		prev=$NUM_ITER_AFTER
	fi



	#############################################################################################################################################
	# check the convergence... 
	# if K = 1, no need to check convergence... so skip this.
	# check the convergence  : !=0 if false => continue running.
	#						   ==0 if true  => check if there are new
	#                                          input involved in this iteration : YES  => converge. 
	#																			: No   => check these free iteration : > 1 =>convergence is valid...
	#############################################################################################################################################
	if [ $K -gt 1 ]
	then			
		#tail -n 2 ./data/SVMfomulahistory > ./data/SVMtailhistory
		#./bin/convergecheck 2 ./data/convergeout < ./data/SVMtailhistory
		tail -n 2 ./data/lines | ./bin/convergecheck 2 ./data/convergeout
		cond=$?					 ## cond == 0: converged; 
		## cond != 0: not converged.
		if [ $cond -eq 0 ]
		then
			if [ $numfail -ne 1 ]
			then
				#	echo -n  "EXIT! classifier converged."
				#	echo  "After <$K> interations."
				break
			fi
		fi
	fi



	#############################################################################################################################################
	# prepare for the next iteration. 
	# K++;
	# get new data to test
	#############################################################################################################################################
	K=$(($K+1))
	#echo "************************************** $K **************************************************************"
	tail -n 1 ./data/lines | awk '{$1="";print $0}' | cut -c2- > ./data/tempfile
	./bin/linearsolver $input_num $prev $RANDOM < ./data/tempfile > "./data/sample"${K}
done


echo $K > ./data/iternum
rm ./data/temp*
rm ./data/sample[0-9]*
rm ./data/train1_*
rm ./data/train2_*
echo "*************************************  end  *************************************************************"
echo -n "adjust coefficiency...>>  "
./bin/adjustcoef ./data/svmline ./data/svmline_adjust
#make -s clean
#rm -rf ./bin
rm -f $test_out
exit 0
