#include <Arduino.h>
#include <actuator.h>
#include "MX_28.h"

MX_28::MX_28(Dynamixel2Arduino *dxl, motor_settings config)
{
    _dxl = dxl;
    _config = config;
    setOperatingMode(JOINT);
    flashLed();
    updateGoalVelocity(500);
    updateGoalPosition(_config.initialTargetPosition);
    sendMotionCommands();
}

void MX_28::flashLed()
{
    _dxl->ledOn(_config.id);
    delay(500);
    _dxl->ledOff(_config.id);
}

void MX_28::updateProtocal()
{
    float activeVersion = _dxl->getPortProtocolVersion();
    if (activeVersion != _config.protocolRev)
    {
        _dxl->setPortProtocolVersion(_config.protocolRev);
    }
}
void MX_28::writeControlTableItem(uint8_t itemIdx, uint32_t value)
{
    // return success or fail (inc. error code?), implement retry
    updateProtocal();
    _dxl->writeControlTableItem(itemIdx, _config.id, value, _config.timeout);
}

uint8_t MX_28::readOperatingMode()
{
    uint16_t mode = readControlTableItem(_OPERATING_MODE);
    return (uint8_t)mode;
}

uint16_t MX_28::readControlTableItem(uint8_t itemIdx)
{
    // Monitor success or fail, etc
    updateProtocal();
    uint16_t tmp = 0;

    for (int i = 0; i < 3; i++)
    {
        tmp = _dxl->readControlTableItem(itemIdx, _config.id, _config.timeout);
        int errorCode = _dxl->getLastLibErrCode();
        if (errorCode != 0)
        {
            //todo; nothing really, just if it errors out print some error message if needed
        }
        else
        {
            return tmp;
        }
    }

    return tmp;
}

void MX_28::setOperatingMode(MX_28_OperatingMode mode)
{
    _mode = mode;
    if (_mode == JOINT)
        setDynOperatingMode(OP_POSITION);
    else if (_mode == APPLIED_FORCE)
        setDynOperatingMode(OP_VELOCITY);
    else if (_mode == TORQUE_POSITION)
        setDynOperatingMode(OP_CURRENT_BASED_POSITION);
}

void MX_28::updateGoalPosition(uint16_t targetPosition)
{
    if (targetPosition > _config.upperLimit)
    {
        targetPosition = _config.upperLimit;
    }
    else if (targetPosition < _config.lowerLimit)
    {
        targetPosition = _config.lowerLimit;
    }
    goalPosition = targetPosition;
}

void MX_28::updateGoalVelocity(uint16_t targetVelocity)
{
    goalVelocity = targetVelocity;
}

void MX_28::updateMotionParameters()
{
    presentPosition = readControlTableItem(_PRESENT_POSITION);
    presentVelocity = readControlTableItem(_PRESENT_VELOCITY);
    //probably is a good thing to have maybe, idk
    //_isMoving = isMoving();
}

void MX_28::updateMetaData()
{
    updateTemperature();
    updateVoltageReading();
    updateAlarm();
}

void MX_28::updateTemperature()
{
    presentTemperature = readControlTableItem(_PRESENT_TEMPARTURE);
}

void MX_28::updateVoltageReading()
{
    presentVoltage = readControlTableItem(_PRESENT_VOLTAGE);
}

bool MX_28::isMoving()
{
    return (readControlTableItem(_IS_MOVING) == 0) ? false : true;
}

void MX_28::setGoalPosition(uint16_t targetPosition)
{
    goalPosition = targetPosition;
}

void MX_28::setGoalVelocity(uint16_t targetCurrent)
{
    goalVelocity = targetCurrent;
}

current_motor_information MX_28::extrapolateFeedback()
{
    current_motor_information information;
    information.f_alarmStatus = alarmStatus;
    information.f_presentPosition = presentPosition;
    information.f_presentTemperature = presentTemperature;
    information.f_presentVelocity = presentVelocity;
    information.f_presentVoltage = presentVoltage;
    return information;
}

void MX_28::sendMotionCommands()
{
    writeControlTableItem(_GOAL_VELOCITY, goalVelocity);
    writeControlTableItem(_GOAL_POSITION, goalPosition);
}

void MX_28::updateAlarm()
{
    alarmStatus = readControlTableItem(_ALARM_ERROR);
}

void MX_28::setDynOperatingMode(uint8_t mode)
{
    updateProtocal();
    _dxl->torqueOff(_config.id);

    uint16_t currentOPMode = readControlTableItem(_OPERATING_MODE);
    if ((uint8_t)currentOPMode != mode)
    {
        _dxl->setOperatingMode(_config.id, mode);
    }
    _dxl->torqueOn(_config.id);
}