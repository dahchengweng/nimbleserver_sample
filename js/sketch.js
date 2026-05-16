// -----------------------------
// Minimal BLE Helper (取代 p5ble)
// -----------------------------
class MiniBLE {
  constructor() {
    this.device = null;
    this.server = null;
  }

  async connect(serviceUUID, callback) {
  try {
    this.device = await navigator.bluetooth.requestDevice({
    acceptAllDevices: true,
    optionalServices: [serviceUUID]
    });


    this.server = await this.device.gatt.connect();
    const service = await this.server.getPrimaryService(serviceUUID);
    const characteristics = await service.getCharacteristics();

    callback(null, characteristics);

  } catch (err) {
    console.error("BLE 連線錯誤：", err.name, err.message);
    callback(err, null);
  }
}


  async write(characteristic, data) {
    await characteristic.writeValue(data);
  }

  async startNotifications(characteristic, callback) {
    await characteristic.startNotifications();
    characteristic.addEventListener("characteristicvaluechanged", (event) => {
      callback(event.target.value);
    });
  }

  disconnect() {
    if (this.device && this.device.gatt.connected) {
      this.device.gatt.disconnect();
    }
  }
}

// -----------------------------
// p5.js 主程式
// -----------------------------
let ble = new MiniBLE();

let serviceUUID = "6e400001-b5a3-f393-e0a9-e50e24dcca9e";
let rxUUID = "6e400002-b5a3-f393-e0a9-e50e24dcca9e"; // write
let txUUID = "6e400003-b5a3-f393-e0a9-e50e24dcca9e"; // notify

let rxChar;
let txChar;

let inputBox;

function setup() {
  createCanvas(400, 420);

  createButton("連線").mousePressed(connectBLE);
  createButton("斷線").mousePressed(disconnectBLE);

  createP("");

  createButton("前進").mousePressed(() => sendCmd("f"));
  createButton("後退").mousePressed(() => sendCmd("b"));
  createButton("原地左轉").mousePressed(() => sendCmd("l"));
  createButton("原地右轉").mousePressed(() => sendCmd("r"));

  createP("");

  createButton("開啟 LED").mousePressed(() => sendCmd("1"));
  createButton("關閉 LED").mousePressed(() => sendCmd("0"));

  createP("");

  inputBox = createInput("");
  createButton("傳送字串").mousePressed(sendText);
}

function draw() {
  background(240);
  textSize(20);
  text("ESP32 BLE 控制介面", 80, 40);
}

// -----------------------------
// BLE 連線
// -----------------------------
function connectBLE() {
  ble.connect(serviceUUID, gotCharacteristics);
}

function gotCharacteristics(error, characteristics) {
  if (error) {
    console.log("連線錯誤:", error);
    return;
  }

  console.log("已連線!");

  for (let c of characteristics) {
    if (c.uuid === rxUUID) rxChar = c;
    if (c.uuid === txUUID) {
      txChar = c;
      ble.startNotifications(txChar, handleNotifications);
    }
  }
}

function disconnectBLE() {
  ble.disconnect();
  console.log("已斷線");
}

// -----------------------------
// 接收資料
// -----------------------------
function handleNotifications(dataView) {
  let text = new TextDecoder().decode(dataView);
  console.log("ESP32:", text);
}

// -----------------------------
// 傳送指令
// -----------------------------
function sendCmd(cmd) {
  if (!rxChar) return;
  let data = new TextEncoder().encode(cmd);
  ble.write(rxChar, data);
  console.log("送出:", cmd);
}

// -----------------------------
// 傳送 UTF-8 字串
// -----------------------------
function sendText() {
  let msg = inputBox.value();
  if (!msg || !rxChar) return;

  let data = new TextEncoder().encode(msg);
  ble.write(rxChar, data);

  console.log("送出字串:", msg);
}
