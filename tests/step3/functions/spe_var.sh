fun()
{
    echo bar;
}
foo()
{
    echo Guinness
    bar()
    {
        echo Baby Guinness
        baz()
        {
            echo $2
            echo 1664;
        }
        echo biere;
        echo $@;
        baz dummy 2
    }
    echo $1
    fun
    bar 86 blonde brune
    baz huhuhuh Heineken
}

foo grimbergen
