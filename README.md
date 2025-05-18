# The Knots Box

An attempt to build a functional battery-powered GPS-based spedometer for small sailboats.

## Hardware

* Based on ESP32 microcontroller.
* [Guition ESP32-S3 4.3-inch LCD display module](https://www.aliexpress.com/item/1005006729585834.html). This module has almost everything needed for the device in a single package, and it is cheaper than other similar modules.
* [QUESCAN G10A-F30 GNSS module](https://www.aliexpress.com/item/1005005211498523.html) based on the UBX-M10050-KB chip - supports multiple GNSS systems and has reasonable speed accuracy.
* Two buttons to provide power on/off and other actions.

## Software

This repository holds all the software for the device. Based on Platform.IO, and uses some functions from ESP-IDF by Esspressif. A fork of [x99/NMEAParser](https://github.com/x99/NMEAParser) is used for parsing NMEA sentences and the display drivers are copied from example code provided by display module manufacturer.
