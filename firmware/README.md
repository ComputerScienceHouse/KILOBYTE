# Firmware
The firmware for our two Atmega328p based display unit and control unit are located here.

# display.ino
This is the firmware for the display unit. Mostly handles I/O


# control.ino
This is the firmware for the control unit. This communicates with both ESCs and manages acceleration calculations.

NOT IMPLEMENTED: The control unit will soon also handle tachometer calculations.

# Notes
Both units communicate to eachother via I2C. The display unit is the parent device, and the control unit is the child with an address of 0x08.
