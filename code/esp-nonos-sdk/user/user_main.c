#include "ets_sys.h"
#include "osapi.h"
#include "os_type.h"
#include "user_config.h"
#include "user_interface.h"
#include "kame.h"
#include "stdout.h"
#include "espmissingincludes.h"
#include "espconn.h"

static os_timer_t fail_timer;

void ICACHE_FLASH_ATTR server_start() {
	kame_init();
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
}
