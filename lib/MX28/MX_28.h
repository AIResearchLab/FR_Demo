#include <Arduino.h>
#include <Dynamixel2Arduino.h>

using namespace ControlTableItem;

enum commands{
    _OPERATING_MODE=OPERATING_MODE,
    _TORQUE_ENABLE=TORQUE_ENABLE,
    _LED=LED,
    _ALARM_ERROR=HARDWARE_ERROR_STATUS,
    _GOAL_VELOCITY=MOVING_SPEED,
    _GOAL_POSITION=GOAL_POSITION,
    _IS_MOVING=MOVING,
    _PRESENT_CURRENT=PRESENT_CURRENT,
    _PRESENT_VELOCITY=PRESENT_SPEED,
    _PRESENT_POSITION=PRESENT_POSITION,
    _PRESENT_VOLTAGE=PRESENT_VOLTAGE,
    _PRESENT_TEMPARTURE=PRESENT_TEMPERATURE
};

enum MX_28_OperatingMode {
    JOINT,
    APPLIED_FORCE,
    TORQUE_POSITION
};

struct motor_settings{
    uint8_t id;
    float protocolRev;
    uint16_t lowerLimit;
    uint16_t upperLimit;
    uint16_t initialTargetPosition;
    uint32_t timeout;
    int retry; // -1 for no retry
    };

struct current_motor_information{
    uint8_t f_alarmStatus;
    uint16_t f_presentPosition;
    uint16_t f_presentVelocity;
    uint8_t f_presentVoltage;
    uint8_t f_presentTemperature;
    bool f_isMoving;
    };

class MX_28 {
    public:
        MX_28(Dynamixel2Arduino *dxl, motor_settings config);
        void writeControlTableItem(uint8_t itemIdx, uint32_t value);
        uint16_t readControlTableItem(uint8_t itemIdx);        
        void setOperatingMode(MX_28_OperatingMode mode);
        void updateGoalPosition(uint16_t targetPosition);
        void updateGoalVelocity(uint16_t targetVelocity);
        void updateMotionParameters();
        void updateVoltageReading();
        void updateTemperature();
        void updateAlarm();
        void setGoalPosition(uint16_t targetPosition);
        void setGoalVelocity(uint16_t targetVelocity);
        current_motor_information extrapolateFeedback();
        void updateMetaData();
        void sendMotionCommands();
        void flashLed();        
    private:
        motor_settings _config;
        MX_28_OperatingMode _mode;
        Dynamixel2Arduino *_dxl;
        uint16_t goalPosition;
        uint16_t goalVelocity;
        uint8_t alarmStatus;
        uint16_t presentPosition;
        uint16_t presentVelocity;
        uint16_t presentCurrent;
        uint8_t presentVoltage;
        uint8_t presentTemperature;
        bool _isMoving;
        void setDynOperatingMode(uint8_t mode);
        uint8_t readOperatingMode();
        bool isMoving();
        void updateProtocal();
    };