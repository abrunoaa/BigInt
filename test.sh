#!/bin/bash

# run this at least once: chmod u+x run.sh

# compile bigint and test program
printf "Compiling... "

javac bigintCheck.java || exit
javac -h . bigintCheck.java || exit
g++ -fPIC -I"/usr/lib/jvm/default-java/include" \
    -I"/usr/lib/jvm/default-java/include/linux" \
    -shared -o libbigint.so bigintCheck.cpp || exit
printf "done\n"


# run tests
time java -Djava.library.path=. bigintCheck


# remove temp files
rm 'bigintCheck.class' 'libbigint.so' 'bigintCheck.h'
