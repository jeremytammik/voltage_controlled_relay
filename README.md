# Voltage Controlled Relay

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
map the ADC range [0,4095] to ca. [0V,30.4V].

## State Machine

The relay states and transitions are managed by a state machine
(image generated using [Finite State Machine Designer](https://www.madebyevan.com/fsm/)):

<center>
<img src="img/2023-02-23_state_machine.png" alt="State machine" title="State machine" width="500"/> <!-- 1004 x 312 pixels -->
</center>

The transitions are complicated by the fact that each state, once attained, must be retained for a certain minimum amount of time before any state change can occur.
Or, to be more precise, the state transition does not happen until the trigger persists for a certain amount of time.
The trigger itself includes a minimum timespan to cause the state change.

We use [YA_FSM](https://github.com/cotestatnt/YA_FSM) 
by [Tolentino Cotesta](https://www.hackster.io/tolentinocotesta), 
described in [Let's learn how to use finite state machine with Arduino](https://www.hackster.io/tolentinocotesta/let-s-learn-how-to-use-finite-state-machine-with-arduino-c524ac),
since it includes functionality to support minimum and maximum timeouts for each state.
It is also equipped with a [wokwi simulation](https://wokwi.com/projects/338248486164103762).
It would be cool to set one up for this project as well.
That might save a lot of effort implementing real-world tests.

## Board

I am working with the [Joy-it NODEMCU ESP32 board](https://joy-it.net/en/products/SBC-NodeMCU-ESP32):

<center>
<img src="img/2023-02-24_joyit_nodemcu_esp32_pins.png" alt="Joy-it NODEMCU ESP32 pins" title="Joy-it NODEMCU ESP32 pins" width="500"/> <!-- 1788 x 1318 pixels -->
</center>


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
