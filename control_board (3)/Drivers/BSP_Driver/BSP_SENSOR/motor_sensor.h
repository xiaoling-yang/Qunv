#ifndef MOTOR_SENSOR_H
#define MOTOR_SENSOR_H
#include "main.h"
#define SENSOR_COUNT 4
#define TIMEOUT 500
enum MOTOR_SENSOR {
    motor1,
    motor2,
    motor3,
    motor4,
};
typedef struct {
    GPIO_TypeDef* port;
    uint16_t pin;
    uint8_t last_level;
} Sensor_t;

uint8_t motor_sensor_level(uint8_t motor);//电机传感器读取电平变化
void motor_sensor_sw(uint8_t motor,uint8_t state);//电机传感器开关1打开0,4个sensor
void motor_sensor_init(void);
#endif
