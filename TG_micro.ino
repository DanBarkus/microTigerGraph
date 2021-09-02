#include <Wire.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_TSL2591.h"
#include "TigerGraphMicro.h"

const int HTTPS_PORT = 443;
const char* ssid = "fiveguysburgersandwifi";
const char* pw = "weonlyusefreshgroundbeef";
// const char* HOST_NAME = "tg-101-2.i.tgcloud.io";
const char* HOST_NAME = "iot.i.tgcloud.io";
const char* USERNAME = "tigergraph";
const char* PASSWORD = "tigergraph";
// const char* TOKEN = "5vdmp68jhoalkfutfq9hvjln5ofm3k9j";
const char* TOKEN = "v22058f0tao8aqa9k37rgshgnnnlp6dc";

// Time setting Stuff
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = -14400;
const int   daylightOffset_sec = 0;
struct tm timeinfo;

// Neopixel Stuff
#define LED_PIN     27
#define LED_COUNT  8
#define BRIGHTNESS 255
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRBW + NEO_KHZ800);

// Light Sensor Stuff
Adafruit_TSL2591 tsl = Adafruit_TSL2591(2591);


String DEVICE_ID = "Batt_2";
String DEVICE_TYPE = "LIGHT";

const char* root_ca = \
"-----BEGIN CERTIFICATE-----\n"  \
"MIIGEzCCA/ugAwIBAgIQfVtRJrR2uhHbdBYLvFMNpzANBgkqhkiG9w0BAQwFADCB\n"  \
"iDELMAkGA1UEBhMCVVMxEzARBgNVBAgTCk5ldyBKZXJzZXkxFDASBgNVBAcTC0pl\n"  \
"cnNleSBDaXR5MR4wHAYDVQQKExVUaGUgVVNFUlRSVVNUIE5ldHdvcmsxLjAsBgNV\n"  \
"BAMTJVVTRVJUcnVzdCBSU0EgQ2VydGlmaWNhdGlvbiBBdXRob3JpdHkwHhcNMTgx\n"  \
"MTAyMDAwMDAwWhcNMzAxMjMxMjM1OTU5WjCBjzELMAkGA1UEBhMCR0IxGzAZBgNV\n"  \
"BAgTEkdyZWF0ZXIgTWFuY2hlc3RlcjEQMA4GA1UEBxMHU2FsZm9yZDEYMBYGA1UE\n"  \
"ChMPU2VjdGlnbyBMaW1pdGVkMTcwNQYDVQQDEy5TZWN0aWdvIFJTQSBEb21haW4g\n"  \
"VmFsaWRhdGlvbiBTZWN1cmUgU2VydmVyIENBMIIBIjANBgkqhkiG9w0BAQEFAAOC\n"  \
"AQ8AMIIBCgKCAQEA1nMz1tc8INAA0hdFuNY+B6I/x0HuMjDJsGz99J/LEpgPLT+N\n"  \
"TQEMgg8Xf2Iu6bhIefsWg06t1zIlk7cHv7lQP6lMw0Aq6Tn/2YHKHxYyQdqAJrkj\n"  \
"eocgHuP/IJo8lURvh3UGkEC0MpMWCRAIIz7S3YcPb11RFGoKacVPAXJpz9OTTG0E\n"  \
"oKMbgn6xmrntxZ7FN3ifmgg0+1YuWMQJDgZkW7w33PGfKGioVrCSo1yfu4iYCBsk\n"  \
"Haswha6vsC6eep3BwEIc4gLw6uBK0u+QDrTBQBbwb4VCSmT3pDCg/r8uoydajotY\n"  \
"uK3DGReEY+1vVv2Dy2A0xHS+5p3b4eTlygxfFQIDAQABo4IBbjCCAWowHwYDVR0j\n"  \
"BBgwFoAUU3m/WqorSs9UgOHYm8Cd8rIDZsswHQYDVR0OBBYEFI2MXsRUrYrhd+mb\n"  \
"+ZsF4bgBjWHhMA4GA1UdDwEB/wQEAwIBhjASBgNVHRMBAf8ECDAGAQH/AgEAMB0G\n"  \
"A1UdJQQWMBQGCCsGAQUFBwMBBggrBgEFBQcDAjAbBgNVHSAEFDASMAYGBFUdIAAw\n"  \
"CAYGZ4EMAQIBMFAGA1UdHwRJMEcwRaBDoEGGP2h0dHA6Ly9jcmwudXNlcnRydXN0\n"  \
"LmNvbS9VU0VSVHJ1c3RSU0FDZXJ0aWZpY2F0aW9uQXV0aG9yaXR5LmNybDB2Bggr\n"  \
"BgEFBQcBAQRqMGgwPwYIKwYBBQUHMAKGM2h0dHA6Ly9jcnQudXNlcnRydXN0LmNv\n"  \
"bS9VU0VSVHJ1c3RSU0FBZGRUcnVzdENBLmNydDAlBggrBgEFBQcwAYYZaHR0cDov\n"  \
"L29jc3AudXNlcnRydXN0LmNvbTANBgkqhkiG9w0BAQwFAAOCAgEAMr9hvQ5Iw0/H\n"  \
"ukdN+Jx4GQHcEx2Ab/zDcLRSmjEzmldS+zGea6TvVKqJjUAXaPgREHzSyrHxVYbH\n"  \
"7rM2kYb2OVG/Rr8PoLq0935JxCo2F57kaDl6r5ROVm+yezu/Coa9zcV3HAO4OLGi\n"  \
"H19+24rcRki2aArPsrW04jTkZ6k4Zgle0rj8nSg6F0AnwnJOKf0hPHzPE/uWLMUx\n"  \
"RP0T7dWbqWlod3zu4f+k+TY4CFM5ooQ0nBnzvg6s1SQ36yOoeNDT5++SR2RiOSLv\n"  \
"xvcRviKFxmZEJCaOEDKNyJOuB56DPi/Z+fVGjmO+wea03KbNIaiGCpXZLoUmGv38\n"  \
"sbZXQm2V0TP2ORQGgkE49Y9Y3IBbpNV9lXj9p5v//cWoaasm56ekBYdbqbe4oyAL\n"  \
"l6lFhd2zi+WJN44pDfwGF/Y4QA5C5BIG+3vzxhFoYt/jmPQT2BVPi7Fp2RBgvGQq\n"  \
"6jG35LWjOhSbJuMLe/0CjraZwTiXWTb2qHSihrZe68Zk6s+go/lunrotEbaGmAhY\n"  \
"LcmsJWTyXnW0OMGuf1pGg+pRyrbxmRE1a6Vqe8YAsOf4vmSyrcjC8azjUeqkk+B5\n"  \
"yOGBQMkKW+ESPMFgKuOXwIlCypTPRpgSabuY0MLTDXJLR27lk8QyKGOHQ+SwMj4K\n"  \
"00u/I5sUKUErmgQfky3xxzlIPK1aEn8=\n"  \
"-----END CERTIFICATE-----";

WiFiClientSecure client;


TigerGraphMicro conn(HOST_NAME, USERNAME, PASSWORD, TOKEN, client, root_ca);

void setup()
{
	Serial.begin(115200);
	Serial.println("Connecting");

	// Init LED Strip
	strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  	strip.show();            // Turn OFF all pixels ASAP
  	strip.setBrightness(BRIGHTNESS); // Set BRIGHTNESS to about 1/5 (max = 255)

	WiFi.mode(WIFI_STA);
	WiFi.begin(ssid, pw);
	int dots = 0;
	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
		Serial.print(".");
		strip.setPixelColor(dots, strip.Color(255,137,0,0));
		strip.show();
		dots ++;
	}
	strip.clear();
	strip.setPixelColor(0, strip.Color(0,255,0,0));
	strip.show();
	Serial.println("");
	Serial.println(WiFi.localIP());

	configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
	
	if(!getLocalTime(&timeinfo)){
		Serial.println("Failed to obtain time");
	}
  	Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  	// printLocalTime();

	if (conn.connect() == true) {
		Serial.println("TG Connection Successful");
	}
	else {
		Serial.println("TG Connection Failed");
	};
}

void loop()
{
	if(!getLocalTime(&timeinfo)){
		Serial.println("Failed to obtain time");
	}
	// Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
	float lux = getLux();
	addReading(lux, &timeinfo);
	// Serial.println(resDict.size());
	// String res = runQuery("most_recent_lux", "{}");
	// getJSON(res);
	// parseLuxResults();
	delay(10000);
}

String echo() {
	client.println(String("GET /restpp/echo HTTP/1.1") + "\r\n" + 
	"Host: tg-101-2.i.tgcloud.io:443" + "\r\n" +
	"Authorization: Bearer " + TOKEN + "\r\n");
	String response = client.readString();
	Serial.println(response);
	return response;
}

// void vertexCount(String vertexType) {
// 	String content = "{\"function\":\"stat_vertex_number\",\"type\":\""+vertexType+"\"}";
// 	String response = makePost("builtins/MyGraph",content);
	
// 	DynamicJsonDocument resDict = getJSON(response);

// 	int count = int(resDict["results"][0]["count"]);
// 	Serial.println(count);
// }

// void upsertVertex(String vertexType, String vertexId, String attributes) {
// 	String content = "{\"vertices\":{\""+vertexType+"\":{\""+vertexId+"\":"+attributes+"}}}";
// 	String response = makePost("graph/MyGraph", content);
// 	getJSON(response);
// }

// void upsertEdge(String sourceVertexType, String sourceVertexId, String edgeType, String targetVertexType, String targetVertexId, String attributes) {
// 	String content = "{\"edges\":{\""+sourceVertexType+"\":{\""+sourceVertexId+"\":{\""+edgeType+"\":{\""+targetVertexType+"\":{\""+targetVertexId+"\":"+attributes+"}}}}}}";
// 	String response = makePost("graph/MyGraph", content);
// 	getJSON(response);
// }

// String makePost(String endpoint, String content) {
// 	int contentLength = content.length();

// 	client.println(String("POST /restpp/"+endpoint+" HTTP/1.1") + "\r\n" + 
// 	"Host: "+HOST_NAME+":443" + "\r\n" +
// 	"Authorization: Bearer "+ TOKEN + "\r\n" +
// 	"Content-Type: text/plain" + "\r\n" +
// 	"Content-Length: " + contentLength + "\r\n" +
// 	"\r\n" +
// 	content);
// 	String response = client.readString();
// 	return response;
// }

// DynamicJsonDocument getJSON(String response) {
// 	int resLen = response.length();
// 	char res[resLen+1];
// 	response.toCharArray(res, resLen);
// 	char * lines;
// 	char * dat;
// 	lines = strtok(res,"\n");
// 	while (lines != NULL)
// 	{
// 		dat = lines;
// 		lines = strtok(NULL,"\n");
// 	}
// 	int dl = strlen(dat);
// 	char data[dl+1];
// 	strcpy(data,dat);
// 	strcat(data + dl, "}");
	
// 	Serial.println(data);
// 	DynamicJsonDocument resDict(2048);
// 	DeserializationError error = deserializeJson(resDict, (const char*)data);
// 	if (error) {
// 		Serial.print(F("deserializeJson() failed: "));
//     	Serial.println(error.f_str());
// 	}
// 	// Serial.println(resDict);
// 	resDict.shrinkToFit();
// 	return resDict;
// }

void printLocalTime(){
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}

float getRSSI() {
	float rssi = WiFi.RSSI();
	return rssi;
}

void addReading(float reading, tm* timeinfo) {
	String readingString = String(reading);
	char hour[3];
	strftime(hour,3,"%H",timeinfo);
	char minute[3];
	strftime(minute,3,"%M",timeinfo);
	char second[3];
	strftime(second,3,"%S",timeinfo);
	char datetime[19];
	strftime(datetime,19,"%F %T",timeinfo);
	char justDate[11];
	strftime(justDate,11,"%F",timeinfo);
	String reading_id = String(DEVICE_ID + DEVICE_TYPE + datetime);
	conn.upsertVertex("Reading", reading_id, "{\"value\":{\"value\":" + readingString + "},\"type\":{\"value\":\"" + DEVICE_TYPE + "\"},\"captured_at\":{\"value\":\"" + String(datetime) + "\"}}");
	conn.upsertEdge("Device", DEVICE_ID, "has_reading", "Reading", reading_id, "{}");
	conn.upsertEdge("Reading", reading_id, "on_day", "Day", String(justDate), "{}");
	conn.upsertEdge("Reading", reading_id, "on_hour", "Hour", hour, "{}");
	conn.upsertEdge("Reading", reading_id, "on_minute", "Minute", minute, "{}");
	conn.upsertEdge("Reading", reading_id, "on_second", "Second", second, "{}");
	conn.upsertEdge("Reading", reading_id, "of_type", "Reading_Type", DEVICE_TYPE, "{}");
}

void setupLux() {
	tsl.setGain(TSL2591_GAIN_MED);
	tsl.setTiming(TSL2591_INTEGRATIONTIME_300MS);
}

float getLux() {
	float lux = tsl.getLuminosity(TSL2591_VISIBLE);
	return lux;
}

void setStrip(uint32_t color) {
	for (int i=0; i<LED_COUNT; i++) {
		strip.setPixelColor(i, color);
	}
	strip.show();
}

String runQuery(String queryName, String params) {
	int contentLength = params.length();

	client.println(String("POST /restpp/query/MyGraph/"+queryName+" HTTP/1.1") + "\r\n" + 
	"Host: "+HOST_NAME+":443" + "\r\n" +
	"Authorization: Bearer "+ TOKEN + "\r\n" +
	"Content-Type: text/plain" + "\r\n" +
	"Content-Length: " + contentLength + "\r\n" +
	"\r\n" +
	params);
	String response = client.readString();
	return response;
}

// void parseLuxResults(){
// 	for(int i=0; i<8; i++){
// 		String results = resDict["results"][0]["(@@lightDates)"][i]["reading"];
// 		int brightness = results.toInt();
// 		// brightness *= 4;
// 		constrain(brightness, 0, 255);
// 		Serial.println(brightness);
// 		strip.setPixelColor(i, strip.Color(0,0,0,brightness));
// 	}
// 	strip.show();
// }