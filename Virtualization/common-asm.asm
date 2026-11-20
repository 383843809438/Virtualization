
.CODE
EXTERN VMXSetupVmcs:PROC

PUSHAQ MACRO
push    rax
push    rcx
push    rdx
push    rbx
push    rbp
push    rsi
push    rdi
push    r8
push    r9
push    r10
push    r11
push    r12
push    r13
push    r14
push    r15
ENDM

POPAQ MACRO
    pop     r15
    pop     r14
    pop     r13
    pop     r12
    pop     r11
    pop     r10
    pop     r9
    pop     r8
    pop     rdi
    pop     rsi
    pop     rbp
    pop     rbx
    pop     rdx
    pop     rcx
    pop     rax
ENDM

AsmVmxLaunch PROC
	pushfq
	PUSHAQ
	
    mov rax,rcx;把InitVMCS函数指针放在rax
    mov rcx,rsp
	mov rdx,VmLaunchToGuest;传入RIP
	sub rsp,100h;提升栈顶，给一些局部变量分配足够的栈空间
	call rax;调用InitVMCS
	add rsp,100h
	
    ;若成功，不会执行到这里
	POPAQ
	popfq
	xor rax,rax
	ret ;返回1代表true

VmLaunchToGuest:

	POPAQ
	popfq
	xor rax,rax
	inc rax
	ret

AsmVmxLaunch ENDP





CmVmCall PROC
	
	vmcall
	ret
CmVmCall ENDP
END