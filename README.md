
# IotSprinklerRelayEsp8266-01

Wi-Fi controller flash configuration that uses the following 
platformio profile:

    platform   =   espressif8266
    board      =   esp01_1m
    framework  =   arduino

Configured for use with [micro-sprinkler-api](https://github.com/wejafoo/iot-sprinkler-relay-esp8266-12e)
which serves as a subscription proxy to a [GCP Pubsub](https://cloud.google.com/pubsub) topic(triggered by
[GCP Scheduler](https://cloud.google.com/scheduler)), which carries a JSON payload that encodes relay/solenoid
firing/duration instructions.

The `src/arduino-ide` directory will be periodically updated with the Arduino IDE equivalent of main.cpp(and associated libs).
Hopefully this gives those more familiar with Arduino IDE get started more quickly,
or maybe serve as a quick reference to aid in transitioning to a c++ workflow, which can be initially
jarring.


FWIW, developed using the following:

[4 channel relay](https://www.amazon.com/gp/product/B00KTEN3TM/ref=ppx_yo_dt_b_search_asin_title?ie=UTF8&psc=1)

[esp-01 WiFi Module](https://www.amazon.com/ESP8266-Updated-Wireless-Transceiver-Arduino/dp/B07H1W6DJZ/ref=pd_psc_dp_d_0_1/147-1493701-5057417?pd_rd_w=wo8Kx&pf_rd_p=83f4b62c-18cd-447a-a1e8-8bdea771fe8a&pf_rd_r=5J5MY9QFQK2G2B2D3P7H&pd_rd_r=5d3a7bda-bd12-44c5-93fb-f84f6ae1a761&pd_rd_wg=yY8V4&pd_rd_i=B07H1W6DJZ&psc=1)

[esp-01 USB breakout](https://www.amazon.com/gp/product/B08F9YLHR5/ref=ppx_yo_dt_b_search_asin_title?ie=UTF8&psc=1)


### Related projects

[IotSprinklerRelayEsp8266-12e](https://github.com/wejafoo/iot-sprinkler-relay-esp8266-12e)
Same general idea, a little more cost, but without the headaches that the esp-01's small footprint can introduce.  

[MicroSprinklerApi](https://github.com/wejafoo/micro-sprinkler-api)
HTTP polling pubsub subscription service that is equally comfortable behind your firewall on Docker or
on a serverless GCP Cloud Run instance.