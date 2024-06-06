#!/bin/bash

dirPath=$1
wordToFind=$2
isRecursive=$3
COMPILED_EX="*.out"
C_FILE="*.c"

if [ "$isRecursive" == "-r" ]
then
    isRecursive=true
else 
    isRecursive=false
fi

# check if there is enough params
if [[ -z "$wordToFind" || "$wordToFind" == "-r" ]]
then
    echo "Not enough parameters"
else

    files=$(find $dirPath -maxdepth 1)

    for file in $files
    do
        if [[ -d $file ]] && [ "$isRecursive" = true ] && [ $file != $dirPath ]
        then
            $($0 $file $wordToFind "-r") 
        else
            if [[ $file == $COMPILED_EX ]] 
            then
                rm $file
            fi
        fi

    done

    cd $dirPath
    
    for FILE in $(grep -liw "${wordToFind}" *.c ) 
    do
        gcc -w -o ${FILE%.*}.out $FILE
    done

fi
