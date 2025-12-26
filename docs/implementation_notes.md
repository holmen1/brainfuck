# Implementation Notes

Working document for tracking progress, decisions, and lessons learned while implementing the Brainfuck interpreter and compiler.

## Progress Log

### Date: 2025-12-25
- Created project structure
- Set up build system (Makefile)
- Added example BF programs

### Date: 2025-12-26
- Cleaned up Makefile for minimal interpreter-only build
  - Removed compiler (bfc), debug, and benchmark targets
  - Added SRCDIR and BINDIR variables to keep project root clean
  - Binary now builds to `bin/bf` instead of project root
- Implemented basic file I/O in bf.c
  - Reads Brainfuck source file into program buffer
  - Returns 0 on success, 1 on error
  - File size limit: PROGRAM_SIZE (100,000 bytes)
- Created test suite (tests/run_tests.sh)
  - Converted to POSIX shell (#!/bin/sh)
  - Tests use && || operators instead of if statements for conciseness
  - 3 tests: valid file, missing file, no arguments
  - All tests passing
- Implemented main execution loop and simple commands
  - Added `execute_program()` function (separate from main)
  - Main loop iterates through program counter (pc)
  - Switch statement for each command
  - All 6 simple commands working: >, <, +, -, ., ,
  - Tests extended to 4 execution tests - all passing
  
**Next steps**: Implement bracket matching for loops ([ and ])


## Phase 2: Interpreter Implementation

### Bracket Matching Strategy
**Chosen approach**: (stack-based / pre-processing / other)

TODO: Document your implementation:
- How did you handle nested brackets?
- Data structure used (array, linked list, etc.)
- Time complexity: O(?)
- Space complexity: O(?)

**Challenges encountered**:
- TODO: What edge cases did you find?
- TODO: How did you debug mismatched brackets?

**Code snippet**:
```c
// Paste your bracket matching code here
```

### Memory Management
TODO: Document your decisions:
- Array size: 30000 bytes (standard) or different?
- Pointer bounds checking: How do you handle out-of-bounds?
- Cell wrapping: Do cells wrap at 255/0 or error?

**Bounds checking approach**:
```c
// Your implementation here
```

### Main Execution Loop
**Chosen approach**: Switch statement in single loop

**Implementation details**:
- Loop counter: `pc` (program counter) iterates 0 to program_length
- Data pointer: `ptr` tracks current memory cell
- Memory: 30,000 byte array initialized to zero

**Command implementation**:
```c
switch (cmd) {
    case '>': ptr++; break;              /* move right */
    case '<': ptr--; break;              /* move left */
    case '+': memory[ptr]++; break;      /* increment cell */
    case '-': memory[ptr]--; break;      /* decrement cell */
    case '.': putchar(memory[ptr]); break;  /* output */
    case ',': memory[ptr] = getchar(); break;  /* input */
    default: /* ignore non-commands */
}
```

**Performance notes**:
- Simple programs: ___ instructions/sec
- Complex programs (mandelbrot): ___ seconds

### Error Handling
TODO: What errors do you check for?
- [ ] Mismatched brackets
- [ ] Pointer underflow (< at position 0)
- [ ] Pointer overflow (> at position 29999)
- [ ] File I/O errors
- [ ] EOF on input (`,` command)

**Error reporting strategy**:
- TODO: Line numbers? Descriptive messages?

### Debug Mode Implementation
If you implemented `-d` flag:
- TODO: What information do you display?
- TODO: Memory visualization approach?

## Phase 3: Writing Brainfuck Programs

### Programs Created
1. **Program name**: TODO
   - Purpose: 
   - Key insight:
   - Challenges:

### Idioms Learned
TODO: Document useful patterns you discovered:
- `[-]` - Clear cell
- `[->+<]` - Move value
- Other patterns:

## Phase 4: Compiler Implementation

### Backend Choice
**Selected**: (LLVM IR / C code / x86-64 asm / ARM64 asm / other)

**Rationale**: TODO: Why did you choose this backend?

---

### LLVM IR Implementation (if chosen)

#### Setup and Tooling
**LLVM version**: TODO
**Compilation command**: `clang -O2 output.ll -o program`

#### IR Generation Strategy
TODO: Document your approach:
- How do you track SSA register numbers?
- Loop label naming scheme?
- How do you handle nested loops?

**Key design decisions**:
- Memory allocation: Stack (`alloca`) or heap?
- Pointer type: `i32` or `i64`?
- Optimization level: -O0, -O1, -O2, or -O3?

#### Command Translation
Document your actual LLVM IR for each command:

**Pointer movement** (`>` and `<`):
```llvm
; Paste your actual IR here
```

**Cell modification** (`+` and `-`):
```llvm
; Paste your actual IR here
```

**I/O** (`.` and `,`):
```llvm
; Paste your actual IR here
```

**Loop implementation** (`[` and `]`):
```llvm
; Paste your actual loop IR here
; Document your check/body/end block structure
```

#### Challenges with LLVM IR
TODO: What was difficult?
- GEP (getelementptr) understanding?
- SSA form constraints?
- Type conversions (i8 ↔ i32)?
- Loop structure?

#### LLVM IR Examples Generated
Paste interesting examples of generated IR:
```llvm
; Example: "+++" (three increments)
; Does your compiler optimize this to a single add?
```

---

### C Code Generation (if chosen)

#### Generated Code Structure
TODO: Document your C code generation approach
- Include statements needed?
- Global vs local memory array?
- Helper functions or inline everything?

#### Example Generated Code
```c
// Paste example of your generated C code
```

---

### x86-64 Assembly Implementation (if chosen)

#### Assembly Syntax
**Chosen syntax**: (NASM / GAS / other)

#### Register Usage
TODO: Document your register allocation:
- Data pointer: `rax`? `rbx`? Other?
- Scratch registers for I/O?
- Stack usage?

#### Syscall Strategy
**I/O approach**: (syscalls / libc functions)

**Output** (`.` command):
```asm
; Paste your assembly here
```

**Input** (`,` command):
```asm
; Paste your assembly here
```

#### Loop Implementation
TODO: Document your label generation:
- Naming scheme for loop labels?
- How do you track matching brackets?

```asm
; Example loop assembly
```

#### Assembly & Linking
**Commands used**:
```bash
# Assemble:
# Link:
```

**Challenges**:
- TODO: What was tricky about assembly generation?
- TODO: Debugging approach for assembly?

---

### Optimizations Implemented

#### Pre-Codegen Optimizations
Check off optimizations you've implemented:

- [ ] **Constant folding**: `+++` → add by 3
  - Implementation notes:
  
- [ ] **Clear loops**: `[-]` or `[+]` → set to 0
  - Pattern recognition approach:
  
- [ ] **Copy loops**: `[->+<]` optimization
  - How did you detect this pattern?
  
- [ ] **Scan loops**: `[>]` or `[<]` optimization
  - Translation (if using x86-64):

- [ ] **Dead code elimination**
  - What constitutes dead code in BF?

- [ ] **Other optimizations**:
  - TODO: Document any novel optimizations

#### Backend-Specific Optimizations
If using LLVM:
- LLVM optimization flags used: `-O0`, `-O1`, `-O2`, `-O3`?
- Did you examine the assembly output? Any surprises?

If using x86-64:
- Register optimizations?
- Special instructions used (e.g., `repne scasb` for scan)?

### Compilation Pipeline
TODO: Document your end-to-end process:
1. Read BF source
2. Validate/parse
3. (Optional) Optimization pass
4. Code generation
5. Write output file (.ll / .c / .asm)
6. Invoke compiler/assembler
7. Produce executable

**Build times**:
- Small programs (<100 BF instructions): ___ ms
- Large programs (1000+ instructions): ___ ms

## Benchmarking Results

### Interpreter Performance
| Program | Instructions Executed | Time | Inst/sec |
|---------|---------------------|------|----------|
| hello_world.bf | TODO | TODO | TODO |
| mandelbrot.bf | TODO | TODO | TODO |
| (your program) | TODO | TODO | TODO |

### Compiler Performance
| Program | Compile Time | Run Time | Speedup vs Interpreter |
|---------|-------------|----------|----------------------|
| hello_world.bf | TODO | TODO | TODOx |
| mandelbrot.bf | TODO | TODO | TODOx |
| (your program) | TODO | TODO | TODOx |

**Notes**:
- TODO: What was the typical speedup? (Expected: 50-100x)
- TODO: Which programs benefit most from compilation?
- TODO: Where is the bottleneck (I/O vs computation)?

### Effect of Optimizations
| Optimization | Before | After | Improvement |
|-------------|--------|-------|-------------|
| Constant folding | TODO | TODO | TODO% |
| Clear loops | TODO | TODO | TODO% |
| (your opt) | TODO | TODO | TODO% |

## Debugging Tips & Lessons Learned

### What Worked Well
TODO: Document your successful debugging strategies:
- 
- 
- 

### Common Mistakes to Avoid
TODO: What bugs did you encounter?
1. 
2. 
3. 

### Tools Used
TODO: What tools helped?
- Debugger (gdb, lldb): Used for?
- Valgrind: Memory issues found?
- Profiling tools: 
- Other:

### Key Insights
TODO: What did you learn about:
- Language implementation?
- Compiler design?
- Performance optimization?
- Assembly programming?

## Future Work & Ideas

### Potential Improvements
TODO: What would you do differently?
- [ ] Different data structures?
- [ ] Better optimization passes?
- [ ] JIT compilation?
- [ ] Different target architecture?

### Extensions to Try
- [ ] Larger cell sizes (16-bit, 32-bit)
- [ ] Additional commands
- [ ] Debugger UI
- [ ] Syntax highlighter
- [ ] Other: TODO

## Resources Used

### Specifications & Documentation
- [Brainfuck Language Spec](https://esolangs.org/wiki/Brainfuck)
- [LLVM Language Reference](https://llvm.org/docs/LangRef.html) (if using LLVM)
- [x86-64 Reference](https://www.felixcloutier.com/x86/) (if using asm)
- [Linux syscalls](https://filippo.io/linux-syscall-table/) (if using syscalls)

### Helpful Examples & Tutorials
TODO: Add links to resources you found helpful:
- 
- 
- 

### Code References
TODO: Did you study other implementations? List them:
- 
- 

---

## Quick Reference

### Build Commands
```bash
# Build interpreter
make bf

# Build compiler
make bfc

# Test interpreter
./bf examples/hello_world.bf

# Test compiler
./bfc examples/hello_world.bf -o hello && ./hello

# Benchmark
make benchmark
```

### File Locations
- Interpreter: `src/bf.c`
- Compiler: `src/bfc.c`
- Backend implementations: `src/codegen_*.c`
- Examples: `examples/*.bf`
- Tests: `tests/*.bf`
