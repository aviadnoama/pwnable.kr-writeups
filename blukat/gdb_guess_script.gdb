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
