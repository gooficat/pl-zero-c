
Bootstrap compiler can be found in `./bootstrap.c`

Logical flow of the program can be found in `./LOGIC.md`

Extended Backus-Naur Form for this language is defined in `./language.ebnf`

example input:
```pl
VAR x, squ;

PROCEDURE square;
BEGIN
   squ:= x * x
END;

BEGIN
   x := 1;
   WHILE x <= 10 DO
   BEGIN
      CALL square;
      ! squ;
      x := x + 1
   END
END.
```

expected output:

```pl
.globl _start

.rodata

.bss

"VAR@x":
    .space 8
"VAR@squ":
    .space 8

.text

"PROC@square":
    mov "VAR@squ"(%rip), %rax
    mul "VAR@squ"(%rip), %rax
    ret

_start:
    mov $1, "VAR@x"(%rip)
"L@1":
    cmp $10, "VAR@x"(%rip)
    jl "L@2"
    call "PROC@square"
    cmp $0, "VAR@squ"(%rip)
    mov "VAR@x"(%rip), %rax
    add $1, %rax
    mov %rax, "VAR@x"(%rip)
    jne "L@3"
"L@3":
    jmp "L@1"
"L@2":
    ret
```

