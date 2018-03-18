# ESP8266 and DHT22 based Temperature Sensor

This arduino scetch exports the sensor readings from the DHT22 to MQTT. I use this scetch together with my [MQTT2Prometheus exporter](https://github.com/hikhvar/mqtt2prometheus) to monitor my
room temperature in several rooms.

## Configuration
To set your config, copy the file `configvariables.h.dist` to `configvariables.h` and edit the values you need.