/*
  Citizen Weather Observer Program Reporting Library for Particle (CWOP-Particle)
  License: CC-0 (Public Domain)
*/

#ifndef cwop_particle_h
#define cwop_particle_h

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
#else
#error Only Particle/Spark Core/Photon/Electron/P1 are supported.
#endif

#include "math.h"
#include "application.h"

#define CWOP_SERVER "cwop.aprs.net"
#define CWOP_IPADDRESS IPAddress(63, 251, 153, 99)
#define CWOP_PORTNUMBER 14580
#define CWOP_VERSION "v0.06"

#ifdef PARTICLE_CORE
#define CWOP_DEVICE "cwop-particle-core"
#elif defined(PARTICLE_PHOTON)
#define CWOP_DEVICE "cwop-particle-photon"
#elif defined(PARTICLE_ELECTRON)
#define CWOP_DEVICE "cwop-particle-electron"
#elif defined(PARTICLE_P1)
#define CWOP_DEVICE "cwop-particle-P1"
#endif
#define PARTICLE_PUBLISH_TTL 60 // Spark "time to live" for published messages
#define PARTICLE_PUBLISH_TOPIC "cwop/debug"

#define OK_SUCCESS 200		 // OK / Success
#define ERR_CONNECT_FAILED -301  // Failed to connect
#define ERR_UNEXPECTED_FAIL -302 // Unexpected failure during write

/**
 * @brief Send weather data to CWOP.
 */
class CWOPClass
{
  public:
	CWOPClass()
	{
	this->lastReadStatus = OK_SUCCESS;
	};

	bool begin(Client &client, const char *regNumber)
	{
	if (!begin(client))
		return false;
	this->setRegNumber(regNumber);
	return true;
	}

	bool begin(Client &client)
	{
		#ifdef PRINT_DEBUG_MESSAGES
			Serial.print("cwop::cwopBegin");
		#endif
		this->setClient(&client);
		this->lastReadStatus = OK_SUCCESS;
		return true;
	};

	void setClient(Client *client) { this->client = client; };

	int setRegNumber(const char *number)
	{
#ifdef PRINT_DEBUG_MESSAGES
	Serial.print("cwop::setRegNumber(number: ");
	Serial.print(number);
	Serial.println("\")");
#endif
	this->regNumber = String(number);
	return OK_SUCCESS;
	};

	/* LAT/LONG FORMATS - FROM WXQA.COM
	 * The format is "ddmm.hhN/dddmm.hhW" without the quotation marks, where d is degrees, m is minutes and h is hundredths
	 * of minutes. The first eight characters represent latitude (north in this case), and the last nine characters
	 * represent longitude (west in this case). The hemispheres are represented by capital letters; N, E, S, W and remove
	 * the need for plus or minus signs on the latitude and longitude. Note that LEADING ZEROS ARE REQUIRED. Also note that
	 * there must be only two numbers to the right of the decimal point in the latitude or longitude, and that colons are
	 * not allowed.
	 */
	void setLatitude(int degrees, double minutes, char hemisphere)
	{
	this->latitude = String::format("%02d%05.2f%c",
					degrees,
					minutes,
					hemisphere);
	};

	void setLongitude(int degrees, double minutes, char hemisphere)
	{
	this->longitude = String::format("%03d%05.2f%c",
					 degrees,
					 minutes,
					 hemisphere);
	};

	void setWindDegrees(double windDeg) { this->windDeg = windDeg; };
	void setWindMph(double windMph) { this->windMph = windMph; };
	void setWindGustMph(double windGustMph) { this->windGustMph = windGustMph; };
	void setTempF(double tempF) { this->tempF = tempF; };
	void setRain1hIn(double rain1hIn) { this->rain1hIn = rain1hIn; };
	void setRain24hIn(double rain24hIn) { this->rain24hIn = rain24hIn; };
	void setRainSinceMidnightIn(double rainSinceMidnightIn) { this->rainSinceMidnightIn = rainSinceMidnightIn; };
	void setHumidityPct(double humidityPct) { this->humidityPct = humidityPct; };
	void setPressureMb(double pressureMb) { this->pressureMb = pressureMb; };
	void setLuminosityWM2(double luminosityWM2) { this->luminosityWM2 = luminosityWM2; };
	void setSnow24hIn(double snow24hIn) { this->snow24hIn = snow24hIn; };

	Client *client = NULL;
	unsigned int port = CWOP_PORTNUMBER;

	String latitude = String("");
	String longitude = String("");
	String regNumber = String("");

	char nextAPRS[256];
	double windDeg = NAN;
	double windMph = NAN;
	double windGustMph = NAN;
	double tempF = NAN;
	double rain1hIn = NAN;
	double rain24hIn = NAN;
	double rainSinceMidnightIn = NAN;
	double snow24hIn = NAN;
	double humidityPct = NAN;
	double pressureMb = NAN;
	double luminosityWM2 = NAN;

	int lastReadStatus;
	const char *number = NULL;

	bool connect()
	{
	bool connectSuccess = false;
	connectSuccess = this->client->connect(CWOP_SERVER, CWOP_PORTNUMBER);
	if (!connectSuccess)
	{
#ifdef PRINT_DEBUG_MESSAGES
		Serial.print("Failed. Try default IP...");
#endif
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
	};

	const char *createAPRS()
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
	if (!isnan(this->luminosityWM2)) {
		if (round(this->luminosityWM2) < 1000) {
			sprintf(tempstring, "L%03.0f", this->luminosityWM2);
		}
		else {
			sprintf(tempstring, "l%03.0f", this->luminosityWM2 - 1000);
		}
		strcat(nextAPRS, tempstring);
	}

	/* SOFTWARE TYPE AND ACTION (.) */
	sprintf(tempstring, ".%s%s", CWOP_DEVICE, CWOP_VERSION);
	strcat(nextAPRS, tempstring);
	return nextAPRS;
	};

	bool writeHeader()
	{
	if (!this->client->print(String::format("user %s pass -1 vers %s %s\r\n", this->regNumber.c_str(), CWOP_DEVICE, CWOP_VERSION).c_str()))
		return false;
	return true;
	};

	bool writeData()
	{
	if (!this->client->print(String::format("%s\r\n", this->nextAPRS).c_str()))
		return false;
	return true;
	};

	int writePacket()
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
		Particle.publish(PARTICLE_PUBLISH_TOPIC, connect_msg, PARTICLE_PUBLISH_TTL, PRIVATE);
	}

	this->client->stop();

	return OK_SUCCESS;
	};

	int abortWritePacket()
	{
	this->client->stop();
#ifdef PRINT_DEBUG_MESSAGES
	Particle.publish(PARTICLE_PUBLISH_TOPIC, "Aborted packet!", PARTICLE_PUBLISH_TTL, PRIVATE);
#endif
	return ERR_UNEXPECTED_FAIL;
	}
};

extern CWOPClass CWOP;

#endif //cwop_particle_h
