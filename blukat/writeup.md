- OK so there is some C code (like always)
- There is a key in here. The key is xor'ed with a string that I can't read sadly that is stored in the file "password". The xor's result must be a valid ASCII right? so techinicly it must be value between:
33 	21 	00100001 	&#33; 	! 	Exclamation mark

and

126 	7E 	01111110 	&#126; 	~ 	Tilde

- About 93 options. Well ok.
- I have an idea - Maybe I can try using a timeing attack in here - if strcmp is going charater by character - maybe I can mesure the time it takes?
# Using the time command we can do so! `time <command>`

- I tried running a few tests - I don't see anything concrete - Maybe it can work in lab enviorment, but I am not sure it can help me here

- WAIT! I have a instestig idea. I don't actually need to guess the password. All I need is to make strcmp return something that is not 0! If so, maybe the program will print the "calculated flag" anyway! I need to see if there is a way to make strcmp return something that is not 0 or 1.

- OK oof I was mistaken. strcmp return 0 when the strings are equal (so `!strcmp` is true when the strings are equal and 0 is returned) - other return values are bigger then 0 or smaller then 0 - depends if the flag is bigger or not.

# WAITTT - if so I can use gdb to read the return value - if the string is smaller - I need to increase/decrease!

- The algorithem should be something like this:


# FOR EXAMPLE:

real password: aaa

try: b<values that are less then 33 which is the lowest value for a readable ascii value>
result: +1

try: a<values that are less then 33 which is the lowest value for a readable ascii value>
result: +1 (because the first charcter is the same, and second charcter should *always* be bigger then my second value)

so a is the first letter! and so on and so on!

- Let's start doing something like this then.

# gdb -x can read a command file!

```bash
set max-value-size unlimited
set print elements 0 
set print repeats 0
set pagination off
set logging file gdb.log
set logging on

b *0x0000555555555292
r < password_to_guess.txt
p/d $eax
q
```

- and it worked!
- When my password started smaller - 1 was returned, when it was bigger -1 was returned!

- Now let's write a python script to automate it!

- WAIT A MINUTE!

# I just found out that strcmp is not returning -1 or 1 - it returns the diff between the charcters! it means that strcmp('a', 'b') returns 1 and strcmp('a', 'c') return 2 and etc!

```python
from os import system


def try_password():
	system('gdb -x gdb_guess_script.gdb /home/blukat/blukat')
	with open('gdb.log', 'r') as f:
		strcmp_return_value = int(f.read().split('$1 = ')[1].split('\n')[0])
	with open('gdb.log', 'w') as f:
		f.write('')

	return strcmp_return_value

def main():
	position_to_guess = 0
	password = 'b'
	while True:
		with open('password_to_guess.txt', 'wb') as f:
			f.write(password.encode())
		
		strcmp_return_value = try_password()
		try:
			password = bytearray(password, 'ascii') 
			password[position_to_guess] = password[position_to_guess] + strcmp_return_value
			password = password.decode('ascii')
		except UnicodeDecodeError:
			password[position_to_guess] = ord('a')
			password = password.decode('ascii')
		print(strcmp_return_value)
		print(password)
		input()
		print('----')
		position_to_guess += 1
		password += 'a'

if __name__ == '__main__':
	main()
```

- And it worked! strcmp is not returning the diff value  consistently on all systems, but on my model - it does. if it will work like this on the pwnable server - then it's an easy win!

- Ok first thing first I need to find the address to use a breakpoint on  - there are no symbols so let's find the address

- Cool - found the address - *0x000000000040086d (test eax, eax)
- Now let's setup everything

```bash
mkdir /tmp/blukat
cd /tmp/blukat
vim gdb_guess_script.gdb # copy my script, with required changes)
vim guess.py # copy my code, with required changes)
touch gdb.log
```

- OH NO!! - I ran it and it didn't work! Maybe I can't run it using GDB? I get permission denied!
- But.. wait a minute - why is it `cat: password: Permission denied`? it's not indicative about anything I did myself? I am not using cat..

# ARE YOU KIDDING ME! THIS IS THE ACCTUAL PASSWORD!

# FLAG: Pl3as_DonT_Miss_youR_GrouP_Perm!!
- Kinda funny - group perms? Maybe I did it "the wrong" way? hahaha

- (I looked at the writeups right now - you can littraly do it like this `./blukat < password` WTFFF)
- Looking at ls -la - I can see the group blukat_pwn *can read the password fileeee* DAFUCKK

- (I looked at ANOTHER writeup - you can littraly do it by reading the variable `password` like this `x/s 0x6010a0` OMG..)

- (EVEN WORSE! Someonw used strace/ltrace - OMG - `strace ./blukat` let you see the password plain text in the `read` syscall, and `ltrace ./blukat` let's you see it plain text in the `strcmp` function)



## #PWNED