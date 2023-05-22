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

