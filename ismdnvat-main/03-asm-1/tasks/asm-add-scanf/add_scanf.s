  .intel_syntax noprefix

  .text
  .global add_scanf

add_scanf:
  push rbp
  mov rbp, rsp
  xor rax, rax
  sub rsp, 8
  mov rdi, OFFSET FLAT:scanf_format_string
  mov rsi, rsp
  sub rsp, 8
  mov rdx, rsp
  
  call scanf

  mov rax, [rsp + 8]
  add rax, [rsp]
  mov rsp, rbp
  pop rbp
  ret

  .section .rodata

scanf_format_string:
  .string "%lld %lld"
