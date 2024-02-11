This is an attempt to create a functional GPS lag for small sailboats. The hardware is based on ESP32, it currently uses ESP32-DevKit-C (a copy from Aliexpress), a Neo6M GPS chip (not accurate enought, will be changed) and 4 inch Waveshare eInc display.

## Hardware

* ESP32 microcontroller - currently a ESP32-DevKit-C copy from Aliexpress. That's the brain of the device.
* Waveshare 4.2 inch eInc display - low power consumption, well visible in bright sunlight. Partial refresh is fast enought for a small sailboat that wont go faster that 8 - 10 knots. And because I had one laying around.
* Neo6M GPS chip - that one will be replaced with QUESCAN G10A-F30 GNSS module that's based on the UBX-M10050-KB chip. The Neo6M is GPS-only and not very precise, in reality the reported speed fluctuates with more than a knot.
* LiIon battery module based on TP4056 - because I had it available.
* Some resistors and button to providepower on/off - actually deep sleep and wake up untill a more advanced schematics are implemented.

At the moment all this is put on a experiment PCB. A better prototype PCB design will eventually be published in a separate repository.

## Software

This repository holds all the software for the device. Based on Platform.IO, and uses some functions from ESP-IDF by Esspressif. [x99/NMEAParser](https://github.com/x99/NMEAParser) is used for parsing NMEA sentences and [zinggjm/GxEPD2](https://github.com/zinggjm/GxEPD2) is used for drawing things on the eInc display.

We're hitting a limitation of the [Adafruit_GFX](https://github.com/adafruit/Adafruit-GFX-Library) library that provides graphics support for GxEPD2. Because it uses 8 bits for various font dimensions, and this limits the maximum font size that can be generated. My experiments got me 92px maximum font size, but that might depend on actual font uses. The same 8 bits limitation also limits fonts to ASCII caracters only, which makes it impossible to use tabular numerals - fixed width font doesn't work here.
