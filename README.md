# Compiler Register Allocation Tool

## Build

Start from the root directory containing the `CMakeLists.txt`.

**Linux / macOS**
```bash
mkdir build && cd build
cmake ..
make
```

**Windows** (requires CMake and a C++ compiler — MinGW or MSVC)
```bat
mkdir build
cd build
cmake ..
cmake --build .
```

> If you are using MinGW on Windows, replace `cmake ..` with `cmake .. -G "MinGW Makefiles"` and `cmake --build .` with `mingw32-make`.

---

## Running — Interactive Menu

**Linux / macOS**
```bash
cd build
./DA-GRAPH-COLORING
```

**Windows**
```bat
cd build
.\DA-GRAPH-COLORING.exe
```

---

## Running — Batch Mode

**Linux / macOS**
```bash
cd build
./DA-GRAPH-COLORING -b <range path> <register path> <output path>
```

**Windows**
```bat
cd build
.\DA-GRAPH-COLORING.exe -b <range path> <register path> <output path>
```

Note: When using the command-line batch parameters, provide the full relative paths (e.g., `../data/ranges/ranges1.txt`).

---


## Running — Default Datasets Output Automatic Generation

This only generates the output for the 6 given instances.

**Linux / macOS**
```bash
cd scripts
bash batch.sh
```

**Windows** (requires Git Bash or WSL)
```bash
cd scripts
bash batch.sh
```

Without Git Bash or WSL, open `batch.sh` and run the commands it contains manually in a terminal.