#!/bin/bash

echo Running valids:
for file in ../programs/valids/*
do
	./golite < $file > /dev/null 2>&1;
	if [ "$?" -eq 0 ]; then
		printf ".";
	else
		echo Failed: $file;
		./golite < $file;
	fi
done

echo
echo Running invalids:
for file in ../programs/invalids/*
do
	./golite < $file > /dev/null 2>&1;
	if [ "$?" -eq 1 ]; then
		printf ".";
	else
		echo Failed: $file;
	fi
done
