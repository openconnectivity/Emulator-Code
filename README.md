# Sample Code for Raspberry Pi

This repository contains code for creating an IoTivity-lite server emulator with GUI running on the the host OS. The examples include support for the glade GUI generator tool and GTK.

Currently, this has only been tested on IoTivity-lite and Ubuntu linux, but other platforms may work with slight modification.

There is currently one sample project.
- dimlight - This project creates a graphical dimmable light bulb that can be turned off or on and can be set to a dimming level between 0% and 100%. There are actually only 4 different brightness pictures, but the code will support any number of pictures if you have better graphics.

# Installation

To install all the samples described above, just type the following command.

- curl https://openconnectivity.github.io/Emulator-Code/pi-boards/install.sh | bash

This command will do a number of things:

- Clone the Emulator-Code repository (which includes all the sample code described above). Each sample subdirectory includes the following files:
  - Several updated scripts (ending in "2") that support building projects in any directory.
  - Makefile: A Makefile description file that will manage the compiling and linking of the project.
  - <sample>.json: The JSON device description file that is the input to DeviceBuilder and describes all the resources available on the device. Each of these resources will show up in the source code and introspection file created by DeviceBuilder.
  - <sample>.c: The C code that includes all the resources plus the code to interface to the glade GUI. This code can be copied over the C code created by DeviceBuilder when the gen2.sh script is run in order to control the hardware.
  - Glade source and graphics that will generate the GUI.

One other note:

Some environment variables may need to be updated. When the curl command is run, the ~/.bashrc file will be modified so that these envorinment variables are set on bootup. The system will need ~/.bashrc to be run. The following command should do it.
- source ~/.bashrc

# Building and Running the samples

A number of convenience scripts have been written to make the development cycle easier.
1. Run the following development cycle as needed
    1. create_project "project name" - create a new project and name it anything you want.
    2. This isn't a script, but you need to "cd "project name"" to run all the other scripts.
    3. Copy the setup.sh from the Emulator-Code/emulator/dimlight/ directory to the current project directory
    4. ./setup.sh - This will load all the necessary stuff to build and run the sample emulator project.
    5. edit_input2.sh - edit the device description input file (<project name>.json) if necessary.
    6. gen2.sh - generate the code, introspection file, PICS file, and onboarding file from the device description file.
    7. build2.sh - compile and link everything
    8. edit_code2.sh - edit the server source code if necessary.
    9. reset2.sh - reset the sever to RFOTM by copying a fresh onboarding file
    10. run2.sh - run the currently compiled server in the appropriate directory
2. To test the project, you will need to run a client. Here are some options.
    1. Discover, onboard and control the server using OTGC
    2. Discover, onboard and control the server using DeviceSpy
    3. Test the server using CTT
