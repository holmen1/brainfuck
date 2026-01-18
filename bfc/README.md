# BF Compiler

Brainfuck to x86-64 native code compiler with optimization.

## Usage

```bash
./bin/bfc program.bf
```

Generates:
- `build/program.s` - x86-64 assembly (intermediate)
- `bin/program` - native executable

## Examples

### Hello World

```bash
$ ./bfc/bin/bfc bf/examples/hello_world.bf 
Reading source file: bf/examples/hello_world.bf
Lexing...
Parsing...
Optimizing AST...
Generating IR...
Generating x86-64 assembly: build/hello_world.s
Assembly written to build/hello_world.s
Compiling to executable: bin/hello_world
Success! Executable: bin/hello_world

$ ./bin/hello_world 
Hello World!
```

### Addition with Input

```bash
$ ./bfc/bin/bfc bf/examples/add.bf 
...
Success! Executable: bin/add

$ echo "35" | ./bin/add
8
```

## Debug Flags

### View Optimized AST

```bash
$ ./bfc/bin/bfc bf/examples/hello_world.bf --print-ast
Reading source file: bf/examples/hello_world.bf
Lexing...
Parsing...
SEQUENCE(66 children)
  INCREMENT
  INCREMENT
  INCREMENT
  INCREMENT
  INCREMENT
  INCREMENT
  INCREMENT
  INCREMENT
  LOOP
    SEQUENCE(18 children)
      MOVE_RIGHT
      INCREMENT
      INCREMENT
      INCREMENT
      INCREMENT
      LOOP
        ...
```

### View IR (Intermediate Representation)

```bash
$ ./bfc/bin/bfc bf/examples/hello_world.bf --print-ir
Reading source file: bf/examples/hello_world.bf
Lexing...
Parsing...
Optimizing AST...
Generating IR...
IR Program (59 instructions):
========================================
   0: ADD_CELL +8
   1: LOOP_START (label 0)
   2: ADD_PTR +1
   3: ADD_CELL +4
   4: LOOP_START (label 1)
   ...
```

Generated assembly: [build/hello_world.s](../build/hello_world.s)


## Build

```bash
$ make          # Optimized build
$ make debug    # Debug build with -g -O0
$ make clean    # Clean build artifacts
```

## Architecture

```
Source.bf → Lexer → Parser → AST → Optimizer → IR → x86-64 Assembly → gcc → Executable
```

**Optimizations**:
- Consecutive pointer movements: `>>>` → `ADD_PTR +3`
- Consecutive cell modifications: `+++` → `ADD_CELL +3`
- Clear loops: `[-]` → `SET_ZERO` (single instruction)

**Output**:
- 30KB stack-based memory array
- System V ABI calling convention
- Registers: `%r12` (base), `%r13` (offset)
- Direct syscalls for I/O (`putchar`, `getchar`)
