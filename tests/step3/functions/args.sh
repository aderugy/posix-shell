foo( ){
    echo $1 $2 $3 ${55};
}

fii(){
    echo biere;
    echo $@;
}

foo 1 2 3 4 5
foo 5 4 3 2 1
fii
foo fii
fii foo

echo $?
