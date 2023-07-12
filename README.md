# ESPEasy-Victron-MPPT-Plugin
This is a Plugin for the esspresif ESPEasy firmware that interptrets VE-direct Serial data from the Victron SmartSolar MPPT.

[1 Simple Review](#1-Simple-Review)

[2 Technical Review](#2-Technical-Review)

[3 How to Add to ESPEasy](#3-How-to-Add-to-ESPEasy)


# 1 Simple Review

An MPPT Charge Controller is a specialized battery charger designed to Maximise the yield from a solar panel array.
Victron MPPT Charge Controllers, feature a VE.direct serial communication interface, allowing access to detailed information about the products operation.

This plugin interptrets serial data from the MPPT and makes the data available to the user interface and other plugins in ESPEasy. 

## ESPeasy Web interface

- Explain serial page setup
- Show devices page
- show feild selection and what a field is
- show end of configuration setup

## Accessing Fields Example: Showing on OLED

- From example string shown, show rules setup with Dummy Device
- Show logs 
- Shjow OLED setup

# 2 Technical Review

## VEDirect protocol

- explain baudrate, modes
- explain fields you can get from mppt 

## ESPEasy Plugin Structure

- Explain similarity to serial proxy plugin

## Plugin data structure

- explain what data struct does


# 3 How to add to ESPEasy 

- explain hopw to add plugin to list of plugins in a custom build of espeasy
