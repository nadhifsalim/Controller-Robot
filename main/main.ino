#include <Bluepad32.h>

#define IN1 27
#define IN2 26
#define IN3 25
#define IN4 33

#define ENA 14
#define ENB 32

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

    if (RIGHT > 0){ 
        digitalWrite(IN1, HIGH);
        digitalWrite(IN2, LOW);
    } else if (RIGHT < 0){ 
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, HIGH);
    } else{ 
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, LOW); 
    }
    if (LEFT > 0){ 
        digitalWrite(IN3, HIGH);
        digitalWrite(IN4, LOW);
    } else if (LEFT < 0){ 
        digitalWrite(IN3, LOW);
        digitalWrite(IN4, HIGH);
    } else{ 
        digitalWrite(IN3, LOW);
        digitalWrite(IN4, LOW);
    }

    analogWrite(ENA, abs(RIGHT));
    analogWrite(ENB, abs(LEFT));
}

void processGamepad(ControllerPtr ctl) {
    int absis = ctl->axisX();
    int ordinat = ctl->axisY();

    int dead = 10;
    if (abs(absis)<dead) absis = 0;
    if (abs(ordinat)<dead) ordinat = 0;
        
    if (absis > 0){
        absis = (float)absis/512.0f;
    } else{
        absis = (float)absis/508.0f;
    }
    if (ordinat > 0){
        ordinat = (float)ordinat/-512.0f;
    } else{
        ordinat = (float)ordinat/-508.0f;
    }

    absis = absis * absis * absis; 
    ordinat = ordinat * ordinat * ordinat; 

    float right_motor = constrain(ordinat + absis, -1, 1);
    float left_motor = constrain(ordinat - absis, -1, 1);

    right_motor = constrain(right_motor*255, -255, 255);
    left_motor = constrain(upd_r_motor*255, -255, 255);

    rotatemotor(right_motor, left_motor);
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
