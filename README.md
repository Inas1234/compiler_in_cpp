# SyClone Programing Language

Welcome to SyClone, Syclone is a programming language that is being developed in C++. It is designed to be easy to use with a simple syntax. The language is compiled in C++ into Assembly code which can then be executed.  

## Requirements
To begin using the language you need:
* `nasm`,
* `g++`

## Getting Started

To get started with SyClone, follow these steps:

1. **Clone the repository**: clone the repo using `git clone https://github.com/Inas1234/compiler_in_cpp.git`

2. **Create a SyClone File**: Create a new file with the `.mpl` extension (e.g., `mypattern.mpl`) using a text editor.

3. **Write Your Code**: Use the SyClone syntax to create your pattern printing code. Below is an example of a simple pattern printing program:

   ```SyClone
   include "io.hy"

   let i = 0;
   let j = 0;

   for(i; i < 5; i = i + 1;) {
       j = 0;
       for(j; j < 5; j = j + 1;) {
           if (i == j) {
               prints("*");
           } else {
               prints(" ");
           }
       }
       println;
   }

   exit (0);
4. **Compile the code and execute it**: Compile using `make` and execute it with `./out`.

## Language Syntax
SyClone's syntax is designed for simplicity and pattern printing. Here are some key elements of the language:
* `include "filepath"` - includes a file of the type filename.hy from the include directory,
* `let x = int | string` - creates a variable of either integer or string,
* `for (initialization; condition; iteration;)` - creates a for loop which could look like this `for(i; i < 10; i = i + 1)`
* `if (condition){}else{}` - creates an if statment where you can use conditions like:
  * `?=` - Equals,
  * `<` - Lesser,
  * `>` - Greater,
  * `!=` - Not equal,
  * `?<` - Lesser or equal,
  * `?>` - Greater or equal,
* `print(int)` - prints an integer to the screen,
* `prints(string)` - prints a string to the screen,
* `println` - prints a new line
* `exit(int)` - exits with an exit code 

## Development Status
SyClone is currently under active development. You can expect updates and improvements in the near future. If you'd like to contribute or have any suggestions, please feel free to reach out in the repository's issue tracker.

## License
SyClone is open-source and released under the MIT License. You are free to use, modify, and distribute it according to the terms of the license.


