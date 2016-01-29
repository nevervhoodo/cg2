#!/bin/bash 
if [ "$#" == "0" ]
then 
echo "need arguments: 1 or 2 or 'both' [and 'sse' or 'cmp' or 'cmpnoout' optional]"
exit 0
fi

if [ "$1" == "1"  ]
then
	if [ "$#" == "2" ]
	then
		if [ "$2" == "cmp" ]
		then
			/usr/bin/time ./build/bin/task2 -d ../data/binary/train_labels.txt -m ./model_binary.txt --train --$2 
			/usr/bin/time ./build/bin/task2 -d ../data/binary/test_labels.txt -m ./model_binary.txt -l ./predictions.txt --predict --$2 
			python ./compare.py ../data/binary/test_labels.txt ./predictions.txt
			/usr/bin/time  ./build/bin/task2 -d ../data/binary/train_labels.txt -m ./model_binary.txt --train 
			/usr/bin/time ./build/bin/task2 -d ../data/binary/test_labels.txt -m ./model_binary.txt -l ./predictions.txt --predict  
			python ./compare.py ../data/binary/test_labels.txt ./predictions.txt
		elif [ "$2" == "cmpnoout" ]
		then
			/usr/bin/time ./build/bin/task2 -d ../data/binary/train_labels.txt -m ./model_binary.txt --train --$2 >/dev/null 
			/usr/bin/time ./build/bin/task2 -d ../data/binary/test_labels.txt -m ./model_binary.txt -l ./predictions.txt --predict --$2 >/dev/null 
			python ./compare.py ../data/binary/test_labels.txt ./predictions.txt
			/usr/bin/time  ./build/bin/task2 -d ../data/binary/train_labels.txt -m ./model_binary.txt --train >/dev/null 
			/usr/bin/time ./build/bin/task2 -d ../data/binary/test_labels.txt -m ./model_binary.txt -l ./predictions.txt --predict >/dev/null  
			python ./compare.py ../data/binary/test_labels.txt ./predictions.txt
		elif [ "$2" == "sse" ]
		then
			/usr/bin/time ./build/bin/task2 -d ../data/binary/train_labels.txt -m ./model_binary.txt --train --$2
			/usr/bin/time ./build/bin/task2 -d ../data/binary/test_labels.txt -m ./model_binary.txt -l ./predictions.txt --predict --$2 
			python ./compare.py ../data/binary/test_labels.txt ./predictions.txt
		fi
	else
	/usr/bin/time  ./build/bin/task2 -d ../data/binary/train_labels.txt -m ./model_binary.txt --train 
	/usr/bin/time ./build/bin/task2 -d ../data/binary/test_labels.txt -m ./model_binary.txt -l ./predictions.txt --predict  
	python ./compare.py ../data/binary/test_labels.txt ./predictions.txt
	fi
elif [ "$1" == "2" ]
then
	if [ "$#" == "2" ]
	then 
		if [ "$2" == "cmp" ]
		then
			/usr/bin/time ./build/bin/task2 -d ../data/multiclass/train_labels.txt -m ./model_multiclass.txt --train --$2
			/usr/bin/time ./build/bin/task2 -d ../data/multiclass/test_labels.txt -m ./model_multiclass.txt -l ./predictions2.txt --predict --$2
			python ./compare.py ../data/multiclass/test_labels.txt ./predictions2.txt
			/usr/bin/time ./build/bin/task2 -d ../data/multiclass/train_labels.txt -m ./model_multiclass.txt --train 
			/usr/bin/time ./build/bin/task2 -d ../data/multiclass/test_labels.txt -m ./model_multiclass.txt -l ./predictions2.txt --predict 
			python ./compare.py ../data/multiclass/test_labels.txt ./predictions2.txt
		elif [ "$2" == "cmpnoout" ]
		then
			/usr/bin/time ./build/bin/task2 -d ../data/multiclass/train_labels.txt -m ./model_multiclass.txt --train --$2 >/dev/null 
			/usr/bin/time ./build/bin/task2 -d ../data/multiclass/test_labels.txt -m ./model_multiclass.txt -l ./predictions2.txt --predict --$2 >/dev/null 
			python ./compare.py ../data/multiclass/test_labels.txt ./predictions2.txt
			/usr/bin/time ./build/bin/task2 -d ../data/multiclass/train_labels.txt -m ./model_multiclass.txt --train >/dev/null 
			/usr/bin/time ./build/bin/task2 -d ../data/multiclass/test_labels.txt -m ./model_multiclass.txt -l ./predictions2.txt --predict >/dev/null 
			python ./compare.py ../data/multiclass/test_labels.txt ./predictions2.txt  
		elif [ "$2" == "sse" ]
		then 
			/usr/bin/time ./build/bin/task2 -d ../data/multiclass/train_labels.txt -m ./model_multiclass.txt --train --$2
			/usr/bin/time ./build/bin/task2 -d ../data/multiclass/test_labels.txt -m ./model_multiclass.txt -l ./predictions2.txt --predict --$2
			python ./compare.py ../data/multiclass/test_labels.txt ./predictions2.txt
		fi
	else
		/usr/bin/time ./build/bin/task2 -d ../data/multiclass/train_labels.txt -m ./model_multiclass.txt --train 
		/usr/bin/time ./build/bin/task2 -d ../data/multiclass/test_labels.txt -m ./model_multiclass.txt -l ./predictions2.txt --predict 
		python ./compare.py ../data/multiclass/test_labels.txt ./predictions2.txt
	fi
elif [ "$1" == "both" ]
then
	if [ "$#" == "2" ]
	then 
		if [ "$2" == "cmp" ]
		then
			/usr/bin/time ./build/bin/task2 -d ../data/multiclass/train_labels.txt -m ./model_multiclass.txt --train --$2
			/usr/bin/time ./build/bin/task2 -d ../data/multiclass/test_labels.txt -m ./model_multiclass.txt -l ./predictions2.txt --predict --$2
			python ./compare.py ../data/multiclass/test_labels.txt ./predictions2.txt
			/usr/bin/time ./build/bin/task2 -d ../data/multiclass/train_labels.txt -m ./model_multiclass.txt --train 
			/usr/bin/time ./build/bin/task2 -d ../data/multiclass/test_labels.txt -m ./model_multiclass.txt -l ./predictions2.txt --predict 
			python ./compare.py ../data/multiclass/test_labels.txt ./predictions2.txt
			/usr/bin/time ./build/bin/task2 -d ../data/binary/train_labels.txt -m ./model_binary.txt --train --$2 
			/usr/bin/time ./build/bin/task2 -d ../data/binary/test_labels.txt -m ./model_binary.txt -l ./predictions.txt --predict --$2  
			python ./compare.py ../data/binary/test_labels.txt ./predictions.txt
			/usr/bin/time ./build/bin/task2 -d ../data/binary/train_labels.txt -m ./model_binary.txt --train 
			/usr/bin/time ./build/bin/task2 -d ../data/binary/test_labels.txt -m ./model_binary.txt -l ./predictions.txt --predict  
			python ./compare.py ../data/binary/test_labels.txt ./predictions.txt
		elif [ "$2" == "cmpnoout" ]
		then
			/usr/bin/time ./build/bin/task2 -d ../data/multiclass/train_labels.txt -m ./model_multiclass.txt --train --$2 >/dev/null 
			/usr/bin/time ./build/bin/task2 -d ../data/multiclass/test_labels.txt -m ./model_multiclass.txt -l ./predictions2.txt --predict --$2 >/dev/null 
			python ./compare.py ../data/multiclass/test_labels.txt ./predictions2.txt
			/usr/bin/time ./build/bin/task2 -d ../data/multiclass/train_labels.txt -m ./model_multiclass.txt --train >/dev/null 
			/usr/bin/time ./build/bin/task2 -d ../data/multiclass/test_labels.txt -m ./model_multiclass.txt -l ./predictions2.txt --predict >/dev/null 
			python ./compare.py ../data/multiclass/test_labels.txt ./predictions2.txt 
			/usr/bin/time ./build/bin/task2 -d ../data/binary/train_labels.txt -m ./model_binary.txt --train --$2 >/dev/null 
			/usr/bin/time ./build/bin/task2 -d ../data/binary/test_labels.txt -m ./model_binary.txt -l ./predictions.txt --predict --$2 >/dev/null 
			python ./compare.py ../data/binary/test_labels.txt ./predictions.txt
			/usr/bin/time ./build/bin/task2 -d ../data/binary/train_labels.txt -m ./model_binary.txt --train >/dev/null 
			/usr/bin/time ./build/bin/task2 -d ../data/binary/test_labels.txt -m ./model_binary.txt -l ./predictions.txt --predict >/dev/null 
			python ./compare.py ../data/binary/test_labels.txt ./predictions.txt 
		elif [ "$2" == "sse" ]
		then 
			/usr/bin/time ./build/bin/task2 -d ../data/multiclass/train_labels.txt -m ./model_multiclass.txt --train --$2
			/usr/bin/time ./build/bin/task2 -d ../data/multiclass/test_labels.txt -m ./model_multiclass.txt -l ./predictions2.txt --predict --$2
			python ./compare.py ../data/multiclass/test_labels.txt ./predictions2.txt
			/usr/bin/time ./build/bin/task2 -d ../data/binary/train_labels.txt -m ./model_binary.txt --train --$2
			/usr/bin/time ./build/bin/task2 -d ../data/binary/test_labels.txt -m ./model_binary.txt -l ./predictions.txt --predict --$2 
			python ./compare.py ../data/binary/test_labels.txt ./predictions.txt
		fi
	else
		/usr/bin/time ./build/bin/task2 -d ../data/multiclass/train_labels.txt -m ./model_multiclass.txt --train 
		/usr/bin/time ./build/bin/task2 -d ../data/multiclass/test_labels.txt -m ./model_multiclass.txt -l ./predictions2.txt --predict 
		python ./compare.py ../data/multiclass/test_labels.txt ./predictions2.txt
		/usr/bin/time ./build/bin/task2 -d ../data/binary/train_labels.txt -m ./model_binary.txt --train  
		/usr/bin/time ./build/bin/task2 -d ../data/binary/test_labels.txt -m ./model_binary.txt -l ./predictions.txt --predict 
		python ./compare.py ../data/binary/test_labels.txt ./predictions.txt
	fi
else
	echo "need arguments: 1 or 2 or 'both' [and 'sse' or 'cmp' or 'cmpnoout' optional]"
	exit 0
fi
