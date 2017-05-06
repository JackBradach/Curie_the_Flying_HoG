Curie the HoG
#############

Overview
********

.. image:: http://i.imgur.com/1G7fxBU.jpg 

One time, OK, see, what happened is one time bgordon found all these Intel TinyTiles
hidden behind a dumpster full of Clearwire modems and brought them over and said,
"Hey, wouldn't these make really good gesture-input devices?" So we started waving
them around and making gestures, but nothing happened because thats not how anything
works so instead I have to write a bunch of firmware to make it do what we want. ‘K, bye.

This sets up the Bluetooth LE services needed to do HID over GATT; that's the magic
that lets your keyboard/mouse/gamepad input devices function without drivers.  We'll
also be tying the IMU output to the HoG so that it can be used as a gesture mouse.
There's a lot of fanciful acceleration hardware in the Curie that should let us do
gesture recognition inside the ARC processor.

We'll be using all three of the cores in the Curie (each with their own architecture!),
so this is going to be fun.

Requirements
************

* 1 x 32-bit x86 ISA-compatible core
* 1 x 32-bit ARC EM4 DSP
* 1 x Nordic NRF51822 Bluetooth LE controller.
* Glue?

-or-

* 1 x Intel (R) Curie(tm) Module
    * Specifically a TinyTile/Arduino 101

Building and Running
********************

Being this *is* 2017 and we *are* on the bad timeline, you'll probably not be
too surprised to read that this doesn't work yet.  The Dockerfile does, however
create a usable Zephyr development environment.

$ make BOARD=arduino_101 ARCH=x86

$ dfu-util -a x86_app -D outdir/arduino_101/zephyr.bin -R

