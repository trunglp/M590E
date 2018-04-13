#include <SoftwareSerial.h>
SoftwareSerial mySerial(2, 3);

#define pinBOOT 5

String sodt = "";
String tinnhan = "";
String mang = "";
String reg = "";
boolean goithanhcong=false;
boolean dareset=false;
String checkcpin="";
String cuphapnhantin="Noi dung tin nhan";
String sdtnt="9231";
int led13=13;

String sim1="";
String sim2="";
char ch; //


void setup()
{
 
  mySerial.begin(9600);   // Setting the baud rate of GSM Module
  Serial.begin(9600);    // Setting the baud rate of Serial Monitor (Arduino)
  Serial.println("Setup....");
  delay(500);
  pinMode(pinBOOT, OUTPUT);
  digitalWrite(pinBOOT, LOW);
  pinMode(led13, OUTPUT);
   digitalWrite(led13, LOW);  
  modemOFF();
  modemON();
  Serial.println("Setup ok");
 }


void loop()
{
 
  Serial.println("Setup module....");
 
  delay(2000);
  
  goithanhcong= false;
  mang = "";
  reg = "";  
  Serial.println("Bat dau loop");

  //checkcpin=sendData("AT+CPIN?\r\n",1000,false);
  checkcpin=sendData("AT+CPIN?\r\n",500,false);
 
  if (!chechAT(checkcpin,"READY") )
  {
       Serial.println("Chua nhan duoc sim.. dang tien hanh reset wating....");
       modemOFF();
       modemON(); 
       
  }else
  {

         Serial.println("bat dau loop check mang mobifone");
          setup_M590E(); 

          mang = sendData("AT+COPS?\r\n",1000,true);
          reg = sendData("AT+CREG=1\r\n",300,true);
          reg = sendData("AT+CREG?\r\n",500,true);

          
          while(true){
              sendData("AT+CREG=1\r\n",300,false);
              reg=sendData("AT+CREG?\r\n",500,false);
              Serial.print(".");
              if (chechAT(reg,"+CREG: 1,1")) break;
          };  

          Serial.println("=====Da nhan mang Mobifone======");
          if (chechAT(mang,"MOBIFONE") && chechAT(reg,"+CREG: 1,1") )
          {
            
            Serial.println("=====Dang kiem tra so CCID wating .... ");
            sodt=sendData("AT+CCID\r\n",300,false);         
            //sim1=sodt.substring(sodt.indexOf(':',20)+1,sodt.indexOf('"',20));
            sim1=sodt.substring(7);
            //+CCID: 8984011609920026186
            Serial.print("So CCID ban la:");
            Serial.println(sim1); 
            Serial.println(sim2);
              
            Serial.println("Dang tien hanh goi tin nhan wating.....");
            if (sim1 != sim2)
            {
                if (goitin())
                {
                    Serial.println("********************");
                    Serial.println("Goi thanh cong");
                    Serial.println("********************");
                    sim2=sim1;               
                    goithanhcong=true;
                }else
                {
                    Serial.println("Goi khong thanh cong");
                     goithanhcong=false;
                }
            
                Serial.println("Vui long dua sim vao"); 
               
            } else {
               Serial.println("So dien thoai nay da goi tin nhan roi");
               goithanhcong=true;
            }// end 2 sim1 khac sim2

         }else
         {
            digitalWrite(led13, LOW);        
         } // end check mang mobifone
         
      while(goithanhcong)
      {

          
          dareset=false;
          digitalWrite(led13, HIGH);
          delay(100);
          
              checkcpin=sendData("AT+CPIN?\r\n",500,false);
              if (!chechAT(checkcpin,"READY") )
              {                  
                  goithanhcong = false;
                  digitalWrite(led13, LOW);                
                  dareset=true;        
              }else{
                 delay(1000);
                 Serial.println("Vui long dua sim vao"); 
              }


//              if(dareset){
//
//                  checkcpin=sendData("AT+CPIN?\r\n",500,false);
//                  if (chechAT(checkcpin,"READY") )
//                  {
//                        goithanhcong = false;
//                        digitalWrite(led13, LOW);
//                        sim1 ="";
//                  
//                  }//end READY
//              
//              }//end dareset
            
              
       } //end while
                            
            digitalWrite(led13, LOW);
            Serial.println("Dang tien hanh nhan sim");
        
      } //end check READY


 
}

void setup_M590E()
{
  //sendData("AT+CREG=1\r\n",1000,false);
  //sendData("AT+CREG?\r\n",1000,false);
  sendData("AT+CSMS=1\r\n",300,false);
  //sendData("AT+CSMS?\r\n",300,false);
  sendData("AT+CMGF=1\r\n",300,false);
  sendData("AT+CSCS=\"GSM\"\r\n",300,false);  
}

boolean chechAT(const String& command,String data)
{ 
    //Serial.println(command);
    if(command.indexOf(data) != -1)
    { 
        return true;
    }
    else
    { 
        return false;
    }  

}

void modemOFF(){
  Serial.println("Start off modem");
  digitalWrite(pinBOOT, HIGH);
  delay(10);
  mySerial.println("AT+CPWROFF");
}

void modemON(){
  Serial.println("Start on modem");
  digitalWrite(pinBOOT, LOW);
  boolean flat=true;
  long int time1 = millis()+4000;
  while(flat){        
        
        //Serial.println(time+timeout);
        //Serial.println(millis());     
        if( time1 > millis())
        {
          if(mySerial.find("MODEM:STARTUP")){
              Serial.println("MODEM:STARTUP");
              delay(2000);
              checkcpin=sendData("AT+CPIN?\r\n",500,false);
              if (chechAT(checkcpin,"READY") )
              {
                  flat=false;
                  
                  break;  
              }  
             
          }
          
        }else{
            time1 = millis()+4000;
            Serial.println("MODEM don't STARTUP waiting....");
            digitalWrite(pinBOOT, HIGH);
            delay(10);
            digitalWrite(pinBOOT, LOW);
            delay(10);            
        }
  }      
  delay(10);
  
  while(!mySerial.find("+PBREADY")){
      Serial.print(".");
  };    
  sendData("ATE0\r\n",500,false);
  Serial.println("Success full Start off modem....");
  
}



boolean goitin(){
  
  sendData("AT+CMGF=1\r\n",1000,false);
  sendData("AT+CMGS=\""+sdtnt+"\"\r\n",1000,false);
  sendData(cuphapnhantin,100,false);
  tinnhan = sendData("\032\r\n",5000,false);
  if(tinnhan.indexOf("+CMGS:") != -1){
    return true;
  }else{
    return false;
  }
 
}
String sendData(String command, const int timeout, boolean debug)
    {
        String response = "";
        mySerial.print(command); // send the read character to the esp8266
        long int time = millis();
        while( (time+timeout) > millis())
        {
          while(mySerial.available())
          {
           // The esp has data so display its output to the serial window 
            char c = mySerial.read(); // read the next character.
            response+=c;
          }  
        }
      
        if(debug)
        {
          Serial.print(response);
        }
        return response;
    }


