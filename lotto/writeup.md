- Ok I opened the code - and I already see that if match == 6 we get the flag.
- I think about BF, but I think that it's not the right idea in here to be honest. Let's see how the code works.
- So the code works like that:
open /dev/urandom and get an fd
then read 6 bytes from it
If the file can't be opened, or the bytes can't be read - the code crashes.
- Then there is the follownig loop:
	for(i=0; i<6; i++){
		lotto[i] = (lotto[i] % 45) + 1;		// 1 ~ 45
	}
This means that the lotto numbers are between 1 to 45.

Then, for every right number I submit I get 1 score. If everything is right I get 6!

- Looking into it a bit more, I see that IT DOESN'T MATTER where the number is - if the lotto starts with 5 and I have 5 in my guess anywhere - it's good.

- Ahah! Another thing I can see, is that there is no break after a successful run! that means that if I guess 6 numbers that are allways the same let's say:
[1, 1, 1, 1, 1, 1]

and the lotto will start with 1 I will get 6 matches - with out considering anything else inside the lotto!
I do have to make sure that there is no another "1" in the lotto, because then I will get MORE then 6 matches.


- EZ. I didn't even need to write a script, I ran the lotto program with gdb and just used '------' ('-' == 45 in ascii). I read the local variables to check if 45 is inside the lotto varaible.
after a few tries I managed to run /bin/cat! but because I was inside of gdb it didn't worked.

- I ran the the program without gdb, and after a few tries I made it :)

# sorry mom... I FORGOT to check duplicate numbers... :(
# pwned.