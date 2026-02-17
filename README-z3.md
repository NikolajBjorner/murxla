# Z3 in Murxla (First-class Solver)

PR: https://github.com/NikolajBjorner/murxla/pull/1  
Title: Integrate Z3 as first-class solver with C++ API  
Status: draft / open  
Author: Copilot

## Summary

Historically, Murxla could exercise Z3 only through the SMT-LIB2 front-end. This pull request integrates Z3 as a first-class solver using Z3’s **C++ API**, enabling Murxla to fuzz Z3 natively at the API level (similar to existing native integrations for other solvers).

High-level impact:
- Adds a native Z3 backend under `src/solver/z3/`.
- Adds CMake detection and a build option to enable Z3.
- Registers Z3 in Murxla’s solver factory and CLI.
- Integrates Z3 into Murxla’s capability/profile mechanism.

## Scope of change (from PR)

- Files changed: 9
- Additions / deletions: +1724 / -2
- Base: `main`
- Head branch: `copilot/make-z3-first-class-solver`

## Implementation overview

### Solver implementation (`src/solver/z3/`)

The Z3 integration is implemented via three main classes:

- **`Z3Sort`**: wraps `z3::sort` and provides numerous sort/type predicates (15+) used for capability checks and safe operation selection.
- **`Z3Term`**: wraps `z3::expr` and provides term introspection utilities.
- **`Z3Solver`**: implements Murxla’s `Solver` interface with 30+ operations and broad operator coverage.

### Supported capabilities (as stated in the PR)

**Theories**:
- ARRAY, BOOL, BV, DT, FP, INT, QUANT, REAL, STRING, UF

**Sorts**:
- Bool, Int, Real
- BV(n)
- FP(e,s), RM
- Array
- String / Seq
- Datatype
- Uninterpreted

### Unsupported / excluded features

- **First-class functions**: Z3 does not support first-class function values in the way expected by Murxla’s `mk_fun()`; the Z3 backend returns `nullptr` for `mk_fun()`.
- The Z3 profile excludes:
  - `SORT_FUN`
  - `SORT_REGLAN`
  - `SORT_BAG`
  - `SORT_SET`

### Notable design decisions

- **Namespace**: Uses `murxla::z3solver` and `using namespace ::z3;` to reduce ambiguity and friction when using Z3’s API.
- **N-ary operators implemented via reduction**: Certain operators (e.g., division) are binary in Z3 but may be generated n-ary by Murxla; the implementation reduces these left-associatively (matching patterns used in other solver integrations).

## Building Murxla with Z3

### Prerequisites

- A Z3 installation that provides the **C++ API headers** and the **Z3 libraries**.
- A CMake toolchain capable of finding Z3. The PR adds `cmake/FindZ3.cmake`; depending on your environment you may need to set `CMAKE_PREFIX_PATH` or `Z3_DIR`.

### Configure & build (example)

```bash
cmake -S . -B build -DENABLE_Z3=ON
cmake --build build
```

Notes:
- When enabled, the build defines `MURXLA_USE_Z3` and links against Z3.

## Running Murxla with Z3

Once built with Z3 enabled, select the Z3 backend using the CLI:

```bash
./murxla --z3
```

(Use your project’s normal invocation pattern if the binary name/path differs.)

## What to validate / review

If you are reviewing or testing the integration, focus on:

- **CMake detection & linking** across platforms (shared vs static Z3, Windows/macOS/Linux).
- Correct **capability profiling** for unsupported sorts/ops (especially function sorts).
- Correctness of **arity lowering** for operators that are binary in Z3 but may be emitted n-ary by Murxla.
- Correct interaction with solver features like **contexts**, **incrementality (push/pop)**, and term/sort lifetime management.