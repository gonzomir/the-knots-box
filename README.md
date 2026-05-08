# The Knots Box

An functional battery-powered GPS-based speedometer for small sailboats.

## Hardware

* Based on ESP32 microcontroller.
* [Waveshare ESP32-S3 4.2inch RLCD Development Board](https://www.waveshare.com/esp32-s3-rlcd-4.2.htm?sku=33507) ([Aliexpress](https://www.aliexpress.com/item/1005010742073289.html)). This module has everything needed for the device in a single package and the perfect display for the task.
* [QUESCAN G10A-F30 GNSS module](https://www.aliexpress.com/item/1005005211498523.html) based on the UBX-M10050-KB chip - supports multiple GNSS systems and has reasonable speed accuracy.
* A standard 18650 LiIon battery - reporpose one from old laptop batery pack or get new.
* A [USB type C 4-wire connector](https://www.aliexpress.com/item/1005004397896079.html) for charging the battery.
* [3D printed enclosure](https://github.com/gonzomir/the-knots-box-case) that hosts the above items.

## Software

This repository holds all the software for the device. Based on Platform.IO, and uses some functions from ESP-IDF by Esspressif. A fork of [x99/NMEAParser](https://github.com/x99/NMEAParser) is used for parsing NMEA sentences and the display drivers are copied from example code provided by display module manufacturer.
