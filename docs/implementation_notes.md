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

**Next Steps**:
- Code cleanup: `lexer_next_token()` exists but not used by parser. Consider making it private (internal only) to simplify public API. Current design keeps position info available for future error reporting.






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
