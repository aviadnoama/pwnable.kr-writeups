- Ok I need to find the key using disassble of this arm code
- key1 is the following:
```bash
(gdb) disass key1
Dump of assembler code for function key1:
   0x00008cd4 <+0>:	push	{r11}		; (str r11, [sp, #-4]!)
   0x00008cd8 <+4>:	add	r11, sp, #0
   0x00008cdc <+8>:	mov	r3, pc
   0x00008ce0 <+12>:	mov	r0, r3
   0x00008ce4 <+16>:	sub	sp, r11, #0
   0x00008ce8 <+20>:	pop	{r11}		; (ldr r11, [sp], #4)
   0x00008cec <+24>:	bx	lr
```
- r0 is where the return value is, looks like the pc register is the return address - this is actually the equivelnt of the instruction pointer in x86.

- I am not sure how I can understand what it is equal to without debugging the code on runtime, but ok for now.

- Oh wait, the address I need is literaly the line that runs it. aka: 0x00008cdc or in decimal "36060" - cool this is one out of the way.

- The second function is this:
```bash
(gdb) disass key2
Dump of assembler code for function key2:
   0x00008cf0 <+0>:	push	{r11}		; (str r11, [sp, #-4]!)
   0x00008cf4 <+4>:	add	r11, sp, #0
   0x00008cf8 <+8>:	push	{r6}		; (str r6, [sp, #-4]!)
   0x00008cfc <+12>:	add	r6, pc, #1
   0x00008d00 <+16>:	bx	r6
   0x00008d04 <+20>:	mov	r3, pc
   0x00008d06 <+22>:	adds	r3, #4
   0x00008d08 <+24>:	push	{r3}
   0x00008d0a <+26>:	pop	{pc}
   0x00008d0c <+28>:	pop	{r6}		; (ldr r6, [sp], #4)
   0x00008d10 <+32>:	mov	r0, r3
   0x00008d14 <+36>:	sub	sp, r11, #0
   0x00008d18 <+40>:	pop	{r11}		; (ldr r11, [sp], #4)
   0x00008d1c <+44>:	bx	lr
End of assembler dump.
```

- I am kind of confused here. r6 is untouched before that function, so I am not sure what the value it holds.

- Ohh OK. In arm, there the add is using 3 operands. Here r6 is the destination - it now holds pc + 1

- Then there is a "branch" - jmp in x86. It should jump to the same line tbh, because in the last line we added pc and 1, so next line is pc+1 - I am not sure what this line does to be honest.
After that, we add r3 and 4 (pc + 4) == (36102 + 4), push it, which will save the line:    `0x00008d08 <+24>:	push	{r3}`
in the stack.
- Then, we pop it into pc, which will run    `0x00008d08 <+24>:	push	{r3}` again?
isn't it an infinate loop?

- OK OK Let's wait with that. Let's look into key3:
```bash
(gdb) disass key3
Dump of assembler code for function key3:
   0x00008d20 <+0>:	push	{r11}		; (str r11, [sp, #-4]!)
   0x00008d24 <+4>:	add	r11, sp, #0
   0x00008d28 <+8>:	mov	r3, lr
   0x00008d2c <+12>:	mov	r0, r3
   0x00008d30 <+16>:	sub	sp, r11, #0
   0x00008d34 <+20>:	pop	{r11}		; (ldr r11, [sp], #4)
   0x00008d38 <+24>:	bx	lr
End of assembler dump.
(gdb) 
```
- Seems like here, we return the value of lr - which is the register that holds the return address in arm. In our case:
```bash
   0x00008d7c <+64>:	bl	0x8d20 <key3>
   0x00008d80 <+68>:	mov	r3, r0
```
probably 0x00008d80 or 36224 in decimal.

- Let's go back to key2
- Well to be honest - if I ignore everything that is not r3 (that is then returned) - I can see that it is equal to to instruction pointer + 4 and then untouched until the return:
```bash
  0x00008d04 <+20>:	mov	r3, pc
  0x00008d06 <+22>:	adds	r3, #4
```

- So the second key can be 36100 + 4 or 36104

- Together all of them is 36060 + 36104 + 36224 = 108388

- It didn't work. I belive key1 and key3 are right - probably key2 is wrong

- I read that:
"the add r0,pc instructions adds pc+4 to that value.

the +4 are added by the processor. I think it is because the pc is incremented quite early in the processors "logic", and the add only can read the value of pc afterwards."

- It means that the pc is incremented before the add.

- It means key1 is actually 36064 and key2 is 36106. key3 stays 36224 = 108394

- Still doesn't work - probably key2 I am sure of it.

- OK! I kinda BF my way into 108400 and got the flag! I think I understand now, pc will be equal to the next instruction + 4, so i was a little bit off. Doesn't really matter - I got the flag :)