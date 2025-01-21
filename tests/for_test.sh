for i in 1 2 3 4 5; do
  if [ $i -eq 3 ]; then
    echa "message"
    for i in 4 5 6 7 8; do
      echo ppppp
      continue 1
      echo "aaaaaaaa"
    done
  fi
  echo "Bonjour $i"
done
