mjplusplus
==========

mjplusplus is a compiler for the MiniJava language, a Java subset. MiniJava code is compiled to 64 bit x86 assembler. It is being developed by a group of students from Karlsruhe Institute of Technology (Group 5).

The code's documentation can be found here: http://djrieger.github.io/mjplusplus/doc/doxygen/html/ 

Compile by running `make` or `make release` in the mjplusplus directory, this will create an `mj++` binary. To obtain a debugging version run `make debug` instead. It is recommended to use the `mj++.sh` wrapper, especially if you do not have libfirm in your library path or outside of mjplusplus at all.

Run the compiler like this:
`./mj++ [--help|--dumplexgraph|--lextest|--parse|--check|--firm|--assembly] [--print-ast] [--suppress-errors] [--keep] [--compile-firm] [--out] FILE`
If libfirm is not in your library path you can use the `mj++.sh` wrapper.

Help options:
- `-h  --help`: Print usage and exit
- `-D  --dumplexgraph`: Print automaton of lexer to the given file name and exit (format: GML)

Run options:
- `-l  --lextest`: Only run the lexer
- `-s  --parse`: Stop after syntax analysis
- `-c  --check`: Stop after semantic analysis
- `-g  --firm`: Stop after generating Firm graphs
- `-a  --assembly`: Stop after generating assembler (default output file: `input_without_ending.S`)

If none of these options is given, generate an executable (default output file: `a.out`)

Other options:
- `-p  --print-ast`: After parsing, pretty print the abstract syntax tree
- `-q  --suppress-errors`: Do not print error messages
- `-k  --keep`: Keep the assembly file after compiling
- `-f  --compile-firm`: Generate assembler using the Firm backend instead of our own codegen
- `-d  --dumpfirmgraph`: Output the created firm graphs as vcg-files.
- `-o  --out FILE`: Set the output for various commands to FILE

Example: `./mj++ --out foo foo.mj`
