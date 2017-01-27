/*
  Citizen Weather Observer Program Reporting Library for Particle (CWOP-Particle)
  License: CC-0 (Public Domain)
  @author: Andrew Keyser <legoguy@gmail.com>
*/

#ifndef cwop_particle_h
#define cwop_particle_h

#include "math.h"
#include "application.h"

#define OK_SUCCESS 200           // OK / Success
#define ERR_CONNECT_FAILED -301  // Failed to connect
#define ERR_UNEXPECTED_FAIL -302 // Unexpected failure during write

#define CWOP_SERVER "cwop.aprs.net"
#define CWOP_IPADDRESS IPAddress(63, 251, 153, 99)
#define CWOP_PORTNUMBER 14580
#define CWOP_VERSION "v0.0.10"

#define PARTICLE_PUBLISH_TTL 60 // Particle "time to live" for published messages
#define PARTICLE_PUBLISH_TOPIC "cwop/debug"
//#define PRINT_DEBUG_MESSAGES

// Create platform defines for Particle devices
#if PLATFORM_ID == 0
#define PARTICLE_CORE
#elif PLATFORM_ID == 6
#define PARTICLE_PHOTON
#define PARTICLE_PHOTONELECTRON
#elif PLATFORM_ID == 8
#define PARTICLE_P1
#define PARTICLE_PHOTONELECTRON
#elif PLATFORM_ID == 10
#define PARTICLE_ELECTRON
#define PARTICLE_PHOTONELECTRON
#elif PLATFORM_ID == 31
#define PARTICLE_PI
#else
#error Only Particle/Spark Core/Photon/Electron/P1 or Raspberry Pi are supported.
#endif

#ifdef PARTICLE_CORE
#define CWOP_DEVICE "cwop-particle-core"
#elif defined(PARTICLE_PHOTON)
#define CWOP_DEVICE "cwop-particle-photon"
#elif defined(PARTICLE_ELECTRON)
#define CWOP_DEVICE "cwop-particle-electron"
#elif defined(PARTICLE_P1)
#define CWOP_DEVICE "cwop-particle-P1"
#elif defined(PARTICLE_PI)
#define CWOP_DEVICE "cwop-particle-Raspberry-Pi"
#endif

class CWOP_Particle
{
public:
  CWOP_Particle();

  bool
      connect(),
      writeHeader(),
      writeData();

  void
      begin(Client &client, const char *regNumber),
      begin(Client &client),
      setClient(Client *client),
      setLatitude(int degrees, double minutes, char hemisphere),
      setLongitude(int degrees, double minutes, char hemisphere),
      setWindDegrees(double windDeg),
      setWindMph(double windMph),
      setWindGustMph(double windGustMph),
      setTempF(double tempF),
      setRain1hIn(double rain1hIn),
      setRain24hIn(double rain24hIn),
      setRainSinceMidnightIn(double rainSinceMidnightIn),
      setHumidityPct(double humidityPct),
      setPressureMb(double pressureMb),
      setLuminosityWM2(double luminosityWM2),
      setSnow24hIn(double snow24hIn),
      setRegNumber(const char *number);

  const char *
      createAPRS();

  int writePacket(),
      abortWritePacket();

private:
  Client *
      client = NULL;

  unsigned int port = CWOP_PORTNUMBER;

  String
      latitude = String(""),
      longitude = String(""),
      regNumber = String("");

  char nextAPRS[256];

  double
      windDeg = NAN,
      windMph = NAN,
      windGustMph = NAN,
      tempF = NAN,
      rain1hIn = NAN,
      rain24hIn = NAN,
      rainSinceMidnightIn = NAN,
      snow24hIn = NAN,
      humidityPct = NAN,
      pressureMb = NAN,
      luminosityWM2 = NAN;

  const char *number = NULL;
};

extern CWOP_Particle CWOP;

#endif //cwop_particle_h
