#!/bin/bash

if [[ -s $1.o ]]
then
        rm -f $1.o
fi
nasm -g -felf64 $1.asm

if [[ -s $1.o ]]
then
  ld $1.o -o $1file
fi

if [[ -s $1file ]]
then
        ./$1file
fi
