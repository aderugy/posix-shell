# ekip49
le repo de secours pour 42sh

## -v
Verbose mode : 

```
42sh$ -v
42sh$Test Hello
```
## -c
Command Line Interpretation :
```
42sh$ -c "echo Hello World"
42sh$Hello World
```

## -t
Token Pretty Print :
```
42sh$ -t "echo Hello World;"
42sh$Test hello
42sh$echo hello world; 
42sh$word: echo
42sh$word: hello
42sh$word: world
42sh$;
42sh$EOF
```
```
