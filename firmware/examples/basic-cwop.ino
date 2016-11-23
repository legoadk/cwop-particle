#include "cwop-particle.h"
#define CWOP_ID "FWxxxx"
#define CWOP_LAT_DEG 00
#define CWOP_LAT_MIN 00.00
#define CWOP_LAT_HEMISPHERE 'N'
#define CWOP_LON_DEG 000
#define CWOP_LON_MIN 00.00
#define CWOP_LON_HEMISPHERE 'W'

TCPClient cwopclient;

volatile bool cwopSend;

Timer tCWOPSend(1000 * 60 * 5, cbCWOPSend); // five minutes

void cbCWOPSend() {
  cwopSend = true;
}

void setup()
{
    CWOP.begin(cwopclient, CWOP_ID);
    CWOP.setLatitude(CWOP_LAT_DEG, CWOP_LAT_MIN, CWOP_LAT_HEMISPHERE);
    CWOP.setLongitude(CWOP_LON_DEG, CWOP_LON_MIN, CWOP_LON_HEMISPHERE);
    tCWOPSend.start();
}

double randMToN(double M, double N)
{
    return M + (rand() / ( RAND_MAX / (N-M) ) ) ;  
}

void loop()
{
    if(cwopSend)
    {
      cwopSend = false;
      
      srand(Time.now());

      /* Set up the data packet with our gathered data (random numbers here, as an example) */
      /* See cwop.h for more data types that can be provided! */
      CWOP.setWindDegrees(randMToN(0.0, 360.0));
      CWOP.setWindMph(randMToN(0.0, 25.0));
      CWOP.setWindGustMph(randMToN(0.0, 50.0));
      CWOP.setTempF(randMToN(-10.0, 100.0));
      CWOP.setRain1hIn(randMToN(0.0, 1.0));
      CWOP.setRain24hIn(randMToN(0.0, 24.0));
      CWOP.setRainSinceMidnightIn(randMToN(0.0, 24.0));
      CWOP.setHumidityPct(randMToN(0.0, 100.0));
      CWOP.setPressureMb(randMToN(1000.0, 1030.0));

      /* CWOP.createAPRS() creates an APRS packet using the last configured values and returns it as a string.
       * For testing, we can simply Publish this so we can see what our app would send to CWOP. */
      Particle.publish("app/debug", CWOP.createAPRS(), 60, PRIVATE);

      /* This requires a valid CWOP ID; behaviour is undefined if an invalid one is provided. */
      /* NOTE: it is not *necessary* (but is not explicitly harmful) to call CWOP.createAPRS() before calling CWOP.writePacket(). */
      //CWOP.writePacket();
    }
}