//----------------------------------------//
//  esp32-ttnmapper-gps.ino
//
//  created 01/06/2019
//  by Luiz H. Cassettari
//----------------------------------------//
// Designed to work with esp32 lora 915MHz
// Create a device with ABP on ttn
// Create a integration with ttnmapper
//----------------------------------------//

const char *devAddr = "00000000";
const char *nwkSKey = "00000000000000000000000000000000";
const char *appSKey = "00000000000000000000000000000000";

//----------------------------------------//

#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>

#define SEND_TIMER 10

#define LORA_HTOI(c) ((c<='9')?(c-'0'):((c<='F')?(c-'A'+10):((c<='f')?(c-'a'+10):(0))))
#define LORA_TWO_HTOI(h, l) ((LORA_HTOI(h) << 4) + LORA_HTOI(l))
#define LORA_HEX_TO_BYTE(a, h, n) { for (int i = 0; i < n; i++) (a)[i] = LORA_TWO_HTOI(h[2*i], h[2*i + 1]); }
#define LORA_DEVADDR(a) (uint32_t) ((uint32_t) (a)[3] | (uint32_t) (a)[2] << 8 | (uint32_t) (a)[1] << 16 | (uint32_t) (a)[0] << 24)

static uint8_t DEVADDR[4];
static uint8_t NWKSKEY[16];
static uint8_t APPSKEY[16];

// Pin mapping
const lmic_pinmap lmic_pins = {
  .nss = 18, 
  .rxtx = LMIC_UNUSED_PIN,
  .rst = 14,
  .dio = {/*dio0*/ 26, /*dio1*/ 33, /*dio2*/ 32}
};

void os_getArtEui (u1_t* buf) { }
void os_getDevEui (u1_t* buf) { }
void os_getDevKey (u1_t* buf) { }

static osjob_t sendjob;

void onEvent (ev_t ev) {
  Serial.print(os_getTime());
  Serial.print(": ");
  switch (ev) {
    case EV_TXCOMPLETE:
      oled_status(" --- TXCOMPLETE --- ");
      Serial.println(F("EV_TXCOMPLETE (includes waiting for RX windows)"));
      if (LMIC.txrxFlags & TXRX_ACK) 
      {
        Serial.println(F("Received ack"));
      }
      if (LMIC.dataLen != 0 || LMIC.dataBeg != 0) {
        uint8_t port = 0;
        if (LMIC.txrxFlags & TXRX_PORT)
        {
          port = LMIC.frame[LMIC.dataBeg - 1];
        }
        message(LMIC.frame + LMIC.dataBeg, LMIC.dataLen , port);
      }
      os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(SEND_TIMER), do_send);
      break;
    case EV_TXSTART:
      oled_status(" --- TXSTART --- ");
      Serial.println(F("EV_TXSTART"));
      break;
  }
}



void do_send(osjob_t* j) {
  // Check if there is not a current TX/RX job running
  if (LMIC.opmode & OP_TXRXPEND) {
    Serial.println(F("OP_TXRXPEND, not sending"));
  } else {
    PayloadNow();
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println(F("Starting"));
  
  oled_setup();
  gps_setup();
  button_setup();
  
  LORA_HEX_TO_BYTE(DEVADDR, devAddr, 4);
  LORA_HEX_TO_BYTE(NWKSKEY, nwkSKey, 16);
  LORA_HEX_TO_BYTE(APPSKEY, appSKey, 16);
  
  if (LORA_DEVADDR(DEVADDR) == 0) while(true);

  os_init();

  LMIC_reset();
  LMIC_setSession (0x13, LORA_DEVADDR(DEVADDR), NWKSKEY, APPSKEY);
  LMIC_setAdrMode(0);
  LMIC_setClockError(MAX_CLOCK_ERROR * 10 / 100);
  LMIC_selectSubBand(1);
  LMIC_setLinkCheckMode(0);

  do_send(&sendjob);
}

void loop() {
  os_runloop_once();
  gps_loop();
  oled_loop();
  if (button_loop())
  {
    oled_mode(button_mode());
    
    if (button_count() == 0)
      do_send(&sendjob);
    else if (button_count() == 2)
    {
      os_clearCallback(&sendjob);
      os_radio(RADIO_RST);
    }
  }
}

void message(const uint8_t *payload, size_t size, uint8_t port)
{
  Serial.println("-- MESSAGE");
  Serial.println("Received " + String(size) + " bytes on port " + String(port) + ":");
  if (port == 0) 
  {
    oled_status(" --- TX_CONFIRMED --- ");
    return;
  }
  if (size == 0) return;
  switch (port) {
    case 1:
      break;
  }
}
