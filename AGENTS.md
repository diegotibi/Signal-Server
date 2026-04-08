# Project Context

## Purpose
- `Signal-Server` is a C++14 radio propagation simulator derived from SPLAT!/CloudRF lineage.
- Main binaries:
  - `signalserver`: standard SDF/SRTM flow
  - `signalserverHD`: higher default tile resolution and wider page budget
  - `signalserverLIDAR`: ASCII-grid LIDAR flow with dynamic dimensions

## Current Build / Runtime Shape
- Build system: CMake from [`src/CMakeLists.txt`](/Users/ceres/CLionProjects/Signal-Server/src/CMakeLists.txt).
- Core deps now include `libpng` and `spdlog` in addition to legacy compression/libs.
- Default raster output is now native PNG through the internal image abstraction.
- Legacy PPM backend still exists as fallback/compatibility backend.

## High-Level Architecture
- [`src/main.cc`](/Users/ceres/CLionProjects/Signal-Server/src/main.cc)
  - CLI parsing
  - runtime mode selection (`signalserver`, `HD`, `LIDAR`)
  - bounds calculation
  - DEM/LIDAR allocation strategy
  - propagation dispatch
  - final raster write
- [`src/inputs.cc`](/Users/ceres/CLionProjects/Signal-Server/src/inputs.cc)
  - SDF/GZ/BZ loading
  - LIDAR tile loading/merging/resampling
  - clutter, UDT, antenna pattern and color map loading
- [`src/outputs.cc`](/Users/ceres/CLionProjects/Signal-Server/src/outputs.cc)
  - map rendering for loss, signal strength, received power, LOS
  - image metadata population
- [`src/image.cc`](/Users/ceres/CLionProjects/Signal-Server/src/image.cc)
  - format dispatch layer
- [`src/image-png.cc`](/Users/ceres/CLionProjects/Signal-Server/src/image-png.cc)
  - native PNG writer using libpng
- [`src/models/los.cc`](/Users/ceres/CLionProjects/Signal-Server/src/models/los.cc)
  - segmented threaded propagation for LOS and rectangular/radial propagation flows
- [`src/tiles.cc`](/Users/ceres/CLionProjects/Signal-Server/src/tiles.cc)
  - ASCII-grid tile parsing and tile rescale utilities

## What Looks Already Implemented Recently
- Native PNG output is integrated and appears to be the default image backend.
- PNG outputs can embed geographic bounds (`north/east/south/west`) as text metadata.
- Logging was modernized to `spdlog` across startup, loading, rendering and model code.
- LIDAR loading supports:
  - multiple input files
  - resolution normalization across tiles
  - clipping to requested plot bounds
  - dynamic merged canvas size instead of fixed square assumptions
- Non-LIDAR DEM allocation now estimates required page count from plot bounds and reduces memory footprint.
- DEM buffers can allocate with dynamic `rows/cols` via `dem_alloc_rows` / `dem_alloc_cols`.
- LOS flow now avoids allocating the signal map with `allocate_signal_map = (prop_model != LOS)`.
- LOS threaded execution supports configurable `-segments`, with auto-tuning when not explicitly provided.
- There is also an experimental radial propagation mode (`-rp`).

## Important Implementation Notes
- Image default format is `IMAGE_PNG` in [`src/image.cc`](/Users/ceres/CLionProjects/Signal-Server/src/image.cc).
- Output filename normalization swaps or appends the extension based on active backend, so callers mostly pass a basename.
- `loadLIDAR()` mutates the input file list with `strtok`, so the incoming buffer must remain writable.
- LIDAR path sets `IPPD`, `width`, `height`, `dem_alloc_rows`, `dem_alloc_cols` from merged tile geometry.
- Standard SDF path still uses square per-page allocation based on `IPPD`.
- `PlotPropagation()` divides the rectangular perimeter into edge segments; `PlotPropagationRadius()` divides a circle into angular sectors.
- LOS output path disables later cropping and writes bounds directly from the circular box.

## Known Inconsistencies / Risk Areas
- `do_allocs()` in [`src/main.cc`](/Users/ceres/CLionProjects/Signal-Server/src/main.cc) calls `alloc_dem()` and then immediately nulls `data_block`, `mask_block`, `signal_block` in the initialization loop. That looks suspicious and should be verified before relying on those fields later.
- Help text says `-segments` must be even and `> 4`, but runtime validation currently allows `4` and LOS code silently normalizes odd values upward.
- Radial mode validation in [`src/models/los.cc`](/Users/ceres/CLionProjects/Signal-Server/src/models/los.cc) accepts multiples of 2 or 3, while rectangular/CLI validation is phrased differently.
- `test.sh` still assumes PPM output and ImageMagick conversion in multiple places, so it is out of sync with the PNG-native path.
- README has been partially updated for PNG default, but some historical text and examples still reflect the old PPM-centric workflow.
- Repo worktree is already dirty on many core files; future edits need care to avoid stomping active in-progress work.

## Benchmark / Performance Context
- [`benchmarking.md`](/Users/ceres/CLionProjects/Signal-Server/benchmarking.md) documents large speedups from W3AXL refactored multi-threaded processing.
- Repo contains recent benchmark artifacts in `benchmark_outputs/`, suggesting active work on LOS segmentation/perf tuning and packaging.

## Repo State Observed During Analysis
- Modified tracked files include `README.md`, `src/main.cc`, `src/inputs.*`, `src/image*`, `src/models/los.cc`, `src/models/cost.cc`, `src/tiles.cc`, `src/outputs.cc`, and others.
- Untracked/generated material includes `.idea/`, `benchmark_outputs/`, `dist/`, `.DS_Store`, and new PNG backend sources.
- Assumption for next tasks: this branch already contains substantial local work not to be reverted blindly.

## Likely Good Feature Entry Points
- New CLI/runtime feature: start in [`src/main.cc`](/Users/ceres/CLionProjects/Signal-Server/src/main.cc), then update the relevant loader/model/output module.
- Output/rendering feature: [`src/outputs.cc`](/Users/ceres/CLionProjects/Signal-Server/src/outputs.cc) and image backend files.
- Terrain/LIDAR ingestion feature: [`src/inputs.cc`](/Users/ceres/CLionProjects/Signal-Server/src/inputs.cc) and [`src/tiles.cc`](/Users/ceres/CLionProjects/Signal-Server/src/tiles.cc).
- Propagation/perf feature: [`src/models/los.cc`](/Users/ceres/CLionProjects/Signal-Server/src/models/los.cc) plus allocation/bounds code in [`src/main.cc`](/Users/ceres/CLionProjects/Signal-Server/src/main.cc).

## Suggested Next Move
- Before implementing the next feature, decide whether it belongs primarily to:
  - CLI and orchestration
  - terrain loading / clipping / memory
  - propagation math / threading
  - output rendering / metadata / packaging
