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
 * Get firmware version
 */
#define CMD_GET_VERSION "AT+GMR"

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
 * Disconnect from AP
 * AT+CWQAP
 */
#define CMD_WIFI_DISCONNECT "AT+CWQAP"

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
#define CMD_KAA_SEND_PROFILE "KAA+PRO"

/*
 * Send log record to Kaa server 
 */
#define CMD_KAA_LOG_UPLOAD "KAA+LOG"

/*
 * Get a config variable
 * KAA+CONFIG?key
 * value
 */
#define CMD_KAA_CONFIG_GETVAR "KAA+CONFIG?"

/*
 * Set a config variable
 * KAA+CONFIG=key value
 */
#define CMA_KAA_CONFIG_SETVAR "KAA+CONFIG="

#endif /* CMD_LIST_H */
