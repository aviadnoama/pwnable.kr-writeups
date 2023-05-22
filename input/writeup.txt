- OK this one is not a h"h pwnable - I need to input stuff in odd ways
- argc need to be 100.
- 'A' (argv 65) needs to be \x00
- 'B' (argv 66) needs to be \x20\x0a\x0d

- I came back MANY days later - after doing harder pwnable challangs.
	+ I am scraping everything and starting anew :)

- I will take as a referance the idea I had back then: 
```bash
gdb --args ./input $(printf "%b" '\xbb') $(printf "%b" '\xbb') ...
```
- I need 99 printfs like this, because the first cmp is if there are 100 args (1 of them is the path)
- Now I need to make sure arg 65 is null and arg 66 is `\x20\x0a\x0d`
	+ After a bit of playing around, in order to make a *null* argument, I need to make add the null to the last argument, because making a new one will create more the 1 *null* arguments.
		* So something like this `bash $(printf "%b" '\xbb\x00') $(printf "%b" '\xbb')`
		* Will create the following arguments: `['\xbb,' '\x00', '\xbb']`
	+ The next one after this needs to be `\x20\x0a\x0d`, this is a problem because `\x0a` is `\n` :(
	+ After some testings, ChatGPT actually found a greate way! Using `$'...` in shell, I can pass hex values, and seems like this way the program is not messing up!
	+ So, something like this:
	+ ```bash
      gdb --args ./input $(printf "%b" '\xbb\x00') $'\x20\x0a\x0d' $(printf "%b" '\xbb')
      ```
    	* Will actually make the program get `['\xbb,' '\x00', '\x20\x0a\x0d', '\xbb']`

- OK cool, now let's test it with 99 arguments ;)
```bash
gdb --args ./input $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb\x00') $'\x20\x0a\x0d' $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb')
```
- AND BOOM!
- # Stage 1 clear!

- OK now I need to use stdin and stderr to pass some hex values. 
- stdin is pretty easy, let's make a file named infile and echo the values:
	+ `echo -ne "\x00\x0a\x00\xff" > infile`
- stderr is a bit more tricky, but let's create an errfile too with the values needed:
	+ `echo -ne "\x00\x0a\x02\xff" > errfile`
- Passing it useing gdb is a bit hard, so using the *r* gdb command I can pass everything like this:
```bash
➜  input gdb ./input

r $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb\x00') $'\x20\x0a\x0d' $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') < infile 2< errfile
```
- I use just normal redirect, to pass the data to stdin (fd 0)
- I use `2< errfile` to pass the content inside errfile to stderr (fd 3) instead of stdin
- # Stage 2 clear!

- Now env variables..
- I played around with this too! the easiest way to do so is a cool trick I figured out!
	+ If you use the `env` command you can add envs to programs. the env command is really cool, because it allows us to use even hex values!
	+ So I need to run gdb with the env command like this:
	```bash
	env $'\xde\xad\xbe\xef'=$'\xca\xfe\xba\xbe' gdb ./input
	```
- # Stage 3 clear

- Now I need to create a file with the name `\x0a`, I had a bit of problems, but ChatGPT is the GOAT here,
- reminding me the use of `$'\x0a'`
```bash
echo -ne "\x00\x00\x00\x00" > $'\x0a'
```
- # Stage 4 clear :)
- This one is a socket input.
	+ The port to connect to is set in the 'C' argument (67th one) - let's use *9090*
```bash
r $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb\x00') $'\x20\x0a\x0d' $(printf "%b" '9090') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') < infile 2< errfile
```
- Now using `netstat -putan | grep 9090` I saw the program is listening!
	+ Using `echo -ne "\xde\xad\xbe\xef" | nc 127.0.0.1 9090` I passed stage 5!
- # Stage 5 clear

- OK OK! Now I need to run it on the actual server!!

- OOF.. It works in my model home, but on the server it does not. The problem is in the arguments - I need to figure this out, which is not that simple with no symbols
- It took some work but I found the args with out symbols!
```bash
# First of all let's get the pointer to argv
(gdb) p/x $rbp-0x60        
$20 = 0x7ffc59c715b0

# Now let's get the value inside it
(gdb) p/x *0x7ffc59c715b0
$23 = 0x59c716f8

# Now we need to add the missing 4 bytes "prefix" (for some reason it's not there)
# 0x59c716f8 -> 0x7ffc 59c716f8
(gdb) p/x *0x7ffc59c716f8
$24 = 0x59c72d0e

# The value we got is the pointer to argv[0], now let's examine it (also adding the prefix)
(gdb) x/20s 0x7ffc59c72d0e
0x7ffc59c72d0e:	"/home/input2/input"
0x7ffc59c72d21:	"\273"
0x7ffc59c72d23:	"\273"
0x7ffc59c72d25:	"\273"
0x7ffc59c72d27:	"\273"
0x7ffc59c72d29:	"\273"
...
```
- I found out the problem, the *null* arg is not there 
```bash
mkdir /tmp/tmpo
cd /tmp/tmpo
echo -ne "\x00\x0a\x00\xff" > infile
echo -ne "\x00\x0a\x02\xff" > errfile
echo -ne "\x00\x00\x00\x00" > $'\x0a'

env $'\xde\xad\xbe\xef'=$'\xca\xfe\xba\xbe' ~/input $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $'\x00' $'\x20\x0a\x0d' $(printf "%b" '9090') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') $(printf "%b" '\xbb') < infile 2< errfile

# On a diffrent shell
echo -ne "\xde\xad\xbe\xef" | nc 127.0.0.1 9090
```
- It worked! BUT it didn't print the flag!!
	+ Why? because I ran it from /tmp/tmpi and the code runs `/bin/cat flag`
	+ Let's create a symlink of the flag: `ln -s ~/flag flag`

- NICE! I got the flag!
- # Mommy! I learned how to pass various input in Linux :)
- # pwned.