# Project LUNA — Low-Cost Urban Runoff Network for Assessment

Paired turbidity sensors that measure whether a bioretention cell is actually cleaning stormwater.

CAD, firmware, images, and bill of materials for a two-node system: one sensor at the inlet, one at the outlet. Parts cost for the sensing pair is about $51.

---

## Background

### What bioretention is

A bioretention cell is a planted basin — often called a rain garden — built into a street, parking lot, or development to treat polluted runoff before it reaches a creek or bay. Water enters from the curb, soaks down through engineered soil and plant roots, and leaves through a perforated underdrain pipe at the bottom. The soil and root layer strips out sediment, metals, and other pollutants along the way.

They are not decorative. Under the San Francisco Bay Municipal Regional Stormwater Permit (MRP), new and redeveloped projects are required to build them. Cities across the Bay Area now maintain thousands of these facilities as regulated infrastructure.

### The problem

Cities are required to build bioretention, but almost never required to measure whether it works.

The permit mandates periodic maintenance inspections — a visual check that the plants are alive, the inlet isn't blocked, and the basin drains. It does not require anyone to measure the water quality going in versus coming out. So a cell can silently stop treating runoff — clogged media, short-circuited flow, failed underdrain — while continuing to pass every inspection it gets.

The reason nobody measures is cost. A professional water-quality monitoring station runs roughly $5,000 to $10,000 per site once you add the sensor, logger, modem, power, and enclosure. A single commercial turbidity sonde alone lists around $2,150. At that price a city can instrument a handful of showcase sites, not the hundreds it actually manages.

### Why turbidity

Turbidity — how much suspended sediment is in the water, measured in NTU — is the standard proxy for stormwater treatment performance. Sediment carries a large share of the pollutant load, it responds immediately during a storm, and it can be measured optically without reagents or sample collection. If the water leaving a cell is consistently clearer than the water entering it, the cell is working.

---

## What LUNA does

LUNA is a **paired** measurement. One sensor node sits at the inlet and reads the runoff coming off the street. A second node sits at the outlet, in the underdrain cleanout, and reads the water leaving the cell. The difference between those two readings is the thing nobody has been measuring: treatment performance, storm by storm.

Both nodes are built around the same core:

- **ESP32 microcontroller** with a LoRa radio for wireless reporting
- **850 nm infrared emitter and phototransistor** in a 3D-printed optical housing
- **Servo-driven wiper** that clears the optical window between readings, so sediment and biofilm don't slowly corrupt the signal
- **LiPo cell** sized for months of unattended operation
- Deployment through a standard 4-inch municipal cleanout — no digging, no construction

Because the hardware is cheap, the unit of deployment changes. Instead of instrumenting one site well, a city can instrument its whole portfolio.

---

## Repository contents

```
Inlet Sensor/          CAD (STL) — v1 through v3, including wiper and magnet-coupled assemblies
Effluent Sensor/       CAD (STL) — v1 through v3, plus electronics holder and battery mount
SensorCode/            ESP32 firmware
Project_LUNA_Images/   Build photos, bench tests, and field conditions
BOM.txt                Bill of materials for a two-node build
```

### Firmware

**`Calibration_FinalRunCode.ino`** — the main sensing and calibration routine.

- Reads the phototransistor with the LED on and off, and uses the **difference** as the signal. This cancels ambient light, which matters in a storm drain where lighting is uncontrolled.
- Averages 1000 fast ADC samples per reading to suppress electrical noise.
- Takes 6 macro-readings per calibration standard and applies an **IQR outlier filter** to discard bubbles and transient junk before averaging.
- Fits both a **linear** and a **quadratic** model to the calibration points, computes R² for each, and automatically selects whichever fits better. Turbidity response is close to linear at low NTU and curves at higher concentrations, so the model shouldn't be assumed in advance.
- Clamps negative results to zero, since negative turbidity is physically meaningless.

**`OutletServoTest.ino`** — positional control for the DS-M005 wiper servo, with pulse widths matched to the datasheet for a full 0–180° sweep.

### CAD

Three design generations for each node. The inlet housing evolved into a multi-part assembly with a separate wiper and a magnet-coupled bottom, letting the optical core stay sealed while the wiper moves. The effluent side includes a rope-deployed sensing head plus a separate electronics holder that keeps the board and battery above the water line.

---

## Current status

**Both sensors are built and validated.**

Calibration was done against formazin NTU standards, then against a commercial reference turbidimeter in a flow loop using sediment collected from the actual pilot sites — moving water, leaf debris, bubbles, and flow surges, not still beakers.

| | Inlet node | Effluent node |
|---|---|---|
| Average error vs. reference | ±2.3 NTU | ±0.85 NTU |
| Validated range | 0–200 NTU | low-NTU treated water |

The inlet unit tracks the reference across the full storm range. The effluent unit resolves the clean end, where the differences that matter are small.

Fifteen-plus hardware iterations so far. Known open items: repeatability is currently limited by breadboard-stage wiring rather than by the optics, and long-term fouling behavior in the field is exactly what the pilots are meant to characterize.

A technical paper on the system is in review ahead of submission to the IEEE Sensors Conference.

---

## Field partners

LUNA is being developed with the people who do this work professionally, and deployed at sites they already monitor.

**San José — Integral Consulting.** Integral runs water-quality monitoring for municipal stormwater programs. They walked the Alviso site with us alongside City stormwater-compliance staff, and shaped the design around what a real site demands: rushing water during events, sediment loading, wet-dry cycling, limited clearance, and access for maintenance crews.

**Oakland — Applied Marine Sciences.** AMS is an environmental sciences firm working on Bay Area stormwater. Paul Salop hosted us at a Port-area site and wrote a letter of support for the project, noting the need for continuous turbidity monitoring that can function through alternating wet and dry conditions — which is precisely the gap LUNA is built for.

Paired deployments at both sites are scheduled for September 2026, ahead of the winter storm season. Partners install and retrieve; LUNA evaluates. Three additional Bay Area cities have expressed interest in the finished system.

---

## Team

Three high school students in Fremont, California.

- **Lalit Batchu** — business and partnerships
- **Ishaan Ashok** — embedded systems and software
- **Saket Sandru** — mechanical design and prototyping

Selected as a Top 15 finalist for the 2026 Youth Business Venture Competition at Stanford.

---

## Why it matters

Cities have spent enormous sums building green infrastructure to protect their waterways, and have almost no way to know which of it still works. LUNA exists to make that answer cheap enough to ask everywhere — for the price of monitoring one site the conventional way, a city could watch its entire portfolio.
