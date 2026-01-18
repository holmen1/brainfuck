# brainfuck

A minimal Brainfuck interpreter and compiler written in C99.

Brainfuck is an esoteric programming language created by Urban Müller in 1993. See the [language specification](https://esolangs.org/wiki/Brainfuck) for details.

## Language Reference

The brainfuck interpreter knows the following instructions:

|Cmd|Effect|                               
|---|------|                               
|+|Increases element under pointer|      
|-|Decrases element under pointer|       
|>|Increases pointer|      
|<|Decreases pointer|                    
|[|Starts loop, flag under pointer|      
|]|Indicates end of loop|             
|.|Outputs ASCII code under pointer|     
|,|Reads char and stores ASCII under ptr|

## Hello World

This cryptic string:
```brainfuck
++++++++[>++++[>++>+++>+++>+<<<<-]>+>+>->>+[<]<-]>>.>---.+++++++..+++.>>.<-.<.+++.------.--------.>>+.>++.
```

Compiles to a native x86-64 executable:
```bash
$ ./bfc/bin/bfc bf/examples/hello_world.bf
Success! Executable: bin/hello_world

$ ./bin/hello_world
Hello World!
```

## Interpreter

Simple brainfuck interpreter with 30,000 byte memory array. See [bf/README.md](bf/README.md) for usage and examples.

## Compiler

Optimizing compiler generating native x86-64 executables. See [bfc/README.md](bfc/README.md) for usage and examples.


