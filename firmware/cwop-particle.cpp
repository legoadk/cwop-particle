/*
  Citizen Weather Observer Program Reporting Library for Particle (CWOP-Particle)
  License: CC-0 (Public Domain)
  @author: Andrew Keyser <legoguy@gmail.com>
  */

#include "cwop-particle.h"


CWOP_Particle::CWOP_Particle()
{
}

void CWOP_Particle::begin(Client &client, const char *regNumber)
{
  begin(client);
  this->setRegNumber(regNumber);
}

void CWOP_Particle::begin(Client &client)
{
  this->setClient(&client);
}

void CWOP_Particle::setClient(Client *client) { this->client = client; }

void CWOP_Particle::setRegNumber(const char *number)
{
  this->regNumber = String(number);
}

/* LAT/LONG FORMATS - FROM WXQA.COM
 * The format is "ddmm.hhN/dddmm.hhW" without the quotation marks, where d is degrees, m is minutes and h is hundredths
 * of minutes. The first eight characters represent latitude (north in this case), and the last nine characters
 * represent longitude (west in this case). The hemispheres are represented by capital letters; N, E, S, W and remove
 * the need for plus or minus signs on the latitude and longitude. Note that LEADING ZEROS ARE REQUIRED. Also note that
 * there must be only two numbers to the right of the decimal point in the latitude or longitude, and that colons are
 * not allowed.
 */
void CWOP_Particle::setLatitude(int degrees, double minutes, char hemisphere)
{
  this->latitude = String::format("%02d%05.2f%c",
                                  degrees,
                                  minutes,
                                  hemisphere);
}

void CWOP_Particle::setLongitude(int degrees, double minutes, char hemisphere)
{
  this->longitude = String::format("%03d%05.2f%c",
                                   degrees,
                                   minutes,
                                   hemisphere);
}

void CWOP_Particle::setWindDegrees(double windDeg) { this->windDeg = windDeg; }
void CWOP_Particle::setWindMph(double windMph) { this->windMph = windMph; }
void CWOP_Particle::setWindGustMph(double windGustMph) { this->windGustMph = windGustMph; }
void CWOP_Particle::setTempF(double tempF) { this->tempF = tempF; }
void CWOP_Particle::setRain1hIn(double rain1hIn) { this->rain1hIn = rain1hIn; }
void CWOP_Particle::setRain24hIn(double rain24hIn) { this->rain24hIn = rain24hIn; }
void CWOP_Particle::setRainSinceMidnightIn(double rainSinceMidnightIn) { this->rainSinceMidnightIn = rainSinceMidnightIn; }
void CWOP_Particle::setHumidityPct(double humidityPct) { this->humidityPct = humidityPct; }
void CWOP_Particle::setPressureMb(double pressureMb) { this->pressureMb = pressureMb; }
void CWOP_Particle::setLuminosityWM2(double luminosityWM2) { this->luminosityWM2 = luminosityWM2; }
void CWOP_Particle::setSnow24hIn(double snow24hIn) { this->snow24hIn = snow24hIn; }

bool CWOP_Particle::connect()
{
  bool connectSuccess = false;
  connectSuccess = this->client->connect(CWOP_SERVER, CWOP_PORTNUMBER);
  if (!connectSuccess)
  {
    connectSuccess = this->client->connect(CWOP_IPADDRESS, CWOP_PORTNUMBER);
  }

#ifdef PRINT_DEBUG_MESSAGES
  if (connectSuccess)
  {
    Particle.publish(PARTICLE_PUBLISH_TOPIC, "Connection Success", PARTICLE_PUBLISH_TTL, PRIVATE);
  }
  else
  {
    Particle.publish(PARTICLE_PUBLISH_TOPIC, "Connection Failure", PARTICLE_PUBLISH_TTL, PRIVATE);
  }
#endif
  return connectSuccess;
}

const char *CWOP_Particle::createAPRS()
{
  nextAPRS[0] = '\0';
  char tempstring[128] = "";

  /* http://www.aprs.org/doc/APRS101.PDF */
  /* http://aprs.org/aprs12/weather-new.txt */
  /* http://www.aprs.org/aprs11/spec-wx.txt */
  /* http://www.aprs.org/APRS-docs/WX.TXT */
  /* BUILD THE DATA STRING, START WITH STATION ID, TIMESTAMP, AND POSITION */
  sprintf(nextAPRS, "%s>APRS,TCPIP*:%s%s/%s_",
          this->regNumber.c_str(),                                            // Build weather record
          Time.format(Time.now() - (Time.zone() * 3600), "@%d%H%Mz").c_str(), // Add date time
          this->latitude.c_str(), this->longitude.c_str());                   // Add Lat Lon

  /* WIND DIRECTION ("COURSE") - required - "..." as fallback */
  if (!isnan(this->windDeg))
  {
    sprintf(tempstring, "%03.0f", this->windDeg);
    strcat(nextAPRS, tempstring);
  }
  else
  {
    strcat(nextAPRS, "_...");
  }

  /* WIND SPEED - required - "..." as fallback */
  if (!isnan(this->windMph))
  {
    sprintf(tempstring, "/%03.0f", this->windMph);
    strcat(nextAPRS, tempstring);
  }
  else
  {
    strcat(nextAPRS, "/...");
  }

  /* WIND GUST (g) - required - "..." as fallback */
  if (!isnan(this->windGustMph))
  {
    sprintf(tempstring, "g%03.0f", this->windGustMph);
    strcat(nextAPRS, tempstring);
  }
  else
  {
    strcat(nextAPRS, "g...");
  }

  /* TEMPERATURE OUTDOOR (t) - required - "..." as fallback */
  if (!isnan(this->tempF))
  {
    sprintf(tempstring, "t%03.0f", this->tempF);
    strcat(nextAPRS, tempstring);
  }
  else
  {
    strcat(nextAPRS, "t...");
  }

  /* RAIN 1H (r) - not required */
  if (!isnan(this->rain1hIn))
  {
    sprintf(tempstring, "r%03.0f", this->rain1hIn * 100); // hundredths of an inch
    strcat(nextAPRS, tempstring);
  }

  /* READ RAIN 24H (p) - not required */
  if (!isnan(this->rain24hIn))
  {
    sprintf(tempstring, "p%03.0f", this->rain24hIn * 100); // hundredths of an inch
    strcat(nextAPRS, tempstring);
  }

  /* RAIN SINCE MIDNIGHT (P) - not required */
  if (!isnan(this->rainSinceMidnightIn))
  {
    sprintf(tempstring, "P%03.0f", this->rainSinceMidnightIn * 100); // hundredths of an inch
    strcat(nextAPRS, tempstring);
  }

  /* SNOW (s) - not required */
  if (!isnan(this->snow24hIn))
  {
    sprintf(tempstring, "s%03.1f", this->snow24hIn); // hundredths of an inch
    strcat(nextAPRS, tempstring);
  }

  /* RELATIVE HUMIDITY OUTDOOR (h) - not required */
  if (!isnan(this->humidityPct))
  {
    sprintf(tempstring, "h%02.0f", this->humidityPct >= 99.5 ? 0 : this->humidityPct); // report >=99.5 as '00'
    strcat(nextAPRS, tempstring);
  }

  /* BAROMETRIC PRESSURE (b) - not required */
  if (!isnan(this->pressureMb))
  {
    sprintf(tempstring, "b%05.0f", (this->pressureMb * 10)); // tenths of milibars
    strcat(nextAPRS, tempstring);
  }

  /* LUMINOSITY (L/l) - not required */
  if (!isnan(this->luminosityWM2))
  {
    if (round(this->luminosityWM2) < 1000)
    {
      sprintf(tempstring, "L%03.0f", this->luminosityWM2);
    }
    else
    {
      sprintf(tempstring, "l%03.0f", this->luminosityWM2 - 1000);
    }
    strcat(nextAPRS, tempstring);
  }

  /* SOFTWARE TYPE AND ACTION (.) */
  sprintf(tempstring, ".%s%s", CWOP_DEVICE, CWOP_VERSION);
  strcat(nextAPRS, tempstring);
  return nextAPRS;
}

bool CWOP_Particle::writeHeader()
{
  if (!this->client->print(String::format("user %s pass -1 vers %s %s\r\n", this->regNumber.c_str(), CWOP_DEVICE, CWOP_VERSION).c_str()))
    return false;
  return true;
}

bool CWOP_Particle::writeData()
{
  if (!this->client->print(String::format("%s\r\n", this->nextAPRS).c_str()))
    return false;
  return true;
}

int CWOP_Particle::writePacket()
{
  if (!connect())
  {
    // Failed to connect
    return ERR_CONNECT_FAILED;
  }

  if (!this->writeHeader())
    return abortWritePacket();
  this->createAPRS();
  if (!this->writeData())
    return abortWritePacket();

  delay(500); // hopefully this is enough time to send the data.

  if (this->client->available() > 0)
  {
    String connect_msg = this->client->readStringUntil('\r');
#ifdef PRINT_DEBUG_MESSAGES
    Particle.publish(PARTICLE_PUBLISH_TOPIC, String("received message: " + connect_msg), PARTICLE_PUBLISH_TTL, PRIVATE);
#endif
  }

  this->client->stop();

  return OK_SUCCESS;
}

int CWOP_Particle::abortWritePacket()
{
  this->client->stop();
#ifdef PRINT_DEBUG_MESSAGES
  Particle.publish(PARTICLE_PUBLISH_TOPIC, "Aborted packet!", PARTICLE_PUBLISH_TTL, PRIVATE);
#endif
  return ERR_UNEXPECTED_FAIL;
}
