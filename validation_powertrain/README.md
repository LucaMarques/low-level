# Validation powertrain

## How to use the ultrasonic

When you first turn on all the devices, the senders will start as turned off and will need a command from the receiver to start up. Once you have all plugged in, you can calibrate the minimal distance or turn on the devices with the standard distance by typing in the receiver terminal window "on" with no arguments. 

If you want to change the default calibration, use the commando "set xx", where the xx must be the new value for the distance. When you change this value, all the device's data will be changed, such as the time sensor as the velocity.

Now that you have the sensors turned on and calibrated, use the command "new" to create a set of passages. The default number is five and for now, the only way to change that is by code. 

In case of an invalid time, use the command "dtl", extends for delete last one, and make another passage.

Bellow, we have all the commands that you can use:

| command |        summary        |
|:-------:|:---------------------:|
|   new   |      new passage      |
|   dtl   |    delete last one    |
|    on   |   turn on the boards  |
|   off   |  turn off the boards  |
|  set xx | set calibration to xx |