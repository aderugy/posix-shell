for i in 1 2 3 4 5; do
  if [ $i -eq 3 ]; then
    echa "message"
  fi
  echo "Bonjour $i"
done
