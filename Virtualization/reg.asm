; reg_x64.asm - x64 implementations for RegGetES and GetGdtBase
; MASM64 syntax



OPTION PROC:PUBLIC


.code
RegGetES PROC
	; Read ES register into AX, zero-extend to RAX and return
	mov ax, es
	movzx eax, ax
	ret
RegGetES ENDP
RegGetCS PROC
	; Read CS register into AX, zero-extend to RAX and return
	mov ax, cs
	movzx eax, ax
	ret
RegGetCS ENDP
RegGetDS PROC
	; Read DS register into AX, zero-extend to RAX and return
	mov ax, ds
	movzx eax, ax
	ret
RegGetDS ENDP



RegGetSS PROC
	; Read SS register into AX, zero-extend to RAX and return
	mov ax, ss
	movzx eax, ax
	ret
RegGetSS ENDP
RegGetFS PROC
	; Read FS register into AX, zero-extend to RAX and return
	mov ax, fs
	movzx eax, ax
	ret
RegGetFS ENDP
RegGetGS PROC
	; Read GS register into AX, zero-extend to RAX and return
	mov ax, gs
	movzx eax, ax
	ret
RegGetGS ENDP

RegGetDr0 PROC
	; Read DR0 register into RAX and return
	mov rax, dr0
	ret
RegGetDr0 ENDP

RegGetDr1 PROC
	; Read DR1 register into RAX and return
	mov rax, dr1
	ret
RegGetDr1 ENDP

RegGetDr2 PROC
	; Read DR2 register into RAX and return
	mov rax, dr2
	ret
RegGetDr2 ENDP

RegGetDr3 PROC
	; Read DR3 register into RAX and return
	mov rax, dr3
	ret
RegGetDr3 ENDP

RegSetDr0 PROC
	mov dr0, rcx
	ret
RegSetDr0 ENDP
RegSetDr1 PROC
	mov dr1, rcx
	ret
RegSetDr1 ENDP
RegSetDr2 PROC
	mov dr2, rcx
	ret
RegSetDr2 ENDP
RegSetDr3 PROC
	mov dr3, rcx
	ret
RegSetDr3 ENDP

GetGdtBase PROC
	; Allocate stack space (shadow + local) and use sgdt to store GDTR
	; SGDT stores: 2-byte limit at offset 0, followed by 8-byte base at offset 2 in 64-bit mode
	sub rsp, 40
	lea rdx, [rsp]
	sgdt [rdx]
	mov rax, qword ptr [rdx+2]
	add rsp, 40
	ret
GetGdtBase ENDP

GetLdtr PROC
    xor eax, eax
    sldt ax
    ret
GetLdtr ENDP

GetTrSelector PROC
    xor eax, eax
    str ax
    ret
GetTrSelector ENDP



GetGdtLimit PROC
    sub rsp, 10h

    sgdt [rsp]

    movzx eax, word ptr [rsp]

    add rsp, 10h
    ret
GetGdtLimit ENDP
GetIdtBase PROC
    sub rsp, 10h

    sidt [rsp]

    mov rax, qword ptr [rsp + 2]

    add rsp, 10h
    ret
GetIdtBase ENDP


GetIdtLimit PROC
    sub rsp, 10h

    sidt [rsp]

    movzx eax, word ptr [rsp]

    add rsp, 10h
    ret
GetIdtLimit ENDP
END
