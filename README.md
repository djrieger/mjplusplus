mjplusplus
==========

mjplusplus is a compiler for the MiniJava language, a Java subset. MiniJava code is compiled to 64 bit x86 assembler. It is being developed by a group of students from Karlsruhe Institute of Technology (Group 5).

Compile by running `make` (debug version) or `make release` (release version) in the mjplusplus directory. This will create an `mj++` binary.

Run the compiler like this:

`./mj++ [--dumplexgraph|--lextest|--print-ast|--check [--firm]|--compile-firm] FILE`

`--dumlexgraph`: Dump the automaton that is used by the lexer to a file. (Format: GML)

`--lextest`: Run a test on the compiler's lexer on the given `FILE`

`--print-ast`: Pretty print the parsed file.

`--check`: Run the semantic analysis on the given `FILE`.

 `--firm` : Initialize libFirm, build the grahphs and create an assembly file. Only works in combination with `--check`.

 `--compile-firm` : Run the semantic analysis, build the FIRM-graph and produce backend-code using FIRM and gcc.

If the compiler is started without an option, the given `FILE` will be parsed and the return signal indicates, if it is correct.
