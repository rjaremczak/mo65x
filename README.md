# mo65x
stands for My Own 65 eXpandable emulator. The project has educational motivation and is a genuine project based on technical specifications of 6502/6510 family microprocessors. See the screenshot of assembler mode running slightly modified program "demoscene.asm" from http://www.6502asm.com

![Alt text](http://mindpart.com/wp-content/uploads/2020/01/mo65x-demoscene.png "Assembler mode view")

since version 0.94 there is a video-ram mode that is compatible with that from 6502asm.com. It's a simple frame buffer mapping 32 x 32 x 8bit image with indexed color mode. Only bits 0-3 are used and maps to Commodore C64 color palette. Initially frame buffer starts at address 0x200 but can be changed live.

## Getting rid of Qt dependencies and the C++ language itself ...
While QT is a great library which along with entire ecosystem and QtCreator IDE gave this project a big boost I had to consider moving away from it. Recent changes in QT's licensing motivated me to think of a fully open-source alternative that is free of any obstacles, possible licensing pitfalls and similar uncertainity. Decision has been made and effects of my efforts can be evaluated on branch "noqt". After taking second thoughts I decided to go one step further with getting rid of unnecesarry burden. The language itself seems to be overloaded and bound to the past way too much. Therefore I'm going to rewrite entire project in Rust which at this point seems to be reasonable and fresh enough to be promising :-). The Rust port is ready, take a look at https://github.com/rjaremczak/mo65x-rs. 

## Installation
As project uses Qt 5.x.x library it should be fully portable wherever that library and a valid c++17 compilers run. So far it's been tested on Ubuntu 19.x and macOS Catalina 10.15.x. Some flavour of Windows 10 has also been checked with success.

Usual drill is the following:
* be sure to have decent GCC (or MinGW) compiler that Qt SDK can work with. On Windows MinGW is a part of distribution but Windows set-up hasn't been tested yet. Code is written using C++17 language level, be sure your version supports this.
* download latest Open Source Qt SDK from https://qt.io .
* clone or download this repository to the local storage.
* open the mo65x.pro file with QtCreator IDE which is a part of Qt SDK.
* configure primary run/build configuration as suggested by the IDE.
* being in IDE go to "Project Mode" (the wrench icon) and select newly created Build configuration. Then click "Clone..." to make a copy of it, name it "Test". Update "Build Directory" so its path differs from the one from the "Debug" configuration, you can simply replace "Debug" with "Test" within the path.
* In the "Test" configuration expand "Build Steps" section and in the "Additional arguments" field enter: CONFIG+=test

Now using regular configuration you should be able to build/run/debug the project. When in the "Test" configuration go to Tools -> Tests -> Run All Tests to run all tests provided with the project.

## Assembler
Integrated assembler supports subset of common control structures of pupular 65xx assemblers. For now it has following limitiations:
* there is no way to define symbol directly (eg. no .EQ nor "=" statement)
* control codes require leading dot (eg. .ORG)

in order to make it compatible with code from 6502asm.com the "<" and ">" operators have been added. When placed in front of literal extracts low (<) or high (>) byte of it respectively. Therefore construct like this:

label_001:
  LDX #<label_001 ; reg. X loaded with LSB of label's address
  LDY #>label_001 ; reg. Y loaded with MSB of label's address
  
is a valid statement now.

## Speed
Proper speed throttling has been implemented, clock speed can be specified with 0.01 MHz precision. Actual speed may vary a bit because of various delays but is fairly accurate.

## Example files
Test files will can be found in /asm directory within the project tree. 

## License
All design and code so far is written entirely by Robert Jaremczak (robert@mindpart.com) and is licensed as GPL.

GPL in short means this: feel free to use as long as sources of your own work remain public. Don't forget to mention me, as an author of this code :-)
