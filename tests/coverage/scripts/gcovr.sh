#!/bin/bash

# argument 1 path of file to analyse
# argument 2 output html coverage

which firefox > /dev/null
if [[ $? -eq 0 ]]
then
    gcovr -r . -f $1 --html --html-details -o $2
    firefox $2
else
    gcovr -r . -f $1
fi
