mjplusplus
==========

mjplusplus is a compiler for the MiniJava language, a Java subset. MiniJava code is compiled to 64 bit x86 assembler. It is being developed by a group of students from Karlsruhe Institute of Technology (Group 5).

The code's documentation can be found here: http://djrieger.github.io/mjplusplus/doc/doxygen/html/ 

Compile by running `make` (debug version) or `make release` (release version) in the mjplusplus directory. This will create an `mj++` binary.

Run the compiler like this:

`./mj++ [--dumplexgraph|--lextest|--print-ast|--parse|--check [--firm]|--compile-firm|--assembly] [--out] FILE`

- `--dumlexgraph`: Dump the automaton that is used by the lexer to a file. (Format: GML)

- `--lextest`: Run a test on the compiler's lexer on the given `FILE`

- `--print-ast`: Pretty print the parsed file.

- `--parse`: Run the syntax analysis on the given `FILE`.

- `--check`: Run the semantic analysis on the given `FILE`.

- `--firm` : Build the graphs and create an assembly file. Only works with `--check`.

- `--compile-firm` : Run semantic analysis, build the FIRM-graph and produce backend-code using FIRM and gcc.

- `--assembly`: Keeps the created assembly file after compilation.

- `--out`: A few options can be given an output file with this command.

If the compiler is started without an alternative mode, like `--compile-firm` or `--print-ast`, the given `FILE` will be compiled using our own code generation and gcc. 
The created assembly file will be deleted afterwards. This behaviour can be altered by `--assembly`.
The created executeable file will be named `FILE.out`. This behaviour can be altered by `--out`.

Example: `./mj++ -out foo foo.mj`
