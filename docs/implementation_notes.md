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

**Next Steps**: Code generation backend (LLVM IR, x86-64 assembly, or C)


## Next Steps (When You Return)

**Current Status**: ✅ Lexer, ✅ Parser, ✅ Naive AST, ✅ AST Optimizer complete

**Recommended Path: IR Generation → Assembly Backend (Pedagogical)**

The natural progression for a minimal, educational compiler:

### Phase 1: IR Generation (Foundation Layer)
**Why IR first**: Decouples high-level AST from low-level assembly details. Makes code generation tractable.

1. **Create `bfc/include/ir.h` and `bfc/src/ir.c`**
   - Define simple `IRInstruction` types:
     - `IR_ADD_PTR(offset)` - pointer arithmetic
     - `IR_ADD_CELL(delta)` - cell modification  
     - `IR_OUTPUT` / `IR_INPUT` - I/O operations
     - `IR_LOOP_START` / `IR_LOOP_END` - control flow with labels
   - Keep it minimal: just enough to represent BF semantics
   
2. **Implement `ast_to_ir(ast)` converter**
   - Linear instruction stream (easy to emit assembly from)
   - Flatten nested sequences into single list
   - Assign numeric labels to loops for jumping
   
3. **Add `--print-ir` flag for debugging**
   - Verify IR correctness before moving to assembly
   - Should be human-readable

**Educational value**: Shows how compilers bridge abstract syntax to concrete instructions. IR is the Rosetta Stone between parsing and code generation.

### Phase 2: x86-64 Assembly Backend (Target Goal)
**Why x86-64**: Native code, minimal dependencies, runs directly on Linux.

1. **Create `bfc/src/codegen_asm.c`**
   - Generate `.s` assembly file from IR
   - Minimal runtime: array on stack/heap, syscalls for I/O
   - Linux syscalls: `write(1, buf, 1)` for output, `read(0, buf, 1)` for input
   
2. **Key mappings**:
   - Memory array: `[rbp - N]` or heap pointer in `r12`
   - Cell pointer: register `r13`
   - `IR_ADD_PTR` → `add r13, offset`
   - `IR_ADD_CELL` → `add byte [r13], delta`
   - `IR_OUTPUT` → syscall wrapper
   - Loops → label + `cmp`/`jz`/`jnz` instructions

3. **Build pipeline**:
   ```bash
   ./bfc program.bf -o program.s    # Generate assembly
   gcc -o program program.s          # Assemble and link
   ./program                         # Run!
   ```

**Educational value**: See exactly how high-level constructs map to CPU instructions. Understand stack frames, registers, syscalls, and ELF binaries.

### Alternative Quick Win: AST Interpreter (Optional)
- Add `ast_execute()` to run optimized AST directly
- Good for validating optimizer correctness
- ~100 lines of code
- Can delay this until after assembly backend works

**Time estimate**: IR generation (1-2 sessions), Assembly backend (2-3 sessions)




## Resources Used

### Specifications & Documentation
- [Brainfuck Language Spec](https://esolangs.org/wiki/Brainfuck)
- [LLVM Language Reference](https://llvm.org/docs/LangRef.html) (if using LLVM)
- [x86-64 Reference](https://www.felixcloutier.com/x86/) (if using asm)
- [Linux syscalls](https://filippo.io/linux-syscall-table/) (if using syscalls)

### Helpful Examples & Tutorials
TODO: Add links to resources you found helpful:


---

## Quick Reference


### File Locations
- Interpreter: `src/bf.c`
- Compiler: `src/bfc.c`
- Backend implementations: `src/codegen_*.c`
- Examples: `examples/*.bf`
- Tests: `tests/*.bf`
