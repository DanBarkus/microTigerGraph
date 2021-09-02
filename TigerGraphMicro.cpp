#include "Arduino.h"
#include "ArduinoJson.h"
#include "WiFiClientSecure.h"
#include "TigerGraphMicro.h"

TigerGraphMicro::TigerGraphMicro(const char* hostName, const char* username, const char* password, const char* token, WiFiClientSecure client, const char* cert) {
    _HOST_NAME = hostName;
    _USERNAME = username;
    _PASSWORD = password;
    _TOKEN = token;
    _CLIENT = client;
    _CERT = cert;
}

bool TigerGraphMicro::connect() {
    _CLIENT.setCACert(_CERT);
	if(!_CLIENT.connect(_HOST_NAME,_HTTPS_PORT)) {
        return false;
	}
	else {
        return true;
    };

}

String TigerGraphMicro::makePost(String endpoint, String content) {
    int contentLength = content.length();

	_CLIENT.println(String("POST /restpp/"+endpoint+" HTTP/1.1") + "\r\n" + 
	"Host: "+_HOST_NAME+":443" + "\r\n" +
	"Authorization: Bearer "+ _TOKEN + "\r\n" +
	"Content-Type: text/plain" + "\r\n" +
	"Content-Length: " + contentLength + "\r\n" +
	"\r\n" +
	content);
	String response = _CLIENT.readString();
	return response;
}

void TigerGraphMicro::upsertVertex(String vertexType, String vertexId, String attributes) {
	String content = "{\"vertices\":{\""+vertexType+"\":{\""+vertexId+"\":"+attributes+"}}}";
	String response = makePost("graph/MyGraph", content);
	getJSON(response);
}

void TigerGraphMicro::upsertEdge(String sourceVertexType, String sourceVertexId, String edgeType, String targetVertexType, String targetVertexId, String attributes) {
	String content = "{\"edges\":{\""+sourceVertexType+"\":{\""+sourceVertexId+"\":{\""+edgeType+"\":{\""+targetVertexType+"\":{\""+targetVertexId+"\":"+attributes+"}}}}}}";
	String response = makePost("graph/MyGraph", content);
	getJSON(response);
}

StaticJsonDocument<2048> TigerGraphMicro::getJSON(String response) {
	int resLen = response.length();
	char res[resLen+1];
	response.toCharArray(res, resLen);
	char * lines;
	char * dat;
	lines = strtok(res,"\n");
	while (lines != NULL)
	{
		dat = lines;
		lines = strtok(NULL,"\n");
	}
	int dl = strlen(dat);
	char data[dl+1];
	strcpy(data,dat);
	strcat(data + dl, "}");
	
	Serial.println(data);
	StaticJsonDocument<2048> resDict;
	DeserializationError error = deserializeJson(resDict, (const char*)data);
	if (error) {
		Serial.print(F("deserializeJson() failed: "));
    	Serial.println(error.f_str());
	}
	// Serial.println(resDict);
	return resDict;
}

String TigerGraphMicro::runQuery(String queryName, String params) {
    int contentLength = params.length();

	_CLIENT.println(String("POST /restpp/query/MyGraph/"+queryName+" HTTP/1.1") + "\r\n" + 
	"Host: "+_HOST_NAME+":443" + "\r\n" +
	"Authorization: Bearer "+ _TOKEN + "\r\n" +
	"Content-Type: text/plain" + "\r\n" +
	"Content-Length: " + contentLength + "\r\n" +
	"\r\n" +
	params);
	String response = _CLIENT.readString();
	return response;
}