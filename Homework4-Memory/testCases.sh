#!/bin/bash

read -p "Press enter to run the first test case, which runs part one of the homework assigment displaying the number of printable characters in a file with the read system call. My example file is called raven.txt"

./proj4 raven.txt

echo ""
echo "" 
read -p "Press enter to run the second test case, which runs part one of the homework assigment displaying the number of printable characters in a file with the mmap system call. My example file is called raven.txt"

./proj4 raven.txt mmap

echo ""
echo "" 
read -p "Press enter to run the third test case, which runs part one of the homework assigment displaying the number of printable characters in a file with a custom chunk size of 4096. My example file is called raven.txt"

./proj4 raven.txt 4096


echo ""
echo "" 
read -p "Press enter to run the fourth test case, which runs part one of the homework assigment displaying the number of printable characters in a file with a custom chunk size of 8193. My example file is called raven.txt"

./proj4 raven.txt 8193




