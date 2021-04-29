#include "ProtocolInterface.h"
#include <ArduinoSTL.h>

bool ProtocolInterface::isAvailable() {
    if (isHardwareSerial){
    return (_serial_hw->available() > 0);
    }else{
    return (_serial_usb->available() > 0);
    }
    
}

list<interfaceObj_t> ProtocolInterface::read() {
    char header[2];
    if (isHardwareSerial){
        _serial_hw->readBytes(header, 2);
    }else{
        _serial_usb->readBytes(header, 2);
    }

    //if(header[0] != 0xFF) return; // Serious issues should occur here
    byte payloadSize = header[1];

    char payloadFooter[payloadSize + 2];
    if (isHardwareSerial){
        _serial_hw->readBytes(payloadFooter, payloadSize + 2);
    }else{
        _serial_usb->readBytes(payloadFooter, payloadSize + 2);
    }
    list<interfaceObj_t> payload;

    for(int i = 0; i < payloadSize / 4; i++) {
        payload.push_back((interfaceObj_t){
            //.idx = static_cast<PROTOCOL_IDX>(payloadFooter[i*3]),
            .dest = static_cast<INTERFACE_DEST>(payloadFooter[i*4+(byte)MSG_FRAGMENT::_DEST]),
            .cmd = payloadFooter[i * 4 + (byte)MSG_FRAGMENT::_CMD],
            .data = INT_JOIN_BYTE(payloadFooter[i*4+MSG_FRAGMENT::_UPPER_BYTE], payloadFooter[i*4+MSG_FRAGMENT::_LOWER_BYTE])
        });
    }

    return payload;
}

void ProtocolInterface::sendString(String input){
    if (isHardwareSerial){
    _serial_hw->println(input);
    }else{
    _serial_usb->println(input);
    }
    }

void ProtocolInterface::clean(){
    if (isHardwareSerial){
    _serial_hw->flush();
    }else{
    _serial_usb->flush();
    }
}

void ProtocolInterface::send(list<interfaceObj_t> msgs) {
    uint8_t payloadSize = msgs.size() * 4;
    byte msg[payloadSize + 4]; // Payload size + 2 header + 2 lower

    int i = 0;
    // Header
    msg[i] = 0xFF;
    msg[++i] = payloadSize;

    // Payload
    for(interfaceObj_t const &payloadMsg : msgs) {
        msg[++i] = payloadMsg.dest;
        msg[++i] = payloadMsg.cmd;
        msg[++i] = UPPER_BYTE(payloadMsg.data);
        msg[++i] = LOWER_BYTE(payloadMsg.data);
    }

    // Footer
    msg[++i] = 0x00;
    msg[++i] = 0x00;

    // Send
    if (isHardwareSerial){
    _serial_hw->write(msg, payloadSize + 4);
    }else{
    _serial_usb->write(msg, payloadSize + 4);
    }
}

void ProtocolInterface::sendBuffer(uint16_t buffer[], int bufferSize) {
    byte outBuffer[bufferSize*2];

    for(int i = 0; i < bufferSize; i++) {
        outBuffer[i*2] = LOWER_BYTE(buffer[i]);
        outBuffer[i*2+1] = UPPER_BYTE(buffer[i]);
    }
    if (isHardwareSerial){
    _serial_hw->write(outBuffer, 32);
    }else{
    _serial_usb->write(outBuffer, 32);
    }
}

ProtocolInterface::ProtocolInterface(Serial_ *serial, uint32_t baud) {
    isHardwareSerial = false;
    _serial_usb = serial;
    _serial_usb->begin(baud);
}

ProtocolInterface::ProtocolInterface(HardwareSerial *serial, uint32_t baud) {
    isHardwareSerial = true;
    _serial_hw = serial;
    _serial_hw->begin(baud);
}