A=1
echo $A
fun() {
  A=2
  echo $3
}
fun 1 2 3
echo $A
