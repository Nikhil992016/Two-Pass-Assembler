loop:
ldc i
ldnl 0
ldc b
ldnl 0
sub
brz done
ldc a
ldnl 0
ldc res
ldnl 0
add
ldc res
stnl 0
ldc i
ldnl 0
ldc 1
add
ldc i
stnl 0
br loop
done:
HALT

i: data 0
a: data -2
b: data 5
res: data 0