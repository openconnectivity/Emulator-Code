#!/bin/bash
CURPWD=`pwd`
PROJNAME=${PWD##*/}

cp ${EXAMPLEPATH}/Emulator-Code/IoTivity/dimlight/SConscript ${CURPWD}/
cp ${EXAMPLEPATH}/Emulator-Code/IoTivity/dimlight/dimlight-config.json ${CURPWD}/${PROJNAME}-config.json
cp ${EXAMPLEPATH}/Emulator-Code/emulator/dimlight/dimlight.json ${CURPWD}/${PROJNAME}.json
