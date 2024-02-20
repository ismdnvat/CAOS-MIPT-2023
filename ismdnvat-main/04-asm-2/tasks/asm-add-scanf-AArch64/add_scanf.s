.text
.global add_scanf

.macro push Xn
  sub sp, sp, 16
  str \Xn, [sp]
.endm

.macro pop Xn
  ldr \Xn, [sp]
  add sp, sp, 16
.endm

add_scanf:
  	push x30
	adr x0, scanf_format_string
	adr x1, var1
	adr x2, var2
	bl scanf
	mov x1, 0
	mov x2, 0
	adr x1, var1
	adr x2, var2
	ldr x1, [x1]
	ldr x2, [x2]
	add x3, x1, x2
	mov x0, x3
	pop x30
	ret


.data

var1:	.xword 0
var2:	.xword 0

.section .rodata

scanf_format_string:
  .string "%lld %lld"
