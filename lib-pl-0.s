.code64

.globl pl.getc
.globl pl.putc
.globl pl.exit

.bss

num_buf:
.space 20

.text

pl.getc:
    sub $8, %rsp
    lea in_str(%rip), %rdi
    lea (%rsp), %rsi
    xor %rax, %rax
    call scanf
    pop %rax
    ret

pl.putc:
    mov %rax, %rsi
    lea out_str(%rip), %rdi
    xor %rax, %rax
    call printf
    ret


pl.exit:
    xor %eax, %eax
    call exit

.data

in_str:
    .asciz "%d"

out_str:
    .asciz "%d\n"
