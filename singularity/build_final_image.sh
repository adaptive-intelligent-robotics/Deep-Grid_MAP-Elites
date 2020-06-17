#!/bin/bash

if [ -f Singularity ]
then
    echo "Singularity file found"
else
    echo "ERROR, Singularity file not found!"
    exit 1
fi

grep -v "NOTFORFINAL" Singularity > tmp.def
IMAGENAME=final_$(basename $(dirname "$(pwd)"))_$(date +%Y-%m-%d_%H_%M_%S).sif
singularity build --force --fakeroot $IMAGENAME tmp.def
rm ./tmp.def
