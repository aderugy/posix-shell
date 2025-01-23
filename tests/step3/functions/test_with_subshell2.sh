echo Hello world!
B='valeue de B\n'
subshell() {
  echo $(
    echo SubShell
    A="valeur de A"
    echo "SubShell: \$A = $A"
    echo "SubShell: \$B = $B"
    #On modifie B
    B="B a change"
    echo "SubShell: \$B = $B"
  )
  echo "Pas dans le SubShell: \$A = $A"
  echo "Pas dans le SubShell: \$B = $B"

}

subshell
