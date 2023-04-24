# Voltage Controlled Relay

The voltage controlled relay controls a bank of relays based on the voltage of a PV battery.
The input is the battery voltage in the range [24V,30V].
This is measured via a voltage divider to turn relays on and off when various thresholds are crossed:

- [Board](#board)
- [Schematic](#schematic)
- [Voltage divider](#voltage-divider)
- [State machine](#state-machine)
- [Second approach](#second-approach)

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

## Voltage Divider

We need to convert the input battery voltage range [24V,30V] to fit into the ESP32 ADC input voltage range [0V,3.3V],
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
map the ADC `int` range [0,4095] to the battery voltage range of ca. [0V,30.4V].

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
<img src="img/2023-02-2023-04-22_plan_b_2_relay.jpg" alt="Second approach" title="Second approach" width="500"/> <!-- 1004 x 312 pixels -->
</center>

It works perfectly, afaict, except that the voltage measurement is extremely unprecise.
In the lower region, under 25V, the ADC conversion reports a too low voltage, and in the high region it is too high.
Also, I am afraid of breaking the ADC by inadvertently applying a too high input voltage, since I am just twiddling a knob by hand to control it.

I'll see whether I can fine-tune it better somehow.
For instance, I could implement a custom mapping of the ADC ranges to the corresponding real-world voltage aapplied.
I think I would like to install this soon for real-world testing and use.

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
