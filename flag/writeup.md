- I moved the elf into one of pwnable's machines to make debuging easier

- Ok I had a few things I did - the binary was really hard to break down.
- I saw UPX in strings (I won't lie - I also saw it in a writeup for this one - I DIDN'T READ THAT! but I saw the flag name :( )
- I remember that UPX is a cool idea that compresses a binary, decompress it on run time and then runs it
- I moved to my kali, used upx decompress and got a REAL binary I can work with
- I disassembled the binary (break point on main) - I saw the following line:
`0x0000000000401184 <+32>:	mov    0x2c0ee5(%rip),%rdx        # 0x6c2070 <flag>`

- There is an address of "flag" there. I used "p/x *0x6c2070" to get the pointer and then read the string like this:
p (char*)0x496628

- I GOT THE FLAG!
# UPX...? sounds like a delivery service :)
# pwned.
