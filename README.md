# Project LUNA — Low-Cost Urban Runoff Network for Assessment

Paired turbidity sensors that measure whether a bioretention cell is actually cleaning stormwater.

CAD, firmware, and build images for a two-node system: one sensor at the inlet, one at the outlet.

---

## Background

### What bioretention is

A bioretention cell is a planted basin — often called a rain garden — built into a street, parking lot, or development to treat polluted runoff before it reaches a creek or bay. Water enters from the curb, soaks down through engineered soil and plant roots, and leaves through a perforated underdrain pipe at the bottom. The soil and root layer strips out sediment, metals, and other pollutants along the way.

They are not decorative. Under the San Francisco Bay Municipal Regional Stormwater Permit (MRP), new and redeveloped projects are required to build them. Cities across the Bay Area now maintain thousands of these facilities as regulated infrastructure.

### The problem

Cities are required to build bioretention, but almost never required to measure whether it works.

The permit mandates periodic maintenance inspections — a visual check that the plants are alive, the inlet isn't blocked, and the basin drains. It does not require anyone to measure the water quality going in versus coming out. So a cell can silently stop treating runoff — clogged media, short-circuited flow, failed underdrain — while continuing to pass every inspection it gets.

The reason nobody measures is cost. A professional water-quality monitoring station runs roughly $5,000 to $10,000 per site once you add the sensor, logger, modem, power, and enclosure. At that price a city can instrument a handful of showcase sites, not the hundreds it actually manages.

### Why turbidity

Turbidity — how much suspended sediment is in the water, measured in NTU — is the standard proxy for stormwater treatment performance. Sediment carries a large share of the pollutant load, it responds immediately during a storm, and it can be measured optically without reagents or sample collection. If the water leaving a cell is consistently clearer than the water entering it, the cell is working.

---

## What Luna does

Luna is a **paired turbidity-monitoring system** for bioretention facilities. One sensor sits at the inlet and measures the stormwater entering the facility, while another measures the water leaving through the underdrain. Comparing those two readings can help show how the facility is behaving during a storm and flag sites that may need closer inspection.

The system uses low-cost optical sensing rather than an off-the-shelf commercial turbidity probe.

The basic sensing setup uses:

- **850 nm infrared emitter**
- **Phototransistor**
- **ESP32 microcontroller**
- **Local data logging**
- **Custom 3D-printed optical housing**
- **Battery-powered electronics**
- **Waterproof enclosure and mounting**

The outlet sensor is our earlier proof of concept. The main thing we are currently building is the **higher-range inlet sensor**, which has to deal with dirtier water, more sediment, debris, bubbles, and buildup on the optical windows.

---

## Current inlet design

The inlet sensor has gone through several CAD versions as we have tried to make it smaller, easier to seal, and practical for an actual bioretention site.

One of the main features we are experimenting with is a **magnetically coupled wiper** for the optical window.

Sediment and biofilm can build up on the sensing surface over time and slowly change the reading. Our current idea uses a servo on the protected side of the enclosure to move a magnet, which then moves another magnet attached to a wiper on the wet side.

This lets us move the wiper without putting the servo directly in the water or running a rotating shaft through the enclosure.

So far we have:

- Designed multiple inlet enclosure versions
- Tested different layouts for the sensing chamber and wiper
- Added removable/threaded sections for cleaning and repairs
- Redesigned the enclosure after discussing real installation constraints
- Printed the first physical inlet prototype
- Used the physical print to identify problems with size, wiring, strain relief, and component placement

The current inlet prototype is still an early physical version. It is **not waterproof or field-ready yet**.

---

## What still needs to be tested

The next stage is turning the current CAD and printed prototype into a complete working inlet sensor.

We still need to:

- Assemble the inlet sensing electronics
- Determine the best amplifier gain for higher-turbidity water
- Test the magnetic wiper under sediment exposure
- Improve wire routing and strain relief
- Finalize waterproofing and sealing
- Run immersion tests
- Run repeated wet-dry cycles
- Test in moving, sediment-filled water
- Test window fouling and cleaning
- Integrate the inlet and outlet logging
- Prepare the system for pilot deployment

These are the main things the Macondo hardware funding would allow us to complete.

---

## Repository contents

```text
Inlet Sensor/          Current inlet CAD and design iterations
Effluent Sensor/       Earlier proof-of-concept CAD used as reference
SensorCode/            ESP32 sensing and test firmware
SensorWiringDiagrams/  Electronics and wiring documentation
Project_LUNA_Images/   CAD screenshots, prototypes, testing, and site photos
BOM.csv                Parts requested for the current build
```
## Field partners

LUNA is being developed with the people who do this work professionally, and deployed at sites they already monitor.

**San José — Integral Consulting.** Integral runs water-quality monitoring for municipal stormwater programs. They walked the Alviso site with us alongside City stormwater-compliance staff, and shaped the design around what a real site demands: rushing water during events, sediment loading, wet-dry cycling, limited clearance, and access for maintenance crews.

**Oakland — Applied Marine Sciences.** AMS is an environmental sciences firm working on Bay Area stormwater. Paul Salop hosted us at a Port-area site and wrote a letter of support for the project, noting the need for continuous turbidity monitoring that can function through alternating wet and dry conditions — which is precisely the gap LUNA is built for.

Paired deployments at both sites are scheduled for September 2026, ahead of the winter storm season. Partners install and retrieve; LUNA evaluates.

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
