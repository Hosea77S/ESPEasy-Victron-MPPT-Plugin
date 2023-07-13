# ESPEasy-Victron-MPPT-Plugin
This is a Plugin for the esspresif ESPEasy firmware that interptrets VE-direct Serial data from the Victron SmartSolar MPPT.

[1. Simple Review](#1-Review)

[2. Technical Review](#2-VEDirect-protocol)

[3. How to Add to ESPEasy](#3-How-to-Add-to-ESPEasy)


# 1 Review

An MPPT Charge Controller is a specialized battery charger designed to Maximise the yield from a solar panel array.
Victron MPPT Charge Controllers, feature a VE.direct serial communication interface, allowing access to detailed information about the products operation.

This plugin interptrets serial data from the MPPT and makes the data available to the user interface and other plugins in ESPEasy. 

The structure and functionality of this plugin is based off of the already existing plugin, P087 [Serial Proxy](https://github.com/letscontrolit/ESPEasy/blob/mega/src/_P087_SerialProxy.ino).


## 1.1 ESPEasy Web interface

- Setting up the Task Settings:

After adding the plugin, in the **Task settings**, is where you set up the plugin's name and serial parameters.
Select an appropriate name, baudrate (where 19200mbps is the typicalbaudrate for Victron VE.Direct protocol), apprpriate GPIO pins and uart configurations (the **Serial Config**). Note, the plugin must be configured to use a hardware Serial port of your chosen ESP module. Thus a particular ESP8266 might not work since they typically only one serial port to receive serial data which is already used by the firmware.

Then enable the plugin to load the other settings. An example of **task Settings** can be viewed below
![alt text](https://github.com/Hosea77S/ESPEasy-Victron-MPPT-Plugin/blob/main/Images/Devices_Page.png)

- Device Settings Field selection:

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

## 1.2 Accessing Fields Example: Showing on OLED

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

![alt text](https://github.com/Hosea77S/ESPEasy-Victron-MPPT-Plugin/blob/main/Images/OLED_setup.png)

# 2 VEDirect protocol

VE.Direct is a UART based communication protocol developed by Victron Energy themselves. It's featured in many of their products, including the MPPT Controller. The VE.Direct interface includes two modes: Text-mode and the HEX-mode. This plugin assumes text-mode.

Serial port configuration is typically setup as shown:

| Serial parameter 	| Value 	|
|------------------	|-------	|
| Baudrate         	| 19200 	|
| Data bits        	| 8     	|
| Parity           	| None  	|
| Stop bits        	| 1     	|
| Flow Control     	| None  	|

Devices typically transmit blocks of data at 1 second intervals. Withing that block are a set of fields. 
Each field is sent using the following format:

```
<Newline><Field-Label><Tab><Field-Value>
```

| Identifier  	| Meaning                                                                                                                        	|
|-------------	|--------------------------------------------------------------------------------------------------------------------------------	|
| Newline     	| `\r\n` - Cariage return followed by a newline                                                                                  	|
| Field-Label 	| An arbitrary length label that identifies the field                                                                            	|
| Tab         	| `\t` - Horrizontal Tab                                                                                                         	|
| Filed-Value 	| The ASCII formatted value of this field. The number of characters  transmitted depends on the magnitude and sign of the value. 	|


# 3 How to add to ESPEasy 

To add the plugin to your own custom build of ESPEasy depends on how many plugins exist in your build. This affects how you name your plugin. In the build I tested on, P153 was the next available plugin ID I could use. If There existed a P153 already, then i would rename it to P154. 

Once you've identified the number of plugins you could use, its time to add the files to the relevant foders.

Add the `_P153_VictronMPPT.ino` file in `/ESPEasy/src/` (assuming you use windows). Then add `P153_data_struct.h` and `P153_data_struct.cpp` in `ESPEasy\src\src\PluginStructs\`. Don't forget to rename the P153 part of the file names to the appropriate value as expressed above.

Next, within the files themselves, you can simply find-and-replace each instance of "P153" to the apprpriate value. 

Finally, in `ESPEasy\tools\pio\pre_custom_esp82xx.py` and/or `ESPEasy\tools\pio\pre_custom_esp32.py`, add the line 

```python
"-DUSES_P153",
```
 within the custom defines list.

# Informal Acknowledgements 

1. ESPEasy development team
2. My supervisor
3. Victron Energy documentation
