#include "AT-Kaa.h"
#include "Arduino.h"

// TODO: copy cmd list to arduino lib dir when compiling sketch
// to obtain latest cmd list
#include "../../firmware/include/cmd_list.h"

#define KAA_AT_SERIAL_BAUDRATE 115200
#define KAA_AT_MAXRET_CHARS 3
#define KAA_AT_DATA_POOL_SIZE 1024

/*void serialEvent() {
    char *str = 
}*/

char dataPool[KAA_AT_DATA_POOL_SIZE];

KaaAT::KaaAT() {
}

bool KaaAT::Init() {
    Serial.begin(KAA_AT_SERIAL_BAUDRATE);
    return check();
}

bool KaaAT::check() {
    Serial.write(CMD_CHECK"\n");
    delay(5);
    if(checkATReturn())
        return false;
    return true;
}

bool KaaAT::wifiConnect(char* ssid, char* password) {
    if(!check())
        return false;
    Serial.write(CMD_WIFI_CONNECT " " ssid " " password "\n");
    delay(5);
    if(!checkATReturn())
        return false;
    delay(3000);
    return checkATReturn();
}

void KaaAT::wifiDisconnect() {
    if(!check())
        return;
    Serial.write(CMD_WIFI_DISCONNECT "\n");
}

bool KaaAT::startKaa() {
    if(!check())
        return false;
    Serial.print(CMD_KAA_START "\n");
    return checkATReturn();
}

char* KaaAT::getip() {
    if(!check)
        return;
    Serial.write(CMD_GET_IP"\n");
    delay(100);
    int i = 0;
    char ret[32];
    while(Serial.available()&&i<31)
        ret[i++] = Serial.read();
    ret[i] = 0;
    return strdup(ret);
}

int KaaAT::checkATReturn() {
    char c[KAA_AT_MAXRET_CHARS];
    for(int i = 0;i <KAA_AT_MAXRET_CHARS && Serial.available()>0; i++)
        c[i] = Serial.read();
    if(memcmp(c, CMD_OK, strlen(CMD_OK))==0)
        return 0;
    //TODO: add return codes for "ERR" message, e.g. "ERR 1"
    return 1;
}

