# mo65x
stands for My Own 65 eXpandable emulator. The project has educational motivation and is a genuine project based on technical specifications of 6502/6510 family microprocessors. Here is the screenshot of assembler mode view:

![Alt text](images/rysunek-1.png?raw=true "Assembler mode view")

and memory dump view:

![Alt text](images/rysunek-2.png?raw=true "Memory dump view")

and disassembler mode view:

![Alt text](images/rysunek-3.png?raw=true "Disassembler mode view")

video-ram mode on the way...

## Installation
As project uses Qt 5.x.x library it should be fully portable wherever that library and valid c++17 compilers run. So far it's been tested on Ubuntu 19.x and macOS Catalina 10.15.x. Windows tests are planned soon ...

Usual drill is the following:
* be sure to have decent GCC (or MSVC/MinGW) compiler that Qt SDK can work with. On Windows MinGW is a part of distribution but Windows set-up hasn't been tested yet. Code is written using C++17 language level, be sure your version supports this.
* download latest Open Source Qt SDK from https://qt.io .
* clone or download this repository to the local storage.
* open the mo65x.pro file with QtCreator IDE which is a part of Qt SDK.
* configure primary run/build configuration as suggested by the IDE.
* being in IDE go to "Project Mode" (the wrench icon) and select newly created Build configuration. Then click "Clone..." to make a copy of it, name it "Test". Update "Build Directory" so its path differs from the one from the "Debug" configuration, you can simply replace "Debug" with "Test" within the path.
* In the "Test" configuration expand "Build Steps" section and in the "Additional arguments" field enter: CONFIG+=test

Now using regular configuration you should be able to build/run/debug the project. When in the "Test" configuration go to Tools -> Tests -> Run All Tests to run all tests provided with the project.

## Assembler
Integrated assembler supports subset of common control structures of pupular 65xx assemblers. For now it has following limitiations:
* labels can only be used as argument in branch instruction
* there is no way to define symbol directly (eg. no .EQ nor "=" statement)
* control codes require leading dot (eg. .ORG)

more features will be added gradually.

## Visual view
There is no video-ram functionality at the moment. This is the first item on the wish-list, though.

## Speed
For now emulator runs as fast as it can. Taking care of proper timing is being developed, to be delivered soon.

## Test files
Test files will can be found in /asm directory within the project tree. 

## License
All design and code so far is written entirely by Robert Jaremczak (robert@mindpart.com) and is licensed as GPL.

GPL in short means this: feel free to use as long as sources of your own work remain public. Don't forget to mention me, as an author of this code :-)
