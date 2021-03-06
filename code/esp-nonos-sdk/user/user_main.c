#include "ets_sys.h"
#include "osapi.h"
#include "os_type.h"
#include "user_config.h"
#include "user_interface.h"
#include "kame.h"
#include "stdout.h"
#include "espmissingincludes.h"
#include "espconn.h"
#include "mem.h"

static os_timer_t fail_timer;
#define user_procTaskPrio        0
#define user_procTaskQueueLen    10
os_event_t    user_procTaskQueue[user_procTaskQueueLen];

// TCP server code from http://www.esp8266.com/viewtopic.php?f=9&t=523
LOCAL uint16_t server_timeover = 60*60*12; // yes. 12h timeout. so what? :)
LOCAL struct espconn masterconn;
LOCAL volatile uint8_t running = 0;
LOCAL char lastCmd;

const char *msg_welcome = "Welcome to ESP8266 miniKame!\n\n";

LOCAL void ICACHE_FLASH_ATTR server_tcp_disconcb(void *arg) {
	struct espconn *pespconn = (struct espconn *) arg;

	os_printf("tcp connection disconnected\n");
}

LOCAL void ICACHE_FLASH_ATTR server_parse_data(char data) {
	switch (data) {
		case 'W':
		case 'w':
			kame_walk(1,550);
			running = 1;
			break;

		case 'L':
		case 'l':
			kame_turnL(1,550);
			running = 1;
			break;

		case 'R':
		case 'r':
			kame_turnR(1,550);
			running = 1;
			break;

		case 'S':
		case 's':
			//STOP
			running = 0;
			break;

		case 'P':
		case 'p':
			kame_pushUp(2,1400);
			break;

		case 'U':
		case 'u':
			kame_upDown(4,250);
			break;

		case 'J':
		case 'j':
			kame_jump();
			break;

		case 'H':
		case 'h':
			kame_hello();
			break;

		case 'M':
		case 'm':
			kame_moonwalkL(4,2000);
			break;

		case 'D':
		case 'd':
			kame_dance(2,1000);
			break;

		case '\n':
		case '\r':
			break;

		default:
			kame_home();
			break;
	}
	lastCmd = data;
}

static void ICACHE_FLASH_ATTR loop(os_event_t *events) {
	server_parse_data(events->par);

	if (running) {
		os_delay_us(100);
		if (running) {
			system_os_post(user_procTaskPrio, 0, events->par);
		}
	}
}

LOCAL void ICACHE_FLASH_ATTR server_tcp_recvcb(void *arg, char *pusrdata, unsigned short length) {
	struct espconn *pespconn = (struct espconn *) arg;

	os_printf(">'%s' ", pusrdata);

	for(int i = 0; i < length; i++) {
		os_printf("0x%02X ", pusrdata[i]);
		system_os_post(user_procTaskPrio, 0, pusrdata[i]);
	}
	os_printf("\n");
	// espconn_sent(pespconn, pusrdata, length); //echo
}


LOCAL void ICACHE_FLASH_ATTR tcpserver_connectcb(void *arg) {
	struct espconn *pespconn = (struct espconn *)arg;

	os_printf("tcp connection established\n");

	espconn_regist_recvcb(pespconn, server_tcp_recvcb);
	// espconn_regist_reconcb(pespconn, tcpserver_recon_cb);
	espconn_regist_disconcb(pespconn, server_tcp_disconcb);
	// espconn_regist_sentcb(pespconn, tcpclient_sent_cb);
	
	espconn_sent(pespconn, (uint8*)msg_welcome, os_strlen(msg_welcome));
}

void ICACHE_FLASH_ATTR server_start() {
	kame_init();
	masterconn.type = ESPCONN_TCP;
	masterconn.state = ESPCONN_NONE;
	masterconn.proto.tcp = (esp_tcp *)os_zalloc(sizeof(esp_tcp));
	masterconn.proto.tcp->local_port = 23;
	espconn_regist_connectcb(&masterconn, tcpserver_connectcb);
	espconn_accept(&masterconn);
	espconn_regist_time(&masterconn, server_timeover, 0);
}

static void ICACHE_FLASH_ATTR fail_timerfunc(void *arg) {
	os_printf("Connect seems to have failed. Going to sleep and trying again in 1s.\n");
	system_deep_sleep(1000*1000);
}

void ICACHE_FLASH_ATTR wifi_handle_event_cb(System_Event_t *evt) {
	os_printf("event %x\n", evt->event);
	switch (evt->event) {
		case EVENT_STAMODE_CONNECTED:
			os_printf("connect to ssid %s, channel %d\n",
					  evt->event_info.connected.ssid,
			 evt->event_info.connected.channel);
			break;
		case EVENT_STAMODE_DISCONNECTED:
			os_printf("disconnect from ssid %s, reason %d\n",
					  evt->event_info.disconnected.ssid,
			 evt->event_info.disconnected.reason);
			break;
		case EVENT_STAMODE_AUTHMODE_CHANGE:
			os_printf("mode: %d -> %d\n",
					  evt->event_info.auth_change.old_mode,
			 evt->event_info.auth_change.new_mode);
			break;
		case EVENT_STAMODE_GOT_IP:
			// Connection done, disable fail timer
			os_timer_disarm(&fail_timer);
			os_printf("ip:" IPSTR ",mask:" IPSTR ",gw:" IPSTR,
					  IP2STR(&evt->event_info.got_ip.ip),
					  IP2STR(&evt->event_info.got_ip.mask),
					  IP2STR(&evt->event_info.got_ip.gw));
			os_printf("\n");

			os_delay_us(1);
			server_start();
			break;
		default:
			break;
	}
}

void ICACHE_FLASH_ATTR init_done(void) {
	os_printf("Init done!\n");

	char ssid[32] = SSID;
	char password[64] = SSID_PASSWORD;
	struct station_config stationConf;
	// Set station mode
	wifi_set_opmode(STATION_MODE);

	// Don't check the mac addr
	stationConf.bssid_set = 0; 
	// Set ap settings
	os_memcpy(&stationConf.ssid, ssid, 32);
	os_memcpy(&stationConf.password, password, 64);
	wifi_station_set_config(&stationConf);
	wifi_station_disconnect();
	wifi_set_event_handler_cb(&wifi_handle_event_cb);
	os_printf("Connecting to Wifi\n");
	wifi_station_connect();
}

void ICACHE_FLASH_ATTR user_init() {
	system_timer_reinit();
	stdoutInit();
	system_init_done_cb(init_done);

	system_os_task(loop, user_procTaskPrio, user_procTaskQueue, user_procTaskQueueLen);
}
