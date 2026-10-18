# A compiler for the PL/0 language, written in C

The PL/0 language is a minimal educational programming language by Niklaus Wirth in 1976. It is very simple. The spec I used is found in `./language.ebnf`.

To use this compiler, you must:

- Build the compiler from the C source.
- Use that compiler to compile your PL/0 code. You must feed the input through stdin and the output assembly comes through stdout.
- Assemble the output, along with the files in `./lib/`
- Link these together, along with libc, which gives us i/o.

