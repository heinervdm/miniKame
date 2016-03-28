#ifndef kame_h
#define kame_h

#define MAX_PULSE_WIDTH 1000
#define MIN_PULSE_WIDTH 2000

void ICACHE_FLASH_ATTR kame_init();
void ICACHE_FLASH_ATTR kame_run(float steps, float period);
void ICACHE_FLASH_ATTR kame_walk(float steps, float period);
//void ICACHE_FLASH_ATTR backward(float steps, float period);
void ICACHE_FLASH_ATTR kame_turnL(float steps, float period);
void ICACHE_FLASH_ATTR kame_turnR(float steps, float period);
void ICACHE_FLASH_ATTR kame_moonwalkL(float steps, float period);
void ICACHE_FLASH_ATTR kame_dance(float steps, float period);
void ICACHE_FLASH_ATTR kame_upDown(float steps, float period);
void ICACHE_FLASH_ATTR kame_pushUp(float steps, float period);
void ICACHE_FLASH_ATTR kame_hello();
void ICACHE_FLASH_ATTR kame_jump();
void ICACHE_FLASH_ATTR kame_home();
void ICACHE_FLASH_ATTR kame_zero();

void ICACHE_FLASH_ATTR kame_setServo(uint8_t id, float target);
void ICACHE_FLASH_ATTR kame_reverseServo(uint8_t id);
float ICACHE_FLASH_ATTR kame_getServo(uint8_t id);
void ICACHE_FLASH_ATTR kame_moveServos(int16_t time, float target[8]);
void ICACHE_FLASH_ATTR kame_execute(float steps, float period[8], int16_t amplitude[8], int16_t offset[8], int16_t phase[8]);

#endif
