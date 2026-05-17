# Compiler Register Allocation Tool

## Project Structure

The final delivery is structured as follows:

```text
📦 DA2026_PRJ2_T<Class>_G<Group>
 ┣ 📂 Documentation        # Doxygen documentation
 ┃ ┣ 📂 html
 ┃ ┗ 📂 latex
 ┣ 📂 Code                 # Project source and data
 ┃ ┣ 📂 code
 ┃ ┃ ┣ 📂 include          # Header files (.h)
 ┃ ┃ ┗ 📂 src              # Source files (.cpp)
 ┃ ┣ 📂 data
 ┃ ┃ ┣ 📂 output           # Generated allocation results
 ┃ ┃ ┣ 📂 ranges           # Input variables and live ranges
 ┃ ┃ ┗ 📂 registers        # Input configuration (K and modes)
 ┃ ┣ 📂 scripts
 ┃ ┃ ┗ 📜 run-default.sh   # Bash script for batch execution
 ┃ ┣ 📜 CMakeLists.txt     # Build configuration
 ┃ ┣ 📜 Doxyfile           # Doxygen settings
 ┃ ┗ 📜 README.md          # Instructions and structure
 ┣ 📜 presentation.pdf     # Slide deck for demo
 ┗ 📜 presentation.html    # Original web-based presentation

---

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
./DA-GRAPH-COLORING -b <range file name> <register file name> <output file name>
```

**Windows**
```bat
cd build
.\DA-GRAPH-COLORING.exe -b <range file name> <register file name> <output file name>
```

---


## Running — Default Datasets Output Automatic Generation

This only generates the output for the 6 given instances.

**Linux / macOS**
```bash
cd scripts
bash run-default.sh
```

**Windows** (requires Git Bash or WSL)
```bash
cd scripts
bash run-default.sh
```

Without Git Bash or WSL, open `run-default.sh` and run the commands it contains manually in a terminal.

---

## Documentation

Run the following command to generate the documentation files:

```bash
doxygen Doxyfile
```