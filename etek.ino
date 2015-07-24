//------------------------
// RC REMOTE SEND code 
//-----------------------
void RC_SEND(int etekID, int etekState)
{
  //RCrecv.disableReceive();
  RCsend.enableTransmit(TX433_PIN);
  RCsend.setProtocol(1);
  RCsend.setPulseLength(pulsetime);
  
  if(etekState==1){
    txRemoteOn(etekID);
  }
  else if(etekState==0){
    txRemoteOff(etekID);
  }
  
  RCsend.disableTransmit();
  //RCrecv.enableReceive(0);
  
}


void txRemoteOn(int p)
{
  etekCODES etekItem1;
  etekCODES etekItem2;
  memcpy_P (&etekItem1, &etekArr[p], sizeof etekItem1);
  memcpy_P (&etekItem2, &etekArr[p+5], sizeof etekItem2);
  for(int k = 0; k<2; k++)
  {
    RCsend.sendTriState(etekItem1.etekCodeArr); 
    RCsend.sendTriState(etekItem2.etekCodeArr); 
    delay(4);
  }	
  //Serial.print(F("EtekON:")); Serial.println(etekItem1.etekIDArr);
  delay(1);
}


void txRemoteOff(int j)
{ 
  etekCODES etekItem1;
  etekCODES etekItem2;
  memcpy_P (&etekItem1, &etekArr[j+10], sizeof etekItem1);
  memcpy_P (&etekItem2, &etekArr[j+15], sizeof etekItem2);
 for(int k = 0; k<2; k++)
 {
    RCsend.sendTriState(etekItem1.etekCodeArr); 
    RCsend.sendTriState(etekItem2.etekCodeArr); 
    delay(4);
  }
  //Serial.print(F("EtekOFF:")); Serial.println(etekItem1.etekIDArr);
  delay(1);
}
