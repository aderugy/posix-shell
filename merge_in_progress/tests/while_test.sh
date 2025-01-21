i=10
while [ $i -gt 1 ]; do
    echo $i
    ls
    let "i-=1"
done

