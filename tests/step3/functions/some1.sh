faa (){
    echo A; echo $0 Munnely
}
foo (){ echo A faa; faa }
foo faa faa
faa
