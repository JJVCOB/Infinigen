# Engine2D

A 2D game engine, built one week at a time, following
*Game Engine Architecture* 4th ed. Vol. I (Gregory).

## Build

Requires CMake 3.28+, a C++23 compiler, and Git. Nothing else.
SDL3 is downloaded and built automatically the first time you configure.

```
cmake --preset debug
cmake --build --preset debug
```

The executable lands in `build/debug/bin/`.

The first configure on any new PC may take several minutes because it is compiling SDL3 from
source. Subsequent configures are faster. This is normal; it is not hung.

## Layout

| Path | What lives here |
|---|---|
| `engine/` | The engine static library. Knows nothing about any game or tool. |
| `sandbox/` | The standalone game runtime. The only place game-shaped code is allowed. |
| `editor/` | The IDE (added Week 2). Panels live here. |
| `tests/` | Unit tests (added Week 2). |
| `cmake/` | Build system modules. |
| `assets/` | Data files loaded at runtime (used from Week 9). |
| `docs/` | Written deliverables - measurement tables, reports, notes. |

## Architecture notes

_TODO: fill this in as the course progresses._
