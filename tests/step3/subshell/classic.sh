B=$(echo a)
echo $B
A=$(echo A | tr A E)
echo $A
C=$(A=E; echo $A | tr A E)
echo $C$A
echo $(echo $A$B$C)
