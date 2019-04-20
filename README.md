# Razer Atrox XBO WinUSB Test

## What is this?
A test application that interfaces with the Razer Atrox XBO arcade stick controller using the 
WinUSB driver. 

## Y, tho?
See https://forums.vigem.org/topic/282/xbo-arcade-sticks-vigem-and-a-whole-ton-of-fun

## How do I use this?

1. Open https://github.com/OOPMan/RazorAtroxWinUSBTest/releases/tag/v1.0 in your browser
2. Download `RazerAtroxWinUSBTest.exe` and `Razer.Atrox.WinUSB.Drivers.zip`
3. Extract `Razer.Atrox.WinUSB.Drivers.zip` to a suitable location on your system
4. Navigate to the `Razer Atrox WinUSB Drivers` folder extracted from `Razer.Atrox.WinUSB.Drivers.zip`
5. Right-click on the `Razer_Atrox_Arcade_Stick.inf` file and click *Install*
6. Once installation of the WinUSB driver is complete, connect your Razer Atrox XBO
7. Run `RazerAtroxWinUSBTest.exe`

## What can I do with this?
This application displays simple information about which buttons are pressed or released as 
well as the stick direction. Additionally, details on the data packets received from the 
Atrox are displayed.

You can use the application to confirm that all buttons on your Atrox are working correctly
and also to observe the general flow of data the device sends.

In order to exit the application, click the *X* button on the window or hold the XBox Guide button 
for 5 seconds and then release it.

## How do I uninstall the WinUSB driver?
After playing around with this you may want to revert to the standard drivers MS provides
for the Razer Atrox XBO. Doing so is really simple:

1. Press *Win* + *R* to open the Run window
2. Input `devmgmt.msc` and click *OK*
3. If the Atrox is not connected, click *View* and then click *Show hidden devices*
4. Expand the *Universal Serial Bus devices* item in the tree
5. Right-click on the *Razer Atrox Arcade Stick* item in the tree
6. Click *Uninstall device* item
7. Check the *Delete the driver software for this device* menu item
8. Click *Uninstall*
9. Unplug your Razer Atrox and then plug it in again. It will now use the standard drivers
   and appear as a *Xbox Gaming Device* under the *Xbox Peripherals* item in the tree

## Can I test this with another XBO Arcade Stick?
You can test it, but it may not work. If you do choose to try it, let me know the results.
In order to do so, you will need to modify the `Razer_Atrox_Arcade_Stick.inf` file:

1. Plug in your XBO Arcade Stick
2. Press *Win* + *R* to open the Run window
3. Input `devmgmt.msc` and click *OK*
4. Expand the *Xbox Peripherals* section
5. Right-click the *Xbox Gaming Device* under the *Xbox Peripherals* item in the tree. If you
   have multiple *Xbox Gaming Device* items listed you will need to determine which one is the
   correct one.
6. Click the *Properites* menu item
7. Click the *Details* tab
8. In the property drop-down, select the *Bus reported device description* item and confirm that
   you are looking at the correct device.
9. In the property drop-down, select the *Hardware IDs* item
10. A list of strings will be displayed. One should look like `USB\VID_1532&PID_0A00`.
   The numbers between the `VID_` and `&` are the *Vendor ID*. The numbers after the `PID_` 
   are the *Product ID*
13. Record the *Vendor ID* and *Product ID* values
12. Open the `Razer_Atrox_Arcade_Stick.inf` in an editor
13. On line 7, edit the *VID* and *PID* values to reflect those you recorded and save
14. Install the `Razer_Atrox_Arcade_Stick.inf` driver as normal and run the `RazerAtroxWinUSBTest.exe` application

The uninstall process is unchanged

## Credits

* **@Nefarious**, **@!ϻega**, **@Sylveon** and **@EvilC** from the *Nefarious Software Solutions* Discord server for 
  their assisstance and advice on this project
* `pdcurses` for the terminal GUI library
* `ZaDig` for the WinUSB driver generator
* Various StackOverFlow threads and webpages for advice on working with `WinUSB`, `pdcurses` and `Visual Studio 2017`

## Disclaimer

This software is supplied as is and I take no responsiblity for any damaged caused while using it. 