echo "Parent Shell: Value of  is $pshell"

# Create a subshell and modify pshell
(
  pshell=10
  echo "Subshell: Value of pshell is $pshell"
)

# Check the value of pshell in the parent shell
echo "Parent Shell: Value of pshell is $pshell"
