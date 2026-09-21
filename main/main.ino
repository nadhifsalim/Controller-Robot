#include <Bluepad32.h>

// in1 & in2 right motor
#define IN1 27
#define IN2 26
// in3 & in4 left motor
#define IN3 25
#define IN4 33
//ena = right & enb = left
#define ENA 14
#define ENB 32

int targetL = 0;
int targetR = 0;

ControllerPtr myControllers[BP32_MAX_GAMEPADS];

void onConnectedController(ControllerPtr ctl) {
    bool foundEmptySlot = false;
    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
        if (myControllers[i] == nullptr) {
            Serial.printf("CALLBACK: Controller is connected, index=%d\n", i);
            ControllerProperties properties = ctl->getProperties();
            Serial.printf("Controller model: %s, VID=0x%04x, PID=0x%04x\n", ctl->getModelName().c_str(), properties.vendor_id,
                           properties.product_id);
            myControllers[i] = ctl;
            foundEmptySlot = true;
            break;
        }
    }
    if (!foundEmptySlot) {
        Serial.println("CALLBACK: Controller connected, but could not found empty slot");
    }
}

void onDisconnectedController(ControllerPtr ctl) {
    bool foundController = false;

    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
        if (myControllers[i] == ctl) {
            Serial.printf("CALLBACK: Controller disconnected from index=%d\n", i);
            myControllers[i] = nullptr;
            foundController = true;
            break;
        }
    }

    if (!foundController) {
        Serial.println("CALLBACK: Controller disconnected, but not found in myControllers");
    }
}

void rotatemotor(int LEFT, int RIGHT){

    if (RIGHT > 0){ //FORWARD
        digitalWrite(IN1, HIGH);
        digitalWrite(IN2, LOW);
        // L_motor = 'F';
    } else if (RIGHT < 0){ //BACKWARD
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, HIGH);
        // L_motor = 'B';
    } else{ //STOP
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, LOW); 
        // L_motor = 'S';
    }
    //right motor
    if (LEFT > 0){ //FORWARD
        digitalWrite(IN3, HIGH);
        digitalWrite(IN4, LOW);
        // R_motor = 'F';
    } else if (LEFT < 0){ //BACKWARD
        digitalWrite(IN3, LOW);
        digitalWrite(IN4, HIGH);
        // R_motor = 'B';
    } else{ //STOP
        digitalWrite(IN3, LOW);
        digitalWrite(IN4, LOW);
        // R_motor = 'S';
    }

    ledcWrite(0, abs(RIGHT));
    ledcWrite(1, abs(LEFT));
}

void processGamepad(ControllerPtr ctl) {
    int absis = ctl->axisX();
    int ordinat = ctl->axisY();

    int dead = 10; //deadpoint of controller
    if (abs(absis)<dead) absis = 0;
    if (abs(ordinat)<dead) ordinat = 0;
        
    float upd_absis = 0.0f;
    float upd_ordinat = 0.0f;
    if (absis > 0){
        upd_absis = (float)absis/512.0f;
    } else{
        upd_absis = (float)absis/508.0f;
    }
    if (ordinat > 0){
        upd_ordinat = (float)ordinat/-512.0f;
    } else{
        upd_ordinat = (float)ordinat/-508.0f;
    }

    upd_absis = upd_absis * upd_absis * upd_absis; 
    upd_ordinat = upd_ordinat * upd_ordinat * upd_ordinat; 

    float upd_l_motor = constrain(upd_ordinat + upd_absis, -1, 1);
    float upd_r_motor = constrain(upd_ordinat - upd_absis, -1, 1);

    targetL = constrain(upd_l_motor*255, -255, 255);
    targetR = constrain(upd_r_motor*255, -255, 255);

    rotatemotor(targetL, targetR);
}

void processControllers() {
    for (auto myController : myControllers) {
        if (myController && myController->isConnected() && myController->hasData()) {
            if (myController->isGamepad()) {
                processGamepad(myController);
            } else {
                Serial.println("Unsupported controller");
            }
        }
    }
}

void setup() {
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);
    ledcSetup(0, 5000, 8);
    ledcAttachPin(ENA, 0);
    ledcSetup(1, 5000, 8);
    ledcAttachPin(ENB, 1);

    Serial.begin(115200);

    Serial.printf("Firmware: %s\n", BP32.firmwareVersion());
    const uint8_t* addr = BP32.localBdAddress();
    Serial.printf("BD Addr: %2X:%2X:%2X:%2X:%2X:%2X\n", addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);

    BP32.setup(&onConnectedController, &onDisconnectedController);

    // BP32.forgetBluetoothKeys();

    BP32.enableVirtualDevice(false);
}

void loop() {
    bool dataUpdated = BP32.update();
    if (dataUpdated) processControllers();
    
    vTaskDelay(1);
}
