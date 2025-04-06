# NES Emulator

This is a NES emulator written in C++. It uses SDL2 and targets Windows with MinGW-w64. The Makefile builds the project using `g++` with C++20.

## Features

- Custom CPU, PPU, and memory bus emulation  
- Support for mappers 000, 001 and (soon) 002
- SDL2-based rendering  
- Disassembler for debugging  
- Controller input support  

## Build Requirements

- **g++** with C++20 support (MinGW-w64 on Windows)  
- **SDL2** (Include and lib paths must match your setup)  
- **Make** (MinGW or compatible on Windows)  

## Building
Windows:
1. Make sure SDL2 is installed and the include/lib paths in the Makefile are correct:  
   Example:
   ```make
   -IC:/Users/user1/SDL2-2.30.10/x86_64-w64-mingw32/include
   -LC:/Users/user1/SDL2-2.30.10/x86_64-w64-mingw32/lib

2. Run make in the project directory  
   `make`
3. The compiled executable will be named run  
   `./run`
   - You can use the space bar to start the emulation or pause it.
   - Games can be changed in olcNes.cc
   - When paused there are keys that can be used to advance the PC:
     - i: Jump to a specific address
     - j: Jump by 128 instructions
     - c: Complete a single instruction
     - f: complete a single frame
