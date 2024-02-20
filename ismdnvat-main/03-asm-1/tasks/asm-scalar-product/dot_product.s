    .intel_syntax noprefix

    .text
    .global dot_product

dot_product:
    mov     rax, rdi         
    and     rax, 7           
    jz      .even_length     

    
    xor     rax, rax         
    xorps   ymm0, ymm0       

.loop_remainder:
    vmovups ymm1, [rsi + rax*4]  
    vmovups ymm2, [rdx + rax*4]  
    vmulps  ymm1, ymm1, ymm2      
    vaddps  ymm0, ymm0, ymm1      
    add     rax, 8                
    cmp     rax, rdi              
    jl      .loop_remainder       

.even_length:
    
    xor     rax, rax         
    xorps   ymm1, ymm1       

.loop:
    vmovups ymm2, [rsi + rax*4]  
    vmovups ymm3, [rdx + rax*4]  
    vmulps  ymm2, ymm2, ymm3      
    vaddps  ymm1, ymm1, ymm2      
    add     rax, 8                
    cmp     rax, rdi              
    jl      .loop                

    
    vhaddps ymm1, ymm1, ymm1
    vhaddps ymm0, ymm1, ymm0

    
    vextractps xmm1, ymm0, 1  
    vaddss   xmm0, xmm0, xmm1 

    ret
