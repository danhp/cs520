#!/bin/bash

echo Running valids:
for file in ../programs/valids/*
do
	./golite parse < $file 1> /dev/null;
	if [ "$?" -eq 0 ]; then
		printf ".";
	else
		echo Failed: $file;
	fi
done

echo
echo Running invalids:
for file in ../programs/invalids/*
do
	./golite parse < $file > /dev/null 2>&1;
	if [ "$?" -eq 1 ]; then
		printf ".";
	else
		echo Failed: $file;
	fi
done
