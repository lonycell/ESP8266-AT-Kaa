#ifndef CMD_LIST_H
#define CMD_LIST_H

/* 
 * Return values 
 */
#define CMD_ERR "ERR"
#define CMD_OK  "OK"

/* 
 * Check if system is working 
 */
#define CMD_CHECK    "AT"

/*
 * Reset system
 */
#define CMD_RST      "AT+RST"


/*
 * Show current ip address
 */
#define CMD_GET_IP   "AT+CIFSR"

/*
 * Connect to WiFi
 * AT+CWJAP <ssid> <pwd>
 */
#define CMD_WIFI_CONNECT "AT+CWJAP"

/*
 * Spawn a task with Kaa client main loop
 */
#define CMD_KAA_START    "KAA+START"
/*
 * Stop Kaa client
 */
#define CMD_KAA_STOP     "KAA+STOP"

/*
 * Send kaa_profile to Kaa server
 */
#define CMD_KAA_SEND_PROFILE "KAA+PROUPD"
#endif /* CMD_LIST_H */
