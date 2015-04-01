#include <stdio.h>
clear
#Run cpplint
printf "\n============\n= CppLint: =\n============\n"
python ./cpplint/cpplint.py src/*.cpp src/*.h
printf "\n\n\n"
#Run cppCheck
printf "=============\n= CppCheck: =\n=============\n"
cppcheck src/*.cpp
printf "\n\n"
