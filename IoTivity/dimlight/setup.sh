#!/bin/bash
CURPWD=`pwd`
PROJNAME=${PWD##*/}

cp ${EXAMPLEPATH}/Emulator-Code/IoTivity/dimlight/SConscript ${CURPWD}/
cp ${EXAMPLEPATH}/Emulator-Code/IoTivity/dimlight/example-config.json ${CURPWD}/${PROJNAME}-config.json
cp ${EXAMPLEPATH}/Emulator-Code/emulator/dimlight/example.json ${CURPWD}/${PROJNAME}.json
