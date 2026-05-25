# Traffic Simulation — README

Overview
- Small event-driven traffic intersection simulator written in C (C11).
- Refactored for clarity: sources in `src/`, headers in `include/`.
- Cross-platform portability shim in `include/portability.h` so code builds on Linux and Visual Studio.

Quick build (Linux)
```
cd software_simulation
make clean && make
./simulation
```

Build in Visual Studio
- Open `software_simulation.sln` in Visual Studio. The project file already includes the `src\` files and sets `include` as an Additional Include Directory.
- Build (Ctrl+Shift+B) and Run.

Project layout
- include/: public headers
  - `types.h` — central data types (Vehicle, Queue, Intersection, stats)
  - `datastructs.h` — queue and drawing helpers
  - `engine.h` — simulation engine functions
  - `io.h` — config/log/stat I/O functions
  - `algos.h` — search / sort utilities
  - `portability.h` — small portability shim (Windows vs POSIX wrappers)
- src/: implementation files matching the headers
- Makefile: simple build that compiles `src/*.c` with `-Iinclude`.