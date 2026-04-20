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

## Layout

- `src/`     – implementation
- `include/` – public headers
- `tests/`   – GoogleTest-based unit tests
- `samples/` – example input files
- `results/` – example LaTeX output
