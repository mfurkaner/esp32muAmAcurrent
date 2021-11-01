#include <ADS1115_lite.h>
ADS1115_lite ads(ADS1115_DEFAULT_ADDRESS);  //0x48 addr pin Connected to GND

#include <WiFi.h>

#ifndef STASSID
#define STASSID "ssid"                                       //Kurulan wifi'in SSID
#define STAPSK  "pass"                           //Kurulan wifi'in şifresi
#endif



IPAddress staticIP(192,168,137,77);                           //Istenilen statik ip adresi
IPAddress gateway(192,168,137,5);                             //Modem ip adresi
IPAddress subnet(255,255,255,0);



#define BAUD_SERIAL 115200                                  

#define MAX_SRV_CLIENTS 2

const char* ssid = STASSID;
const char* password = STAPSK;



const int port = 24;                                        //Acilan serverin portu
WiFiServer server(port);                                    //Server ayarlari
WiFiClient serverClients[MAX_SRV_CLIENTS];






void setup() {
  Serial.begin(BAUD_SERIAL);
  WiFi.mode(WIFI_STA);      
  Serial.print("status:\n");
  Serial.println(WiFi.status());
  WiFi.config(staticIP, gateway, subnet);       //Wifi ayarlari yapildi
  WiFi.begin(ssid, password);                   //Wifi'a baglaniyor.
  while (WiFi.status() != 3) {                  //Baglanana kadar deniyor.
    WiFi.begin(ssid,password);
    delay(5000);
    Serial.println(WiFi.status());
  }
  server.begin();
  server.setNoDelay(true);
  Serial.println("Server created. Use the ip and port:*");
  Serial.println(WiFi.localIP());
  Serial.println(port);
  Serial.println("Ready*");
  delay(10);
  ads_config();                                                 //ADC ayarlarini yapiyor
 }

void loop(){
 if (server.hasClient()) {                                        //Server'a biri baglandiginda buraya giriyor.
    Serial.println("Server has a client");
    int i = 0;
    for (;i < MAX_SRV_CLIENTS; i++)                               //Max baglanti sayisina ulasilmadigindan emin olup baglanan clienta isim veriyor.
      if (!serverClients[i]) {
        serverClients[i] = server.available();
        break;
      }
    if (i == MAX_SRV_CLIENTS) {
      server.available().println("busy");
    }
  }
  for (int i = 0; i < MAX_SRV_CLIENTS; i++){                      //Serverdakilere "CURtw54468th00004***" seklinde okunan degerleri yolluyor.
      serverClients[i].write("CURtw");                            
      ads_writeToSv2(i);
      serverClients[i].write("th");
      ads_writeToSv3(i);
      serverClients[i].write("***");
  }
 delay(100); 
}





void ads_config(){                                        //ADC ayari fonksiyonu
  ads.setGain(ADS1115_REG_CONFIG_PGA_4_096V);             // max 4.096V
  ads.setSampleRate(ADS1115_REG_CONFIG_DR_128SPS);        //Ortalama hız - çözünürlük kaybı yok
}

int16_t ads_read0(){                            //A0 pini okuma fonksiyonu
  ads.setMux(ADS1115_REG_CONFIG_MUX_SINGLE_0);  //Single mode input on AIN0
  ads.triggerConversion();  
  return ads.getConversion();  
}

int16_t ads_read1(){                            //A1 pini okuma fonksiyonu
  ads.setMux(ADS1115_REG_CONFIG_MUX_SINGLE_1);  //Single mode input on AIN1
  ads.triggerConversion();  
  return ads.getConversion();  
}

int16_t ads_read2(){                            //A2 pini okuma fonksiyonu
  ads.setMux(ADS1115_REG_CONFIG_MUX_SINGLE_2);  //Single mode input on AIN2
  ads.triggerConversion();  
  return ads.getConversion();  
}


int16_t ads_read3(){                            //A3 pini okuma fonksiyonu
  ads.setMux(ADS1115_REG_CONFIG_MUX_SINGLE_3);  //Single mode input on AIN3
  ads.triggerConversion();  
  return ads.getConversion();  
}
  
void ads_writeToSv1(int i){                      //A1 pinini serverdaki herkese yazma fonksiyonu
   int16_t ads = ads_read1();                    //A1 okuyor
   for(int j = floor(log10(abs(ads)))+1;5-j > 0;j++){     //okunan degeri 5 basamakli yapana kadar basina 0 koyuyor..
      serverClients[i].write('0');
      delay(5);
    }
    serverClients[i].write(String(ads).c_str());    //kalan basamaklari yaziyor.  
}

void ads_writeToSv2(int i){                       //A2 pinini serverdaki herkese yazma fonksiyonu       A1 in aynisi.
   int16_t ads = ads_read2(); 
    for(int j = floor(log10(abs(ads)))+1;5-j > 0;j++){
      serverClients[i].write('0');
      delay(5);
    }
    serverClients[i].write(String(ads).c_str());
}

void ads_writeToSv3(int i){                       //A3 pinini serverdaki herkese yazma fonksiyonu        A1 in aynisi.
   int16_t ads = ads_read3();
    for(int j = floor(log10(abs(ads)))+1;5-j > 0;j++){
      serverClients[i].write('0');
      delay(5);
    }
    serverClients[i].write(String(ads).c_str());
}
