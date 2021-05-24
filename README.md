
# IotSprinklerRelayEsp8266-01

Wi-Fi controller flash configuration that uses the following 
platformio profile:

    platform   =   espressif8266
    board      =   esp01_1m
    framework  =   arduino

Configured for use with [micro-sprinkler-api](https://github.com/wejafoo/iot-sprinkler-relay-esp8266-12e)
as a  subscription proxy to a GCP pubsub topic which carries JSON relay instruction as payload.

If you are transitioning from Arduino IDE to c/c++, the `src/arduino-ide` directory
is periodically synced with main.cpp(and any libs) for development parity intended to aid
in troubleshooting issues, such as serial device communication with which I continue to struggle with 
in my IDE(CLion).

Arduino IDE does a great of job of hiding the bits of the mundane complexity that can trip you up early-on,
and can serve as a ready reality check as you get comfortable with your IDE's equivalencies.

