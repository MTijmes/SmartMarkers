# Smart Markers

Smart Markers are devices that are used as a tool to aid in cadastral surveying.
By taking GPS measurements, compensating for erroneous GPS data and transmitting
error-corrected data to a gateway, a network of Smart Markers is able to produce
a map on which property boundaries are displayed.

# Tools used

The development tools used during this project are listed below. A decision was
made by the project team to develop the project with platform agnostic
development in mind, meaning the code projects can be built on a wide variety of
operating systems. Windows, OS X and Linux were used during the development of
the project.

### [GNU Arm Embedded Toolchain](https://developer.arm.com/open-source/gnu-toolchain/gnu-rm)

### [STMicroelectronics ST-Link Tools](https://www.st.com/en/development-tools/st-link-v2.html)

This utility tool allows for the flashing and debugging of STM32 hardware.

[GitHub page](https://github.com/texane/stlink)

### [KPN LoRaWAN development portal](https://loradeveloper.mendixcloud.com/index.html)

To reduce costs we chose to make use of the KPN network, instead of obtaining
our own gateway. KPN provides the developer with ten LoRaWAN keys.

### [Hookbin](https://hookbin.com/)

The KPN portal is too minimalistic to display any useful packet information,
because of this reason Hookbin was used to quickly inspect the incoming data.

### [NooElec NESDR Mini2+](http://www.nooelec.com/store/sdr/sdr-receivers/nesdr/nesdr-mini-2-plus.html)

Though not a software tool, like the others in this list, this software-defined
radio was used to monitor LoRa transmissions. Using an SDR helped us narrow down
certain problems we were running into when we were expecting to see incoming
data in our application server.

# Hardware used

* STM32L073RZ Nucleo development board
* u-blox 7 GPS/GNSS Evaluation Kit (EVK-7)
* RF-LORA-868-SO (LoRa radio)

# Project team

Embedded Systems and Automation students @ Windesheim, Zwolle, the Netherlands
* Rick de Bondt - [codedwrench](https://github.com/codedwrench)
* Marthijn Tijmes - [MTijmes](https://github.com/MTijmes)
* Jerko Lenstra - [MtlSnk](https://github.com/MtlSnk)
