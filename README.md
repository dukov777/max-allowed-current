# calculate-tank-caps
Tank capacitor bank calculation software in C++.

## Requirements

[Software Requirement Specification](SoftwareRequirementSpecification.md)

## Design decisions

The code is organized with 2 intends: 
* Performance for the calculation part
* Simplicity for serving utility code - 
   * Input - command line reading, JSON file parsing, 
   * Output - dumping output directly to the console.
* The code does not impose various assertions and guards in calculations and relies on functional programming.

## Design
The capacitor circuit is assembled, and calculations are done in the `TankCalculator` class. The actual composition is made in the method `compose_capacitors_tank` using the Composition pattern. The capacitor calculation itself is executed in `calculate_capacitors_tank`. The results are directly dragged into the console by empowering decorators for each capacitor/group.  

The display of the calculations result and range exceedings is printed to the console. This is done by Decorating each capacitor/group and thus letting the decorator decide how to display results.    
This decision is made to keep the family of Capacitor classes clean and with a single responsibility: calculation, while result display and validation are delegated to others.

## Install
1. Clone the repo
2. Get the submodules

   `git submodule update`

4. Build the project 
```bash
  mkdir build
  cd build
  cmake ..
  make
```
4. Run the app

   `./calculate-tank-caps -i 100000 -f 10000 -group1 23uF_500V 1uF_1000V  -group2 1uF_1000V -spec ../capacitors-spec.json` 

the result is:
```bash
Capacitor: 23uF_500V, Current: 3075, Voltage: 2128, Power: 6542022
Capacitor: 23uF_500V, Current: 3075, Voltage: 2128, Power: 6542022
Capacitor: parallel1, Current: 6150, Voltage: 2128, Power: 13084043
Warning: Overcurrent condition on parallel1. The current is 6150A, which exceeds the maximum current of 2000A!
Warning: Overpower condition on parallel1. The power is 13084043W, which exceeds the maximum power of 1000000W!
Capacitor: 1uF_1000V, Current: 6150, Voltage: 97872, Power: 601866007
Capacitor: parallel2, Current: 6150, Voltage: 97872, Power: 601866007
Warning: Overcurrent condition on parallel2. The current is 6150A, which exceeds the maximum current of 500A!
Warning: Overpower condition on parallel2. The power is 601866007W, which exceeds the maximum power of 500000W!
Capacitor: serial, Current: 6150, Voltage: 100000, Power: 614950050
Allowed current: 62.8319
```