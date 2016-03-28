#define USE_US_TIMER

#include "ets_sys.h"
#include "osapi.h"
#include "os_type.h"
#include "kame.h"
#include "gpio.h"
#include "espmissingincludes.h"

#define PI 3.1415

#ifndef DEG2RAD
  #define DEG2RAD(g) ((g)*M_PI)/180
#endif

struct oscillator {
	int16_t period;
	int16_t amplitude;
	int16_t phase;
	int16_t offset;
	int16_t trim;
	bool stop;
	uint32_t ref_time;
	uint32_t delta_time;
};

static struct oscillator osc[8];
uint16_t servo[8];
int16_t board_pins[8];
int16_t trim[8];
bool reverse[8];
uint32_t _init_time;
uint32_t _final_time;
uint32_t _partial_time;
float _increment[8];
float _servo_position[8];

static os_timer_t pulse_timer;
static os_timer_t servo_timer[8];

// https://stackoverflow.com/questions/18805627/decode-an-aproximation-of-sin-taylor-series
float sine(float x) {
    float val = 1;
	int j = 6;
    for (int k = j - 1; k >= 0; --k)
        val = 1 - x*x/(2*k+2)/(2*k+3)*val;

    return x * val;
}

float ICACHE_FLASH_ATTR time_to_radians(struct oscillator osc, double time) {
    return time*2*PI/osc.period;
}

float ICACHE_FLASH_ATTR degrees_to_radians(float degrees){
    return degrees*2*PI/360.;
}

float ICACHE_FLASH_ATTR degrees_to_time(struct oscillator osc, float degrees) {
    return degrees*osc.period/360.;
}

float ICACHE_FLASH_ATTR oscillator_refresh(struct oscillator osc) {
    osc.delta_time = (system_get_time()-osc.ref_time) % osc.period;
    return      (float)osc.amplitude*sine(time_to_radians(osc, osc.delta_time)
                + degrees_to_radians(osc.phase))
                + osc.offset
                + osc.trim;
}

void ICACHE_FLASH_ATTR oscillator_reset(struct oscillator osc) {
	osc.ref_time = system_get_time();
}

void ICACHE_FLASH_ATTR oscillator_init(struct oscillator osc) {
	osc.period = 2000;
    osc.amplitude = 50;
    osc.phase = 0;
    osc.offset = 0;
    osc.stop = false;
    osc.ref_time = system_get_time();
    osc.delta_time = 0;
    osc.trim = 0;
}

int16_t ICACHE_FLASH_ATTR angToUsec(float value){
    return value/180 * (MAX_PULSE_WIDTH-MIN_PULSE_WIDTH) + MIN_PULSE_WIDTH;
}

static void ICACHE_FLASH_ATTR servo0_timerfunc(void *arg) {
	GPIO_OUTPUT_SET(board_pins[0],0);
}

static void ICACHE_FLASH_ATTR servo1_timerfunc(void *arg) {
	GPIO_OUTPUT_SET(board_pins[1],0);
}

static void ICACHE_FLASH_ATTR servo2_timerfunc(void *arg) {
	GPIO_OUTPUT_SET(board_pins[2],0);
}

static void ICACHE_FLASH_ATTR servo3_timerfunc(void *arg) {
	GPIO_OUTPUT_SET(board_pins[3],0);
}

static void ICACHE_FLASH_ATTR servo4_timerfunc(void *arg) {
	GPIO_OUTPUT_SET(board_pins[4],0);
}

static void ICACHE_FLASH_ATTR servo5_timerfunc(void *arg) {
	GPIO_OUTPUT_SET(board_pins[5],0);
}

static void ICACHE_FLASH_ATTR servo6_timerfunc(void *arg) {
	GPIO_OUTPUT_SET(board_pins[6],0);
}

static void ICACHE_FLASH_ATTR servo7_timerfunc(void *arg) {
	GPIO_OUTPUT_SET(board_pins[7],0);
}

static void ICACHE_FLASH_ATTR pulse_timerfunc(void *arg) {
	os_timer_func_t *timerfuncarr[8] = {
		(os_timer_func_t *)servo0_timerfunc,
		(os_timer_func_t *)servo1_timerfunc,
		(os_timer_func_t *)servo2_timerfunc,
		(os_timer_func_t *)servo3_timerfunc,
		(os_timer_func_t *)servo4_timerfunc,
		(os_timer_func_t *)servo5_timerfunc,
		(os_timer_func_t *)servo6_timerfunc,
		(os_timer_func_t *)servo7_timerfunc
	};

	for (uint8_t i=0;i<8;i++) {
		GPIO_OUTPUT_SET(board_pins[i],1);
		os_timer_disarm(&servo_timer[i]);
		os_timer_setfn(&servo_timer[i], timerfuncarr[i], NULL);
		os_timer_arm_us(&servo_timer[i], servo[i], 0);
	}
}

void ICACHE_FLASH_ATTR kame_init(){
    board_pins[0] = 1;
    board_pins[1] = 4,
    board_pins[2] = 8;
    board_pins[3] = 6;
    board_pins[4] = 7;
    board_pins[5] = 5;
    board_pins[6] = 2;
    board_pins[7] = 3;

	os_timer_disarm(&pulse_timer);
	os_timer_setfn(&pulse_timer, (os_timer_func_t *)pulse_timerfunc, NULL);
	os_timer_arm(&pulse_timer, 20, 1);

    trim[0] = 1;
    trim[1] = -8;
    trim[2] = 6;
    trim[3] = 4;
    trim[4] = 2;
    trim[5] = -5;
    trim[6] = 6;
    trim[7] = 2;
    for (uint8_t i=0; i<8; i++) reverse[i] = 0;


    for(uint8_t i=0; i<8; i++) osc[i].trim = trim[i];
//     for(uint8_t i=0; i<8; i++) servo[i].attach(board_pins[i]);
    kame_home();
}

void ICACHE_FLASH_ATTR kame_turnR(float steps, float T){
    int16_t x_amp = 15;
    int16_t z_amp = 15;
    int16_t ap = 15;
    int16_t hi = 23;
    float period[] = {T, T, T, T, T, T, T, T};
    int16_t amplitude[] = {x_amp,x_amp,z_amp,z_amp,x_amp,x_amp,z_amp,z_amp};
    int16_t offset[] = {90+ap,90-ap,90-hi,90+hi,90-ap,90+ap,90+hi,90-hi};
    int16_t phase[] = {0,180,90,90,180,0,90,90};

    kame_execute(steps, period, amplitude, offset, phase);
}

void ICACHE_FLASH_ATTR kame_turnL(float steps, float T){
    int16_t x_amp = 15;
    int16_t z_amp = 15;
    int16_t ap = 15;
    int16_t hi = 23;
    float period[] = {T, T, T, T, T, T, T, T};
    int16_t amplitude[] = {x_amp,x_amp,z_amp,z_amp,x_amp,x_amp,z_amp,z_amp};
    int16_t offset[] = {90+ap,90-ap,90-hi,90+hi,90-ap,90+ap,90+hi,90-hi};
    int16_t phase[] = {180,0,90,90,0,180,90,90};

    kame_execute(steps, period, amplitude, offset, phase);
}

void ICACHE_FLASH_ATTR kame_dance(float steps, float T){
    int16_t x_amp = 0;
    int16_t z_amp = 40;
    int16_t ap = 30;
    int16_t hi = 20;
    float period[] = {T, T, T, T, T, T, T, T};
    int16_t amplitude[] = {x_amp,x_amp,z_amp,z_amp,x_amp,x_amp,z_amp,z_amp};
    int16_t offset[] = {90+ap,90-ap,90-hi,90+hi,90-ap,90+ap,90+hi,90-hi};
    int16_t phase[] = {0,0,0,270,0,0,90,180};

    kame_execute(steps, period, amplitude, offset, phase);
}

void ICACHE_FLASH_ATTR kame_run(float steps, float T){
    int16_t x_amp = 15;
    int16_t z_amp = 15;
    int16_t ap = 15;
    int16_t hi = 15;
    int16_t front_x = 6;
    float period[] = {T, T, T, T, T, T, T, T};
    int16_t amplitude[] = {x_amp,x_amp,z_amp,z_amp,x_amp,x_amp,z_amp,z_amp};
    int16_t offset[] = {    90+ap-front_x,
                        90-ap+front_x,
                        90-hi,
                        90+hi,
                        90-ap-front_x,
                        90+ap+front_x,
                        90+hi,
                        90-hi
                    };
    int16_t phase[] = {0,0,90,90,180,180,90,90};

    kame_execute(steps, period, amplitude, offset, phase);
}

void ICACHE_FLASH_ATTR kame_moonwalkL(float steps, float T){
    int16_t z_amp = 45;
    float period[] = {T, T, T, T, T, T, T, T};
    int16_t amplitude[] = {0,0,z_amp,z_amp,0,0,z_amp,z_amp};
    int16_t offset[] = {90, 90, 90, 90, 90, 90, 90, 90};
    int16_t phase[] = {0,0,0,120,0,0,180,290};

    kame_execute(steps, period, amplitude, offset, phase);
}

void ICACHE_FLASH_ATTR kame_walk(float steps, float T){
    volatile int16_t x_amp = 15;
    volatile int16_t z_amp = 20;
    volatile int16_t ap = 20;
    volatile int16_t hi = 10;
    volatile int16_t front_x = 12;
    volatile float period[] = {T, T, T/2, T/2, T, T, T/2, T/2};
    volatile int16_t amplitude[] = {x_amp,x_amp,z_amp,z_amp,x_amp,x_amp,z_amp,z_amp};
    volatile int16_t offset[] = {   90+ap-front_x,
                                90-ap+front_x,
                                90-hi,
                                90+hi,
                                90-ap-front_x,
                                90+ap+front_x,
                                90+hi,
                                90-hi
                    };
    volatile int16_t  phase[] = {90, 90, 270, 90, 270, 270, 90, 270};

    for (uint8_t  i=0; i<8; i++){
        osc[i].ref_time = system_get_time();
        osc[i].period = period[i];
        osc[i].amplitude = amplitude[i];
        osc[i].phase = phase[i];
        osc[i].offset = offset[i];
    }

    _final_time = system_get_time() + period[0]*steps;
    _init_time = system_get_time();
    bool side;
    while (system_get_time() < _final_time){
        side = (int)((system_get_time()-_init_time) / (period[0]/2)) % 2;
        kame_setServo(0, oscillator_refresh(osc[0]));
        kame_setServo(1, oscillator_refresh(osc[1]));
        kame_setServo(4, oscillator_refresh(osc[4]));
        kame_setServo(5, oscillator_refresh(osc[5]));

        if (side == 0){
            kame_setServo(3, oscillator_refresh(osc[3]));
            kame_setServo(6, oscillator_refresh(osc[6]));
        }
        else{
            kame_setServo(2, oscillator_refresh(osc[2]));
            kame_setServo(7, oscillator_refresh(osc[7]));
        }
        os_delay_us(1000);
    }
}

void ICACHE_FLASH_ATTR kame_upDown(float steps, float T){
    int16_t x_amp = 0;
    int16_t z_amp = 35;
    int16_t ap = 20;
    int16_t hi = 25;
    int16_t front_x = 0;
    float period[] = {T, T, T, T, T, T, T, T};
    int16_t amplitude[] = {x_amp,x_amp,z_amp,z_amp,x_amp,x_amp,z_amp,z_amp};
    int16_t offset[] = {    90+ap-front_x,
                        90-ap+front_x,
                        90-hi,
                        90+hi,
                        90-ap-front_x,
                        90+ap+front_x,
                        90+hi,
                        90-hi
                    };
    int16_t phase[] = {0,0,90,270,180,180,270,90};

    kame_execute(steps, period, amplitude, offset, phase);
}

void ICACHE_FLASH_ATTR kame_pushUp(float steps, float T){
    int16_t z_amp = 40;
    int16_t x_amp = 65;
    int16_t hi = 30;
    float period[] = {T, T, T, T, T, T, T, T};
    int16_t amplitude[] = {0,0,z_amp,z_amp,0,0,0,0};
    int16_t offset[] = {90,90,90-hi,90+hi,90-x_amp,90+x_amp,90+hi,90-hi};
    int16_t phase[] = {0,0,0,180,0,0,0,0};

    kame_execute(steps, period, amplitude, offset, phase);
}

void ICACHE_FLASH_ATTR kame_hello(){
    float sentado[]={90+15,90-15,90-65,90+65,90+20,90-20,90+10,90-10};
    kame_moveServos(150, sentado);
    for (uint8_t i=0;i<200;i++) os_delay_us(1000);

    int16_t z_amp = 40;
    int16_t x_amp = 60;
    int16_t T=350;
    float period[] = {T, T, T, T, T, T, T, T};
    int16_t amplitude[] = {0,50,0,50,0,0,0,0};
    int16_t offset[] = {90+15,40,90-65,90,90+20,90-20,90+10,90-10};
    int16_t phase[] = {0,0,0,90,0,0,0,0};

    kame_execute(4, period, amplitude, offset, phase);

    float goingUp[]={160,20,90,90,90-20,90+20,90+10,90-10};
    kame_moveServos(500, goingUp);
    for (uint8_t i=0;i<200;i++) os_delay_us(1000);
}

void ICACHE_FLASH_ATTR kame_jump(){
    float sentado[]={90+15,90-15,90-65,90+65,90+20,90-20,90+10,90-10};
    int16_t ap = 20;
    int16_t hi = 35;
    float salto[] = {90+ap,90-ap,90-hi,90+hi,90-ap*3,90+ap*3,90+hi,90-hi};
    kame_moveServos(150, sentado);
    for (uint8_t i=0;i<200;i++) os_delay_us(1000);
    kame_moveServos(0, salto);
    for (uint8_t i=0;i<100;i++) os_delay_us(1000);
    kame_home();
}

void ICACHE_FLASH_ATTR kame_home(){
    int16_t ap = 20;
    int16_t hi = 35;
    int16_t position[] = {90+ap,90-ap,90-hi,90+hi,90-ap,90+ap,90+hi,90-hi};
    for (uint8_t i=0; i<8; i++) kame_setServo(i, position[i]);
}

void ICACHE_FLASH_ATTR kame_zero(){
    for (uint8_t i=0; i<8; i++) kame_setServo(i, 90);
}

void ICACHE_FLASH_ATTR kame_reverseServo(uint8_t id){
    if (reverse[id])
        reverse[id] = 0;
    else
        reverse[id] = 1;
}

void ICACHE_FLASH_ATTR kame_setServo(uint8_t id, float target){
    if (!reverse[id])
        servo[id] = (angToUsec(target+trim[id]));
    else
        servo[id] = (angToUsec(180-(target+trim[id])));
    _servo_position[id] = target;
}

float ICACHE_FLASH_ATTR kame_getServo(uint8_t id){
    return _servo_position[id];
}

void ICACHE_FLASH_ATTR kame_moveServos(int16_t time, float target[8]) {
    if (time>10){
        for (uint8_t i = 0; i < 8; i++)	_increment[i] = (target[i] - _servo_position[i]) / (time / 10.0);
        _final_time =  system_get_time() + time;

        while (system_get_time() < _final_time){
            _partial_time = system_get_time() + 10;
            for (uint8_t i = 0; i < 8; i++) kame_setServo(i, _servo_position[i] + _increment[i]);
            while (system_get_time() < _partial_time); //pause
        }
    }
    else{
        for (uint8_t i = 0; i < 8; i++) kame_setServo(i, target[i]);
    }
    for (uint8_t i = 0; i < 8; i++) _servo_position[i] = target[i];
}

void ICACHE_FLASH_ATTR kame_execute(float steps, float period[8], int16_t amplitude[8], int16_t offset[8], int16_t phase[8]){
    for (uint8_t i=0; i<8; i++){
        osc[i].period = period[i];
        osc[i].amplitude = amplitude[i];
        osc[i].phase = phase[i];
        osc[i].offset = offset[i];
    }

    uint32_t global_time = system_get_time();

    for (uint8_t i=0; i<8; i++) osc[i].ref_time = global_time;

    _final_time = system_get_time() + period[0]*steps;
    while (system_get_time() < _final_time){
        for (uint8_t i=0; i<8; i++){
            kame_setServo(i, oscillator_refresh(osc[i]));
        }
        os_delay_us(1);
    }
}
