//----------------------------------------//
//  lmic_payload.ino
//
//  created 03/06/2019
//  by Luiz Henrique Cassettari
//----------------------------------------//

uint8_t txBuffer[9];
uint32_t LatitudeBinary, LongitudeBinary;
uint16_t altitudeGps;
uint8_t hdopGps;

void PayloadNow()
{
  boolean confirmed = false;

  if (button_count() == 1) confirmed = true;
  
  if (gps_read()) {

    LatitudeBinary = ((gps_latitude() + 90) / 180) * 16777215;
    LongitudeBinary = ((gps_longitude() + 180) / 360) * 16777215;

    txBuffer[0] = ( LatitudeBinary >> 16 ) & 0xFF;
    txBuffer[1] = ( LatitudeBinary >> 8 ) & 0xFF;
    txBuffer[2] = LatitudeBinary & 0xFF;

    txBuffer[3] = ( LongitudeBinary >> 16 ) & 0xFF;
    txBuffer[4] = ( LongitudeBinary >> 8 ) & 0xFF;
    txBuffer[5] = LongitudeBinary & 0xFF;

    altitudeGps = gps_meters();
    txBuffer[6] = ( altitudeGps >> 8 ) & 0xFF;
    txBuffer[7] = altitudeGps & 0xFF;

    hdopGps = gps_HDOP() * 10;
    txBuffer[8] = hdopGps & 0xFF;

    LMIC_setTxData2(1, txBuffer, sizeof(txBuffer), confirmed);
  }
  else
  {
    LMIC_setTxData2(1, txBuffer, 0, confirmed);
  }
}
