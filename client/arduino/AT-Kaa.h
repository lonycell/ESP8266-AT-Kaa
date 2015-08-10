#ifndef AT_KAA_H
#define AT_KAA_H

class KaaAT {
public:
    KaaAT();
    bool wifiConnect(char* ssid, char* password);
    void wifiDisconnect();
    char* getip();
    bool check();

private:
    int checkATReturn();
};

#endif //AT_KAA_H
