#!/bin/bash

echo Running valid syntax:
for file in ../programs/valids/syntax/*
do
	./golite parse $file 1> /dev/null;
	if [ "$?" -eq 0 ]; then
		printf ".";
	else
		echo Failed: $file;
	fi
done

echo
echo Running valid milestone 1 test cases:
for folder in ../programs/valids/m1-testcases/*
do
	for file in $folder/*
	do
		./golite pretty $file 1> /dev/null;
		if [ "$?" -eq 0 ]; then
			printf ".";
		else
			echo Failed: $file;
		fi
	done
done

echo
echo Running valid types:
for file in ../programs/valids/type/*
do
	./golite type $file 1> /dev/null;
	if [ "$?" -eq 0 ]; then
		printf ".";
	else
		echo Failed: $file;
	fi
done

echo
echo Running invalid syntax:
for file in ../programs/invalids/syntax/*
do
	./golite type $file > /dev/null 2>&1;
	if [ "$?" -eq 1 ]; then
		printf ".";
	else
		echo Failed: $file;
	fi
done

echo
echo Running invalid milestone 1 test cases:
for folder in ../programs/invalids/m1-testcases/*
do
	for file in $folder/*
	do
		./golite pretty $file > /dev/null 2>&1;
		if [ "$?" -eq 1 ]; then
			printf ".";
		else
			echo Failed: $file;
		fi
	done
done

echo
echo Running invalid types:
for file in ../programs/invalids/type/*
do
	./golite type $file > /dev/null 2>&1;
	if [ "$?" -eq 1 ]; then
		printf ".";
	else
		echo Failed: $file;
	fi
done

