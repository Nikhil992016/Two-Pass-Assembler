ldc 0x1000 
a2sp 
adj -1 
ldc 11 
stl 0 
ldc array
call sort
HALT 

sort:
adj -1
stl 0
stl 2
adj -3
ldc 0
stl 0
ldc 1
stl 1
ldc 0
stl 2

loop_out:
ldl 4
ldl 2
sub
ldc 1
sub
brz done
ldc 1
stl 1

loop_in:
ldl 4
ldl 2
sub
ldl 1
sub 
brz addx
ldl 5
ldl 1
ldc 1
sub
ldl 5
add
stl 6
ldl 1
ldl 5
add
stl 7
ldl 7
ldnl 0
ldl 6
ldnl 0
sub
brlz swap
br addy

swap: 
ldl 6
ldnl 0
stl 0
ldl 7
ldnl 0
ldl 6
stnl 0
ldl 0
ldl 7
stnl 0

addy:
ldc 1
ldl 1
add
stl 1
br loop_in

addx:
ldc 1
ldl 2 
add
stl 2
ldc 1
ldl 5
add
br loop_out

done:
ldl 3
adj 5
return

array:
data 2 
data 8 
data 66 
data 7
data 5 
data 42 
data 3 
data 11
data 9
data 0
data 0