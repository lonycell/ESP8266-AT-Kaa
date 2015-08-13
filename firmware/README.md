#ESP8266-AT-Kaa Firmware

This directory contains firmware for Kaa-powered AT command set for ESP8266 WiFi chip.

##Requirements

For information on how to connect ESP8266 to your PC, compile and install Xtensa-lx106 toolchain, ESP RTOS SDK, and esptool.py, please refer to this [page](http://docs.kaaproject.org/display/KAA/ESP8266).

##Building

Please follow these simple steps.

* Generate Kaa `Reduced C SDK` in [Kaa Sandbox](http://docs.kaaproject.org/display/KAA/Kaa+Sandbox).
* Put downloaded SDK into `libs/kaa` folder.

* Execute the following to build:
````bash
./build.sh build
````

###Customising build

To customze ESP8266-AT-Kaa firware build, edit `include/config.h` file. There is a number of defines which configure the firmware.

| Name | Description | Default value |
|------|-------------|---------------|
| `DEFAULT_STACK_SIZE` | FreeRTOS-specific stack size value (measured in machine words) for parser and Kaa client tasks. Should be rarely changed by user. May be changed by developer. | 512 |
| `MAX_CMD_LEN` | Maximum length of single command | 1024 |
| `CMD_ECHO` | Enable echoing of recieved bytes back to user if defined. Useful if you access firmware from virtual terminal. Should be commented out if you are going use firmware with Arduino or other client chip (see `client/` directory of this repository). | Defined |
| `CMD_KAA_PROC_MAX_DELAY` | Maximum number of ticks to wait for Kaa client procedures (like Log Upload or Profile Update) to finish. **Do not change this if you are user.** Developers may change this. | 200 |
| `CMD_KAA_LOOP_TASK_MAX_DELAY` | Maximum number of ticks to wait for command event in Kaa client loop function. **Do not change this if you are user.** Developers may change this. | 15 |
| `SHOW_SYSTEM_MESSAGES` | If defined, output Kaa client logs and system messages to UART. Can be really useful for debugging. Should be disabled if you are going use firmware with Arduino or other client chip. | Defined |

##Playing around

* To write compiled firmware to flash execute the following:
````bash
./build.sh flash
````

* You can also compile&flash with one command:
````bash
./build.sh deploy
````

After the firmware has been flashed, you should see some output from firmware. 
Specifically, note message `ESP8266-AT-Kaa ready`.
Now you can press `^C`key and use virtual console (GNU screen, minicom, or whatever else you like) to access AT command line.
If you prefer to use GNU screen, use the following command:

````bash
sudo screen /dev/ttyUSBX 115200
````
where X is either 0 or 1.
Now type the following:
````
AT
````
And press `^Enter`.
If you see `OK` reply, the firware is up and is waiting for your commands!

Here is a list of currently supported commands:

####AT Commands

|Name| Description | Example |Sample output |
|----|:-----------:|:-------:|:------------:|
| `AT`  | Check if firmware is alive | `AT` | `OK` |
| `AT+RST` | Reboot chip  | `AT+RST` | - |
| `AT+CIFSR` | Print current IP address | `AT+CIFSR` | `194.66.82.11` |
| `AT+CWJAP` | Connect to Wifi | `AT+CWJAP myWifi superPassword` | `OK` if successfull, `ERR` if failed |
| `AT+CWQAP` | Disconnect from WiFi | `AT+CWQAP` | `OK` |

####Kaa-specific commands

|Name| Description | Example |Sample output |
|----|:-----------:|:-------:|:------------:|
| `KAA+START` | Spawn Kaa client into background task | `KAA+START` | `OK` if Kaa client was successfully created, `ERR` if error occured |
| `KAA+STOP` | Deinitialize all Kaa transport channels and kill Kaa client background task | `KAA+KILL` | `OK` or `ERR` |
| **WIP** `KAA+PRO` | Update Kaa endpoint profile |`KAA+PRO { profile json according to profile schema created in Kaa Admin UI }`| `OK` or `ERR` |
| **WIP** `KAA+LOG` | Send Log Record to Kaa Server | `KAA+LOG { log record json according to log record schema created in Kaa Admin UI }` | `OK` or `ERR` |



