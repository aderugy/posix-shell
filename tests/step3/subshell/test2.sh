<<<<<<< HEAD
echo "Parent Shell: Value of  is $pshell"
=======
pshell="pshells"
echo "Parent Shell: Valuef is $pshell"
>>>>>>> 0c2227eb84b39659deefe89dcafdfa69ba3c364a

# Create a subshell and modify pshell
(
  pshell=10
  echo "Subshell: Value of pshell is $pshell"
<<<<<<< HEAD
=======
  echo "aaa"
>>>>>>> 0c2227eb84b39659deefe89dcafdfa69ba3c364a
)

# Check the value of pshell in the parent shell
echo "Parent Shell: Value of pshell is $pshell"
<<<<<<< HEAD
=======
echo a
>>>>>>> 0c2227eb84b39659deefe89dcafdfa69ba3c364a
