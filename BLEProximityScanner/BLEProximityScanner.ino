/*********************************************************************
 This is an example for our nRF52 based Bluefruit LE modules

 Pick one up today in the adafruit shop!

 Adafruit invests time and resources providing this open source code,
 please support Adafruit and open-source hardware by purchasing
 products from Adafruit!

 MIT license, check LICENSE for more information
 All text above, and the splash screen below must be included in
 any redistribution
*********************************************************************/

#include <Adafruit_CircuitPlayground.h>
#include <bluefruit.h>
#include <cstring>

const uint8_t target[] = { 0x03, 0x03, 0xF3, 0xFE };

uint32_t lastPing = 0;
uint32_t deviceLostThreshold = 1000;

void setup() 
{
  Serial.begin(115200);

  if(Serial) {
    Serial.println("Bluefruit52 Central Scan Example");
    Serial.println("--------------------------------\n");
  }

  CircuitPlayground.begin();
  CircuitPlayground.speaker.off();

  // Initialize Bluefruit with maximum connections as Peripheral = 0, Central = 1
  // SRAM usage required by SoftDevice will increase dramatically with number of connections
  Bluefruit.begin(0, 1);
  Bluefruit.setTxPower(4);    // Check bluefruit.h for supported values
  Bluefruit.setName("Bluefruit52");

  // Start Central Scan
  Bluefruit.Scanner.setInterval(32, 40);
  Bluefruit.Scanner.setRxCallback(scan_callback);
  Bluefruit.Scanner.start(0);

  if(Serial) {
    Serial.println("Scanning ...");
  }
}

void visualize_rssi(int8_t rssi, int8_t min = -80, int8_t max = -40) {
  // Calculate fraction of range (0.0 – 1.0)
  float fraction = float(rssi - min) / float(max - min);
  fraction = constrain(fraction, 0.0f, 1.0f);

  // Convert to "pixel units"
  float lit = fraction * 10.0f;

  int fullPixels = int(lit);               // completely on
  float partial = lit - fullPixels;        // 0.0 – 1.0 brightness for next pixel

  for (int i = 0; i < 10; i++) {
    if (i < fullPixels) {
      // Fully on
      if(fullPixels >= 5) {
        CircuitPlayground.setPixelColor(i, 0, 255, 0);
      }
      else {
        CircuitPlayground.setPixelColor(i, 0, 0, 255);
      }
    }
    else if (i == fullPixels && partial > 0.0f) {
      // Partially lit → scale brightness
      uint8_t brightness = uint8_t(partial * 255);
      if(fullPixels >= 5) {
        CircuitPlayground.setPixelColor(i, 0, brightness, 0);
      }
      else {
        CircuitPlayground.setPixelColor(i, 0, 0, brightness);
      }
    }
    else {
      // Off
      CircuitPlayground.setPixelColor(i, 0, 0, 0);
    }
  }
}

void scan_callback(ble_gap_evt_adv_report_t* report)
{
  if(report->data.len >= sizeof(target)) {
    for (uint16_t i = 0; i <= report->data.len - sizeof(target); i++)
    {
      if (memcmp(&report->data.p_data[i], target, sizeof(target)) == 0)
      {
        if(Serial) {
          Serial.println("Timestamp Addr              Rssi Data");

          lastPing = millis();
          Serial.printf("%09d ", lastPing);
          
          // MAC is in little endian --> print reverse
          Serial.printBufferReverse(report->peer_addr.addr, 6, ':');
          Serial.print(" ");

          Serial.print(report->rssi);
          Serial.print("  ");

          Serial.printBuffer(report->data.p_data, report->data.len, '-');
          Serial.println();

          // Check if advertising contain BleUart service
          if ( Bluefruit.Scanner.checkReportForUuid(report, BLEUART_UUID_SERVICE) )
          {
            Serial.println("                       BLE UART service detected");
          }

          Serial.println();
        }
        visualize_rssi(report->rssi);
      }
    }
  }
  
  // For Softdevice v6: after received a report, scanner will be paused
  // We need to call Scanner resume() to continue scanning
  Bluefruit.Scanner.resume();
}

void loop() 
{
  if(millis() - deviceLostThreshold > lastPing) {
    CircuitPlayground.clearPixels();
  }
}
