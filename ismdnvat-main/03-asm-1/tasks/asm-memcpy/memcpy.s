  .intel_syntax noprefix
  .text
  .global my_memcpy

my_memcpy:
    mov r8, rdi
    cmp edx, 8
    jb last_bytes

loop:
    mov rax, [rsi]
    mov [rdi], rax
    add rsi, 8
    add rdi, 8
    sub edx, 8
    cmp edx, 8
    jae loop

last_bytes:
    cmp edx, 0
    je afterloop
    mov al, [rsi]
    mov [rdi], al
    add rsi, 1
    add rdi, 1
    sub edx, 1  
    jae last_bytes
    
afterloop:
    mov rax, r8
    ret
