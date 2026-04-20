# ExamQuestionGenerator 2.0

A tool that reads draft exam questions from a text file and generates LaTeX
markup for printed exams.

## Build

```
cmake -S . -B build
cmake --build build
```

The executable is always placed at `build/begin_exam_generator` (the
`CMAKE_RUNTIME_OUTPUT_DIRECTORY` is pinned to `${CMAKE_SOURCE_DIR}/build` in
`CMakeLists.txt`), regardless of how cmake is invoked.

## Run

```
./build/begin_exam_generator <input_file>
```

Reads the input file and writes LaTeX output to a file with the same base
name and a `.tex` extension (e.g. `input.txt` -> `input.tex`). If the input
file cannot be read, an error message is written to standard out.

## Tests

GoogleTest is fetched automatically via `FetchContent`. After building:

```
./build/test_exam_generator
```

Or run the suite via CTest (one test entry runs all GoogleTest cases):

```
ctest --test-dir build --output-on-failure
```

**Visual Studio (and other multi-config generators):** CTest needs a build
type. After `cmake --build build --config Debug`, run:

```
ctest --test-dir build -C Debug --output-on-failure
```

(Use `-C Release` if you built Release.) On Windows you can also run
`.\run-ctest.ps1` from the project root (defaults to Debug).

**Single-config generators** (Ninja, MinGW Makefiles, Unix Makefiles): the
`-C` flag is not required; `ctest --test-dir build` is enough.

## Layout

- `src/`     – implementation
- `include/` – public headers
- `tests/`   – GoogleTest-based unit tests
- `samples/` – example input files
- `results/` – example LaTeX output
