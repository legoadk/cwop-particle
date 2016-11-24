#include "cwop-particle/cwop-particle.h"
// This is your CWOP ID as provided to you when you registered.
#define CWOP_ID "FWxxxx"

// See http://aprs.fi/; I recommend using their map to find your location:
// 1. Enter your nearest address in the second box
// 2. Right-click to drop a marker exactly where your station's barometer is mounted
// 3. Use the lat/lon coordinates with decimal minutes provided by the marker.

// Latitude, degrees
#define CWOP_LAT_DEG 00
// Latitude, minutes (in decimal).
#define CWOP_LAT_MIN 00.00
// Latitude, hemisphere (must be "char", not a string!) N / S
#define CWOP_LAT_HEMISPHERE 'N'

// Longitude, degrees
#define CWOP_LON_DEG 000
// Longitude, minutes (in decimal).
#define CWOP_LON_MIN 00.00
// Longitude, hemisphere (must be "char", not a string!) E / W
#define CWOP_LON_HEMISPHERE 'W'

CWOP_Particle CWOP = CWOP_Particle();

TCPClient cwopclient;

#if PLATFORM_ID!=0 && PLATFORM_ID!=31
// The Core is a special case and does not (by default) support Timers.
volatile bool cwopSend;

Timer tCWOPSend(1000 * 60 * 5, cbCWOPSend); // five minutes

void cbCWOPSend() {
  cwopSend = true;
}
#endif
SerialLogHandler logHandler(LOG_LEVEL_TRACE);

void setup()
{
    CWOP.begin(cwopclient, CWOP_ID);
    CWOP.setLatitude(CWOP_LAT_DEG, CWOP_LAT_MIN, CWOP_LAT_HEMISPHERE);
    CWOP.setLongitude(CWOP_LON_DEG, CWOP_LON_MIN, CWOP_LON_HEMISPHERE);
    #if PLATFORM_ID!=0 && PLATFORM_ID!=31
    tCWOPSend.start();
    #endif
}

double randMToN(double M, double N)
{
    return M + (rand() / ( RAND_MAX / (N-M) ) ) ;  
}

void loop()
{
    #if PLATFORM_ID!=0 && PLATFORM_ID!=31
    if(cwopSend)
    {
      cwopSend = false;
      
    #endif
      srand(Time.now());

      /* Set up the data packet with our gathered data (random numbers here, as an example) */
      /* See cwop-particle.cpp for more data types that can be provided! */
      CWOP.setWindDegrees(randMToN(0.0, 360.0));
      CWOP.setWindMph(randMToN(0.0, 25.0));
      CWOP.setWindGustMph(randMToN(0.0, 50.0));
      CWOP.setTempF(randMToN(-10.0, 100.0));
      CWOP.setRain1hIn(randMToN(0.0, 1.0));
      CWOP.setRain24hIn(randMToN(0.0, 24.0));
      CWOP.setRainSinceMidnightIn(randMToN(0.0, 24.0));
      CWOP.setHumidityPct(randMToN(0.0, 100.0));
      CWOP.setPressureMb(randMToN(1000.0, 1030.0));

      /* CWOP_Particle::createAPRS() creates an APRS packet using the last configured values and returns it as a string.
       * For testing, we can simply Publish this so we can see what our app would send to CWOP. */
      Particle.publish("app/debug", CWOP.createAPRS(), 60, PRIVATE);

      /* This requires a valid CWOP ID; behaviour is undefined if an invalid one is provided. */
      /* NOTE: it is not *necessary* (but is not explicitly harmful) to call CWOP_Particle::createAPRS() before calling
       * CWOP_Particle::writePacket(). */
      //CWOP.writePacket();
    #if PLATFORM_ID!=0 && PLATFORM_ID!=31
    }
    #else
    // The Core *does* support delay(), so that'll have to be good enough.
    // CWOP.writePacket() introduces a delay of 500 while sending data, so we compensate for that here.
    delay((1000 * 6 * 5) - 500);
    #endif
}