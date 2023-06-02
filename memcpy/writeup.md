- OK OK! new day, new challange
- First thing first, seems like there is no hacking in this challange - we will see if that's true
- There is a problem here - the source code has the following line:
`// compiled with : gcc -o memcpy memcpy.c -m32 -lm`
- After trying to compile this code, I found out I can't compile a 32bit elf on my 64bit system - I CAN make it work if needed - for now let's wait with it
- After compiling it (to 64bit) I tried using it normally. Seems like I just plug in numbers (for now I have no idea what is going on in the background)
- After I put in 9 numbers - the flag SHOULD be printed - atleast acording to the source code I have
- This is defintly not going to work but let's have a go
- # The actuall code runs on `nc pwnable.kr 9022`
- Guess what - I get the elapsed time with no flag :( (shocking!)
- Seems like the code stoped after the *5th* test.. that's odd
- It's cyber time then, let's understand the c code
- Just like last time (the asm challange) I will comment every line of code to make sure I understand everything
```c
// compiled with : gcc -o memcpy memcpy.c -m32 -lm
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/mman.h>
#include <math.h>

// rdstc is getting the current CPU time - we use it to check how much time it took for something to run
unsigned long long rdtsc(){
        asm("rdtsc");
}

// Just copy byte by byte from src to dst - pretty simple
char* slow_memcpy(char* dest, const char* src, size_t len){
	int i;
	for (i=0; i<len; i++) {
		dest[i] = src[i];
	}
	return dest;
}

char* fast_memcpy(char* dest, const char* src, size_t len){
	size_t i;
	// 64-byte block fast copy
	if(len >= 64){
		i = len / 64;
		len &= (64-1);
		while(i-- > 0){
			__asm__ __volatile__ (
			"movdqa (%0), %%xmm0\n"
			"movdqa 16(%0), %%xmm1\n"
			"movdqa 32(%0), %%xmm2\n"
			"movdqa 48(%0), %%xmm3\n"
			"movntps %%xmm0, (%1)\n"
			"movntps %%xmm1, 16(%1)\n"
			"movntps %%xmm2, 32(%1)\n"
			"movntps %%xmm3, 48(%1)\n"
			::"r"(src),"r"(dest):"memory");
			dest += 64;
			src += 64;
		}
	}

	// byte-to-byte slow copy
	if(len) slow_memcpy(dest, src, len);
	return dest;
}

int main(void){

	setvbuf(stdout, 0, _IONBF, 0);  // Setting stdout to have no buffer
	setvbuf(stdin, 0, _IOLBF, 0);  // Setting stdin to have no buffer

	printf("Hey, I have a boring assignment for CS class.. :(\n");
	printf("The assignment is simple.\n");

	printf("-----------------------------------------------------\n");
	printf("- What is the best implementation of memcpy?        -\n");
	printf("- 1. implement your own slow/fast version of memcpy -\n");
	printf("- 2. compare them with various size of data         -\n");
	printf("- 3. conclude your experiment and submit report     -\n");
	printf("-----------------------------------------------------\n");

	printf("This time, just help me out with my experiment and get flag\n");
	printf("No fancy hacking, I promise :D\n");

	unsigned long long t1, t2; // t1 and t2 will hold the times
	int e;  // Will be used to get memcpy amounts
	char* src;  // Will hold the src data
	char* dest;  // Memory from src will be copied to it
	unsigned int low, high;  // Just to get input from the user
	unsigned int size;  // Every size the user will insert will be inserted to it before it will be added to "sizes"
	// allocate memory

	// addr is null (the first arg) - so the kernel will choose the address
	// mmap will allocate 0x4000 bytes
	// the 3rd arg is the protection - there are 4 options:
	// #define	PROT_NONE	0x00	/* no permissions */
        // #define	PROT_READ	0x01	/* pages can be read */
        // #define	PROT_WRITE	0x02	/* pages can be written */
        // #define	PROT_EXEC	0x04	/* pages can be executed */
        // 0|1|2|4 = 7 - so the memory is rwx :)
	// MAP_PRIVATE - will make the memory copy-on-write
	// MAP_ANONYMOUS - this means the mapping is in memory, no fd, no offset - it's not backed by any file
	// the fd is -1 and the offset is 0 (because we are using MAP_ANONYMOUS)
	char* cache1 = mmap(0, 0x4000, 7, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
	char* cache2 = mmap(0, 0x4000, 7, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
	
	src = mmap(0, 0x2000, 7, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);  // The explanation of cache is pretty much the same, just the size is smaller

	size_t sizes[10];  // All the sizes the user gave 
	int i=0;  // Used just the way you think

	// This loop is not that intersting - just getting sizes from the user to copy each test
	// setup experiment parameters
	for(e=4; e<14; e++){	// 2^13 = 8K
		low = pow(2,e-1);
		high = pow(2,e);
		printf("specify the memcpy amount between %d ~ %d : ", low, high);
		scanf("%d", &size);
		if( size < low || size > high ){
			printf("don't mess with the experiment.\n");
			exit(0);
		}
		sizes[i++] = size;
	}

	sleep(1);
	printf("ok, lets run the experiment with your configuration\n");
	sleep(1);

	// run experiment
	for(i=0; i<10; i++){
		size = sizes[i];
		printf("experiment %d : memcpy with buffer size %d\n", i+1, size);
		dest = malloc( size );

		memcpy(cache1, cache2, 0x4000);		// to eliminate cache effect
		t1 = rdtsc();
		slow_memcpy(dest, src, size);		// byte-to-byte memcpy
		t2 = rdtsc();
		printf("ellapsed CPU cycles for slow_memcpy : %llu\n", t2-t1);

		memcpy(cache1, cache2, 0x4000);		// to eliminate cache effect
		t1 = rdtsc();
		fast_memcpy(dest, src, size);		// block-to-block memcpy
		t2 = rdtsc();
		printf("ellapsed CPU cycles for fast_memcpy : %llu\n", t2-t1);
		printf("\n");
	}

	printf("thanks for helping my experiment!\n");
	printf("flag : ----- erased in this source code -----\n");
	return 0;
}
```
- I have not commented everything yet - but I am staring to get the idea
- I am actually starting to think about what the actuall challange is
- # There is no file to read - seems like the flag is plain text in the binary (inside the printf)
- Seems like for some reason their code just stops? I don't get any error, seg fault or something so I am not sure
- It works when I run it so something is odd.
- I have a feeling the challange is one of 2 things
- # Either somehow making their code reach the end, or read the string from memory somehow
- I noticed that using the output (minimum):
```
8
16
32
64
128
256
512
1024
2048
4096
```
- Are reaching test 5
- Those number on the other hand (maximum):
```
16
32
64
128
256
512
1024
2048
4096
8192
```
- Reach test 4
- I wonder if this is brute force-able actually
- Ok there are way too many options to use BF, at least not foolishly
- I wanted to see why the code stops - because there is not logic in the source code, so it must be a crash!
- # I compiled it on the pwnable server as 32bit elf and BOOM! it crashes with segfault
- `Program received signal SIGSEGV, Segmentation fault.`
- Using gdb (I compiled with symnbols of course) I found out where the crash is: 
- `=> 0x80487cc <fast_memcpy+52>:	movntps XMMWORD PTR [edx],xmm0` 
- It's actually late, so I'll go to sleep now, but so far so good!
	+ Tomarrow I should use gdb to see what in memory is causing this segfault
	+ What in the first input is causing it to fail on test 5, and the second on test 4
	+ Then - probably building my way to give inputs that will allow the code to finish it's execution

- Here I am back, compiled it again on the pwnable server, with symbols this time
- Builiding a payload with `echo -ne '16\n32\n64\n128\n256\n512\n1024\n2048\n4096\n8192' > shellcode.bin`
- The seg fault happends inside of fast_memcpy
	+ I use `b *0x080487a4` to break one line after `DWORD PTR [ebp+0x10],0x3f`
	+ `DWORD PTR [ebp+0x10],0x3f` is the check `if(len >= 64){`
		* The idea is checking what is the diffrence when it crash and when it's not
- The first break happends when `len=64`, but it it's not crashing

# Let's start by understanding __asm__ __volatile__
- So first of all `__asm__ __volatile__` will run assembly code, and make the code run "as is"
	+ This means that on compilation there won't be any optimzation vodo to the code

```c
__asm__ __volatile__ (
// movdqa - Move Aligned Double Quadword
// It is usde to move XMM register, but WAIT what is an XMM register?
// XMM register is 128-bit size register, there are 8 of them in non-64bit modes and 16 in long mode
// The main idea of XMM registers is to handle data, and CAN NOT be used to address memory
// The xmm registers can be handled like diffrent sized arrays

// %0 is equal to the "first input/output operand defined in your code"
// I debuged it using gdb, it's actually eqaual to the src address that fast_memcpy is getting
// %1 is equal to dest
// To sum it up:
//	%0 = src =  eax
//      %1 = dest = edx
// What is actually happaning here I belive, is that the memory inside the src varibale is stored inside the XMM registers
"movdqa (%0), %%xmm0\n"
"movdqa 16(%0), %%xmm1\n"
"movdqa 32(%0), %%xmm2\n"
"movdqa 48(%0), %%xmm3\n"

// movntps - Store Packed Single-Precision Floating-Point Values Using Non-Temporal Hint
// This is a very long name for an opcode, the documnatation says:
// Moves the packed single-precision floating-point values in the source operand (second operand) to the destination operand (first operand) using a non-temporal hint to prevent caching of the data during the write to memory. The source operand is an XMM register which is assumed to contain packed single-precision, floating-pointing. The destination operand is a 128-bit, 256-bit or 512-bit memory location. The memory operand must be aligned on a 16-byte (128-bit version), 32-byte (VEX.256 encoded version) or 64-byte (EVEX.512 encoded version) boundary otherwise a general-protection exception (#GP) will be generated.

// What I believe is happaning here, is that this opcode is storing the data inside the XMMs register inside the dest registers
"movntps %%xmm0, (%1)\n"
"movntps %%xmm1, 16(%1)\n"
"movntps %%xmm2, 32(%1)\n"
"movntps %%xmm3, 48(%1)\n"
::"r"(src),"r"(dest):"memory");
```

- The actual code I am seeing in GDB is actually the opisite?
```c
=> 0x080487b9 <+33>:	movdqa xmm0,XMMWORD PTR [eax]
   0x080487bd <+37>:	movdqa xmm1,XMMWORD PTR [eax+0x10]
   0x080487c2 <+42>:	movdqa xmm2,XMMWORD PTR [eax+0x20]
   0x080487c7 <+47>:	movdqa xmm3,XMMWORD PTR [eax+0x30]
   0x080487cc <+52>:	movntps XMMWORD PTR [edx],xmm0
   0x080487cf <+55>:	movntps XMMWORD PTR [edx+0x10],xmm1
   0x080487d3 <+59>:	movntps XMMWORD PTR [edx+0x20],xmm2
   0x080487d7 <+63>:	movntps XMMWORD PTR [edx+0x30],xmm3
```
- In the src code:
`movdqa (%0), %%xmm0` (%0 stands for src)
- The actual code:
`movdqa xmm0,XMMWORD PTR [eax]` (eax stands for src)
	+ WHATTT?
- I'll ignore it for now, because I think that the code that was compiled is doing the right thing
- Now that I understand the code, let's see where it crash
	+ `0x080487cc <+52>:	movntps XMMWORD PTR [edx],xmm0`
	+ This means that the data was successfuly transfered to the XMMs
		* Then, when trying to write to memory it failed.
	+ Let's break only in this line `b *0x080487cc`
		* See what is inside xmm and edx the first time, when it works
		* Then the second time when it failes!
		* # TODO: The above :)
- I'm back!
- I ran the test above, seems like the diff is the address `edx` holds.
	+ In the 3th run it is `0x804d050`
	+ In the 4th run it is `0x804d098`
- I am not sure if this is what causing the segfault, but let's see if changing the payload changes the addresses

- Running it with the payload `echo -ne '8\n16\n32\n64\n128\n256\n512\n1024\n2048\n4096' > shellcode.bin` is actually giving us the addresses
	+ In the 4th run it is `0x804d060`
	+ In the 5th run it is `0x804d0a8`
- This is intersting, because this payload is making us go into `if(len >= 64){` just in the 4th run, thus we "survive" one more iteration

- I can say that the problem is indeed inside of fastcopy
- If so, I have 2 options
	+ Trying not to go inside fastcopy
	+ Understanding what makes fastcopy crash and fix it
- If i will take a look on the following code:
```c
char* fast_memcpy(char* dest, const char* src, size_t len){
	size_t i;
	// 64-byte block fast copy
	if(len >= 64){
		i = len / 64;
		len &= (64-1);
		while(i-- > 0){
			...
```
- This is the code that will decide if to run the fastcopy logic
- `i` is equal to `len / 64` , I control `len`
	+ If `i` is `0`, `i--` will return `i` first and only then will increase it
- In order to understand the memory permissions I ran
`info proc`
	+ Then using the pid `!cat /proc/363489/maps`
```bash
0804c000-0806e000 rw-p 00000000 00:00 0   [heap]
```
- I can see that the heap ends in `0x0806e000`
	+ I was afraid that maybe I try to write more memory then the heap holds - but it's not the problem from what I see

- I have an idea - reasearching about segfault in `movntps`, I found about *memory aligment*
	+ The question I saw what about `movups` but I guess the same idea works here
- The memory address offset must be *aligend to 16 bit* - I test it out to see - every time `dest` is not ending with `0` (AKA not aligned to 16 bit) it crashes
- I used gdb to `set dest=0x804d010` every time dest had a bad memory address and the code *didn't crash!*
- # Now I need to understand how to make sure the addresses are always good

- I figured out a way!
- What I do, is I run the code with gdb, using a break point on `fast_memcpy`
	+ Every time segfault is raised I find the memory address given by `malloc`, let's say `dest=0x804d4d8`
	+ Then, I find out the last *good* address we got in the last iteration, let's say `dest=0x804d2d0`
	+ By doing `0x804d4d8 - 0x804d2d0` I get the addresses diff, and this is the size I needed to allocate in the last iteration! (in order to make the next `malloc` call return a 16 bit aligned address)
	+ So in this case, *experiment 8* crashed, because I needed to give *520* in *experiment 7* and so on.

- The reason it works, is that malloc is allocating *continues 8 bit aligned* memory, so we need to make sure that we gives sizes that will result in *16 bit aligned* memory to make sure `movntps` won't result in a segfault


- I managed to use the following payload: `!echo -ne '8\n24\n60\n104\n200\n264\n520\n1032\n2056\n4096' > shellcode.bin`
	+ It worked on my compiled binary!

- Now, just running it on the actuall challage ;)
- `nc 0 9022 < shellcode.bin`
- BOOM!

- # FLAG: 1_w4nn4_br34K_th3_m3m0ry_4lignm3nt
- # Pwned.