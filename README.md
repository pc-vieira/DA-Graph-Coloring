# Register Allocation via Graph Coloring

**DA — Design of Algorithms (L.EIC016) | FEUP | Spring 2026**

A register allocation tool that uses graph coloring heuristics to assign physical registers to variable webs, minimizing register usage.

## Features

- **Live Range Parsing** — Reads variable live ranges from input files
- **Web Construction** — Merges overlapping live ranges into webs using Union-Find
- **Interference Graph** — Builds an undirected graph where edges represent web conflicts
- **4 Coloring Algorithms:**
  - `basic` — Greedy simplification + coloring (T2.1)
  - `spilling` — Spills up to K webs to memory (T2.2)
  - `splitting` — Splits up to K webs at largest gap (T2.3)
  - `free` — DSatur (Degree of Saturation) heuristic (T2.4)

---

## Prerequisites

- **C++17** compatible compiler
- **CMake** ≥ 3.17

### Installing prerequisites

#### Linux (Ubuntu/Debian)
```bash
sudo apt update
sudo apt install build-essential cmake
```

#### macOS
```bash
xcode-select --install
brew install cmake
```

#### Windows
- Install [CMake](https://cmake.org/download/)
- Install [MinGW-w64](https://www.mingw-w64.org/) or use [Visual Studio](https://visualstudio.microsoft.com/) with C++ workload

---

## Building

### Linux / macOS

```bash
mkdir -p build && cd build
cmake ..
make -j$(nproc)
```

### Windows (Command Prompt with MinGW)

```cmd
mkdir build && cd build
cmake .. -G "MinGW Makefiles"
mingw32-make
```

### Windows (PowerShell with Visual Studio)

```powershell
mkdir build; cd build
cmake ..
cmake --build . --config Release
```

The executable `myProg` (or `myProg.exe` on Windows) will be created in the `build/` directory.

---

## Running

### Batch Mode

Run the program with the `-b` flag, providing three file paths:

```
./myProg -b <ranges_file> <registers_file> <output_file>
```

#### Linux / macOS (from inside `build/`)

```bash
./myProg -b ../data/basic/ranges/ranges1.txt ../data/basic/registers/registers2.txt ../data/basic/output/allocation1.txt
```

#### Windows (from inside `build/`)

```cmd
myProg.exe -b ..\data\basic\ranges\ranges1.txt ..\data\basic\registers\registers2.txt ..\data\basic\output\allocation1.txt
```

#### Example output

```
webs: 3
web0: 1+,2,3,4,5,6-
web1: 9+,10,11,12+-,13,14-,20+
web2: 7+,8,9,10-
registers: 2
r0: web0
r0: web2
r1: web1
```

### Interactive Mode

Run without arguments to enter the interactive menu:

```bash
./myProg
```

```
===== Register Allocation Tool =====
1. Load ranges file
2. Load registers/config file
3. Display parsed live ranges
4. Display webs
5. Display interference graph
6. Run register allocation
7. Display results
8. Save results to file
0. Exit
=====================================
Option:
```

---

## Input File Formats

### Ranges file (`ranges.txt`)

Each line maps a variable to a live range. Lines starting with `#` are comments.

```
# comment
sum: 7+,8,9,10-
i: 1+,2,3,4,5,6-
i: 9+,10,11,12-
```

- `+` after a line number = variable is **defined** (written) at that line
- `-` after a line number = variable is **last used** (read) at that line
- No annotation = variable is alive but not directly read/written

### Registers file (`registers.txt`)

```
# comment
registers: 2
algorithm: basic
```

**Algorithm options:**
| Algorithm | Format | Description |
|-----------|--------|-------------|
| `basic` | `algorithm: basic` | Greedy graph coloring |
| `spilling` | `algorithm: spilling, K` | Spill up to K webs to memory |
| `splitting` | `algorithm: splitting, K` | Split up to K webs |
| `free` | `algorithm: free` | DSatur heuristic |

---

## Test Datasets

The `data/basic/` directory contains 6 test cases:

| Ranges | Registers | Expected Registers |
|--------|-----------|-------------------|
| `ranges1.txt` | `registers2.txt` | 2 |
| `ranges2.txt` | `registers2.txt` | 2 |
| `ranges3.txt` | `registers2.txt` | 2 |
| `ranges4.txt` | `registers1.txt` | 1 |
| `ranges5.txt` | `registers1.txt` | 1 |
| `ranges6.txt` | `registers3.txt` | 3 |

### Run all tests (Linux/macOS)

```bash
cd build
for i in 1 2 3; do
  for r in 1 2 3; do
    echo "=== ranges${i}.txt + registers${r}.txt ==="
    ./myProg -b ../data/basic/ranges/ranges${i}.txt ../data/basic/registers/registers${r}.txt /dev/null 2>&1
  done
done

echo "=== ranges4 + registers1 ==="
./myProg -b ../data/basic/ranges/ranges4.txt ../data/basic/registers/registers1.txt /dev/null

echo "=== ranges5 + registers1 ==="
./myProg -b ../data/basic/ranges/ranges5.txt ../data/basic/registers/registers1.txt /dev/null

echo "=== ranges6 + registers3 ==="
./myProg -b ../data/basic/ranges/ranges6.txt ../data/basic/registers/registers3.txt /dev/null
```

---

## Project Structure

```
DA-Graph-Coloring/
├── CMakeLists.txt
├── README.md
├── data_structures/
│   ├── Graph.h                 # TP-provided graph (extended with color/enabled)
│   └── MutablePriorityQueue.h  # TP-provided priority queue
├── src/
│   ├── main.cpp                # Entry point (batch + interactive)
│   ├── Parser.h / .cpp         # Input file parsing
│   ├── LiveRange.h             # ProgramPoint & LiveRange structs
│   ├── Web.h / .cpp            # Web class
│   ├── WebBuilder.h / .cpp     # Merges live ranges → webs (Union-Find)
│   ├── InterferenceGraph.h/.cpp# Builds Graph<int> from webs
│   ├── GraphColoring.h / .cpp  # All 4 coloring algorithms
│   ├── OutputWriter.h / .cpp   # Output formatting
│   └── Menu.h / .cpp           # Interactive menu
├── data/
│   └── basic/                  # Sample test datasets
│       ├── ranges/
│       ├── registers/
│       └── output/
└── docs/                       # Doxygen documentation (generated)
```
