# Autonomous Mars Rover Prototype

Embedded firmware for an autonomous mobile robot developed as part of the **Embedded Systems Lab at FH Dortmund**, inspired by the **NASA Mars Pathfinder** architecture.

## Skills & Technologies

**Programming:** C
**RTOS:** FreeRTOS
**Microcontroller:** STM32
**Embedded:** ADC, GPIO, sensor interfacing, motor control, real-time control
**Sensors:** Analog IR proximity sensors, digital IR sensors, contact switches / microswitches
**Software Architecture:** State machines, behavior-based control, sensor-driven decision making
**Systems Engineering:** Hardware/software integration, autonomous navigation, collision recovery
**Modeling:** UML Activity Diagrams, Sequence Diagrams, State Charts
**Engineering:** Embedded debugging, low-level firmware development

---

## Project Overview

The objective was to develop an autonomous mobile robot capable of navigating an environment, avoiding obstacles, detecting a target, and recovering from collisions.

The firmware was implemented in **C on an STM32-based platform using FreeRTOS**.

The rover combined three sensing mechanisms:

* **2 analog IR proximity sensors** for short-range obstacle detection.
* **2 digital IR sensors/receivers** for target detection and target orientation.
* **2 digital contact switches / microswitches** for physical collision detection.

The different sensing mechanisms were combined to improve the rover's ability to navigate reliably in the environment.

---

## Autonomous Navigation

A behavior-based **state-machine approach** was implemented to control the rover's movement.

The firmware handled:

* Forward movement
* Left/right obstacle avoidance
* Corner detection
* Re-orientation
* Collision recovery
* Target detection
* Target orientation and approach

The main control logic was executed within a **FreeRTOS task**, coordinating sensor acquisition, navigation decisions, and motor control.

---

## Sensor Processing

### Obstacle Detection

The two analog IR sensors were connected to the STM32 ADC and continuously sampled to detect obstacles.

Depending on the relative sensor readings, the rover could determine whether to:

* Continue forward
* Turn left
* Turn right
* Reverse
* Perform additional environment checks

### Collision Detection

Two contact switches acted as physical bump sensors.

They provided a fallback mechanism for obstacles that could be missed by the IR sensors, such as walls or objects below the IR sensor detection area.

When contact was detected, the rover reversed, rotated away from the obstacle, checked the environment again, and resumed navigation.

### Target Detection

Digital IR sensors were used to detect the designated target.

The rover performed a rotational search, measuring target detection while rotating in both directions. The results were compared to determine the target's relative direction before re-orienting and moving toward it.

---

## Software Architecture

The firmware used a state-based control approach to separate different navigation behaviors.

Simplified control flow:

```text
          Sensor Acquisition
                 │
                 ▼
        ┌──────────────────┐
        │ Navigation State │
        └────────┬─────────┘
                 │
       ┌─────────┼─────────┐
       ▼         ▼         ▼
   Obstacle   Collision   Target
   Handling   Recovery   Detection
       │         │         │
       └─────────┼─────────┘
                 ▼
           Motor Control
                 │
                 ▼
              Repeat
```

The project also included system-level modeling using:

* UML Activity Diagram
* UML Sequence Diagram
* State Chart Diagram

These were used to model system behavior, state transitions, and sensor/actuator interactions.

---

## Key Engineering Experience

This project provided hands-on experience with:

* Embedded C development
* STM32 microcontrollers
* FreeRTOS task-based execution
* ADC and GPIO interfaces
* Sensor acquisition and processing
* Motor control
* State-machine design
* Autonomous navigation
* Obstacle avoidance
* Collision detection and recovery
* Target detection and orientation
* Hardware/software integration
* Embedded debugging
* UML-based system modeling

---

## Repository

This repository contains the **main application source code** developed during the laboratory project.

The STM32 board-support package, hardware abstraction libraries, and other laboratory-provided files are not included because they were provided as part of the FH Dortmund lab environment.

The repository also contains the available system-design documentation created during the project.

---
