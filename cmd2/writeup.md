- I finished cmd1 a long time ago and I don't know have a writeup for it so I am taking the flag for it from the internet (for the ssh password)
cmd1 flag: "mommy now I get what PATH environment is for :)"
- OK! let's go.
- I can see some C code - let's h"h it!
- OK so seems like cmd2 is a suid that will be able to cat the flag
- I can run ./cmd2 with an argument that it will execute - seems good right? let's just run "cat flag" - but there are filters :(
- The function filters the following: 
```
"="
"PATH
"export
"/"
"`"
"flag"
```
- If one of those strings are inside the command won't be executed
- Let's take a look on the function delete_env - above it there is the following line:
"extern char** environ;"
- The extern keyword is telling the compiler to look for the varaible outside of the scope 
- Reading the MAN page, it allows us to access the env variables, then the function delete_env, using memcpy to overwrite them.
- I think it's time to compile it with symbols in order to gdb it and learn what is going on!
- Cool so using gdb, I read the variable with p *envp, p *(envp+x) and see the env variables
- After the delete_env, the envp is empty :(
- environ and envp - points to the same variables
- I used getenv to make sure the env variables are actualy getting removed because from the internet I read the envp should not be changed because it holds the parent env variables - I figured that maybe it is not the variable putenv is taking from
- Another important thing is that after the putenv I can't find the new variable inside envp or environ!
- I was wrong, deleting the variables from envp actually removed them from getenv
- I had a cool idea - I can set the HOME env variable to let's say "/bin/ls" then running '~' will run ls and overpassing the filters! the problem is that my env vars are getting deleted - if I can pass that that's a win for me
- OK so I see I can't run things because I don't have a path
- I ran help - to get all the builtin commands in bash - to see if I can use them
- I was trying this idea:
/home/cmd2/cmd2 "bash -c builtin source test.sh" in order to run the code there - but bash is not found
- The system commands run sh and not bash. The way to source in sh is using .[script] - but the problem is that it uses PATH to find it.
- Ok so I came back a good amount of days later! I did a few things, I will write the important stuff for later.
- The main idea, is creating a payload that uses diffrent kind of characters, "opens" itself and run the real code after the filter function made sure there is nothing inside. 
- The problem is that I can't run any program beacuse of the PATH sh'tik so I worked on echo. I found a few diffrences between sh and bash, but I came up with somthing like this:
```bash
$(echo -n '\057bin\057echo test')
```
- I use octal because sh only use this base (yes. yes it does.)
- This has no forward slashes (that the filter function don't like) - BUT! it does not work. Why? because my shell (the one I run ./cmd2 "command" from) is interperting the \0 and when it goes as input to cmd2, it's already "slash" so we fall in filter :(
- As I write the ideas for tomarrow I had a great idea THAT WORKEDDD!!! I will write what I did that didn't work anyway 
- So the first idea was, as I said, making a payload that will open itself, somthing like this:
```bash
$(echo -n '\044\050\0145\0143\0150\0157\040\055\0156\040\047\0134\060\061\066\063\0134\060\061\064\065\0134\060\061\066\064\047\051')
```
- The odd thing is that it worked well on the model I compiled with symbols, but not in the field!!
- I tried a few things, it seems like a parsing f'iston, but I had a great idea!!
- The line $(echo -n '\057bin\057cat \057home\057cmd2\057flag') for example is a great line, if I could send it to the filter function, but the problem was that my bash was interperting the slashes right? SOOO what if I will write this payload into a file and will use $(cat <file>) as input?
- WELL GUESS WHAT, it worked! I create a file inside tmp with this data: `$(echo -n '\057bin\057cat \057home\057cmd2\057flag')`
- Then ran the code like this: `./cmd2 "$(cat /tmp/bliblo/test.txt)"`
- The first POC, was just writing "hello" with echo - then I tried to print the flag
- It did not work! But I knew it's not something with the way I did it, because it worked with echo hello, so it must be somthing in the filter
- Indeed the word "flag" is filterd - so I used the following command instead: `$(echo -n '\057bin\057cat \057home\057cmd2\057fla*')`
- EASY PEASYYY
- # FuN_w1th_5h3ll_v4riabl3s_haha
- # pwend.
