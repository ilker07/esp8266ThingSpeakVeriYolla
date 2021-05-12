
#include "stm32f4xx.h" 
#include "rcc.h"
#include "gpio.h"
#include "usart.h"
#include "timer.h"
#include "veri.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>


//char adres []="GET /update?api_key=50AGOLCWM5CHEQ1K&field1=";


uint8_t gonderilecekSayi=33;
volatile uint16_t msn=0;
volatile uint16_t birsn=0;
void bekle(uint32_t beklenen)
{

  msn=0;
	while(msn<beklenen);
}

volatile bool sureDoldu=0;

volatile char gelenCevap[100]="";
char karakterDizisi[100]="";
volatile int sayac=0;
uint8_t mod=0;
uint8_t saat=0;
uint8_t dakika=0;
uint8_t saniye=0;



void uygula(const char * gonderilecek,const char * beklenen)
{

      if(strstr((char *)gelenCevap,beklenen))
			{
				
				memset((char *)gelenCevap,0,sizeof(gelenCevap));//Array temizlendi.
				sayac=0;
				mod++;
				veri_yollaESP8266(gonderilecek);
				
			}


}






void USART2_IRQHandler()   //Espden gelen veriler.
{
	
	NVIC->ICER[1] =1<<6;//Interrupt pasif

 
	if(USART2->SR & 1<<5)  //RX bayrak kontrolu.
	{
		
	 
		gelenCevap[sayac]=USART2->DR;//Veri alindi.
		sayac++;
		if(sayac>100)
			sayac=0;
		
		
	}
	
	if(USART2->SR & 1<<6) //TX bayrak kontrolu.
	{
		 
		 USART2->SR &=~(1<<6);//Bayrak temizlemek icin.
	}
	
	
	NVIC->ISER[1] =1<<6;//Interrupt aktif.
	
}



void TIM2_IRQHandler(void)
{
   	if (TIM2->SR & (1<<0))  //UIF Bayragi mi
  {
			
			msn++;
		  birsn++;
		
		if(birsn>=1000)
		{
		  birsn=0;
			sureDoldu=1;
		}
		 
			if(msn>=65000)  
			{
				 msn=0;
			}
      TIM2->SR &=~(1<<0); //UIF temizlendi.
  }


}


int main()
	
{
  RCC_Config();//PLL ile sistem 168 Mhz.
	GPIO_Config();
	USART_Config();
	timer_Ayar();
	veri_yollaUSBTTL(" Maindeyim!!!!\r\n");
	veri_yollaUSBTTL(" Maindeyim!!!!\r\n");
	veri_yollaUSBTTL(" Maindeyim!!!!\r\n");
	//bekle(5000);
	veri_yollaESP8266(" AT\r\n");//Gelen cevap:AT\r\r\n\r\nOK\r\n
	NVIC->ISER[1] =1<<6;//USART2 Kesme Aktif.
	
	
	
	
while(1)
{
  
	
   
	
  if(sureDoldu==1)
	{
	 veri_yollaUSBTTL((char *)gelenCevap);
	 sureDoldu=0;
	}
	
	
	if(mod==0) 
		uygula("AT+CIPSTART=\"TCP\",\"184.106.153.149\",80\r\n","OK");//Thingspeak adresi
	
	if(mod==1)
		uygula("AT+CIPSEND=56\r\n","CONNECT");//56 bayt veri yollayacagiz ve CONNECT cevabi beklenir.
		
	if(mod==2) //AT+CIPSEND gonderildikten sonra 1sn oldu ise.&& msn>=1000
	{
		
		
		char adres []="GET /update?api_key=50AGOLCWM5CHEQ1K&field1=";
		char stringSayi[5];
		sprintf(stringSayi,"%d\r\n",gonderilecekSayi);
		strcpy(adres+strlen(adres),stringSayi);
		strcpy(adres+strlen(adres),"HTTP/1.1\r\n");
		uygula(adres,">");
		

		//uygula("GET /update?api_key=50AGOLCWM5CHEQ1K&field1=42\r\nHTTP/1.1\r\n",">");//GET /update?api_key=50AGOLCWM5CHEQ1K&field1=42
		msn=0;
		gonderilecekSayi++;
		if(gonderilecekSayi>=100)
			 gonderilecekSayi=10;
		
	}
	
	if(mod==3 && msn>=1000)
	{
		 msn=0;
		 char *ptr=NULL;
		 ptr=strstr((char *)gelenCevap,"+IPD");
		 if(ptr)
			{
				//veri_yollaESP8266("AT+CIPCLOSE=0\r\n");
			  mod=0;
	     // bekle(1000);
			}
	 
	}
	
    if(strstr((char *)gelenCevap,"ERROR") || strstr((char *)gelenCevap,"FAIL") || strstr((char *)gelenCevap,"busy p") ) //Herhangi bir anda ERROR geldiyse.
		{
			veri_yollaESP8266("AT\r\n");//Yeniden AT yolla.Süre ayarlanabilir(Ornegin 5 sn bir AT yolla busy durumunda ise.)
			mod=0;
			/*Ustte yazanlara OK veya CONNECT gibi cevaplar
			10 sn gibi bir sürede gelmiyorsa IWDG ile resetle.*/
			
			/*
			
			
			 
			
			*/
			
			
	  }
	 


}

}

/*

  USART3 USB-TTL STM32 arasi (Sadece veri yolla)
  USART2 ESP8266 STM32 arasi (Veri al-yolla.)
*/



