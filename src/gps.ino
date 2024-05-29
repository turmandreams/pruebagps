 #include <Arduino.h>
 #include <SoftwareSerial.h>

 SoftwareSerial SoftSerial(26,25);

 
#include "TFT_eSPI.h" // Hardware-specific library
#include "SPI.h"

//#define TFT_RED         0xF800      /* 255,   0,   0 */

TFT_eSPI tft = TFT_eSPI(); // Invoke custom library with default width and height

//touch variables
int x22; //x position
int y22; //y position

int x11; //x position
int y11; //y position

unsigned long touchTimer;//keeps track of touch timings
const int debounceSensitivity = 5;//300 milliseconds debounce

int contador = 0;

double xc,yc;
double xc2,yc2;

String lineas[32];

String dato="";

int cont=0;

void pintapuntoslinea() {
  
  int difx=0;
  int dify=0;
  double m;

  if((x22-x11)==0){ 
      tft.fillCircle(x22,y22, 5, TFT_BLUE);  
      return;
  }
  
  m=(y22-y11)/(x22-x11);
  
  Serial.print("M : ");Serial.println(m);Serial.println("");

  difx=x22-x11; Serial.print("Diferencia x : ");Serial.println(difx);
  dify=y22-y11; Serial.print("Diferencia y : ");Serial.println(dify);

  int absdifx=abs(difx);
  int absdify=abs(dify);

  if(m==0.0) {
    
      tft.fillCircle(x22,y22, 5, TFT_BLUE);  
      
  }else{    
  
      if(absdifx>absdify) {
        
        if(difx>0) {
          for(int i=1;i<=difx;i++){
              int y=(m*i)-y11;
              tft.fillCircle(x11+i,y, 5, TFT_BLUE);                  
          }      
        }else{
          for(int i=-1;i>=difx;i--){
              int y=(m*i)-y11;
              tft.fillCircle(x11+i,y, 5, TFT_BLUE);                  
          }      
        }
                
      }else{
        if(dify>0) {
          
          for(int i=1;i<=dify;i++){
              int x=(i/m)-x11;
              tft.fillCircle(x,y11+i, 5, TFT_BLUE);                  
          }      
          
        }else{
          
          for(int i=-1;i>=difx;i--){
              int x=(i/m)-x11;
              tft.fillCircle(x,y11+i, 5, TFT_BLUE);                  
          }      
        }    
      } 
  
  }
  


}

void Press(){
  
    uint16_t xx,yy,zz;
    uint16_t xx2,yy2,zz2;
    
    tft.getTouchRaw(&xx,&yy);//only bother getting readings when needed
    zz=tft.getTouchRawZ();

    
    if(zz>1500){

      if(contador>=100) {  
        xc=xx;yc=yy; 
        for(int i=0;i<50;i++){
          tft.getTouchRaw(&xx,&yy);//only bother getting readings when needed
          xc=(xc*0.95)+(xx*0.05);
          yc=(yc*0.95)+(yy*0.05);  
        }
      }
      else{
        xc=(xc*0.95)+(xx*0.05);
        yc=(yc*0.95)+(yy*0.05);
      }
      
      contador=0;
      
      //Serial.print(xc);Serial.print(",");
      //Serial.print(yc);Serial.print(",");
      //Serial.println(zz);

      x22 = (xc - 380) / 10; //9.75
      y22 = (yc - 248) / 7; //7.04

      tft.fillCircle(x22,y22,3,TFT_BLUE);  
      
    }else{

      if(contador<100) { contador++; }
      
      
    }
  
}

//deals with screen being touched
void screenPress() {
  uint16_t xx, yy;
  unsigned long currentMillis;
  currentMillis = millis();
  //deals with debouncing
  if ((currentMillis - touchTimer) >= debounceSensitivity) {

    touchTimer = currentMillis;
   
    if (tft.getTouchRawZ() > 200) {

      tft.getTouchRaw(&xx,&yy);//only bother getting readings when needed

      Serial.print("X : ");Serial.println(xx);
      Serial.print("Y : ");Serial.println(yy);
      Serial.println("");

      
      //will need alterations if screen is rotated
      
      x22 = (xx - 380) / 10; //9.75
      y22 = (yy - 248) / 7; //7.04

      tft.fillCircle(x22,y22, 5, TFT_BLUE);   

      /*
      if ((x22 < (x11 + 10)) && (x22 > (x11 - 10)) && (y22 < (y11 + 10)) && (y22 > (y11 - 10))) {

        contador = 0;
        tft.fillCircle(x22,y22, 5, TFT_BLUE);   
        x11 = x22; y11 = y22;

      } else if (contador > 50) {

        contador = 0;
        tft.fillCircle(x22,y22, 5, TFT_BLUE);   
        x11 = x22; y11 = y22;

      }
      */
    } else {
      contador++;
    }
    
  }
  
}

void muestralineas(){
	
  for(int i=0;i<32;i++){
      int pos=i*10;
      tft.setCursor(0,pos);
      tft.setTextColor(TFT_WHITE);
      tft.setTextSize(1);
      tft.println(lineas[i]); 
  }


}

void setup() {

  Serial.begin(115200);
  SoftSerial.begin(9600); 

  Serial.println("ESP32_SPI_9481_v1");

  tft.begin();
  tft.init();
  tft.setRotation(1);

  //Calibracion obtenida con el programa touchcalibrate
  uint16_t calData[5] = {265,3439,444,2937, 5 };
  tft.setTouch(calData);
	

  tft.fillScreen(TFT_BLACK);

  for(int i=0;i<32;i++){	
	lineas[i]="";
  }
  
  dato="";

}

void loop() {
  
  //screenPress();
  //Press();
  
  while (SoftSerial.available()){
	
	char c=SoftSerial.read();

	if(c!='\n'){
		dato+=c;
	}else{
		
	     for(int i=0;i<31;i++){  lineas[i]=lineas[i+1];  }
	     lineas[31]=dato;
	     dato="";

	     cont++;
	     if(cont>10){
		cont=0;
  	        tft.fillScreen(TFT_BLACK);
		muestralineas();
             }

	}

	Serial.print(c);

  }  

}

