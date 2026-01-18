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
- Refactored to professional project structure
  - Created `include/bf.h` header for constants and function declarations
  - Updated Makefile with `-Iinclude` flag and header dependency tracking
  - Reorganized bf.c: main() at top, helper functions below
  - Changed from array indexing to pointer-based approach
    - `int ptr = 0; memory[ptr]` → `unsigned char *cell = memory; *cell`
    - More idiomatic C, better for teaching pointer arithmetic
    - Direct mapping: `>` is `cell++`, `<` is `cell--`
  - All tests still passing after refactoring

- Implemented bracket matching for loops ([ and ])
  - Chosen approach: **depth-counting scan during execution** (no preprocessing)
  - `find_matching_bracket(program, pos, direction)` function
    - `direction`: +1 for forward scan (at `[`), -1 for backward scan (at `]`)
    - Starts with `depth = 1` (already inside one bracket pair)
    - Scans through program incrementing/decrementing depth at brackets
    - Returns position where `depth == 0` (matching bracket found)
  - Time: O(n) per bracket jump in worst case
  - Space: O(1) - no stack or preprocessing arrays needed
  - Loop behavior:
    - At `[`: if cell is 0, jump forward past matching `]`
    - At `]`: if cell is non-zero, jump backward to matching `[`
  - Test suite extended to 7 tests:
    - Test 5: Simple loop `++[>+<-]>.` (moves value between cells)
    - Test 6: Loop multiplication `+++[>++<-]>.` (3×2=6)
    - Test 7: Nested loops `++[>++[>++<-]<-]>>.` (2×2×2=8)
  - All tests passing

### Date: 2025-12-27

**Code Quality Refactoring Session**

- Header file organization
   - Removed unused `#include <string.h>` and `#include <stdlib.h>`
- Memory management improvements
  - Moved `memory` array from `execute_program()` to `main()`
  - **Rationale**: Resources should be owned by `main()`, not recreated on every function call
  - More efficient (stack allocation only once) and better separation of concerns
- Function return values
  - Changed `execute_program()` signature from `void` to `int`
  - Returns 0 on success (standard Unix convention)
  - Main now cleanly returns `execute_program()`'s result directly
- Internal function visibility
  - Made `find_matching_bracket()` static (internal to bf.c only)
  - **Rationale**: Not part of public API, should be hidden implementation detail

**Bracket Validation and Jump Table Implementation**

- **Problem identified**: Original approach would validate brackets twice (once for errors, once for building jump table)
- **Solution**: Combined validation and jump table building in a single pass
  - Function: `build_jump_table(program, program_length, jump_table)`
  - Uses stack-based algorithm to track opening brackets
  - Builds bidirectional jump table for O(1) lookups during execution
  - Validates bracket matching simultaneously - no redundant work

**Algorithm**:
```c
// Single pass through program:
// - '[' pushes position onto stack
// - ']' pops from stack and records both jump directions
// - Empty stack on ']' means unmatched closing bracket
// - Non-empty stack at end means unmatched opening brackets
```

**Key improvements**:
1. **One pass validation**: Bracket errors detected during jump table construction
2. **O(1) bracket jumps**: Pre-computed jumps eliminate runtime scanning
3. **Exit code reflects errors**: Returns count of unmatched brackets (opening + closing)
4. **Efficient**: From O(n) per bracket jump to O(1) lookup

**Test suite expanded**:
- Test 8-11: Various bracket mismatch scenarios
- Test 12: Mixed mismatches  
- Test 13: Properly balanced brackets still work
- Exit codes correctly reflect number of unmatched brackets

### Date: 2026-01-02

- **Write Brainfuck Programs**
  - `add.bf` Add two single digits
  - Added `add.bf` to tests
- Reorganize project structure
- Began **compiler** implementation (bfc)
  - Implemented lexer (lexer.c/lexer.h)
  - O(n) single-pass tokenization
  - Updated compile_commands.json for LSP support across both projects

### Date: 2026-01-03

**Compiler AST & Parser Implementation**

- **AST Design** (bfc/include/ast.h)
  - Defined 6 node types: `SEQUENCE`, `MOVE_PTR`, `MODIFY_CELL`, `OUTPUT`, `INPUT`, `LOOP`
  - Optimization at AST level: consecutive moves/modifications combined into single nodes
    - Example: `>>>` becomes one `MOVE_PTR(+3)` node, not three separate nodes
    - Reduces memory usage and simplifies later phases
  - Union-based data storage for type-specific information

- **AST Implementation** (bfc/src/ast.c)
  - `ast_create_node()`: Allocates and initializes nodes based on type
  - `ast_free()`: Recursively frees entire tree (handles all union types)
  - `ast_print()`: Debug output with proper tree indentation
    - Shows node type, counts, and operation values (offsets/deltas)
    - Helps visualize parse results during development

- **Lexer Enhancements** (bfc/src/lexer.c)
  - Added `lexer_peek()`: Look at current token type without consuming
  - Added `lexer_next()`: Advance to next token
  - Caching mechanism: Stores current token to avoid re-scanning
  - Public API now includes these simple operations (positions available if needed later)

- **Parser Implementation** (bfc/src/parser.c)
  - `parser_parse()`: Main entry point, returns root `SEQUENCE` node
    - Dynamically grows children array as statements are added
    - Returns NULL on error (handles parse failures gracefully)
  - `parse_statement()`: Handles individual tokens
    - **Optimization**: Accumulates consecutive operations before creating nodes
    - Example: Multiple `TOKEN_RIGHT` tokens combine into single `MOVE_PTR` node
  - `parse_loop()`: Recursive descent for bracket matching
    - Creates loop node with sequence body
    - Tracks bracket nesting and detects mismatches
    - Returns NULL if brackets unmatched
  - Error handling: Unmatched brackets, EOF detection

- **Build System Updated**
  - Makefile now compiles: `bfc.c`, `lexer.c`, `ast.c`, `parser.c`
  - Successfully compiles without warnings or errors

- **Lexer API Refinement**
  - **Primary goal**: Remove `lexer_next_token()` from public API (implementation detail only)
  - Made `lexer_next_token()` static/private - no longer exposed to parser or main
  - **Benefit**: Cleaner public API with only high-level operations (`lexer_peek()`, `lexer_next()`)
  - Added `lexer_format_tokens()` - returns dynamically allocated formatted string
    - Moves output responsibility from library code to caller
  
### Date: 2026-01-12

**AST Architecture Refactoring - From Premature Optimization to Naive Representation**

- **Problem Identified**: AST was prematurely optimizing during parsing
  - Parser combined consecutive operations (`+++` → `MODIFY_CELL(+3)`)
  - Mixed parsing and optimization concerns in single phase
  - Less educational - hid the natural program structure
  - Harder to debug and trace from source to AST

- **Solution: Refactored to Naive AST**
  - **Node type changes**: Split `AST_MOVE_PTR`/`AST_MODIFY_CELL` into specific types:
    - `AST_MOVE_RIGHT`, `AST_MOVE_LEFT` (no offset field)  
    - `AST_INCREMENT`, `AST_DECREMENT` (no delta field)
  - **1:1 token mapping**: Each token creates exactly one AST node
  - **Removed optimization loops**: Parser no longer accumulates consecutive operations

- **Updated Implementation**
  - `ast.h`: Removed `offset`/`delta` fields from union, added specific node types
  - `ast.c`: Updated `ast_create_node()`, `ast_free()`, `ast_print_helper()` for new types
  - `parser.c`: Simplified `parse_statement()` - no accumulation loops, direct token-to-node mapping

- **Educational Benefits**
  - **Clear traceability**: Source `+++` → 3 `INCREMENT` nodes in AST
  - **Separation of concerns**: Parsing builds structure, optimization is separate phase
  - **Debuggable**: Easy to verify parser correctness and AST structure
  - **Real-world architecture**: Mirrors production compilers (multiple specialized passes)

**Key Insight**: Premature optimization in parsing phase was architecturally wrong. Clean separation of parsing and optimization phases creates better learning experience and more maintainable codebase.

### Date: 2026-01-14

- Cleanup
- gdb run

### Date: 2026-01-15

- Cleanup, remove token print

**AST Optimizer Implementation**

- **Extended AST node types** (bfc/include/ast.h)
  - Added `AST_MOVE_PTR` - combines consecutive `>>>` into single node with offset field
  - Added `AST_MODIFY_CELL` - combines consecutive `+++` into single node with delta field
  - Added `AST_CLEAR_LOOP` - recognizes `[-]` and `[+]` clear loop patterns
  - Added union fields: `int offset` for MOVE_PTR, `int delta` for MODIFY_CELL

- **Created ast_optimizer.c** (bfc/src/ast_optimizer.c)
  - `ast_optimize()` - main entry point for optimization passes
  - `optimize_sequence()` - combines consecutive operations in sequences
    - Accumulates multiple `MOVE_RIGHT`/`MOVE_LEFT` → single `MOVE_PTR(offset)`
    - Accumulates multiple `INCREMENT`/`DECREMENT` → single `MODIFY_CELL(delta)`
    - Eliminates zero-effect operations (e.g., `++-` cancels to nothing)
  - `is_clear_loop()` - detects `[-]` and `[+]` patterns, replaces with `CLEAR_LOOP`
  - Recursively optimizes nested loop bodies

- **Updated ast.c**
  - Extended `ast_create_node()` to initialize offset/delta fields
  - Extended `ast_free()` to handle new node types (no extra cleanup needed)
  - Extended `ast_print()` to display optimized nodes with values:
    - `MOVE_PTR(+3)` shows net pointer movement
    - `MODIFY_CELL(-5)` shows net cell modification
    - `CLEAR_LOOP` indicates recognized pattern

- **Updated bfc.c**
  - Added `--optimize` command-line flag
  - Optimization runs after parsing, before code generation
  - Can be combined with `--print-ast` to visualize optimizations

- **Build system updated**
  - Makefile includes ast_optimizer.c compilation
  - All tests passing with zero warnings

**Optimization Results**:
- `hello_world.bf`: 66 nodes → 31 nodes (53% reduction)
- `+++[-].` example: 5 nodes → 3 nodes with clear loop detection
- No behavioral changes - purely representation optimization

**Educational Benefits**:
- Shows separation between parsing and optimization phases
- Demonstrates common compiler optimization techniques
- Easy to visualize with `--print-ast` before/after comparison
- Clean architecture: optimizer is independent module



### Date: 2026-01-16

**IR (Intermediate Representation) Implementation**

- **Created IR layer** - Linear instruction stream between AST and code generation
  - `bfc/include/ir.h` - IR data structures and API
  - `bfc/src/ir.c` - IR generation and manipulation
  
- **IR Instruction Types**
  - `IR_ADD_PTR(operand)` - Pointer arithmetic: `ptr += operand`
  - `IR_ADD_CELL(operand)` - Cell modification: `*ptr += operand`
  - `IR_OUTPUT` - Output current cell
  - `IR_INPUT` - Input to current cell
  - `IR_LOOP_START(label)` - Begin loop with unique label
  - `IR_LOOP_END(label)` - End loop, jump back if non-zero
  - `IR_SET_ZERO` - Optimized clear loop pattern

- **Key Design Decisions**
  - **Linear representation**: Array of instructions (easier to emit code from than tree)
  - **Label-based loops**: Each loop gets unique numeric ID for jump targets
  - **Direct AST mapping**: Converts both naive and optimized AST nodes
  - **Dynamic array**: Grows as needed, starts at 128 instruction capacity

- **AST to IR Conversion** (`ir_from_ast`)
  - Recursive tree traversal with flattening
  - Maps all AST node types (naive and optimized) to IR instructions
  - Sequences are flattened into linear instruction stream
  - Nested loops get unique labels via `next_label` counter
  - `AST_CLEAR_LOOP` → `IR_SET_ZERO` (optimization preserved)

- **Updated bfc.c**
  - Added `--print-ir` flag for debugging
  - IR generation runs after AST optimization
  - Pipeline: Source → Lexer → Parser → AST → [Optimizer] → IR → [Code Gen]

- **Testing Results** (hello_world.bf)
  - Naive AST → 106 IR instructions
  - Optimized AST → 59 IR instructions (44% reduction!)
  - Clear loop pattern preserved: `[-]` → `SET_ZERO`
  - Loop labels correctly assigned (0, 1, 2, etc.)

**Educational Value**:
- Shows how compilers bridge high-level AST to low-level code generation
- IR is platform-independent (can target LLVM, assembly, C, etc.)
- Linear form makes code generation tractable
- Demonstrates separation of concerns (parsing → optimization → IR → codegen)

**x86-64 Assembly Backend - Initial Setup**

- **Created minimal assembly code generator** (bfc/src/codegen_asm.c)
  - `codegen_asm()` - Entry point for IR → assembly conversion
  - Initial implementation: Hardcoded minimal test program
  - Emits x86-64 AT&T syntax assembly (.s file)
  - Proof of concept: Prints 'H' character using putchar

- **Minimal assembly structure**:
  ```asm
  .text
  .globl main
  main:
      mov $72, %edi      # Load 'H' into first arg
      call putchar        # Call putchar('H')
      xor %eax, %eax     # Return 0
      ret
  ```

- **Integration with compiler** (bfc/src/bfc.c)
  - Added `#include "codegen_asm.h"`
  - Phase 5: Opens output file and calls codegen_asm()
  - Writes assembly to file specified with `-o` flag
  - Proper error handling and resource cleanup

- **Build system updated** (bfc/Makefile)
  - Added codegen_asm.c to SOURCES
  - Compiles and links successfully

- **Testing**:
  ```bash
  ./bfc/bin/bfc program.bf -o test.s    # Generate assembly
  gcc test.s -o test                     # Assemble and link
  ./test                                 # Run (prints 'H')
  ```

**Status**: Foundation in place for full IR translation.

### Date: 2026-01-17

**Compiler Configuration - Optimization Made Default**

- **Updated bfc.c** - Optimization now runs by default before IR generation
  - Removed `--optimize` flag requirement

**x86-64 Assembly Backend - Memory Allocation and IR Translation**

- **Memory Allocation and Stack Frame Setup**
  - Allocate 30,000 bytes on stack for BF memory array
  - Proper prologue/epilogue with frame pointer management
  - Memory initialization using `rep stosb` (fast zero-fill)
  - Stack alignment maintained for function calls

- **Register Assignment**
  - `%r12` = Base pointer to BF memory array (callee-saved)
  - `%r13` = Current cell offset (0-29999) (callee-saved)
  - `%eax/%al` = Temporary for load/store operations
  - `%edi` = Function argument register (for putchar/getchar)

- **IR Instruction Translation Loop**
  - Iterate through all IR instructions
  - Switch statement dispatches to handler for each opcode
  - Comments in assembly show original IR instruction

- **Implemented IR_ADD_PTR (Pointer Movement)**
  - Translates to: `add $N, %r13` or `sub $N, %r13`
  - Handles positive and negative offsets
  - Optimized pointer movements work correctly (e.g., `>>>` → `ADD_PTR +3`)
  - Tested: `>>><` → `add $2, %r13` ✓

- **Implemented IR_ADD_CELL (Cell Modification)**
  - Load: `movzbl (%r12,%r13), %eax` - Zero-extend byte to 32-bit
  - Modify: `add $N, %eax` or `sub $N, %eax`
  - Store: `movb %al, (%r12,%r13)` - Write back low byte
  - Handles byte overflow naturally (wraps at 256)
  - Tested: `+++>++<-` produces correct assembly ✓

- **Implemented IR_OUTPUT (Character Output)**
  - Load byte: `movzbl (%r12,%r13), %edi` - Into first argument register
  - Call: `call putchar` - Print character
  - Uses x86-64 System V calling convention (first arg in %edi)

- **Implemented IR_LOOP_START and IR_LOOP_END (Control Flow)**
  - Loop structure with labeled jumps:
    ```asm
    loop_N_start:
        movzbl (%r12,%r13), %eax   # Load current cell
        test %eax, %eax             # Set zero flag
        jz loop_N_end               # Skip loop if zero
        # ... loop body ...
        movzbl (%r12,%r13), %eax   # Load again
        test %eax, %eax             # Test
        jnz loop_N_start            # Repeat if non-zero
    loop_N_end:
    ```
  - Each loop gets unique label ID from IR
  - Handles nested loops correctly (each has own label)

- **Generated Assembly Quality**
  - Clean, readable output with comments
  - Proper AT&T syntax (`%` for registers, `$` for immediates)
  - Memory addressing: `(%r12,%r13)` = base+offset
  - Each IR instruction generates 3-4 assembly instructions

- **Testing Results**
  - Loop test: Multiplication (2×3=6) works correctly
  - **hello_world.bf: Full compilation and execution successful!**
  - Programs execute with clean exit (code 0)
  - No segfaults or memory errors

**Educational Observations**:
- x86-64 addressing modes are powerful: `(%base,%offset)` in one instruction
- `movzbl` (zero-extend) prevents sign-extension issues with byte values
- `test reg, reg` is efficient way to check for zero (sets flags)
- Conditional jumps (`jz`, `jnz`) make loops straightforward
- Callee-saved registers (`%r12`, `%r13`) persist across function calls
- `rep stosb` is hardware-accelerated memset (very fast)

### Date: 2026-01-18

**x86-64 Assembly Backend - COMPLETION**

- **Implemented IR_INPUT (Character Input)**
  - Call: `call getchar` - Returns character in %eax
  - Store: `movb %al, (%r12,%r13)` - Store low byte to current cell
  - Handles EOF naturally (getchar returns -1, stored as 0xFF)
  - Tested: add.bf works! "35" input → '8' output (3+5=8) ✓

- **Implemented IR_SET_ZERO (Optimized Clear Loop)**
  - Single instruction: `movb $0, (%r12,%r13)` - Set cell to 0
  - Replaces `[-]` loop pattern (saves ~8 instructions per clear)
  - Much faster than loop-based clear
  - Tested: `+++++[-]+++++.` outputs ASCII 5 (clear works) ✓

- **Complete IR → x86-64 Translation Table**
  | IR Instruction | x86-64 Assembly | Instructions |
  |----------------|-----------------|--------------|
  | IR_ADD_PTR(N) | `add/sub $N, %r13` | 1 |
  | IR_ADD_CELL(N) | `movzbl, add/sub, movb` | 3 |
  | IR_OUTPUT | `movzbl, call putchar` | 2 |
  | IR_INPUT | `call getchar, movb` | 2 |
  | IR_LOOP_START | `movzbl, test, jz` | 3 |
  | IR_LOOP_END | `movzbl, test, jnz` | 3 |
  | IR_SET_ZERO | `movb $0, (...)` | 1 |

- **Comprehensive Testing**
  - hello_world.bf: Compiles and prints "Hello World!" ✓
  - add.bf: Input/output working, arithmetic correct ✓
  - Nested loops: Correct label generation and jumps ✓
  - Clear loop optimization: [-] → single instruction ✓
  - All test programs execute cleanly (exit code 0)

- **Performance Characteristics**
  - Stack-based memory (30,000 bytes)
  - Zero memory allocation overhead (all stack)
  - Optimized clear loops (1 instruction vs 8+)
  - Direct register usage (no unnecessary memory traffic)
  - Function calls follow System V ABI

- **Generated Code Quality**
  - Readable assembly with descriptive comments
  - Proper register preservation (callee-saved %r12/%r13)
  - Correct stack alignment for function calls
  - Efficient instruction selection
  - Small code size (~10-20 instructions per IR instruction)

**Compiler Pipeline Complete**:
```
Source.bf → Lexer → Parser → AST → Optimizer → IR → x86-64 Asm → Executable
     ✓         ✓       ✓       ✓        ✓         ✓       ✓          ✓
```

**🎉 MILESTONE: Full working Brainfuck compiler with native code generation! 🎉**

### Addendum:

**Compiler UX Improvements - Direct Binary Output**

- **Integrated gcc invocation into bfc**
  - Compiler now calls `gcc` automatically after generating assembly
  - Single command: `./bfc/bin/bfc program.bf` → ready-to-run executable
  - Eliminates manual two-step compilation process

- **Directory Structure**
  - `build/` - Intermediate assembly files (`.s`)
  - `bin/` - Final BF executables (native binaries)

- **Simplified Command-Line Interface**
  - Removed `-o` output flag (was confusing)
  - Output name always derived from input: `program.bf` → `bin/program`
  - Assembly intermediate: `build/program.s`
  - Only debug flags remain: `--print-ast`, `--print-ir`

- **Code Cleanup**
  - Removed all external dependencies: no `libgen.h`, no `basename()`, no `strdup()`
  - Pure standard C library (`string.h` only for path manipulation)
  - Removed `-D_POSIX_C_SOURCE` flag (no longer needed)
  - Simple `strrchr()` for path parsing, `strncpy()` for string handling

- **Build System Polish**
  - Makefile uses `wildcard` for automatic source discovery
  - Aligned variable definitions for readability
  - Order-only prerequisites for directory dependencies
  - Clean, minimal 21-line Makefile

**Usage**:
```bash
./bfc/bin/bfc bf/examples/hello_world.bf
# Generates:
#   build/hello_world.s  (assembly intermediate)
#   bin/hello_world      (executable)

./bin/hello_world
# Output: Hello World!
```

**Educational Value Achieved**:
- Understood full compilation pipeline from source to machine code
- Learned x86-64 assembly, calling conventions, stack management
- Implemented IR as abstraction layer for portability
- Experienced incremental development and testing methodology
- Built real, working compiler generating native executables

**Possible Future Enhancements**:
- Additional backends: ARM64, RISC-V, C code generation
- More optimizations: Dead code elimination, constant folding
- Better error messages with line numbers
- Debugger integration (DWARF debug info)
- Compilation to shared library
- JIT compilation support


## Resources Used

### Specifications & Documentation
- [Brainfuck Language Spec](https://esolangs.org/wiki/Brainfuck)
- [x86-64 Reference](https://www.felixcloutier.com/x86/)
- [System V ABI Documentation](https://wiki.osdev.org/System_V_ABI)

### Development Tools & Assistance
- **Claude Sonnet 4.5** (GitHub Copilot) - AI pair programming assistant
  - Architectural guidance for compiler phases (lexer, parser, AST, IR, codegen)
  - x86-64 assembly expertise and calling convention details
  - Code generation patterns and optimization strategies
  - Incremental development methodology and testing approaches
  - Documentation and educational explanations throughout
  - Made this pedagogical journey possible through step-by-step guidance


---

## Quick Reference


### File Locations
- Interpreter: `src/bf.c`
- Compiler: `src/bfc.c`
- Backend implementations: `src/codegen_*.c`
- Examples: `examples/*.bf`
- Tests: `tests/*.bf`
