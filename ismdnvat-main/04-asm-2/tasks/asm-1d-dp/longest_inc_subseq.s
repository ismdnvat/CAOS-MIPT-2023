  .text
  .global longest_inc_subseq

longest_inc_subseq:
    cmp x2, #0
    beq .done

    
    mov x3, #1

    
    mov x4, #0

    
    mov x5, #0

.loop:
    
    ldr x6, [x0, x4, lsl #3]

    
    mov x7, #1

    
    mov x8, #0

.inner_loop:
    
    ldr x9, [x1, x5, lsl #3]

    
    cmp x6, x9
    b.lo .not_increasing

    
    
    add x8, x8, #1
    .not_increasing:

    
    add x5, x5, #1

    
    cmp x5, x2
    b.hs .inner_loop

    
    cmp x8, x7
    b.lo .not_max_subsequence

    
    mov x7, x8
    .not_max_subsequence:

    
    add x4, x4, #1

    
    add x5, x5, #1

    
    cmp x4, x2
    b.hs .loop

    
    mov x0, x7

.done:
    ret

