# ForestGuard

A low-cost forest monitoring system that detects chainsaw sounds in real time
using a XIAO ESP32S3 and a machine learning model trained on Edge Impulse.
Multiple units can be deployed across trees and connected in a local mesh
network — whenever a chainsaw is detected, an alert with the unit's location
is sent to the owner.

## How it works

A microphone fitted to the XIAO ESP32 continuously captures audio. The on-device
ML model (trained on chainsaw audio via Edge Impulse) classifies the sound
locally. On detection, the unit triggers an alert over the local network with
its GPS coordinates.

## Stack

| Part | Details |
|---|---|
| Hardware | Seeed XIAO ESP32 + MEMS Mic |
| ML Platform | Edge Impulse |
| Firmware | Arduino / ESP-IDF |
| Networking | ESP-NOW / MQTT (local mesh) |
| Alert System | Location-tagged notification |

## Getting Started

1. Flash the firmware from `/firmware` onto the XIAO ESP32
2. Drop the exported Edge Impulse model into `/model`
3. Configure Wi-Fi/network credentials in `config.h`
4. Deploy units on trees and power them up

## Use Case

Aimed at detecting illegal logging and poaching activity in forests. Designed
to be low-power, weatherproof, and deployable in areas with no internet — the
local mesh handles communication between units and a base node.

## Assets

![Setup](./assets/Working Demo.mp4)