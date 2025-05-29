#ifdef USE_SSE
    .text
    .globl fast_memcpy
    .type fast_memcpy,@function
fast_memcpy:
    mov %rdi, %rax        # return value = dst
    mov %rdx, %rcx        # bytes to copy
    shr $4, %rcx          # number of 16-byte blocks
    jz 2f
1:
    movdqu (%rsi), %xmm0
    movdqu %xmm0, (%rdi)
    add $16, %rsi
    add $16, %rdi
    dec %rcx
    jne 1b
2:
    mov %rdx, %rcx
    and $15, %rcx         # tail bytes
    jz 4f
3:
    movsb
    dec %rcx
    jne 3b
4:
    ret

    .globl fast_memcmp
    .type fast_memcmp,@function
fast_memcmp:
    test %rdx, %rdx
    je .equal
    mov %rdi, %r8         # p1
    mov %rsi, %r9         # p2
    mov %rdx, %rcx
    shr $4, %rcx          # blocks of 16
    jz .tail
.loop:
    movdqu (%r8), %xmm0
    movdqu (%r9), %xmm1
    pcmpeqb %xmm1, %xmm0
    pmovmskb %xmm0, %eax
    cmp $0xFFFF, %eax
    jne .diff_block
    add $16, %r8
    add $16, %r9
    dec %rcx
    jne .loop
.tail:
    mov %rdx, %rcx
    and $15, %rcx
    jz .equal
.tail_loop:
    movzbq (%r8), %rax
    movzbq (%r9), %rdx
    sub %rdx, %rax
    jne .ret
    inc %r8
    inc %r9
    dec %rcx
    jne .tail_loop
.equal:
    xor %eax, %eax
.ret:
    ret
.diff_block:
    movdqu (%r8), %xmm0
    movdqu (%r9), %xmm1
    pcmpeqb %xmm1, %xmm0
    pmovmskb %xmm0, %eax
    not %eax
    bsf %eax, %eax
    add %rax, %r8
    add %rax, %r9
    movzbq (%r8), %rax
    movzbq (%r9), %rdx
    sub %rdx, %rax
    ret
#endif
