#!/bin/bash
CURPWD=`pwd`
PROJNAME=${PWD##*/}

cp ${EXAMPLEPATH}/Emulator-Code/IoTivity-lite/dimlight/Makefile ${CURPWD}/
cp ${EXAMPLEPATH}/Emulator-Code/emulator/dimlight/dimlight.json ${CURPWD}/${PROJNAME}.json
cp ${EXAMPLEPATH}/Emulator-Code/IoTivity-lite/dimlight/dimlight.c ${CURPWD}/src/${PROJNAME}.c
cp ${EXAMPLEPATH}/Emulator-Code/IoTivity-lite/dimlight/dimlight-gen.c ${CURPWD}/src/${PROJNAME}-gen.c
mkdir ${CURPWD}/bin/glade/
cp ${EXAMPLEPATH}/Emulator-Code/emulator/dimlight/glade/* ${CURPWD}/bin/glade/
