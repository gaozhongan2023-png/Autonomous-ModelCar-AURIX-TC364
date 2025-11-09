# PC OpenCV Debug and Simulation Platform

This module provides an offline OpenCV-based simulation environment for developing and debugging the image processing algorithm used in the AURIX Line Following Car project.

It replicates the exact same lane detection and center fitting logic as used in the MCU firmware, allowing visualization, parameter tuning, and intermediate value inspection.

---

## Dependencies

This project uses OpenCV 4.5.4 for image processing.

To reduce repository size, the required OpenCV runtime libraries are provided as a compressed file:

**opencv_world454.rar

---


## File Structure

| File / Directory | Description |
|-----------------|-------------|
| **imageDeal/main.cpp**        | Entry point, starts image sequence simulation and processing |
| **imageDeal/imageDeal.cpp**   | Core image processing module integrating preprocessing and lane detection |
| **imageDeal/imagePreDeal.cpp**| Preprocessing functions (noise filtering, ROI selection, thresholding). Slightly different from MCU but produces equivalent results |
| **imageDeal/Track.cpp**       | Portable tracking algorithm, identical to MCU code for lane fitting and deviation calculation |
| **imageDeal/ui.cpp**          | Custom visualization interface for debugging and displaying intermediate variables |
| **image/**                    | Input image sequence for testing (0.bmp, 1.bmp, ...) |
| **opencv_world454.rar**       | Compressed OpenCV 4.5.4 runtime library. Extract in current folder before running |
| **x64/Debug/**                | Debug build output files |
| **OpenCVProjectTemplate.sln / .vcxproj** | Visual Studio project and solution files |
| **README.md**                 | Documentation for PC simulation |

---

## Framework Reference

This simulation framework is based on the open-source project:  
https://github.com/w1oves/imageDeal

The code structure has been extended and optimized to fully replicate the embedded environment of the AURIX MCU for debugging and testing purposes.

---

## Input Data

Place the image sequence to be processed in the `/image/` folder at the root of the project.  
Files should be named in numerical order, for example:

image/
├── 0.bmp
├── 1.bmp
├── 2.bmp
└── ...


---

## How to Run

1. **Extract `opencv_world454.rar`** in the current folder (`PC_OpenCV/`).  
2. run `OpenCVProjectTemplate.sln` (no additional dependencies are required).  
3. Run the project using the local Windows debugger (Start Debugging / F5) — no additional dependencies are required.
4. The program automatically loads the images from the `/image/` folder.  
5. Each frame will go through:
   - Preprocessing (grayscale conversion, filtering, binarization)
   - Lane edge scanning
   - Center line fitting
   - Visualization and debugging interface

The result simulates how the MCU performs onboard image processing during real-time driving.

---

## Features

- Identical lane detection and center fitting algorithm as MCU firmware  
- Quick testing of new thresholding or fitting strategies  
- Real-time visualization of intermediate variables (via `ui.cpp`)  
- No external OpenCV dependency required — already integrated in the project  

---

## Notes

- Designed for debugging algorithm performance offline before MCU deployment.  
- The difference between PC and MCU implementations lies only in data types and preprocessing routines.  
- For portability, ensure that all algorithm functions in `Track.cpp` remain platform-independent.

---
