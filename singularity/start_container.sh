#!/bin/bash

LOCAL_EXP_PATH=$(dirname $(pwd))
EXP_NAME=$(basename $LOCAL_EXP_PATH)
if [ -z "${IMAGENAME}" ]; then
  IMAGENAME=$(basename $(dirname "$(pwd)")).sif
fi
echo "image name: "$IMAGENAME
DEFNAME='Singularity'
SANDBOX=true

BLD_ARGS=""
RUN_ARGS=""

while getopts n flag
do
    case $flag in
	
        n)
            echo Nvidia runtime ON
	    RUN_ARGS=$RUN_ARGS" --nv"
            ;;
        ?)
            exit
            ;;
    esac
done



if $SANDBOX; then
    BLD_ARGS=$BLD_ARGS" --sandbox"
    RUN_ARGS=$RUN_ARGS" -w"
fi
   
if [ -f "$IMAGENAME" ] || [ -d "$IMAGENAME" ]; then
    echo "$IMAGENAME exists"
else
    echo "$IMAGENAME does not exist, building it now from $DEFNAME"
    if [ -f "$DEFNAME" ]; then
	singularity build --force --fakeroot $BLD_ARGS $IMAGENAME $DEFNAME
    else
	echo "$DEFNAME not found"
	exit
    fi
fi

echo "Visualisation available after activating the visu_server.sh script"
singularity shell $RUN_ARGS --bind $LOCAL_EXP_PATH:/git/sferes2/exp/$EXP_NAME $IMAGENAME
