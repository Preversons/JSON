# JSON
A JSON string parser by C++

# Let me show you what it can do!
At the first, we need to have a comprehensive understanding of the project.
### The structure of the project
All of the source codes are in the src directory, and there are 2 C++ header file and a C++ source code file:
- colors.hpp: In this file, we defined some color string literal of ANSI Escape Character of Colors likes:
```cpp
#define RED     "\033[91m"
#define GREEN   "\033[92m"
// And there are some other definitions.
```
- json.hpp: The core of this project, we will have a more detailed understanding after (it will may have some introuce later on TxT).
- main.cpp: It is approximately an example program, but now, we use it for test our code usually, so if you want to get a program that it can run, don't compile this file now; and you need to read our source code and write a another file to run it; but maybe you don't want to read these source code so long, so we'll write some using documents later.We will write some example program at the begin of 2026 November.
> If you have good program, you can tell us by PR of Issues!

# Start quickly
### How to clone this repository:
```bash
git clone https://github.com/Preversons/JSON.git
```
### And let's compile it (Use GCC as an example):
(If you didn't make build directory)
```bash
mkdir build
```
Compile the source file if you want to run it:
```bash
g++ src/main.cpp -o build/main.exe
```
If you just need to use the header file:
Add it in your cpp code file at the begin:
```cpp
#include "json.hpp" // the json.hpp is in the src directory
```
If you need to use Pre-compile:
```bash
g++ src/json.hpp -o YOUR_SOURCE_CODE_PATH/json.hpp.gch
```

> Note: The author is Chinese and he isn't good at English.If you find some mistake, welcome to tell us and we'll listen and correct it.