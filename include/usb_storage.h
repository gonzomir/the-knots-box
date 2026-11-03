#pragma once

/**
 * Set up the USB device: a serial console plus a mass-storage endpoint that
 * exposes the SD card's raw blocks to a connected PC as a removable drive.
 * Call once from setup(), after sdcard_init().
 */
void usb_storage_init();

/**
 * Whether a USB host is currently connected. Safe to call from any task,
 * but only touch LVGL from loop()'s task when reacting to a change.
 */
bool usb_storage_is_connected();
