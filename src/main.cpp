#include <Arduino.h>
#include <Dynamixel2Arduino.h>
#include <MX_28.h>
#include <ProtocolInterface.h>
#define DYNAMIXEL_TIMEOUT 60

 motor_settings jr0_config = {
    .id = 1,
    .protocolRev = 1.0,
    .lowerLimit = 0,
    .upperLimit = 4095,
    .initialTargetPosition = 2048,
    .timeout = DYNAMIXEL_TIMEOUT,
    .retry = -1
};

motor_settings jr1_config = {
    .id = 2,
    .protocolRev = 1.0,
    .lowerLimit = 0,
    .upperLimit = 4095,
    .initialTargetPosition = 2048,
    .timeout = DYNAMIXEL_TIMEOUT,
    .retry = -1
};

motor_settings jr2_config = {
    .id = 3,
    .protocolRev = 1.0,
    .lowerLimit = 0,
    .upperLimit = 4095,
    .initialTargetPosition = 2048,
    .timeout = DYNAMIXEL_TIMEOUT,
    .retry = -1
};

motor_settings jr3_config = {
    .id = 4,
    .protocolRev = 1.0,
    .lowerLimit = 0,
    .upperLimit = 4095,
    .initialTargetPosition = 2048,
    .timeout = DYNAMIXEL_TIMEOUT,
    .retry = -1
};

Dynamixel2Arduino dxl(Serial1, 2);
ProtocolInterface *pc_command_interface;
MX_28 *joint0;
MX_28 *joint1;
MX_28 *joint2;
MX_28 *joint3;
void setup() {
 
  // put your setup code here, to run once:
  pc_command_interface = new ProtocolInterface(&Serial, 9600);
  //Serial.begin(9600);
  //while(true){
  //pc_command_interface->sendString(String(22)+","+String(43)+","+String(54)+","+String(89));
  //delay(1000);
  //}
  dxl.setPortProtocolVersion(1.0);
  dxl.begin(115200);
  joint0 = new MX_28(&dxl,jr0_config);
  joint1 = new MX_28(&dxl,jr1_config);
  joint2 = new MX_28(&dxl,jr2_config);
  joint3 = new MX_28(&dxl,jr3_config);
}


interfaceObj_t getFeedbackObject(uint8_t destination, uint8_t CMD,uint16_t data_packet){
  return (interfaceObj_t){
      .dest = (INTERFACE_DEST)destination,
      .cmd = (byte)CMD,
      .data = (uint16_t)data_packet
    };
}

unsigned long motionFeedbackInterval = 50000;
unsigned long motionFeedbackLast = 0;
void getPositionFeedback(){
  unsigned long microsNow = micros();
  list<interfaceObj_t> payload;
  current_motor_information f_j0 = joint0->extrapolateFeedback();
  current_motor_information f_j1 = joint1->extrapolateFeedback();
  current_motor_information f_j2 = joint2->extrapolateFeedback();
  current_motor_information f_j3 = joint3->extrapolateFeedback();

  
  //pc_command_interface->sendString("Got here");
  //pc_command_interface->sendString(String(microsNow)+","+String(motionFeedbackInterval)+","+String(motionFeedbackLast));
  if(microsNow >= (motionFeedbackLast + motionFeedbackInterval)) {
    
    //pc_command_interface->sendString("Reached here");
    payload.push_back(getFeedbackObject(JOINT_J0,INTERFACE_CMD_DYNAMIXEL::DYN_PRESENT_POSITION,f_j0.f_presentPosition));
    payload.push_back(getFeedbackObject(JOINT_J1,INTERFACE_CMD_DYNAMIXEL::DYN_PRESENT_POSITION,f_j1.f_presentPosition)); 
    payload.push_back(getFeedbackObject(JOINT_J2,INTERFACE_CMD_DYNAMIXEL::DYN_PRESENT_POSITION,f_j2.f_presentPosition)); 
    payload.push_back(getFeedbackObject(JOINT_J3,INTERFACE_CMD_DYNAMIXEL::DYN_PRESENT_POSITION,f_j3.f_presentPosition));

    payload.push_back(getFeedbackObject(JOINT_J0,INTERFACE_CMD_DYNAMIXEL::DYN_PRESENT_SPEED,f_j0.f_presentVelocity));
    payload.push_back(getFeedbackObject(JOINT_J1,INTERFACE_CMD_DYNAMIXEL::DYN_PRESENT_SPEED,f_j1.f_presentVelocity)); 
    payload.push_back(getFeedbackObject(JOINT_J2,INTERFACE_CMD_DYNAMIXEL::DYN_PRESENT_SPEED,f_j2.f_presentVelocity)); 
    payload.push_back(getFeedbackObject(JOINT_J3,INTERFACE_CMD_DYNAMIXEL::DYN_PRESENT_SPEED,f_j3.f_presentVelocity));

    motionFeedbackLast = microsNow;
   
    //String stringtest =  String(f_j0.f_presentPosition)+","+String(f_j1.f_presentPosition)+","+String(f_j2.f_presentPosition)+","+String(f_j3.f_presentPosition);
    //String stringTwo =  String(f_j0.f_presentVelocity)+","+String(f_j1.f_presentVelocity)+","+String(f_j2.f_presentVelocity)+","+String(f_j3.f_presentVelocity); 

    //pc_command_interface->sendString(stringtest);
    //pc_command_interface->sendString(stringTwo);
    pc_command_interface->send(payload);
  }
  //
}

void checkIncomming() {
  if(pc_command_interface->isAvailable()) {
    list<interfaceObj_t> payload = pc_command_interface->read();

    for(interfaceObj_t data : payload) {
      //DEBUG_WRITE("INCOMING dest: " + (String)data.dest + ", cmd: " + (String)data.cmd + ", data: " + (String)data.data);
      switch(data.dest) {
        // Leg 1
        case JOINT_J0:
          if(data.cmd==DYN_TARGET_POSITION){
            joint0->updateGoalPosition(data.data);
          }
          break;
        case JOINT_J1:
          if(data.cmd==DYN_TARGET_POSITION){
            joint1->updateGoalPosition(data.data);
          }
          break;
        case JOINT_J2:
          if(data.cmd==DYN_TARGET_POSITION){
            joint2->updateGoalPosition(data.data);
          }
          break;
        case JOINT_J3:
          if(data.cmd==DYN_TARGET_POSITION){
            joint3->updateGoalPosition(data.data);
          }
          break;
        default:
          break;
      }
    }
  }
}

void sendMotionParameters(){
  joint0->sendMotionCommands();
  joint1->sendMotionCommands();
  joint2->sendMotionCommands();  
  joint3->sendMotionCommands();
}
void updateMotionFeedback(){
  joint0->updateMotionParameters();
  joint1->updateMotionParameters();
  joint2->updateMotionParameters();  
  joint3->updateMotionParameters();
}


//This region of code updates the dynamixel motions
unsigned long motionCommandInterval = 50000;
unsigned long motionCommandLast = 0;

unsigned long motionUpdateInterval = 50000;
unsigned long motionUpdateLast = 0;

void dynamixelInterfacing(){
  unsigned long microsNow = micros();
  if(microsNow >= (motionCommandLast + motionCommandInterval)) {
    //Serial.println("Hit motion command sending item");
    sendMotionParameters();
    motionCommandLast = microsNow;
  }

  if(microsNow >= (motionUpdateLast + motionUpdateInterval)) {
    //Serial.println("Hit motion update feedback item");
    updateMotionFeedback();
    motionUpdateLast = microsNow;
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  dynamixelInterfacing();
  getPositionFeedback();
  checkIncomming();
}