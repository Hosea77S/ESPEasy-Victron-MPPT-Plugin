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

According to the authors of ESPEasy, variables can be passed betwenn tasks and dummy devices. This section explains how to setup a dummy device, rules and an OLEd display to show the battery current value collected from the MPPT plugin.

Firstly, setup a dummy device in Single value mode. That single value will be used to store the battery current value using rules.

![alt text](https://github.com/Hosea77S/ESPEasy-Victron-MPPT-Plugin/blob/main/Images/Dummy_setup.png)

Next, you can enable Rules and create a Set as shown in the code snippet below. The rule instructs ESPEasy to listyen for a an event from the MPPT plugin and uses the `substring` command to select the relevant value. Then uses `async` event to send the extracted value to an event called `MPPTvalue`. In the `MPPTValue` event, The command `TaskValueSet` selects the dummy device (which in this example is device number 4), and sets its `Value` parameter to the `eventvalue` delivered by `asyncevent`.

```
On MPPT#Value* do
 asyncevent,MPPTValue={substring:0:4:%eventvalue1%}
endon

on MPPTValue do
 TaskValueSet 4,1,%eventvalue1%
endon
```

After saving the rule and rebooting ESPEasy, you should see something similar to this withing your logs. 

![alt text](https://github.com/Hosea77S/ESPEasy-Victron-MPPT-Plugin/blob/main/Images/Logs.png)

In addition, if correctly setup, you can see the Dummy devices Value being set within the devices page.

![alt text](https://github.com/Hosea77S/ESPEasy-Victron-MPPT-Plugin/blob/main/Images/DUM.png)

Once you've setup the OLED device, with the lines to display, you can select the Dummy devices value as `[DUM#Value]`. The after saving the MPPT I value will be displayed oin the screen. I know, very Complicated...

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
