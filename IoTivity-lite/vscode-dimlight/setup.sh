#!/bin/bash
CURPWD=`pwd`
PROJNAME=${PWD##*/}

cp ${EXAMPLEPATH}/Emulator-Code/IoTivity-lite/vscode-dimlight/devbuildmake ${CURPWD}/devbuildmake
cp ${EXAMPLEPATH}/Emulator-Code/IoTivity-lite/vscode-dimlight/vscode-dimlight-config.json ${CURPWD}/${PROJNAME}-config.json
cp ${EXAMPLEPATH}/Emulator-Code/IoTivity-lite/vscode-dimlight/CMakeLists.txt ${CURPWD}/CMakeLists.txt
cp ${EXAMPLEPATH}/Emulator-Code/IoTivity-lite/vscode-dimlight/.gitignore ${CURPWD}/.gitignore
MY_COMMAND="sed -i 's/vscode-dimlight/${PROJNAME}/g' ${CURPWD}/CMakeLists.txt"
eval ${MY_COMMAND}
cp ${EXAMPLEPATH}/Emulator-Code/IoTivity-lite/vscode-dimlight/sdkconfig ${CURPWD}/sdkconfig
cp ${EXAMPLEPATH}/Emulator-Code/IoTivity-lite/vscode-dimlight/main/vscode-dimlight.c ${CURPWD}/main/${PROJNAME}.c
cp ${EXAMPLEPATH}/Emulator-Code/IoTivity-lite/vscode-dimlight/main/vscode-dimlight-main.c ${CURPWD}/main/${PROJNAME}-main.c
MY_COMMAND="sed -i 's/vscode-dimlight/${PROJNAME}/g' ${CURPWD}/main/${PROJNAME}.c"
eval ${MY_COMMAND}
mkdir ${CURPWD}/bin/
mkdir ${CURPWD}/bin/glade/
cp -r ${EXAMPLEPATH}/Emulator-Code/IoTivity-lite/vscode-dimlight/bin/* ${CURPWD}/bin
cp -r ${EXAMPLEPATH}/Emulator-Code/IoTivity-lite/vscode-dimlight/.vscode ${CURPWD}
