#!/bin/bash
set -x #echo on
#############################
#
# copyright 2018 Open Connectivity Foundation, Inc. All rights reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
#############################

CURPWD=`pwd`

if [[ ! -v EXAMPLEPATH ]]; then
    export EXAMPLEPATH='.'
    echo "export EXAMPLEPATH=`pwd`" >> ~/.bashrc
fi

git clone https://github.com/openconnectivity/Emulator-Code.git

cp ./Emulator-Code/IoTivity-lite/*.sh ${OCFPATH}/../iot-lite/

sudo apt-get -y install glade
sudo apt-get -y install libgtk-3-dev

cd ${CURPWD}
