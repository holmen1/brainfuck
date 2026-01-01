# Brainfuck Learning Path

A structured approach to learning Brainfuck by implementing an interpreter in C99 and writing programs.

## Phase 1: Understanding Brainfuck

### Language Basics
- **8 Commands**: `>`, `<`, `+`, `-`, `.`, `,`, `[`, `]`
- **Memory Model**: 30,000 byte cells initialized to zero
- **Pointer**: Data pointer starts at cell 0
- **I/O**: Character-based input/output

### Command Reference
- `>` - Move pointer right
- `<` - Move pointer left
- `+` - Increment cell value
- `-` - Decrement cell value
- `.` - Output cell as ASCII character
- `,` - Input character to cell
- `[` - Jump past matching `]` if cell is zero
- `]` - Jump back to matching `[` if cell is non-zero

### Resources to Study
- [Esolangs Brainfuck page](https://esolangs.org/wiki/Brainfuck)
- Read example programs to understand patterns
- Learn common idioms (clear cell `[-]`, move value `[->+<]`)

## Phase 2: Simple Interpreter (C99 Implementation)

### Step 1: Basic Structure
**Goal**: Create a minimal working interpreter

**Files to create**:
- `src/bf.c` - Main interpreter
- `Makefile` - Build configuration

**Core components**:
```c
// Memory array (30,000 cells)
unsigned char memory[30000] = {0};
int ptr = 0;  // Data pointer

// Program storage
char program[MAX_SIZE];
int pc = 0;  // Program counter
```

**Implementation order**:
1. File I/O (read Brainfuck source)
2. Main execution loop
3. Implement simple commands (`>`, `<`, `+`, `-`, `.`, `,`)
4. Bracket matching (build jump table)
5. Loop commands (`[`, `]`)

**Test with**: `examples/hello_world.bf`

### Step 2: Bracket Matching
**Challenge**: Match `[` with corresponding `]`

**Approach**: Pre-process and build jump table
```c
int jumptable[MAX_SIZE];  // jumptable[i] = matching bracket position

// Use stack to match brackets
int stack[1000];
int sp = 0;

for (int i = 0; i < program_length; i++) {
    if (program[i] == '[') {
        stack[sp++] = i;
    } else if (program[i] == ']') {
        int start = stack[--sp];
        jumptable[start] = i;     // [ jumps to ]
        jumptable[i] = start;     // ] jumps back to [
    }
}
```

### Step 3: Error Handling
- Check for mismatched brackets
- Handle EOF on input (`,` command)
- Validate pointer bounds (0-29999)
- Report meaningful error messages

### Step 4: Debug Mode (Optional)
Add features for learning:
- `-d` flag for step-by-step execution
- Print memory dump around pointer
- Instruction counter
- Pause at breakpoints

## Phase 3: Write Brainfuck Programs

### Beginner Programs
1. **hello_world.bf** - Output "Hello World!"
2. **echo.bf** - Echo input until EOF
3. **add.bf** - Add two single digits
4. **multiply.bf** - Multiply two numbers

### Intermediate Programs
1. **fibonacci.bf** - Generate Fibonacci sequence
2. **primes.bf** - Print prime numbers
3. **mandelbrot.bf** - ASCII art Mandelbrot set

### Understanding Patterns
- Cell manipulation idioms
- Memory layout strategies
- Comment conventions

## Phase 4: Compiler

### Why Build a Compiler?
**Advantages**:
- **10-100x faster** than interpreted execution
- Learn code generation and compilation pipeline
- Understand different intermediate representations
- Create standalone executables
- Access to powerful optimizations

**Learning value**: Deeper understanding of how languages become executables

### Compiler Phases Summary

This compiler follows the **"My First Language Frontend with LLVM Tutorial"** architecture:

1. **Lexer**: Source text → Token stream
2. **Parser**: Tokens → Abstract Syntax Tree (AST)
3. **IR Generation**: AST → Intermediate Representation
4. **Optimization**: Pattern matching and transformations on IR
5. **Code Generation**: IR → Target code (LLVM IR / C / Assembly)
6. **Backend Compilation**: Target code → Executable binary

This structure makes the compiler:
- **Modular**: Each phase has clear input/output
- **Testable**: Test each phase independently
- **Retargetable**: Easy to add new backends
- **Optimizable**: IR enables powerful transformations

### Compiler Architecture Overview

Following the LLVM tutorial structure, we'll build a proper compiler frontend:

```
Source Code (.bf)
      ↓
   LEXER (Tokenization)
      ↓
   PARSER (Build AST)
      ↓
   AST (Abstract Syntax Tree)
      ↓
   IR GENERATION (Intermediate Representation)
      ↓
   CODE GENERATION (LLVM IR / Assembly / C)
      ↓
   OPTIMIZATION
      ↓
   Machine Code
```

### Step 1: Lexer (Tokenization)

The lexer converts source text into a stream of tokens, filtering out comments and whitespace.

**Purpose**: Transform raw `.bf` file into meaningful tokens

**Token types**:
```c
typedef enum {
    TOKEN_RIGHT,      // >
    TOKEN_LEFT,       // <
    TOKEN_INC,        // +
    TOKEN_DEC,        // -
    TOKEN_OUTPUT,     // .
    TOKEN_INPUT,      // ,
    TOKEN_LOOP_START, // [
    TOKEN_LOOP_END,   // ]
    TOKEN_EOF         // End of file
} TokenType;

typedef struct {
    TokenType type;
    int position;     // Position in source (for error messages)
} Token;
```

**Implementation** (`src/lexer.c`):
```c
typedef struct {
    const char *source;
    int position;
    int length;
} Lexer;

Lexer* lexer_create(const char *source, int length);
Token lexer_next_token(Lexer *lexer);
void lexer_free(Lexer *lexer);
```

**What the lexer does**:
- Skips non-command characters (whitespace, letters, etc.)
- Maps BF commands to token types
- Tracks position for error reporting
- Returns TOKEN_EOF when done

**Example**:
```
Input:  "++[>++<-]"
Output: [INC, INC, LOOP_START, RIGHT, INC, INC, LEFT, DEC, LOOP_END, EOF]
```

### Step 2: Parser & AST (Abstract Syntax Tree)

The parser consumes tokens and builds an Abstract Syntax Tree that represents the program structure.

**Purpose**: Create a structured representation of the program that's easy to analyze and transform

**AST Node types**:
```c
typedef enum {
    AST_SEQUENCE,     // List of commands
    AST_MOVE_PTR,     // > or < (combined)
    AST_MODIFY_CELL,  // + or - (combined)
    AST_OUTPUT,       // .
    AST_INPUT,        // ,
    AST_LOOP          // [ ... ]
} ASTNodeType;

typedef struct ASTNode {
    ASTNodeType type;
    union {
        struct {
            int offset;              // For MOVE_PTR: +1 for >, -1 for <
        } move;
        struct {
            int delta;               // For MODIFY_CELL: +1 for +, -1 for -
        } modify;
        struct {
            struct ASTNode **children;
            int count;
        } sequence;
        struct {
            struct ASTNode *body;    // For LOOP: body of loop
        } loop;
    } data;
} ASTNode;
```

**Why AST?**
- **Optimization-ready**: Easy to recognize patterns like `+++` → single node with delta=3
- **Type safety**: Each node has semantic meaning
- **Composable**: Loops contain sub-trees, sequences contain lists
- **Analysis**: Can compute properties (constant folding, dead code detection)

**Parser implementation** (`src/parser.c`):
```c
typedef struct {
    Lexer *lexer;
    Token current_token;
} Parser;

Parser* parser_create(Lexer *lexer);
ASTNode* parser_parse_program(Parser *parser);
ASTNode* parser_parse_sequence(Parser *parser);
ASTNode* parser_parse_loop(Parser *parser);
void parser_free(Parser *parser);
```

**Parsing approach**:
1. **Sequence parsing**: Parse list of commands/loops into SEQUENCE node
2. **Loop parsing**: When hitting `[`, recursively parse body until `]`
3. **Optimization during parse**: Combine consecutive `+`, `-`, `>`, `<`
4. **Validation**: Ensure brackets are balanced

**Example AST**:
```
Program: "++[>+<-]"

AST:
SEQUENCE
  ├─ MODIFY_CELL(delta=+2)
  └─ LOOP
      └─ SEQUENCE
           ├─ MOVE_PTR(offset=+1)
           ├─ MODIFY_CELL(delta=+1)
           ├─ MOVE_PTR(offset=-1)
           └─ MODIFY_CELL(delta=-1)
```

### Step 3: IR (Intermediate Representation)

Before generating target code, optionally convert AST to a linear IR for additional optimization.

**Purpose**: Platform-independent intermediate form that's easier to optimize than AST

**IR Instruction types**:
```c
typedef enum {
    IR_ADD_PTR,       // Add to pointer (can be negative)
    IR_ADD_CELL,      // Add to current cell value
    IR_OUTPUT,        // Output current cell
    IR_INPUT,         // Input to current cell
    IR_LOOP_START,    // Begin loop
    IR_LOOP_END,      // End loop
    IR_SET_ZERO,      // Optimized: set current cell to 0
    IR_MOVE_DATA,     // Optimized: move value to offset
    IR_SCAN_RIGHT,    // Optimized: scan for zero moving right
    IR_SCAN_LEFT      // Optimized: scan for zero moving left
} IROpcode;

typedef struct {
    IROpcode opcode;
    int operand;          // e.g., amount to add, offset to move
} IRInstruction;
```

**AST to IR conversion** (`src/ir.c`):
```c
typedef struct {
    IRInstruction *instructions;
    int count;
    int capacity;
} IRProgram;

IRProgram* ir_from_ast(ASTNode *ast);
void ir_optimize(IRProgram *program);
void ir_free(IRProgram *program);
```

**Why IR?**
- **Pattern matching**: Easier to detect `[-]` clear loops in linear form
- **Optimization passes**: Can run multiple optimization passes
- **Retargetable**: Same IR can target LLVM, x86-64, ARM, or C

**Optimization opportunities**:
```c
// Pattern: [-] or [+] → IR_SET_ZERO
LOOP_START
  ADD_CELL -1
LOOP_END
→ SET_ZERO

// Pattern: [->+<] → IR_MOVE_DATA
LOOP_START
  ADD_PTR +1
  ADD_CELL +1
  ADD_PTR -1
  ADD_CELL -1
LOOP_END
→ MOVE_DATA offset=1

// Pattern: [>] → IR_SCAN_RIGHT
LOOP_START
  ADD_PTR +1
LOOP_END
→ SCAN_RIGHT
```

### Choosing Your Backend

Pick based on your learning goals:

| Backend | Difficulty | Learning Focus | Speed |
|---------|-----------|----------------|-------|
| **LLVM IR** | Medium | Modern compilers, IR design, SSA | Excellent |
| **C Code** | Easy | Quick results, portable | Good (gcc optimizes) |
| **x86-64 Asm** | Hard | Low-level systems, ISA | Excellent |
| **ARM64 Asm** | Hard | Different architecture | Excellent |

**Recommendation**: Start with LLVM IR following the tutorial structure.

### Step 4: Code Generation Backends

#### Backend A: LLVM IR Code Generation (Recommended)
**Output**: `.ll` text file → `clang` → optimized executable

**Why LLVM IR?**
- **Platform independent**: Works on x86-64, ARM, RISC-V, etc.
- **Professional optimizer**: Industry-standard optimization passes
- **Readable**: Higher-level than assembly, easier to debug
- **Modern approach**: Used by Clang, Rust, Swift, Julia
- **Great learning**: Understand SSA form and IR design

**Key LLVM Concepts to Learn**:
1. **SSA Form**: Single Static Assignment - each variable assigned once
2. **Basic Blocks**: Code sequences ending in terminator (`br`, `ret`)
3. **Types**: `i8` (byte), `i32` (int), `i8*` (pointer)
4. **Instructions**: `load`, `store`, `add`, `icmp`, `br`, `call`
5. **GEP**: `getelementptr` for array indexing
6. **Intrinsics**: Built-ins like `llvm.memset`

**LLVM IR Structure**:
```llvm
; Declare external functions
declare i32 @putchar(i32)
declare i32 @getchar()
declare void @llvm.memset.p0i8.i64(i8*, i8, i64, i1)

define i32 @main() {
entry:
  ; Allocate memory on stack
  %memory = alloca [30000 x i8], align 1
  %ptr = alloca i32, align 4
  
  ; Initialize memory to zero
  %memptr = getelementptr inbounds [30000 x i8], [30000 x i8]* %memory, i32 0, i32 0
  call void @llvm.memset.p0i8.i64(i8* %memptr, i8 0, i64 30000, i1 false)
  store i32 0, i32* %ptr, align 4
  
  br label %block_0

block_0:
  ; Your BF commands translate here
  ; Example: + (increment cell)
  %r0 = load i32, i32* %ptr, align 4
  %r1 = getelementptr inbounds [30000 x i8], [30000 x i8]* %memory, i32 0, i32 %r0
  %r2 = load i8, i8* %r1, align 1
  %r3 = add i8 %r2, 1
  store i8 %r3, i8* %r1, align 1
  
  ret i32 0
}
```

**Translation Guide**:
```
> → load ptr, add 1, store ptr
< → load ptr, sub 1, store ptr  
+ → GEP to cell, load, add 1, store
- → GEP to cell, load, sub 1, store
. → GEP to cell, load, zext to i32, call putchar
, → call getchar, trunc to i8, GEP to cell, store
[ → Create loop: check block → body block → end block
] → Branch back to check block
```

**Loop Implementation** (the tricky part):
```llvm
; For BF code: [->+<]
label %loop_0_check:
  %c0 = load i32, i32* %ptr
  %c1 = getelementptr [30000 x i8], [30000 x i8]* %memory, i32 0, i32 %c0
  %c2 = load i8, i8* %c1
  %c3 = icmp ne i8 %c2, 0
  br i1 %c3, label %loop_0_body, label %loop_0_end

label %loop_0_body:
  ; Code inside [ ... ] goes here
  br label %loop_0_check

label %loop_0_end:
  ; Continue after loop
```

**Implementation Steps**:
1. Read BF source, validate brackets
2. Generate `.ll` file with module header and declarations
3. Allocate memory array and pointer in `entry` block
4. For each BF command, emit corresponding LLVM IR
5. Use bracket stack to track loop nesting
6. Compile: `clang -O2 output.ll -o program`

**Resources**:
- [LLVM Language Reference](https://llvm.org/docs/LangRef.html)
- [My First Language Frontend with LLVM Tutorial](https://llvm.org/docs/tutorial/)
- Study: `clang -S -emit-llvm simple.c` to see real IR

#### Backend B: C Code Generation

**Simplest approach**: Generate C code from IR, compile with gcc
- Faster development
- Portable across architectures  
- Easy optimizations (gcc does the work)

```c
// Generated C code example from IR
unsigned char mem[30000] = {0};
unsigned char *ptr = mem;
int main() {
    ptr += 1;           // IR: ADD_PTR +1
    *ptr += 2;          // IR: ADD_CELL +2  
    putchar(*ptr);      // IR: OUTPUT
    while(*ptr) {       // IR: LOOP_START
        *ptr -= 1;      // IR: ADD_CELL -1
    }                   // IR: LOOP_END
    *ptr = 0;           // IR: SET_ZERO (optimized)
    return 0;
}
```

**Code generator** (`src/codegen_c.c`):
```c
void codegen_c_from_ir(IRProgram *program, FILE *output);
```

#### Backend C: x86-64 Assembly
**Output**: NASM/GAS syntax assembly → link with `gcc`/`ld`

**Translation strategy**:
```
> → add rax, 1        (or lea rax, [rax+1])
< → sub rax, 1
+ → inc byte [rax]
- → dec byte [rax]
. → mov rdi, 1; mov rsi, rax; mov rdx, 1; mov rax, 1; syscall
, → mov rdi, 0; mov rsi, rax; mov rdx, 1; mov rax, 0; syscall
[ → cmp byte [rax], 0; je .L_end_X
] → cmp byte [rax], 0; jne .L_start_X
```

**Implementation steps**:
1. Generate assembly text file (`output.asm`)
2. Allocate 30,000-byte BSS section for memory
3. Set up registers (rax = data pointer)
4. Translate each BF command to asm
5. Assemble: `nasm -f elf64 output.asm -o output.o`
6. Link: `gcc output.o -o program -nostartfiles` or use `ld`

**Code generator** (`src/codegen_x64.c`):
```c
void codegen_x64_from_ir(IRProgram *program, FILE *output);
```

#### Backend D: ARM64 Assembly
Good for Raspberry Pi and modern ARM systems
- Similar concepts to x86-64 but different syntax
- More registers, cleaner instruction set
- Good second assembly target after x86-64

**Code generator** (`src/codegen_arm64.c`):
```c
void codegen_arm64_from_ir(IRProgram *program, FILE *output);
```

### Step 5: Compiler Implementation Roadmap

**Phase 4.1: Lexer (2-3 hours)**
1. Create `src/lexer.h` and `src/lexer.c`
2. Define token types and structures
3. Implement `lexer_next_token()` function
4. Test: tokenize "Hello World" program
5. Validate: skip comments, handle EOF

**Phase 4.2: Parser & AST (4-6 hours)**
1. Create `src/ast.h` and `src/ast.c`
2. Define AST node types and structures
3. Create `src/parser.h` and `src/parser.c`
4. Implement sequence parsing
5. Implement loop parsing with bracket matching
6. Add optimization: combine consecutive operations
7. Test: parse and print AST for sample programs
8. Validate: bracket matching errors

**Phase 4.3: IR Generation (3-4 hours)**
1. Create `src/ir.h` and `src/ir.c`
2. Define IR instruction types
3. Implement AST-to-IR conversion
4. Walk AST recursively, emitting IR instructions
5. Test: print IR for sample programs

**Phase 4.4: IR Optimization (4-6 hours)**
1. Implement pattern matching for common idioms
2. Clear loop optimization: `[-]` → SET_ZERO
3. Copy loop optimization: `[->+<]` → MOVE_DATA
4. Scan loop optimization: `[>]` → SCAN_RIGHT/LEFT
5. Dead code elimination
6. Test: verify optimizations preserve semantics

**Phase 4.5: Code Generation (6-8 hours)**
Choose your backend and implement:
1. Create `src/codegen_<backend>.h` and `src/codegen_<backend>.c`
2. Generate prologue (setup memory, initialize pointer)
3. Walk IR, emit target code for each instruction
4. Generate epilogue (cleanup, return)
5. Test: compile "Hello World"
6. Debug: compare output with interpreter

**Phase 4.6: Main Compiler Driver (2-3 hours)**
1. Create `src/bfc.c` (compiler main)
2. Parse command-line arguments
3. Wire together: Lexer → Parser → IR → CodeGen
4. Write output file
5. Optionally invoke backend compiler (clang/gcc/nasm)
6. Test full pipeline

### Compiler Project Structure

Following LLVM tutorial conventions:

```
bfc                     # Compiler executable
├── src/
│   ├── bfc.c           # Main compiler driver
│   ├── lexer.c         # Tokenization
│   ├── lexer.h
│   ├── ast.c           # AST node creation/management
│   ├── ast.h
│   ├── parser.c        # Parse tokens → AST
│   ├── parser.h
│   ├── ir.c            # IR generation & optimization
│   ├── ir.h
│   ├── codegen_llvm.c  # LLVM IR backend (recommended)
│   ├── codegen_c.c     # C backend (simplest)
│   ├── codegen_x64.c   # x86-64 backend (advanced)
│   └── codegen_arm64.c # ARM64 backend (alternative)
└── tests/
    ├── test_lexer.c
    ├── test_parser.c
    ├── test_ir.c
    └── test_codegen.c
```

### Step 6: Optimizations

**IR-level optimizations** (apply during IR generation or as separate pass):
1. **Constant folding**: Combine `+++` → ADD_CELL +3 (do in parser)
2. **Clear loops**: Recognize `[-]` and `[+]` → SET_ZERO instruction
3. **Copy loops**: Optimize `[->+<]` → MOVE_DATA instruction
4. **Scan loops**: Optimize `[>]` and `[<]` → SCAN_RIGHT/LEFT instructions
5. **Dead code elimination**: Remove operations after unconditional loops
6. **Pointer offset tracking**: Combine movements with operations

**LLVM's optimizations** (if using LLVM IR with `-O2`):
- Dead code elimination
- Constant propagation
- Loop unrolling
- Register allocation
- Instruction scheduling
- And 100+ more passes!

**Assembly-level optimizations** (if hand-coding x86-64):
1. `+++` → `add byte [rax], 3`
2. `[-]` → `mov byte [rax], 0`
3. `[>]` → `repne scasb` (scan for zero)
4. Use `lea` for pointer arithmetic

### Step 7: Testing & Benchmarking

**Testing strategy**:
1. Unit tests for each compiler phase (lexer, parser, IR, codegen)
2. Integration tests: compile sample programs, compare output with interpreter
3. Validation: ensure compiled programs produce identical output

**Benchmarking**:
Create test suite with:
- Long-running programs (mandelbrot.bf)
- Time both interpreter and compiler
- Typical speedup: **50-100x** for compiler
- Profile with `perf`, `gdb`, `valgrind`

**Benchmark script** (`benchmarks/bench.sh`):
```bash
#!/bin/bash
echo "Running mandelbrot benchmark..."
time ./bf examples/mandelbrot.bf > /dev/null
time ./mandelbrot_compiled > /dev/null
```

## Phase 5: Optimizations (Advanced)

### Interpreter Optimizations (if not compiling)
1. **Run-length encoding**: Combine repeated `+`, `-`, `>`, `<`
2. **Clear loops**: Recognize `[-]` and execute as single operation
3. **Copy loops**: Optimize `[->+<]` patterns
4. **Scan loops**: Optimize `[>]` and `[<]`

### Just-In-Time Compilation
- Translate to native code at runtime
- Use libraries like GNU lightning or libjit
- Best of both worlds (portability + speed)

### Benchmarking
- Create benchmark suite
- Profile with `gdb`, `valgrind`, `perf`
- Compare: interpreter vs optimized vs compiler

## Phase 6: Extensions (Optional)

### Language Extensions
- Larger cell sizes (16-bit, 32-bit)
- Different wrapping behaviors
- Additional memory models
- Debug commands (`#` for breakpoint)

### Tooling
- Syntax highlighter
- Formatter/prettifier
- Debugger with breakpoints
- Visualizer

## Learning Milestones

### Interpreter Path
- [ ] **Milestone 1**: Understand all 8 commands
- [ ] **Milestone 2**: Basic interpreter runs "Hello World"
- [ ] **Milestone 3**: Complete interpreter with error handling
- [ ] **Milestone 4**: Write your own BF programs (echo, add, etc.)

### Compiler Path (Following LLVM Tutorial Structure)
- [ ] **Milestone 5**: Implement lexer - tokenize BF source
- [ ] **Milestone 6**: Implement parser - build AST from tokens
- [ ] **Milestone 7**: Generate IR from AST
- [ ] **Milestone 8**: Implement basic code generation (LLVM IR or C)
- [ ] **Milestone 9**: Compile "Hello World" to executable
- [ ] **Milestone 10**: Implement control flow (loops with brackets)
- [ ] **Milestone 11**: Add IR optimizations (clear loops, copy loops, scan loops)
- [ ] **Milestone 12**: Benchmark: 50x+ speedup vs interpreter
- [ ] **Milestone 13**: (Optional) Try a different backend (x86-64, ARM64)

## Project Structure

### Interpreter + Compiler (LLVM Tutorial Style)
```
brainfuck/
├── README.md
├── Makefile
├── src/
│   ├── bf.c              # Interpreter
│   ├── bf.h              # Interpreter header
│   ├── bfc.c             # Compiler main driver
│   ├── lexer.c           # Tokenization
│   ├── lexer.h
│   ├── ast.c             # AST node management
│   ├── ast.h
│   ├── parser.c          # Parse tokens → AST
│   ├── parser.h
│   ├── ir.c              # IR generation & optimization
│   ├── ir.h
│   ├── codegen_llvm.c    # LLVM IR code generation
│   ├── codegen_c.c       # C code generation
│   ├── codegen_x64.c     # x86-64 assembly generation
│   └── codegen_arm64.c   # ARM64 assembly generation
├── examples/
│   ├── hello_world.bf
│   ├── echo.bf
│   ├── fibonacci.bf
│   └── mandelbrot.bf     # Good benchmark
├── tests/
│   └── test_*.bf
├── benchmarks/
│   └── bench.sh          # Compare interpreter vs compiler
└── docs/
    └── implementation_notes.md
```

## Build & Run Commands

```bash
# Build everything
make

# Interpreter: run a program
./bf examples/hello_world.bf

# Interpreter: debug mode
./bf -d examples/hello_world.bf

# Compiler: generate and compile to executable
./bfc examples/hello_world.bf -o hello
./hello

# View generated LLVM IR (if using LLVM backend)
cat hello.ll

# Compare interpreter vs compiled performance
make benchmark

# Run test suite
make test
```

## Recommended Learning Path

### Week 1: Foundation (4-8 hours)
1. Read Brainfuck language specification thoroughly
2. Understand memory model and all 8 commands
3. Write "Hello World" on paper (manually trace execution)
4. Study example programs to see patterns

### Week 2: Interpreter (6-12 hours)
1. Create `src/bf.c` with basic structure
2. Implement simple commands (`>`, `<`, `+`, `-`, `.`, `,`)
3. Add bracket matching algorithm
4. Implement loops (`[`, `]`)
5. Test with all example programs
6. Add error handling and validation

### Week 3: Write Programs (4-8 hours)
1. Write your own "echo" program
2. Create an addition program
3. Try multiplication or Fibonacci
4. Learn common BF idioms and patterns

### Week 4+: Compiler (20-30 hours)

**Following LLVM Tutorial Structure**:

**Phase 4.1 - Lexer (2-3 hours)**:
1. Implement tokenizer in `src/lexer.c`
2. Define token types (TOKEN_INC, TOKEN_DEC, etc.)
3. Test: tokenize sample programs

**Phase 4.2 - Parser & AST (4-6 hours)**:
1. Design AST node types in `src/ast.h`
2. Implement parser in `src/parser.c`
3. Parse tokens into AST
4. Combine consecutive operations during parsing
5. Validate bracket matching
6. Test: print AST for sample programs

**Phase 4.3 - IR Generation (3-4 hours)**:
1. Define IR instruction set in `src/ir.h`
2. Convert AST to linear IR
3. Test: print IR for sample programs

**Phase 4.4 - IR Optimization (4-6 hours)**:
1. Implement pattern matching for idioms
2. Optimize `[-]` to SET_ZERO
3. Optimize `[->+<]` to MOVE_DATA
4. Optimize `[>]` and `[<]` to SCAN operations
5. Test: verify output correctness

**Phase 4.5 - Code Generation (6-10 hours)**:
1. Choose backend: LLVM IR (recommended) or C (easier)
2. Implement code generator in `src/codegen_*.c`
3. Generate setup/teardown code
4. Translate IR instructions to target code
5. Get "Hello World" working end-to-end

**Phase 4.6 - Integration & Testing (3-4 hours)**:
1. Wire all phases together in `src/bfc.c`
2. Add command-line argument parsing
3. Test with all example programs
4. Benchmark: expect 50-100x speedup over interpreter
5. (Optional) Try a different backend

## Next Steps

1. Read about Brainfuck language specification
2. Create `src/bf.c` with basic structure
3. Implement command execution loop
4. Test with simple programs
5. Iterate and improve

## Resources

- [Brainfuck on Wikipedia](https://en.wikipedia.org/wiki/Brainfuck)
- [Esolangs Wiki](https://esolangs.org/wiki/Brainfuck)
- [Sample programs collection](https://github.com/fabianishere/brainfuck)

