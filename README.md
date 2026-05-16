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
./DA_Register_Allocation
```

**Windows**
```bat
cd build
.\DA_Register_Allocation.exe
```

---

## Running — Batch Mode

This only runs the 6 given instances (the default datasets). If you want to run a custom instance without using the interactive menu, see the next section.

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

---

## Running — Custom Instances Via Terminal

**Linux / macOS**
```bash
cd build
./DA_Register_Allocation -b <range path> <register path> <output path>
```

**Windows**
```bat
cd build
.\DA_Register_Allocation.exe -b <range path> <register path> <output path>
```

We suggest you follow this template for the paths:
```
../data/ranges/ranges1.txt
```
