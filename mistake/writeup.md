- Running ssh mistake@pwnable.kr -p2222
- There was a binary there with it's source code
- There is a function called xor, which takes a string, and xors every char with a key
- The main is opening the file /home/mistake/password
- Then in order to disable BF, there is a sleep
- The main reads 10 bytes from the password file, then takes my input and xor it with the key, and then use strcmp.
- I tried running GDB in order to get the password from the memory but it does not work because I don't have the permissions to read the password file when using GDB
- Ok, so the xor key is 1. It means that if the bit I give is 1 - the output will be 0, if it's 0 it will be 1 - so every bit is flipped.
- strncmp is checking if two strings are equal, bigger then or smaller then each other - considering just the n bytes given as argument
- The sleep function is also quite odd - instead of just waiting a few seconds - it uses "time(0) % 20" - if I can change the time maybe I can win here too?
- Just noticed - there is a line in the pwnable site: "hint : operator priority" - that's cool, but I can't see right a way a place where there is a bug there.
- I have a few ideas about it - let's compile my own version - which read my own password file
"gcc -Wall -g mistake.c -o symmistake"
- Well - pretty easy - found the bug. the line "if(fd=open("/home/mistake/password",O_RDONLY,0400) < 0){" has a bug
fd is equal to "open("/home/mistake/password",O_RDONLY,0400) < 0" - which is not the return value of open, but a boolean value, which is always equal to 0 in this case. 0? fd 0 is stdin - which is great for me! I can just put my own password!
- Now I just need to find two 10 characters strings, that the second one after xor with 1 is equal to the first
- So, I used gdb, to see what happends to the pair of strings "aaaaaaaaaa" as the password and "bbbbbbbbbb" as the key.
- After the xor, the b, turns into c - and "aaaaaaaaaa" is not equal to "bbbbbbbbbb" - but that's it, I did it.
- I ran the binary on the server with the password "cccccccccc" and the key as "bbbbbbbbbb" - And got the flag! :)

# "Mommy, the operator priority always confuses me :("
# pwned.