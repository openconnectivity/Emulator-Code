#!/bin/bash
CURPWD=`pwd`
PROJNAME=${PWD##*/}

MY_COMMAND="cp ${EXAMPLEPATH}/Emulator-Code/IoTivity-lite/dimlight/Makefile $CURPWD/src/"
eval ${MY_COMMAND}
MY_COMMAND="cp ${EXAMPLEPATH}/Emulator-Code/emulator/dimlight/dimlight.json $CURPWD/$PROJNAME.json"
eval ${MY_COMMAND}
MY_COMMAND="cp ${EXAMPLEPATH}/Emulator-Code/IoTivity-lite/dimlight/dimlight.c $CURPWD/src/$PROJNAME.c"
eval ${MY_COMMAND}
MY_COMMAND="mkdir $CURPWD/bin/glade/"
eval ${MY_COMMAND}
MY_COMMAND="cp ${EXAMPLEPATH}/Emulator-Code/emulator/dimlight/glade/* $CURPWD/bin/glade/"
eval ${MY_COMMAND}
