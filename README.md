# Autonomous-ModelCar-AURIX-TC364

A high-speed autonomous model car project based on the Infineon AURIX TC364 microcontroller.  
The system performs real-time lane detection, path fitting, and closed-loop PID control using onboard image processing, achieving stable tracking at speeds up to **3.3 m/s**.

---

## Project Overview

This project was developed for a vision-based autonomous car competition.  
The model car (approximately 25 cm in length) drives on a track resembling real road lanes.  
It detects both left and right lane boundaries using a custom image processing algorithm and performs high-speed closed-loop control completely offline on the MCU.

The same algorithm can also be simulated on a PC using OpenCV for debugging and visualization.

---

## Demonstration

**1. Real Vehicle Running on Track**

<p align="center">
  <img src="Docs/real_car.gif" width="600">
</p>

**2. PC Simulation Visualization**

<p align="center">
  <img src="Docs/pc_simulation.gif" width="600">
</p>


---

## System Architecture

