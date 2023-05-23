# Voltage Controlled Relay

The voltage controlled relay controls a bank of relays based on the voltage of a PV battery.
The input is the battery voltage in the range [24V,31V].
This is measured via a voltage divider to turn relays on and off when various thresholds are crossed:

- [Board](#board)
- [Schematic](#schematic)
- [Voltage divider with Resistor](#voltage-divider-with-resistor)
- [Voltage shifter with zener diode](voltage-shifter-with-zener-diode)
- [State machine](#state-machine)
- [Second approach](#second-approach)
- [Third approach with heat pump](#third-approach-with-heat-pump)

## Board

I am working with the [Joy-it NODEMCU ESP32 board](https://joy-it.net/en/products/SBC-NodeMCU-ESP32):

<center>
<img src="img/2023-02-24_joyit_nodemcu_esp32_pins.png" alt="Joy-it NODEMCU ESP32 pins" title="Joy-it NODEMCU ESP32 pins" width="500"/> <!-- 1788 x 1318 pixels -->
</center>

## Schematic

Here is the schematic to hook up the voltage divider measuring the 24V battery input voltage and control the relays and the LEDs showing their state:

<center>
<img src="img/2023-02-22_relay_schematic_02.jpg" alt="Voltage controlled switch schematic" title="Voltage controlled switch schematic" width="500"/> <!-- 1788 x 1318 pixels -->
</center>

## Voltage Divider with Resistor

We need to convert the input battery voltage range [24V,31V] to fit into the ESP32 ADC input voltage range [0V,3.3V],
cf. [ESP32 ADC tutorial &ndash; read analog voltage in Arduino](https://deepbluembedded.com/esp32-adc-tutorial-read-analog-voltage-arduino/).
For simplicity, we implement a simple voltage divider using resistors.
Unfortunately, this means that the lower part of the range will never be used.
To determine the voltage divider resistors Ra + Rb resistance in Ohm:
Their resistance in parallel should come to ca. 10k,
cf. [What is the most efficient voltage divider for Arduino?](https://arduino.stackexchange.com/questions/78768/what-is-the-most-efficient-voltage-divider-for-arduino)
It should also be as high as possible to reduce the amount of current and thus the battery drain.
The [voltage divider calculator](https://ohmslawcalculator.com/voltage-divider-calculator) tells us that
30V converts to 3.289V using Ra = 81.2k = 75k + 6.2k and Rb = 10k, and the conversion forumlae back and forth are

- Vadc = Vbat * Rb/(Ra+Rb)
- Vbat = Vadc * (Ra+Rb)/Rb

The first test showed significant inaccuracy.
A better approach might be to pick two resistors for the voltage divider
that more or less fall into the correct range and then tweak the software
calculation values until the voltage reported by the Arduino matches the
real voltage applied externally.
With Ra = 82k and Rb = 10k, a constant `a2b` of ca. 0.00742 can be used to
map the ADC `int` range [0,4095] to the battery voltage range of ca. [0V,31V].

This voltage divider translates the voltage range [0V,33V] to the ADC `int` range [0,4095],
resulting in a resolution of ca. 0.8 mV per bit.

## Voltage Shifter with Zener Diode

Another [level shifting](https://itp.nyu.edu/physcomp/lessons/electronics/level-shifting/) option
uses Zener diodes to create a [voltage shifter](https://en.wikipedia.org/wiki/Zener_diode#Voltage_shifter).

### State Machine

In the first approach, we used the YA_FSM library to implement a state machine to manage the relay states and transitions
(image generated using [Finite State Machine Designer](https://www.madebyevan.com/fsm/)):

<center>
<img src="img/2023-02-23_state_machine.png" alt="State machine" title="State machine" width="500"/> <!-- 1004 x 312 pixels -->
</center>

The transitions are complicated by the fact that each state, once attained, must be retained for a certain minimum amount of time before any state change can occur.
Or, to be more precise, the state transition does not happen until the trigger persists for a certain amount of time.
The trigger itself includes a minimum timespan to cause the state change.

We used [YA_FSM](https://github.com/cotestatnt/YA_FSM)
by [Tolentino Cotesta](https://www.hackster.io/tolentinocotesta),
described in [Let's learn how to use finite state machine with Arduino](https://www.hackster.io/tolentinocotesta/let-s-learn-how-to-use-finite-state-machine-with-arduino-c524ac),
since it includes functionality to support minimum and maximum timeouts for each state.
It is also equipped with a [wokwi simulation](https://wokwi.com/projects/338248486164103762).
It would be cool to set one up for this project as well.
That might save a lot of effort implementing real-world tests.

## Second approach

Rewrote the switch using a very simple state machine implemented manually with no librasry and no timer settings.
Just switch states immediately on crossing the following thresholds for the battery voltage Ub:

- Start == OFF:
    - Ub > 25.5V &rarr; R1_ON
- R1_ON:
    - Ub < 24.9V &rarr; OFF
    - Ub > 27V &rarr; R1_AND_R2_ON
- R1_AND_R2_ON:
    - Ub < 24.9V &rarr; OFF
    - Ub < 26.5V &rarr; R1_ON

I simplified the code by implementing the state machine in a more minimal fashion and eliminating all timing considerations.
Check out simplified algorithm in [main.cpp](src/main.cpp).
I built and successfully tested the new approach that according to the schematic above with two relays connected, which is all I will need in real life:

<center>
<img src="img/2023-04-22_plan_b_2_relay.jpg" alt="Second approach" title="Second approach" width="500"/> <!-- 1004 x 312 pixels -->
</center>

It works perfectly, afaict, except that the voltage measurement is imprecise.
In the lower region, under 25V, the ADC conversion reports a too low voltage, and in the high region it is too high.
Also, I am afraid of breaking the ADC by inadvertently applying a too high input voltage, since I am just twiddling a knob by hand to control it.

I'll see whether I can fine-tune it better somehow.
For instance, I could implement a custom mapping of the ADC ranges to the corresponding real-world voltage aapplied.
I think I would like to install this soon for real-world testing and use.

## Third Approach with Heat Pump

The [hot water heat pump](https://waldrain.github.io/moniwonig#wwwp) is up and running.
It needs to run at least three hours and consume 1.5 kWh per day.
It also needs to run uninterrupted for as long as possible, at least an hour at a time, preferably two or three in a go.
Also, to ensure enough hot water is available if the sun does not shine enough during one day,
it must be able to run on grid mains power after sunset to fulfil the daily requirement.
Since the PV system and the grid mains use separate electrical residual current circuit breakers, FI-Schutzschalter,
both the phase and the neutral lines need to be switched when switching between PV and grid mains, requiring two separate relays.
The two relays can both be controlled by one single pin, though.
So, we have the following relays to switch:

- R1 &ndash; general PV power
- R2n + R2p &ndash; PV power for the heat pump
- R3n + R3p &ndash; grid mains power for the heat pump

The three relays have four states:

- OFF
- R1_ON
- R1_AND_R2_ON
- R3_ON

The relays and the heat pump time counter T can be controlled using the following state machine that needs to be restarted every day:

- Start:
    - &rarr; OFF
OFF:
    - T = 0
    - Ub > 25.5V &rarr; R1_ON
- R1_ON:
    - Ub < 24.9V &rarr; OFF
    - Ub > 26.8V &rarr; R1_AND_R2_ON, Tstart = current_time
- R1_AND_R2_ON:
    - Ub < 24.9V &rarr; OFF, T += current_time - Tstart
    - Ub < 26.5V &rarr; R1_ON, T += current_time - Tstart
- current_time > sunset:
    - if T < 3_hours &rarr; R3_ON, Tend = current_time + max( 1_hour, 3_hours - T )
    - if current_time > Tend &rarr; OFF

I introduced Tend and added the call to max to ensure that the heat pump is not switched on for too short time.

## Fourth Approach with DPDT and Timer

Ulrich suggests more rigorously ensuring the separation of the grid mains and PV electrical circuits using a DPDT relay controlled by the Arduino.
Furthermore, the grid mains can go through a programmable timer outlet switch before reaching the relay.
Then, the Arduino can run the heat pump on PV as long as possible, keep track of the running time per day, and switch the relay to grid mains for the remaining duration.
The timer can be added to ensure that the heat pump does not run around the clock if the arduino fails.
Ulrich also mentioned using [TinkerCAD](https://www.tinkercad.com/) for both electronic schematics and
3D printing projects, ald also [fritzing.org](https://fritzing.org/.

## System Time

The ESP32 supports [system time](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/system_time.html).
Set it using `settimeofday`, cf. [test code](https://github.com/espressif/esp-idf/blob/9a55b42f0841b3d38a61089b1dda4bf28135decd/components/fatfs/test/test_fatfs_common.c#L218-L228).

## Authors

[Allan Kipkirui Koech](https://github.com/allankkoech)
and Jeremy Tammik,
[The Building Coder](http://thebuildingcoder.typepad.com),
[Autodesk Platform Services APS](http://aps.autodesk.com),
[ADN](http://www.autodesk.com/adn)
[Open](http://www.autodesk.com/adnopen),
[Autodesk Inc.](http://www.autodesk.com)

## License

This sample is licensed under the terms of the [MIT License](http://opensource.org/licenses/MIT).
Please see the [LICENSE](LICENSE) file for full details.
