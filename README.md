mjplusplus
==========

mjplusplus is a compiler for the MiniJava language, a Java subset. MiniJava code is compiled to 64 bit x86 assembler. It is being developed by a group of students from Karlsruhe Institute of Technology (Group 5).

Compile by running `make` (debug version) or `make release` (release version) in the mjplusplus directory. This will create an `mj++` binary.

Run the compiler like this:

`./mj++ [--lextest|--print-ast] FILE`

`--lextest`: Run a test on the compiler's lexer on the given `FILE`
`--print-ast`: Pretty print the parsed file.
