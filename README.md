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

- Setting up the Task Settings:

After adding the plugin, in the **Task settings**, is where you set up the plugin's name and serial parameters.
Select an appropriate name, baudrate (where 19200mbps is the typicalbaudrate for Victron VE.Direct protocol), apprpriate GPIO pins and uart configurations (the **Serial Config**).
Then enable the plugin to load the other settings. An example of **task Settings** can be viewed below
![alt text](https://github.com/Hosea77S/ESPEasy-Victron-MPPT-Plugin/blob/main/Images/Devices_Page.png)

- The Devices page:



- Device Settings Feild selection:

In the Victron VE.FDirect documentation, they describe the serial information is sent as one long string that can be broken down into a list of fields.
Each field consists of a **Label** and a **Value**. A short list of fields sent by the MPPT can be seen below:

| Label 	| Units of Value 	| Description                    	|
|-------	|----------------	|--------------------------------	|
| V     	| mV             	| Main Channel 1 battery voltage 	|
| I     	| mA             	| Main Channel 1 battery current 	|
| PPV   	| W              	| Panel Power                    	|
| LOAD  	| No unit        	| load output state (ON/OFF)     	|

Within the device settings, you can select which fields you would like to collect from the serial data as shown in the image below. 
Currently, you can select up to four fields. In addition you must tell ESPEasy how many fields you'd like to collect as shown in the **Number of Labels** Web form.

![alt text](https://github.com/Hosea77S/ESPEasy-Victron-MPPT-Plugin/blob/main/Images/Serial_parameters_2.png)

- Devices Page:

Once saved, on the devices page of ESPEasy, You should see something similar to the image below.
The `Success` parameter tells you how many complete strings were received from the MPPT. The `Error` parameter tells you how many incomplete strings were read or if the string was abnormally long. Mysterious things happen with this MPPT.
The `Length Last` partameter tells you how many fields were collected in each string. Finally, the `Value` parameter tells you the fields you values requested in the devices page. As far as I know, ESPEasy doesnt show string values in this page yet thus you will not see anything here.
The actual Values can be seen with the Device Settings if the plugin was setup correctly.

![alt text](https://github.com/Hosea77S/ESPEasy-Victron-MPPT-Plugin/blob/main/Images/Devices_Page.png)

In the **Device settings**, The Values (also known as the **Current Sentence**) you've requested as well as other values is shown  under the **Statistics** subheading.
As shown below, the user requested 4 values; the battery current (I), the Panel power (PPV), the load state (LOAD) and some value called the checksum (Checksum).
The **Current Sentence** shows that the battery is discharging at -210mA, the Panel Power is at 0W, the Load is ON, and the value of the checksum. The next value shows the number of fields the user requested, which is 4. The `1111`, indicates whether the value requested was found or not. Each `one`'s position corresponds to the values prior. 
If the value is `0` instead of a `one` it either means the value does not exist or an error occured in retreiving that value. The final value in **Current Sentence** tells you how many fields were collected from the MPPT.

![alt text](https://github.com/Hosea77S/ESPEasy-Victron-MPPT-Plugin/blob/main/Images/Field_selection.png) 

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
