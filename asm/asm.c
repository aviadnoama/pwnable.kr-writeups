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
	setvbuf(stdin, 0, _IOLBF, 0); // Set stdout as unbuffered

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
	alarm(1000);
	
	// OK we are getting chrooted - there are tricks to break it I belive, but for now let's let it be
	chroot("/home/asm_pwn");	// you are in chroot jail. so you can't use symlink in /tmp
	
	// Now sandbox is called!
	sandbox();

	// ANDD the code is executed - starting with the stub code, then mine
	((void (*)(void))sh)();
	return 0;
}