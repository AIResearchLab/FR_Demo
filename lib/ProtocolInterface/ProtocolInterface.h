#pragma once
#include <Arduino.h>
#include <list>
//#include <Joint.h>
using namespace std;

#define USB_SERIAL_BAUD 12000000
#define INPUT_BUFFER_SIZE 3

/**
 * Preform AND operation to get the lower 8 bit of a 16 bit type
 * @param b uint16_t to split
 * @returns uint8_t Lower 8 bits
 */
#define LOWER_BYTE(b) b & 0xff

/** 
 * Shift bits right 8 bits to get the upper 8 bits of a 16 bit type
 * @param b uint16_t to split
 * @returns uint8_t Upper 8 bits
 */
#define UPPER_BYTE(b) (b >> 8)

/**
 * Join two bytes to create a 16 bit integer
 * @param u Upper byte
 * @param l Lower byte
 * @returns uint16_t Reconstructed integer
 */
#define INT_JOIN_BYTE(u, l) (u << 8) | l
//Motion buffer status, 1 if still moving through the buffer
enum INTERFACE_DEST {
    SYSTEM = 20,
    META,
    CONFIG_OPT,
    JOINT_J0,
    JOINT_J1,
    JOINT_J2,
    JOINT_J3,
    SENSOR_IMU,
    SENSOR_ENCODINATOR_1,
    SENSOR_ENCODINATOR_2,
    SENSOR_ENCODINATOR_3,
    SENSOR_ENCODINATOR_4};

enum INTERFACE_CMD_DYNAMIXEL {
    DYN_TARGET_POSITION = 40,
    DYN_PRESENT_POSITION,
    DYN_TORQUE_LIMIT,
    DYN_TARGET_CURRENT,
    DYN_TORQUE_ENABLED,
    DYN_PRESENT_LOAD,
    DYN_TARGET_SPEED,
    DYN_PRESENT_SPEED,
    DYN_VOLTAGE,
    DYN_TEMPERATURE,
    DYN_ERROR_MSG,
    DYN_PASSTHROUGH,
    DYN_IS_MOVING};

enum INTERFACE_CMD_STEPPER {
    STP_TARGET_POSITION = 60,
    STP_PRESENT_POSITION,
    STP_PRESENT_SPEED,
    STP_TARGET_VELOCITY,
    STP_IS_MOVING,
    STP_ENABLED,
    STP_ENABLE,
    STP_ERROR_MSG};

enum INTERFACE_CMD_META {
    IDENTIFIER = 80,
    STATUS,
    DEBUG_ENABLED};

enum MSG_FRAGMENT {
    _DEST,
    _CMD,
    _UPPER_BYTE,
    _LOWER_BYTE};

/** Data received from an interface command */
struct interfaceObj_t {
    INTERFACE_DEST dest;   /**< Message processor destination */
    uint8_t cmd; /**< Command */
    uint16_t data;      /**< Payload */
};

class ProtocolInterface {
    public:
        //ProtocolInterface(HardwareSerial *serial, uint32_t baud);
        ProtocolInterface(Serial_ *serial, uint32_t baud);
        ProtocolInterface(HardwareSerial *serial, uint32_t baud);
        bool isAvailable();
        list<interfaceObj_t> read();
        void send(list<interfaceObj_t> msgs);
        void sendString(String input);
        void clean();
        void sendBuffer(uint16_t buffer[], int bufferSize);
    private:
        //HardwareSerial *_serial;
        Serial_ *_serial_usb;
        HardwareSerial *_serial_hw;
        bool isHardwareSerial;
};