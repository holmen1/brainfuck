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

### Choosing Your Backend

Pick based on your learning goals:

| Backend | Difficulty | Learning Focus | Speed |
|---------|-----------|----------------|-------|
| **LLVM IR** | Medium | Modern compilers, IR design, SSA | Excellent |
| **C Code** | Easy | Quick results, portable | Good (gcc optimizes) |
| **x86-64 Asm** | Hard | Low-level systems, ISA | Excellent |
| **ARM64 Asm** | Hard | Different architecture | Excellent |

**Recommendation**: Start with LLVM IR or C, then try assembly if curious.

### Compilation Targets

#### Option A: LLVM Intermediate Representation (Recommended)
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
- [LLVM Tutorial](https://llvm.org/docs/tutorial/)
- Study: `clang -S -emit-llvm simple.c` to see real IR

#### Option B: x86-64 Assembly
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

#### Option C: ARM64 Assembly
Good for Raspberry Pi and modern ARM systems
- Similar concepts to x86-64 but different syntax
- More registers, cleaner instruction set
- Good second assembly target after x86-64

#### Option D: C Code Generation
**Simplest approach**: Generate C code, compile with gcc
- Faster development
- Portable across architectures
- Easy optimizations (gcc does the work)

```c
// Generated C code example
unsigned char mem[30000] = {0};
unsigned char *ptr = mem;
int main() {
    ++ptr;              // >
    ++*ptr;             // +
    putchar(*ptr);      // .
    while(*ptr) {       // [
        --*ptr;         // -
    }                   // ]
    return 0;
}
```

### Compiler Project Structure
```
bfc                     # Compiler executable
├── src/
│   ├── bfc.c           # Main compiler
│   ├── parser.c        # Read source, match brackets
│   ├── codegen_llvm.c  # LLVM IR backend (recommended)
│   ├── codegen_x64.c   # x86-64 backend (alternative)
│   ├── codegen_c.c     # C backend (simplest)
│   └── optimize.c      # Pre-codegen optimizations
└── runtime/
    └── startup.asm     # Minimal runtime (if not using libc)
```

### Optimizations for Compiler

**Your optimizations** (apply before code generation):
1. **Constant folding**: Combine `+++` → single add by 3
2. **Clear loops**: Recognize `[-]` and `[+]` → set to 0
3. **Copy loops**: Optimize `[->+<]` patterns
4. **Scan loops**: Optimize `[>]` and `[<]`

**LLVM's optimizations** (if using LLVM IR with `-O2`):
- Dead code elimination
- Constant propagation
- Loop unrolling
- Register allocation
- Instruction scheduling
- And 100+ more passes!

**Assembly optimizations** (if hand-coding x86-64):
1. `+++` → `add byte [rax], 3`
2. `[-]` → `mov byte [rax], 0`
3. `[>]` → `repne scasb` (scan for zero)
4. Use `lea` for pointer arithmetic

### Benchmarking Interpreter vs Compiler
Create test suite with:
- Long-running programs (mandelbrot.bf)
- Time both implementations
- Typical speedup: **50-100x** for compiler

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

### Compiler Path (choose your backend)
- [ ] **Milestone 5**: Generate LLVM IR / C code / x86-64 asm for simple commands
- [ ] **Milestone 6**: Compile "Hello World" to executable
- [ ] **Milestone 7**: Implement control flow (loops with brackets)
- [ ] **Milestone 8**: Add optimizations (constant folding, clear loops)
- [ ] **Milestone 9**: Benchmark: 50x+ speedup vs interpreter
- [ ] **Milestone 10**: (Optional) Try a different backend

## Project Structure

### Interpreter + Compiler
```
brainfuck/
├── README.md
├── Makefile
├── src/
│   ├── bf.c              # Interpreter
│   ├── bfc.c             # Compiler main
│   ├── codegen_x64.c     # x86-64 code generation
│   ├── codegen_c.c       # C code generation
│   ├── optimize.c        # Optimization passes
│   └── common.h          # Shared definitions
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

### Week 4+: Compiler (12-20 hours)
1. Choose backend: **LLVM IR** (recommended), C (easiest), or x86-64 (hardest)
2. Start `src/bfc.c` with file I/O and parsing
3. Generate code for simple commands
4. Implement control flow (loops)
5. Get "Hello World" compiling and running
6. Add optimizations
7. Benchmark vs interpreter (expect 50-100x speedup)
8. (Optional) Try a different backend

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

