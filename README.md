This is an embedded temperature control system that I designed in C for a PIC18 microcontroller. (For a Uni. Proj.)

The system heats a thermal mass using a heater until it reaches a target temperature of 50°C. It then initiates an active cooling phase with a fan for a duration of five minutes.
Throughout the cooling phase, the system samples the temperature once per second and transmits the time-temperature data via UART serial communication for external analysis. The program halts after a single heating-cooling trial.


Workflow:

Initialization:

  Sets internal oscillator to 16MHz

  Configures ADC for temperature reading

  Initializes UART for serial communication

Heating Phase:

  Turns on heater

  Continuously monitors temperature until it reaches 50°C

  Sends "Heating started..." message

Cooling Phase:

  Turns off heater and turns on fan

  For 5 minutes (300 seconds):

  -  Reads temperature every second

 -   Sends time-temperature data via UART (e.g., "0,50.00")

Sends completion message and halts until reset

Isaac Krapp
