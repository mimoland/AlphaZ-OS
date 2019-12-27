[section .text]

global disp_str

disp_str:
	push	ebp
	push    esi
	push    edi
	push    eax
	push    ebx
	mov	ebp, esp

	mov	esi, [ebp + 24]	; pszInfo
	mov	edi, 0x00
	mov	ah, 0Fh
.1:
	lodsb
	test	al, al
	jz	.2
	cmp	al, 0Ah	; 是回车吗?
	jnz	.3
	push	eax
	mov	eax, edi
	mov	bl, 160
	div	bl
	and	eax, 0FFh
	inc	eax
	mov	bl, 160
	mul	bl
	mov	edi, eax
	pop	eax
	jmp	.1
.3:
	mov	[gs:edi], ax
	add	edi, 2
	jmp	.1

.2:

	pop	ebx
	pop 	eax
	pop 	edi
	pop	esi
	pop 	ebp
	ret
