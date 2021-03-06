# GoLite Compiler

- Justin Domingue – 260588454
- Daniel Pham – 260526252
- Seguei Nevarko – 260583807

Reference: https://golang.org/ref/spec

Code viewed:
  - ANSI C grammar, Lex specification: http://www.lysator.liu.se/c/ANSI-C-grammar-l.html
  - ANSI C grammar, Yacc grammar: http://www.lysator.liu.se/c/ANSI-C-grammar-y.html
  - Go 1.2, Yacc grammar: https://github.com/golang/go/blob/402d3590b54e4a0df9fb51ed14b2999e85ce0b76/src/cmd/gc/go.y
  - Go 1.6, Test cases: https://github.com/golang/go/tree/master/test

## Development
#### Compile
```
// In root directory
$ ./run
```
or

```
$ cd src/
$ make
```

#### Testing
Run the parser on all files in programs/
```
$ cd src
$ make test
```

### Usage

Run parser

    $ ./golite parse file

Run pretty printer

    $ ./golite pretty file

Dump symbol table

    $ ./golite symbol file

Run type checker

    $ ./golite type file

Run type checker and pretty print with types

    $ ./golite pptype file

Generate code

    $ ./golite code file

## For Vince

Everything can be done from the top directory

**Compile**

    $ ./run

**[`-pretty`] Pretty print the input file**

    $ ./run  -pretty file.go
    $ ls outputs/
    foo.pretty.go

**[`-pptype`] Pretty print with expression types**

    $ ./run  -pptype file.go
    $ ls outputs/
    foo.pptype.go

**[`-dumpsymtab`] Dump top-most frame of the symbol table each time a scope is exited**

    $ ./run  -dumpsymtab file.go
    $ ls outputs
    foo.symtab

**[`-code`] Generate Code**

    $ ./run -code file.go
    $ ls outputs/
    file.cpp

**Flags can be used together**

    $ ./run  -pptype -dumpsymtab file.go
    $ ls outputs/
    foo.symtab  foo.pptype.go

**If no flags are provided, the run command will attempt to generate all the files**

#### Comments

- Errors will be emmitted to stderr
- Calling `./run file.go [-pptype -dumpsymtab]` will delete everything in output before proceeding with the command

## Submited Files

programs/valids/sorting.go

programs/valids/calc.go

programs/valids/set2square.go

## Notes

The complier tends to segfault randomly on POSIX machines (OS X)

Tested to run fine on the Trotier machines.
