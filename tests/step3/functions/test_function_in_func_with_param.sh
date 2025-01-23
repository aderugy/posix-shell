A=1
echo $A
fun() {
  ls() {
    echo "\$1 = $1"
    echo "\$* = $*"
  }
  echo "\$A = $A"
  ls $3 hello world
}
fun 1 $A 3
