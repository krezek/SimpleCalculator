.data

.code

asm_wchar_to_bcd proc
	; void asm_wchar_to_bcd(const wchar_t* str, size_t strLen, unsigned __int8* pBCD)

	push rbp
	mov rbp, rsp

	push rsi
	push rdi
	push r9

	mov rsi, rcx
	mov rdi, r8

	mov rax, rdx
	and rax, 01h
	
	cmp rax, 0
	je LB_1

	mov ax, word ptr [rsi]
	sub ax, '0'
	mov byte ptr [rdi], al
	add rsi, 2
	inc rdi
LB_1:

LB_2:
	mov ax, word ptr [rsi]
	cmp ax, 0
	je END_1

	sub ax, '0'
	xor r9b, r9b
	or r9b, al
	shl r9b, 4
	add rsi, 2
	mov ax, word ptr [rsi]
	sub ax, '0'
	or r9b, al
	mov byte ptr [rdi], r9b
	add rsi, 2
	inc rdi

	jmp LB_2

END_1:

	pop r9
	pop rdi
	pop rsi
	
	mov rsp, rbp
	pop rbp

	ret

asm_wchar_to_bcd endp

asm_double_dabble_rev proc
	; void asm_double_dabble_rev(unsigned __int8* pBCD, size_t bufLen, unsigned __int64* value, size_t size)

	push rbp
	mov rbp, rsp

	push r10
	push r11
	push r12
	push r13

	mov r10, r9
	shl r10, 3
LB_1:
	cmp r10, 0
	je END_1

	xor r11, r11
	xor r12, r12
LB_2:
	cmp r11, rdx
	je END_2

	mov al, byte ptr [rcx + r11]
	
	shr r12, 1
	rcr al, 1
	rcl r12, 1

	mov ah, al
	and al, 00fh
	and ah, 0f0h
	shr ah, 4

	cmp al, 8
	jl LB_3
	sub al, 3
LB_3:

	cmp ah, 8
	jl LB_4
	sub ah, 3
LB_4:

	shl ah, 4
	or al, ah

	mov byte ptr [rcx + r11], al
	
	inc r11
	jmp LB_2
END_2:

	mov r13, r9
LB_5:
	cmp r13, 0
	je END_4
	
	sub r13, 8

	shr r12, 1
	rcr qword ptr [r8 + r13], 1
	rcl r12, 1

	jmp LB_5
END_4:

	dec r10
	jmp LB_1
END_1:

	pop r13
	pop r12
	pop r11
	pop r10

	mov rsp, rbp
	pop rbp

	ret

asm_double_dabble_rev endp

asm_complete2 proc
	; void asm_complete2(void* value, size_t size)

	push rbp
	mov rbp, rsp

	push r8

	xor rax, rax
	xor r8, r8
LB_1:
	cmp r8, rdx
	je END_1

	xor qword ptr [rcx + r8], 0FFFFFFFFFFFFFFFFh

	cmp r8, 0
	jne END_2

	shr rax, 1
	adc qword ptr [rcx + r8], 1
	rcl rax, 1
	jmp END_3
END_2:

	shr rax, 1
	adc qword ptr [rcx + r8], 0
	rcl rax, 1

END_3:

	add r8, 8
	jmp LB_1
END_1:

	
	pop r8

	mov rsp, rbp
	pop rbp

	ret

asm_complete2 endp

asm_double_dabble proc
	; void asm_double_dabble(unsigned __int8* pBCD, size_t bufLen, void* value, size_t size)

	push rbp
	mov rbp, rsp

	push r10
	push r11
	push r12
	push r13
	
	mov r10, r9
	shl r10, 3
LB_1:
	cmp r10, 0
	je END_1

	xor r13, r13
	xor r12, r12
LB_3:
	cmp r13, r9
	je END_3
	
	shr r12, 1
	rcl qword ptr [r8 + r13], 1
	rcl r12, 1

	add r13, 8

	jmp LB_3
END_3:

	mov r11, rdx
LB_2:
	cmp r11, 0
	je END_2

	dec r11

	mov al, byte ptr [rcx + r11]
	
	shr r12, 1
	rcl al, 1
	rcl r12, 1

	mov ah, al

	cmp r10, 1
	je LB_4

	and al, 01111b
	cmp al, 5
	jl LB_5
	add al, 3
LB_5:

	shr ah, 4
	and ah, 01111b
	cmp ah, 5
	jl LB_6
	add ah, 3
LB_6:

	shl ah, 4
	or al, ah

LB_4:
	mov byte ptr [rcx + r11], al
	
	jmp LB_2
END_2:

	dec r10
	jmp LB_1
END_1:

	pop r13
	pop r12
	pop r11
	pop r10

	mov rsp, rbp
	pop rbp

	ret

asm_double_dabble endp

asm_bcd_to_wchar proc
	; void asm_bcd_to_wchar(unsigned __int8* pBCD, size_t bufLen, wchar_t* n)

	push rbp
	mov rbp, rsp

	push r9
	push r10
	push r11
	
	xor r9, r9
LB_1:
	cmp r9, rdx
	je END_1

	mov al, byte ptr [rcx + r9]
	
	mov r10b, al
	and r10b, 11110000b
	shr r10b, 4
	add r10b, '0'

	mov r11b, al
	and r11b, 00001111b
	add r11b, '0'

	mov byte ptr [r8 + r9 * 4 + 0], r10b
	mov byte ptr [r8 + r9 * 4 + 1], 0
	mov byte ptr [r8 + r9 * 4 + 2], r11b
	mov byte ptr [r8 + r9 * 4 + 3], 0

	inc r9
	jmp LB_1
END_1:

	mov byte ptr [r8 + r9 * 4 + 0], 0
	mov byte ptr [r8 + r9 * 4 + 1], 0

	pop r11
	pop r10
	pop r9

	mov rsp, rbp
	pop rbp

	ret

asm_bcd_to_wchar endp

asm_ignore_leading_zeros proc
	; void asm_ignore_leading_zeros(wchar_t* n)

	push rbp
	mov rbp, rsp

	push r8
	push r9
	push r10

	xor rax, rax

LB_1:
	cmp word ptr [rcx + rax], '0'
	jne END_1
	cmp word ptr [rcx + rax + 2], 0
	je END_1

	add rax, 2
	jmp LB_1

END_1:

	xor r8, r8
LB_2:
	mov r10, r8
	add r10, rax

	cmp word ptr [rcx + r10], 0
	jl END_2

	mov r9w, word ptr [rcx + r10]
	mov word ptr [rcx + r8], r9w

	add r8, 2
	jmp LB_2
END_2:

	pop r10
	pop r9
	pop r8

	mov rsp, rbp
	pop rbp

	ret

asm_ignore_leading_zeros endp

end
