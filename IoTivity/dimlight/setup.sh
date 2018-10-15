#!/bin/bash
CURPWD=`pwd`
PROJNAME=${PWD##*/}

cp ${EXAMPLEPATH}/Sample-Raspberry-Pi-Code/IoTivity/dimlight/SConscript ${CURPWD}/
cp ${EXAMPLEPATH}/Sample-Raspberry-Pi-Code/IoTivity/dimlight/example-config.json ${CURPWD}/${PROJNAME}-config.json
cp ${EXAMPLEPATH}/Sample-Raspberry-Pi-Code/emulator/dimlight/example.json ${CURPWD}/${PROJNAME}.json
