i=10
while [ $i -gt 1 ]; do
  echo $i
  while [ $i -gt 1 ]; do
    echo "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
    continue 2
  done
  echo "b"
  i=0

done
