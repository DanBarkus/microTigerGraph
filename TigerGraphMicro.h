/*
    TigerGraphMicro.h - Library for connecting and communicating with TigerGraph.
    Created by Daniel Barkus, 08-31-21
*/

#ifndef TigerGraphMicro_h
#define TigerGraphMicro_h

#include "Arduino.h"
#include "ArduinoJson.h"
#include "WiFiClientSecure.h"


class TigerGraphMicro {
    public:
        TigerGraphMicro(const char* hostName, const char* username, const char* password, const char* token, WiFiClientSecure client, const char* cert);
        bool connect();
        StaticJsonDocument<2048> getJSON(String response);
        String makePost(String endpoint, String content);
        void upsertVertex(String vertexType, String vertexId, String attributes);
        void upsertEdge(String sourceVertexType, String sourceVertexId, String edgeType, String targetVertexType, String targetVertexId, String attributes);
        String runQuery(String queryName, String params);
    private:
        const char* _HOST_NAME = "";
        const char* _USERNAME = "tigergraph";
        const char* _PASSWORD = "tigergraph";
        WiFiClientSecure _CLIENT;
        const char* _TOKEN = "";
        const char* _CERT = "";
        const int _HTTPS_PORT = (int)443;
        StaticJsonDocument<2048> _resDict;
};

#endif