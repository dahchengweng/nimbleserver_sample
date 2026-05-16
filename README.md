# nimbleserver_sample
這個專案示範如何使用 ESP32 + NimBLE-Arduino 建立一個 低功耗藍牙（BLE）UART 服務，並透過 Web Bluetooth、手機 App 或其他 BLE 客戶端進行雙向通訊。  本程式提供：  📶 BLE UART Service（Nordic UART Service 相容）  🔄 RX（接收）/ TX（通知）Characteristic  💡 LED 控制（開 / 關）  🚗 基本移動指令（前進 / 後退 / 左轉 / 右轉）  📤 ESP32 主動回傳通知（Notify）  📱 可搭配 Web Bluetooth / p5.js / 手機 App 使用

🧩 功能說明
🔧 1. BLE UART Service
本程式使用 NimBLE 建立一個 UART 服務，UUID 採用常見的 Nordic UART Service 格式：

功能	UUID	說明
Service	6e400001-b5a3-f393-e0a9-e50e24dcca9e	BLE UART 主服務
RX Characteristic	6e400002-b5a3-f393-e0a9-e50e24dcca9e	客戶端 → ESP32（Write / Write Without Response）
TX Characteristic	6e400003-b5a3-f393-e0a9-e50e24dcca9e	ESP32 → 客戶端（Notify）


💡 2. LED 控制
客戶端可傳送：

指令	功能
0	關閉 LED
1	開啟 LED


ESP32 會同步透過 Notify 回傳執行結果。

🚗 3. 移動控制指令
本程式示範如何接收簡單的移動指令：

指令	功能
f	前進
b	後退
l	原地左轉
r	原地右轉


收到指令後，ESP32 會回傳對應的文字訊息，可用於 UI 顯示或除錯。

🔔 4. Notify 回傳訊息
ESP32 會在以下情況主動通知客戶端：

LED 狀態變更

收到移動指令

收到未知指令

這讓前端（例如 p5.js）可以即時顯示狀態。

🔌 5. NimBLE 優化
本程式使用 NimBLE-Arduino，相較於原生 BLE：

記憶體占用更低

連線更穩定

支援多連線

更適合 Web Bluetooth

並設定：

MTU = 512 bytes（可傳送較長字串）

Scan Response 開啟

廣播 Service UUID（讓 Chrome Web Bluetooth 能找到裝置）

🖥️ 適用場景
Web Bluetooth（Chrome / Edge）

p5.js BLE 控制介面

手機 BLE App（nRF Connect、LightBlue…）

BLE 遙控車 / 機器人

BLE LED 控制器

BLE UART 資料傳輸

📦 依賴套件
請先在 Arduino IDE Library Manager 安裝：

NimBLE-Arduino by h2zero

🚀 快速開始
將程式燒錄至 ESP32

開啟序列監控（115200）

使用 Web Bluetooth / App 搜尋裝置名稱：
ESP32-NimBLE-UART

連線後即可傳送指令並接收回覆
