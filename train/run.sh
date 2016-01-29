#!/bin/bash 
./build/bin/task2 -d ../data/binary/train_labels.txt -m ./model_binary.txt --train
./build/bin/task2 -d ../data/binary/test_labels.txt -m ./model_binarytxt -l ./predictions.txt --predict
python ./compare.py ../data/binary/test_labels.txt ./predictions.txt

./build/bin/task2 -d ../data/multiclass/train_labels.txt -m ./model_multiclass.txt --train
./build/bin/task2 -d ../data/multiclass/test_labels.txt -m ./model_multiclass.txt -l ./predictions2.txt --predict
python ./compare.py ../data/multiclass/test_labels.txt ./predictions2.txt

