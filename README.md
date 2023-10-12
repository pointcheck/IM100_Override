# Paslode IM100C vendor lock-in override 

This repository contains PCB schematics and firmware for the device to override vendor lock-in for Paslode IM100C gas actuated nail hammer.

## Disclaimer

This small one weekend project is provided here solely for educational purpose to demostrate how weak and futile vendor lock-ins can be at places it should not be present in essence and how easy it is to overcome it. Note, that overcomming (removing) vendor lock-in is illigal in many countries even for personal, one can be sued or fined for doing so. You've been warned!

## Details

Paslode IM100C is a gas actuated nailing system (stapling hammer) that uses natural gas (propane/butane mixture) to fuel hammer working by internal combustion principle, i.e. by injecting a dose of fuel into combustion chamber and igniting mixture with an electrical spark, like it goes in any gasoline engine.

Paslode IM100C is very useful and handy device which can be used both by DIY and professional carpenters or wood workers. The problem is that fuel cartridges (cylindrical tanks with gas mixture) are equipped with RFID tags (NFC 13MHz standard) that store a decreasing counter of nails you can fire, usually not more than 1200 shots, thus limiting users and enforcing them to purchase "genuine" fuel cartridges. These cartridges come at a cost of course, somewhat about 100+ EURO each. Many users discovered that it is possible to refill cartridges with a gas purchased from nearby hardware store, but you still will be limited to a number of shots because of the above vendor lock-in mechanism.

One can disassemble IM100C hammer and see that vendor made significant effort to make lock hard to override. There are two MCU powered devices (PCBs) inside. One is controlling fuel injection and ignition mechanism, and the other for gas valve. It is the last one that prevents fuel to come from cartridge to the chamber if not proper (or outdated) RFID tag found. These two controllers communicate to each other using digital (I2C) interface. There are also two digital buttons that produce PWM signals on their outputs. One button is tied to shutter mechanism and signals when it is time to refill chamber with new dose of fuel, the other one is a fire trigger user pulls to make a nail shot. Both controllers (PCBs) are immersed into compound making reverse-engineering not easy to perform.

Yet, there is a way to work-around this vendor lock-in. A simple analysis shown that sparks are being produced even if hammer is missing proper fuel cartridge. So, if one can open gas valve in time (and for a proper time period) to let fuel in, the hammer will work. Thus, to overcome vendor lock-in a simple MCU device can be used to control gas valve directly bypassing vendor's mechanism. This MCU device should be connected directly to shutter button and to the valve, making valve to open for a 15ms when shutter button is pressed. It is not necessary to disconnect shutter button and valve from original electronics, one can keep them running in parallel.

## Safty instructions

If you decide to implement this vendor lock-in override device, please make sure you disassemble and assemble back your hammer with a) bettery and fuel cartridge removed, b) with gas chamber emptied priorly. Not doing so may lead to unexpected gas ignition and shot causing serious injury! Also, test device with hammer fully assembled and all the nuts are tightened! Please, be precautious!

