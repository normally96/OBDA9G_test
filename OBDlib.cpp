#include "Arduino.h"
#include "A9G.h"
#include "OBDlib.h"

/*Variable hold DTC's mode 3 OBD*/
String DTC_buff[4]; 
int DTC_count;
String OBD_VIN_ID[18];

/****************************************************
  Author      : Phuoc, Hieu
  Description : Lay du lieu tu uart dua vao buffer
*****************************************************/
void OBD::getResponse(){
  while( Serial2.available()>0){
    char c = Serial2.read();
    rxDta += c;
  }

#if debug
  Serial1.println(rxDta);
#endif   
}

/****************************************************
  Author      : Phuoc, Hieu
  Description : Doc Nhiet do nuoc lam mat
*****************************************************/
int OBD::ReadTemp(){

  /*Return -1 if not support*/
  if (modedata[5] == 0){return -1;} // kiểm tra PID 05 có được hỗ trợ không?

  Serial2.flush();
  Serial2.write(PID_COOLANT_TEMP); // 05 là PID của đọc nhiệt độ nước

  delay(Delay_getting_data);
  getResponse(); // gọi hàm đọc giá trị từ uart

  /*Caculate due to OBD*/ 
  int Temp = strtol(&rxDta[10],0,16)-40; // chuyển đổi giá trị hex nhận được sang dec

  /*Reset buffer*/
  rxDta =""; // xóa chuỗi vừa lưu để tiếp tục đọc giá trị khác
  
  return Temp;

#if debug
  Serial1.print("THW: ");
  Serial1.println(Temp);
#endif   
}

/****************************************************
  Author      : Phuoc, Hieu
  Description : Doc Toc do dong co (RPM)
*****************************************************/
int OBD::ReadRPM(){

  /*Return -1 if not support*/
  if(modedata[12] == 0){return -1;}
  
  Serial2.flush();
  Serial2.write(PID_RPM);
  delay(Delay_getting_data);
  getResponse(); 

  /*Caculate due to OBD*/ 
  int vehicleRPM = ((strtol(&rxDta[10],0,16)*256)+strtol(&rxDta[13],0,16))/4;

  /*Reset buffer*/
  rxDta = ""; 

  return vehicleRPM;

#if debug
  Serial1.print("speedRPM: ");
  Serial1.println(vehicleRPM);
#endif 
}

/****************************************************
  Author      : Phuoc, Hieu
  Description : Doc Toc do xe (Vehicle Speed)
*****************************************************/
int OBD::ReadSpeed() {

  /*Return -1 if not support*/
  if(modedata[13] == 0){return -1;}

  Serial2.flush();
  Serial2.write(PID_VEH_SPEED);

  delay(Delay_getting_data);
  getResponse();

  /*Caculate due to OBD*/ 
  int vspeed = strtol(&rxDta[10],0,16);

  /*Reset buffer*/
  rxDta ="";

  return vspeed;
  
#if debug
  Serial1.print("speedvehicle: ");
  Serial1.println(vspeed);
#endif   
}

/****************************************************
  Author      : Phuoc, Hieu
  Description : Doc Nhiet do khi nap
*****************************************************/
int OBD::ReadIntemperature() {

  /*Return -1 if not support*/
  if(modedata[15] == 0){return -1;}

  Serial2.flush();
  Serial2.write(PID_INT_AIR_TEMP);

  delay(Delay_getting_data);
  getResponse();

  /*Caculate due to OBD*/ 
  int Intemp =(strtol(&rxDta[10],0,16))-40;

  /*Reset buffer*/
  rxDta ="";

  return Intemp;
  
#if debug
  Serial1.print("intaketemp: ");
  Serial1.println(Intemp);
#endif   
}

/****************************************************
  Author      : Phuoc, Hieu
  Description : Doc Khoi luong khi nap
*****************************************************/
int OBD::ReadMAF(){
  /*Return -1 if not support*/
  if(modedata[18] == 0){return -1;}

  Serial2.flush();
  Serial2.write(PID_MAF_AIR_FLOW);

  delay(Delay_getting_data);
  getResponse();

  /*Caculate due to OBD*/ 
  int MAF=((strtol(&rxDta[10],0,16)*256)+strtol(&rxDta[13],0,16))/100;

  /*Reset buffer*/
  rxDta = ""; 

  return MAF;
  
#if debug
  Serial1.print("MAF: ");
  Serial1.println(MAF);
#endif   
}

/****************************************************
  Author      : Phuoc, Hieu
  Description : Doc Vi tri buom ga
*****************************************************/
int OBD::ReadThrottleposition() {
  /*Return -1 if not support*/
  if(modedata[17] == 0){return -1;}

  Serial2.write(PID_THROTTLE_POS);
  /*Reset Tx and send PID_THROTTLE_POS*/
  Serial2.flush();
  Serial2.write(PID_THROTTLE_POS);

  delay(Delay_getting_data);
  getResponse();

  /*Caculate due to OBD*/
  int Thro_position = ((strtol(&rxDta[10],0,16))*100)/255;

  /*Reset buffer*/
  rxDta ="";

  return Thro_position;

#if debug
  
Serial1.print("VTBuomga: ");
  Serial1.println(Thro_position);
#endif   
}

/****************************************************
  Author      : Phuoc, Hieu
  Description : Vi tri ban dap ga
*****************************************************/
int OBD::ReadPedalposition() {
  int Pposition = 0;
  if(modedata[90] == 0){return -1;}
  Serial2.flush();
  Serial2.write("015a\r");
  delay(Delay_getting_data);
  getResponse();

  /*Caculate due to OBD*/
  Pposition = (strtol(&rxDta[10],0,16))*100/255;

  /*Reset buffer*/
  rxDta ="";
  return Pposition;
}

/****************************************************
  Author      : Phuoc, Hieu
  Description : Doc Goc danh lua som
*****************************************************/
int OBD::ReadTimingadvance (void) {

  if(modedata[14] == 0){return -1;} // return 1 if mode 14 not support

  Serial2.flush();
  Serial2.write(PID_TIMING_ADV);

  delay(Delay_getting_data);
  getResponse();

  /*Caculate due to OBD*/
  int a = ((strtol(&rxDta[10],0,16))/2) - 64;
  
  /*Reset buffer*/
  rxDta ="";
  return a;
}

/****************************************************
  Author      : Phuoc, Hieu
  Description : Doc Thoi gian phun nhien lieu
*****************************************************/
int OBD::ReadFuelinjectiontiming (void){
  int b = 0;
  if(modedata[93] == 0){return -1;}
  Serial2.flush();
  Serial2.write(PID_FUEL_INJ_TIME);
  delay(Delay_getting_data);
  getResponse();

  /*Caculate due to OBD*/ 
  b= (((strtol(&rxDta[10],0,16)*256)+strtol(&rxDta[13],0,16))/128)-210;

  /*Reset buffer*/
  rxDta = ""; 
 return b;
}

/****************************************************
  Author      : Phuoc, Hieu
  Description : Doc Nhiet do dau dong co
*****************************************************/
int OBD::ReadEngineoiltemperature () {
  if(modedata[92] == 0){return -1;}
  Serial2.flush();
  Serial2.write(PID_OIL_ENG_TEMP);
  delay(Delay_getting_data);
  getResponse();

  /*Caculate due to OBD*/
  int c = strtol(&rxDta[10],0,16)-40;

  /*Reset buffer*/
  rxDta ="";
  return c;
}

/****************************************************
  Author      : Phuoc, Hieu
  Description : Doc Thoi gian chay cua dong co tu luc khoi dong
*****************************************************/
int OBD::ReadRuntime(void){

  if(modedata[31] == 0){return -1;}

  Serial2.flush();
  Serial2.write("011f\r");
  delay(Delay_getting_data);
  getResponse(); 

  /*Caculate due to OBD*/
  int runTime= (strtol(&rxDta[10],0,16)*256)+strtol(&rxDta[13],0,16);

  /*Reset buffer*/
  rxDta = ""; 
  return runTime;
}

/****************************************************
  Author      : Phuoc, Hieu
  Description : Doc Dien ap ac quy
*****************************************************/
float OBD::ReadVoltage(void){

  Serial2.flush();
  Serial2.write("atrv\r");

  delay(Delay_getting_data);
  getResponse(); 

  /*Caculate due to OBD*/
  float Voltage = rxDta.substring(4,'V').toFloat();

  /*Reset buffer*/
  rxDta ="";
  
  return Voltage;
}

/****************************************************
  Author      : Phuoc, Hieu
  Description : Reset OBD II
*****************************************************/
int OBD::ResetOBDII (void){
  byte ResetBoard = 0;
  rxDta ="";
  Serial2.flush();
  Serial2.write("atz\r");
  delay(2000);
  getResponse();
  if(rxDta.substring(3,9) == "ELM327"){ResetBoard =1;} else {ResetBoard =0;}
  rxDta ="";
  return ResetBoard;
}

/****************************************************
  Author      : Phuoc, Hieu
  Description : Kiem tra ket noi voi ELM327
*****************************************************/
int OBD::SetupConnect (void){
  int g = 0;
  Serial2.flush();
  Serial2.write(PID_SUPPORT00); 

  delay(Delay_OBD2_Init);

  getResponse();

  if( rxDta.substring(0,9) == "010041 00"){g =1;} else {g =0;}
  rxDta ="";
  return g;
}

/****************************************************
  Author      : Phuoc, Hieu
  Description : Kiem tra PIDs mode 1 ho tro
*****************************************************/
void OBD::SupportBoard(){

  int m,a,b,c,d; 
  byte rxData[32],arxDta1[32],arxDta2[32],arxDta3[32],arxDta4[32],arxDta5[32];
  
 // mang 0100
  rxDta = ""; 
  Serial2.flush();
  Serial2.write("0100\r");
  delay(Delay_OBD2_Init_SupportBoard);
  getResponse();
  
 if(( rxDta.substring(4,6) == "NO") || (rxDta.substring(4,6) == "SE")) {
  for ( int i = 0; i < 32; i++) {
  arxDta1[i] = 0;
  }
  }else{

 a = strtol(&rxDta[10],0,16);
 b = strtol(&rxDta[13],0,16); 
 c = strtol(&rxDta[16],0,16); 
 d = strtol(&rxDta[19],0,16);  
   rxDta = "";

 //chuyen a
 if (d > 0) {
 for(int i = 0; d>0;i++){
 rxData [i]= d%2;
 d=d/2; 
 }
 }
 
  //chuyen b
 if (c > 0) {
 for(int i = 8; c>0;i++){
 rxData [i]= c%2;
 c=c/2;  
 }
 }
 
  //chuyen c
 if (b > 0) {
 for(int i = 16; b>0;i++){
 rxData [i]= b%2;
 b=b/2;  
 }
 }
  //chuyen d
 if (a > 0) {
 for(int i = 24; a>0;i++){
 rxData [i]= a%2;
 a=a/2;  
 }
 }
 
 m =31;
  for ( int i = 0; i < 32; i++) {
  arxDta1[m] = rxData[i];
  m = m -1;}
  }
  
// mang 0120
  Serial2.flush();
  Serial2.write("0120\r");
  delay(1000);
  getResponse();
 if(( rxDta.substring(4,6) == "NO") ||( rxDta.substring(4,6) == "SE")) {
    for ( int i = 0; i < 32; i++) {
    arxDta2[i] = 0;}
  } else{ 
 a = strtol(&rxDta[10],0,16);
 b = strtol(&rxDta[12],0,16); 
 c = strtol(&rxDta[15],0,16); 
 d = strtol(&rxDta[18],0,16);
   rxDta = "";
 //chuyen a
 if (d > 0) {
 for(int i = 0; d>0;i++){
 rxData [i]= d%2;
 d=d/2; 
 }
 }
 
  //chuyen b
 if (c > 0) {
 for(int i = 8; c>0;i++){
 rxData [i]= c%2;
 c=c/2;  
 }
 }
 
  //chuyen c
 if (b > 0) {
 for(int i = 16; b>0;i++){
 rxData [i]= b%2;
 b=b/2;  
 }
 }
 
  //chuyen d
 if (a > 0) {
 for(int i = 24; a>0;i++){
 rxData [i]= a%2;
 a=a/2;  
 }
 }
 
 m =31;
  for ( int i = 0; i < 32; i++) {
  arxDta2[m] = rxData[i];
  m = m -1;}
  } 


  // mang 0140

  Serial2.flush();
  Serial2.write("0140\r");
  delay(1000);
  getResponse();
 if(( rxDta.substring(4,6) == "NO") || (rxDta.substring(4,6) == "SE")) {
     for ( int i = 0; i < 32; i++) {
     arxDta3[i] = 0;}
  }else{ 
 a = strtol(&rxDta[10],0,16);
 b = strtol(&rxDta[12],0,16); 
 c = strtol(&rxDta[15],0,16); 
 d = strtol(&rxDta[18],0,16);
   rxDta = "";
 
 //chuyen a
 if (d > 0) {
 for(int i = 0; d>0;i++){
 rxData [i]= d%2;
 d=d/2; 
 }
 }
 
  //chuyen b
 if (c > 0) {
 for(int i = 8; c>0;i++){
 rxData [i]= c%2;
 c=c/2;  
 }
 }
 
  //chuyen c
 if (b > 0) {
 for(int i = 16; b>0;i++){
 rxData [i]= b%2;
 b=b/2;  
 }
 }
 
  //chuyen d
 if (a > 0) {
 for(int i = 24; a>0;i++){
 rxData [i]= a%2;
 a=a/2;  
 }
 }
 
 m =31;
  for ( int i = 0; i < 32; i++) {
  arxDta3[m] = rxData[i];
  m = m -1;}
 
} 

  // mang 0160

  Serial2.flush();
  Serial2.write("0160\r");
  delay(1000);
  getResponse();
 if(( rxDta.substring(4,6) == "NO") || (rxDta.substring(4,6) == "SE")) {
   for ( int i = 0; i < 32; i++) {
   arxDta4[i] = 0;}
   }else{
 a = strtol(&rxDta[10],0,16);
 b = strtol(&rxDta[12],0,16); 
 c = strtol(&rxDta[15],0,16); 
 d = strtol(&rxDta[18],0,16);
   rxDta = "";
 //chuyen a
 if (d > 0) {
 for(int i = 0; d>0;i++){
 rxData [i]= d%2;
 d=d/2; 
 }
 }
 
  //chuyen b
 if (c > 0) {
 for(int i = 8; c>0;i++){
 rxData [i]= c%2;
 c=c/2;  
 }
 }
 
  //chuyen c
 if (b > 0) {
 for(int i = 16; b>0;i++){
 rxData [i]= b%2;
 b=b/2;  
 }
 }
 
  //chuyen d
 if (a > 0) {
 for(int i = 24; a>0;i++){
 rxData [i]= a%2;
 a=a/2;  
 }
 }
 
 m =31;
  for ( int i = 0; i < 32; i++) {
  arxDta4[m] = rxData[i];
  m = m -1;}
 } 
 
  // mang 0180

  Serial2.flush();
  Serial2.write("0180\r");
  delay(1000);
  getResponse();
 if(( rxDta.substring(4,6) == "NO") || (rxDta.substring(4,6) == "SE")) {
  for ( int i = 0; i < 32; i++) {
  arxDta5[i] = 0;}
  } else {  
 a = strtol(&rxDta[10],0,16);
 b = strtol(&rxDta[12],0,16); 
 c = strtol(&rxDta[15],0,16); 
 d = strtol(&rxDta[18],0,16);
   rxDta = "";
 
 //chuyen a
 if (d > 0) {
 for(int i = 0; d>0;i++){
 rxData [i]= d%2;
 d=d/2; 
 }
 }

  //chuyen b
 if (c > 0) {
 for(int i = 8; c>0;i++){
 rxData [i]= c%2;
 c=c/2;  
 }
 }
 
  //chuyen c
 if (b > 0) {
 for(int i = 16; b>0;i++){
 rxData [i]= b%2;
 b=b/2;  
 }
 }
 
  //chuyen d
 if (a > 0) {
 for(int i = 24; a>0;i++){
 rxData [i]= a%2;
 a=a/2;  
 }
 }
 
 m =31;
  for ( int i = 0; i < 32; i++) {
  arxDta5[m] = rxData[i];
  m = m -1;}
}

  for ( int i = 0; i<= 31; i++){
    modedata[i] = arxDta1[i];
    }
  for ( int i = 32; i<= 63; i++){
    modedata[i] = arxDta2[i-32];
    }
  for ( int i = 64; i<= 95; i++){
    modedata[64] = arxDta3[i-64];
    }
  for ( int i = 96; i<= 127; i++){
    modedata[96] = arxDta4[i-96];
    }
  for ( int i = 128; i<= 159; i++){
    modedata[i] = arxDta5[i-128];
    }
}

/****************************************************
  Author      : Phuoc, Hieu
  Description : Lay ket qua xuat ra
*****************************************************/
int *OBD::getOBData(){
  int *pOBD = dataOBD;
  *pOBD =10;
  *(pOBD + 1) = ReadThrottleposition(); 
  *(pOBD + 2) = ReadIntemperature();
  *(pOBD + 3) = ReadTemp();
  *(pOBD + 4) = ReadRPM();
  *(pOBD + 5) = ReadVoltage();
  *(pOBD + 6) = ReadMAF();
  *(pOBD + 7) = ReadTimingadvance();
  *(pOBD + 8) = ReadFuelinjectiontiming();
  *(pOBD + 9) = ReadEngineoiltemperature();
  return pOBD;
}

/****************************************************
  Author      : Phuoc, Hieu
  Description : Doi du lieu tho ra DTC
  Comment     : VD: "03 40" -> "P0340"
*****************************************************/
void OBD::Mode03_Bit01_Trans(String inStr){
  String str = "00000";
  
  /*Copy into buffer*/
  str[2]= inStr[1];
  str[3]= inStr[3];
  str[4]= inStr[4];
  
  /*Translate in to DTC code*/
  switch(inStr[0]){
    case '0':
    str[0]= 'P';str[1]= '0';
    break;

    case '1':
    str[0]= 'P';str[1]= '1';
    break;

    case '2':
    str[0]= 'P';str[1]= '2';
    break;

    case '3':
    str[0]= 'P';str[1]= '3';
    break;

    case '4':
    str[0]= 'C';str[1]= '0';
    break;

    case '5':
    str[0]= 'C';str[1]= '1';
    break;

    case '6':
    str[0]= 'C';str[1]= '2';
    break;

    case '7':
    str[0]= 'C';str[1]= '3';
    break;

    case '8':
    str[0]= 'B';str[1]= '0';
    break;
    
    case '9':
    str[0]= 'B';str[1]= '1';
    break;
    
    case 'A':
    str[0]= 'B';str[1]= '2';
    break;

    case 'B':
    str[0]= 'B';str[1]= '3';
    break;

    case 'C':
    str[0]= 'U';str[1]= '0';
    break;

    case 'D':
    str[0]= 'U';str[1]= '1';
    break;

    case 'E':
    str[0]= 'U';str[1]= '2';
    break;

    case 'F':
    str[0]= 'U';str[1]= '3';
    break;
  }

  /*Save DTC code in to DTC_temp array*/
  DTC_buff[DTC_count++]= str;
}

/****************************************************
  Author      : Phuoc, Hieu
  Description : Doc loi mode 3 OBD
*****************************************************/
void OBD::Mode03_Read(){
  /*Reset DTC's count to 0*/
  DTC_count = 0;

  int i = 0;
  int temp = 0;

  Serial2.flush();
  Serial2.write("03\r");

  delay(Delay_OBD2_Init);
  getResponse();
  
  /*Get pointer of "43" token*/
  while (rxDta[i]){
    if (rxDta[i]=='4' && rxDta[i+1] == '3') {
      break;
      }
    i++;
  }

  i += 6;

  /*Get DTC's code and send to Translate function*/
  while (i < rxDta.length()){
      Mode03_Bit01_Trans(rxDta.substring(i,i+6));
      i+= 6;
  }

  /*Reset rxDta*/
  rxDta = "";
};

/****************************************************
  Author      : Phuoc, Hieu
  Description : Doc so VIN cua xe 
*****************************************************/
void OBD::Read_VIN(void){

  Serial2.flush();
  Serial2.write("0902\r");

  delay(1000);
  getResponse();

  /*Data for testing*/
  //rxDta = "014 0: 49 02 01 31 44 34 1: 47 50 30 30 52 35 35 2: 42 31 32 33 34 35 36";

  char str[100]; 
  /*Reset field in OBD VIN*/
  memset(str,'\0',100);
  memset(OBD_VIN_ID,'\0',20);

  int i,j = 0;
  while (rxDta[i]){
    if (rxDta[i+1] == ':') {
      i += 3;
      continue;
    }
    str[j++] = rxDta[i];
    i++;
  }
  
  char * pEnd = &str[13];
  char buff[]=" "; j =0;
  for (i =0 ; i < 17; i ++){
      long int li1 = strtol (pEnd,&pEnd,16);
      OBD_VIN_ID[j++] = (char) li1;
  }

  /*Reset rxDta*/
  rxDta = "";
};