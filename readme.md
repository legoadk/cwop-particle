# CWOP-Particle: Citizen Weather Observer Program Reporting Library for Particle devices

This library was created to facilitate sending weather data in the correct APRS packet format to CWOP. 

This is meant to be a supplement to your own weather-gathering firmware, and makes no attempt to actually gather any data - you must provide it!

Please note that in order to send data and have it be used by CWOP, you must register for an ID here: [CWOP Sign-Up][1]

## Examples
See the [basic-cwop example][2] for a sample of how to use this library, and have fun!

## Reference

### `CWOP_Particle`

`CWOP_Particle cwop = CWOP_Particle();`

Creates an object to use in sending CWOP data.

### `begin`

`cwop.begin(client);`

Prepares the object for communication with CWOP by providing a TCPClient.

`cwop.begin(client, "ID");`

Prepares the object for communication with CWOP by providing a TCPClient and a CWOP ID.

### `setLatitude`, `setLongitude`

`cwop.setLatitude(degrees, minutes, hemisphere);`
`cwop.setLongitude(degrees, minutes, hemisphere);`

Set the latitude/longitude respectively, by specifying the degrees, minutes, and hemisphere (a single char; ex: 'N', 'S', 'E', 'W')

### `setWindDegrees`

`cwop.setWindDegrees(degrees);`

Set the "wind degrees" field of the CWOP packet, in degrees.

### `setWindMph`

`cwop.setWindMph(mph);`

Set the "wind speed" field of the CWOP packet, in MPH.

### `setWindGustMph`

`cwop.setWindGustMph(mph);`

Set the "wind gust speed" field of the CWOP packet, in MPH.

### `setTempF`

`cwop.setTempF(degreesF);`

Set the "Temperature" field of the CWOP packet, in degrees Fahrenheit.

### `setRain1hIn`

`cwop.setRain1hIn(inches);`

Set the "one-hour rainfall" field of the CWOP packet, in inches.

### `setRain24hIn`

`cwop.setRain24hIn(inches);`

Set the "24-hour rainfall" field of the CWOP packet, in inches.

### `setRainSinceMidnightIn`

`cwop.setRainSinceMidnightIn(inches);`

Set the "rainfall since midnight" field of the CWOP packet, in inches.

### `setHumidityPct`

`cwop.setHumidityPct(percent);`

Set the "humidity" field of the CWOP packet, in percent.

### `setPressureMb`

`cwop.setPressureMb(millibar);`

Set the "relative pressure" field of the CWOP packet, in millibars.

### `setLuminosityWM2`

`cwop.setLuminosityWM2(wm2);`

Set the "luminosity" field of the CWOP packet, in WM².

### `setSnow24hIn`

`cwop.setSnow24hIn(inches)`

Set the "24-hour snowfall" field of the CWOP packet, in inches.

### `setRegNumber`

`cwop.setRegNumber(regNumber);`

Set the Registration Number to use for the CWOP packet.

### `createAPRS`

`cwop.createAPRS();`

Stages an APRS packet internally; returns the resulting string. Useful for doing
a "dry run" to see the APRS packet without sending it.

### `writePacket`

`cwop.writePacket();`

Prepares an APRS packet, connects to the CWOP server and sends the entire packet.

## References
[1]: http://www.wxqa.com/SIGN-UP.html
[2]: firmware/examples/basic-cwop

## License
CC0 (Public Domain) - see license.md