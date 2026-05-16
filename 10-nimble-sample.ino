
//引入檔頭，需先在manage libraies安裝NIMBLE-Arduino by h2zero


#include <NimBLEDevice.h>


#define SERVICE_UUID   "6e400001-b5a3-f393-e0a9-e50e24dcca9e"
#define CHAR_UUID_RX   "6e400002-b5a3-f393-e0a9-e50e24dcca9e"
#define CHAR_UUID_TX   "6e400003-b5a3-f393-e0a9-e50e24dcca9e"

// #define SERVICE_UUID   "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"  // 整個 UART Service 的 UUID
// #define CHAR_UUID_RX   "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"  // RX：手機傳資料給 ESP32
// #define CHAR_UUID_TX   "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"  // TX：ESP32 傳資料給手機

#define LED_PIN 2

NimBLECharacteristic * pTxChar = nullptr;

bool deviceConnected = false;

// 連線時會呼叫以下函式
class ServerCallbacks: public NimBLEServerCallbacks{

  void onConnect(NimBLEServer * pServer, NimBLEConnInfo &coninfo) override{
    deviceConnected = true;
    Serial.println("客戶端連線");

  }
  void onDisconnect(NimBLEServer * pServer, NimBLEConnInfo &coninfo , int reason) override{
    deviceConnected = false;
    Serial.println("客戶端斷線，重新廣播");
    NimBLEDevice::startAdvertising();

  }
};

// 收到手機或客戶端發送訊息
class RxCallbacks: public NimBLECharacteristicCallbacks {

  
  void onWrite( NimBLECharacteristic * pChar, NimBLEConnInfo &connInfo) override {

    if(!deviceConnected){
      Serial.println("未連線卻Characteristic被呼叫");
      return;
    }
    std::string value = pChar->getValue();

    if(value.empty())
    {
      Serial.println("客戶端送空資料過來");
      return;
    }
    //取一個字元當作命令
    char cmd = value[0];
    Serial.print("收到指令是  : ");
    Serial.print(cmd);
    switch(cmd){
       case '0':
        Serial.println("關閉Led");
        digitalWrite(LED_PIN,LOW);
        //通知客戶端
        pTxChar->setValue("LED已關閉\n");
        pTxChar->notify();
        break;
       case '1':    
        Serial.println("開啟Led");
        digitalWrite(LED_PIN,HIGH);
        //通知客戶端
        pTxChar->setValue("LED已開啟\n");
        pTxChar->notify();
        break;
       case 'f':
        Serial.println("前進");
        pTxChar->setValue("收到前進 \n");
        pTxChar->notify();
        break;
       case 'b':
        Serial.println("後退");
        pTxChar->setValue("收到後退 \n");
        pTxChar->notify();
        break;
       case 'l':
        Serial.println("原地左轉");
        pTxChar->setValue("收到原地左轉 \n");
        pTxChar->notify();
        break;
       case 'r':
        Serial.println("原地右轉");
        pTxChar->setValue("收到原地右轉 \n");
        pTxChar->notify();
        break;
       default:
        pTxChar->setValue("未知指令，請重傳指令\n");
        pTxChar->notify();
        Serial.print("通知客戶指令未知");          
        
        break;           
    }


  }

};

//初始化



void setup() {
  // put your setup code here, to run once:
  //esp32的Serial Baud rate是115200
  Serial.begin(115200);
  pinMode(LED_PIN,OUTPUT);
  digitalWrite(LED_PIN,HIGH);

  //初始化ESP裡面的BLE，掃描設備中可以從清單看到
  NimBLEDevice::init("David_Bluetooth");
  // 設定 MTU 為 512 bytes（預設只有 23 bytes，實際可用僅 20 bytes）
  // 協商後最多可傳約 509 bytes，足以傳送較長的中文字串或 JSON 資料
  // 注意：需要 App 端也同步呼叫 requestMtu(512) 才能生效
  NimBLEDevice::setMTU(512);
  
  //建立伺服器
  NimBLEServer * pServer = NimBLEDevice::createServer();
  //註冊連線與斷線之回呼函式，
  pServer->setCallbacks(new ServerCallbacks());

  //建立UART Service，綁定UUID
  NimBLEService * pService = pServer->createService(SERVICE_UUID);

  //建立TX Characteristic (ESP32->客戶端)
  //NOTIFY: 廣播由ESP32主動推送資料給客戶端
  pTxChar = pService->createCharacteristic(CHAR_UUID_TX, NIMBLE_PROPERTY::NOTIFY);

  //建立RX，接收從客戶端傳來的訊息
  // NR (No Response)，不用等待回應，減少延遲

  NimBLECharacteristic * pRxChar = pService->createCharacteristic(
    CHAR_UUID_RX,
    NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::WRITE_NR
  );

  pRxChar->setCallbacks(new RxCallbacks());

  //啟動服務
  pServer->start();

  //設定廣播，讓客戶端可以配對


  NimBLEAdvertising * pAdvertising = NimBLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->enableScanResponse(true);
  pAdvertising->start();


  //NimBLEDevice::startAdvertising();
  Serial.println("BLE 廣播中，等待客戶端連線 ...");
  Serial.print("裝置名稱 : ");
  Serial.println("David_Bluetooth");
  Serial.print("MAC 位址 : ");
  Serial.println(NimBLEDevice::getAddress().toString().c_str());

}

void loop() {
  // put your main code here, to run repeatedly:
  //避免CPU空轉，佔住運算
  delay(100);
}
