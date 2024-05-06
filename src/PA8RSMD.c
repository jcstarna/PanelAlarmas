////////////////////////////////////////////////////////////////////////////////
////                   PANEL DE ALARMAS PA8R SMD
////FECHA:31/07/2012
////REVISION: 1.00
////AUTOR:CRTARG
////////////////////////////////////////////////////////////////////////////////

//******************************************************************************
//  NOTAS REVISIONES:
//
/*
Ver: 1.00
   Version inicial
*/



#include <16F883.h>
#fuses XT,NOWDT,PROTECT,NOLVP,NOBROWNOUT,MCLR,NODEBUG,PUT
#use delay(clock=4000000)
#use rs232(baud=9600, BITS=8, xmit=PIN_C6 ,rcv=PIN_C7, enable=PIN_A3, ERRORS)// RS232 Estándar 
#pragma use fast_io(A)
#pragma use fast_io(B)
#pragma use fast_io(C)
//valores por defecto de la EEPROM
#rom 0x2100={01,10,10,10,10,10,10,10,10,0,0}

#include <PA8RSMD.h>
#include <teclado.c>
#include "ModBus.h"
#include "Modbus.c"

void Esc_EEPROM(void);
// INTERRUPCIONES /////////////////////////////////////////////////////////////
#INT_RDA
void RX_isr(){
rcvchar=0x00;                        // Inicializo caracter recibido
   if(kbhit()){                           // Si hay algo pendiente de recibir ...
      rcvchar=getc();                     // lo descargo y ...
      ModBusRX(rcvchar);                // lo añado al buffer y ... 
   }   
   setup_timer_2(T2_DIV_BY_16,0xF0,10);
}


#int_TIMER2
void TMR2_isr(){
int16 CRC=0;
int16 CRC_Leido=0;
   IF(fParami){                        //Calculo checksum de dato recibido
         CRC=CRC_Calc(rxbuff,rxpuntero-3); //
         CRC_Leido= make16(rxBuff[rxpuntero-2],rxBuff[rxpuntero-1]);
         if(CRC_Leido==CRC)flagcommand=1;
            setup_timer_2(T2_DISABLED,0,1);
            rxpuntero=0;
         }
   else{
         inicbuffRX();
         rxpuntero=0;
         }
}


#INT_TBE
void TX_isr(){
   ModBusTX ();
}

//PROTOTIPO DE FUNCIONES
Void ScanAlarmas(Void);
Void ActLed(void);
Void ActEstado(void);
void LogicaAl(void);
void Comando(void);
void Lee_EEPROM(void);
int8 norPort(int8 Dato);
int8 norDir (int8 Dir);

#int_RTCC
void RTCC_isr() {                      // Interrupción Timer 0
if (--rTmr10ms==0){ //pasaron los 10ms?
   rTmr10ms=k10ms;
   ScanTeclado();
   ScanAlarmas();
   
   if(--rTmr05s==0){//pasaron 500ms?
      rTiempos++;
      rBTAl=rTiempos & rPreBTAl;
      //output_toggle(pled);
      rTmr05s=k05s;
      if(f05seg)
         f05seg=0;
      else
         f05seg=1;
      }
}
if (rSpTrAl!=0) fAutoRes=1;
else fAutoRes=0;
}

//FUNCIONES
void set_LED(int LEDS){
int i=0;
int8 temp=0;
//acomodo leds de acuerdo a posicion en mux
//A1
if(bit_test(LEDS,0)) bit_set(temp,3);
else bit_clear(temp,3);
//A2
if(bit_test(LEDS,1)) bit_set(temp,4);
else bit_clear(temp,4);
//A3
if(bit_test(LEDS,2)) bit_set(temp,5);
else bit_clear(temp,5);
//A4
if(bit_test(LEDS,3)) bit_set(temp,6);
else bit_clear(temp,6);
//D1
if(bit_test(LEDS,4)) bit_set(temp,2);
else bit_clear(temp,2);
//D2
if(bit_test(LEDS,5)) bit_set(temp,1);
else bit_clear(temp,1);
//D3
if(bit_test(LEDS,6)) bit_set(temp,0);
else bit_clear(temp,0);
//D4
if(bit_test(LEDS,7)) bit_set(temp,7);
else bit_clear(temp,7);

output_low(pG);
   for(i=0;i<8;i++){
      IF (bit_test(temp,I))output_high(pSER);//
      ELSE output_low(pSER);
      delay_us( 100 );
      output_high(pSCK);
      delay_us( 100 );
      output_low(pSCK);
      //LEDS<<1;
   }
output_LOW(pRCK);
delay_us( 100 );
output_high(pRCK);
}

////////////////////////////////////////////////////////
//                         MAIN
////////////////////////////////////////////////////////
void Main(){
int8 rTempDir=0;
//++++++++++++++++++Configura puertos analogicos++++++++++++++++++++++++++++++
setup_timer_0(RTCC_INTERNAL|RTCC_DIV_4);  //interrumpe cada 1.024ms
setup_timer_2(T2_DIV_BY_4,0xF0,10);        //control de inactividad del bus
setup_adc_ports( ADC_OFF );               //no analogicas
setup_comparator(NC_NC_NC_NC);            //no comparadores

//Configuracion de puertos 
set_tris_a(0B11010101);
set_tris_b(0XFF);
set_tris_c(0B10000000);

//Inicio Variables
rTmr10ms=k10ms;
rTmr05s=k05s;

//Seteos iniciales
output_high(pLed);
//encendido de los leds para testeo
set_LED(255);
delay_us(300);
rInmunidad=15; 
rLeds=0;
fProg=0;
//lee configuracion del equipo
Lee_EEPROM ();
rA1=rpA1;
rA2=rpA2;
rA3=rpA3;
rA4=rpA4;
rD1=rpD1;
rD2=rpD2;
rD3=rpD3;
rD4=rpD4;
rTrAl=rSpTrAl;
if (rSpTrAl!=0) fAutoRes=1;


////seteo interrupciones

   enable_interrupts(INT_rda);
   enable_interrupts(int_TIMER2);                  // Habilita Interrupción TIMER2   
   enable_interrupts(INT_RTCC);   
   delay_us(500);
   enable_interrupts(global);   
   delay_ms(1000);
if(fIntPPl)fProg=1,fIntPPL=0;

set_LED(0);   

while(true){
   if (fProg){ 
      //Led azul para indicar modo prog.
      if(f05seg)
         output_high(pled);
      else
         output_low(pled);
         
      rTempDir=norDir(ModbusAddress);
      set_led(rTempDir);
      if(fIntPAA)
         ModbusAddress--,fIntPAA=0;
      if(fIntPAC)
         ModbusAddress++,fIntPAC=0;
      if(fIntPPL){
         write_eeprom(kaddress,ModbusAddress);
         fIntPPL=0;
         set_led(255);
         delay_ms(1000);
         set_led(0);
         fProg=0;
         output_high(pled);
      }
   }
   else{ //Si esta en modo Normal
         //Logica Alarma
           LogicaAl();
         //Actualiza estados
           ActEstado();           
         //Actualizar leds
           ActLed();
         //Revisa comando desde modbus
           Comando();
         //autoreset de campana
           if (fCampana & fAutoRes){
               if (rBTAl != rAuxBTAl){
                  output_toggle(pled);
                  if(--rTrAl==0){
                     fCampana=0;
                     rTrAl=rSpTrAl;
                     output_high(pled);
                  }
               }
               rAuxBTAl=rBTAl;
           }
      if(flagcommand){         // Si hay comando pendiente lo procesa
               ModBus_exe();        
               flagcommand=0;
               inicbuffRX();
               rxpuntero=0; 
         }
      if(fEscEEPROM){
         fEscEEPROM=0;
         Esc_EEPROM();
      }
      }
   }
}   
////////////////////////////////////////////////////////
////////////////////////////////////////////////////////

//detecta cambios en las entradas, detecta 
//flancos de aparicion de alarmas
void ScanAlarmas(void){
//activo para ver las alarmas
output_low(pMux1);
output_high(pMux2);
delay_us(50);
//leo alarmas
rAlarmas = norPort(input_b());
   //Alarma 1
   if (pA1){
       if(--rA1==0)fA1=1; //
   }
   else fA1=0, rA1=rpA1;
   //Alarma 2
   if (pA2){
       if(--rA2==0)fA2=1;//
   }
   else fA2=0, rA2=rpA2;   
   //Alarma 3
   if (pA3){
       if(--rA3==0)fA3=1;//
   }
   else fA3=0, rA3=rpA3;
   //Alarma 4
   if (pA4){
       if(--rA4==0)fA4=1;//
   }
   else fA4=0, rA4=rpA4;   
   //Disparo 1
   if (pD1){
      if(--rD1==0)fD1=1; // 
   }
   else fD1=0, rD1=rpD1;   
   //Disparo 2
   if (pD2){
       if(--rD2==0)fD2=1;//
   }
   else fD2=0, rD2=rpD2;    
   //Disparo 3
   if (pD3){
       if(--rD3==0)fD3=1; //
   }
   else fD3=0, rD3=rpD3;   
   //Disparo 4
   if (pD4){
       if(--rD4==0)fD4=1; //
   }
   else fD4=0, rD4=rpD4;  
   
  
   //detecto flanco positivo de las entradas
   //hago complemento a uno del estado anterior
   //y hago un and bit a bit para detectar alarma recien aparecida
   rFpAl= rEstado & ~rEstAntAl;
   rEstAntAl=rEstado;
   //si hay flanco positivo de alguna, activo alarma
   if ( rFpAl!=0){
      fCampana=1;
      rFpAlTemp=rFpAl;
   }
}

//acutaliza leds de frente
void ActLed(void){
   set_LED(rLeds);
}

//Actualizo estados
//Reviso estado de la alarma y modifico
//registro de estado de cada alarma
void actEstado(void){
//recorro los flancos positivos
//si aparece coloco un uno en estado
int16 DirReg=0;   //direccion del primer registro de estado
int i=0;
DirReg=&eA1;
//recorro todas las alarmas
   for(i=0;i<8;i++){
      if (bit_test(rFpAl,i))        //si hay flanco positivo estado=1 -> recien aparecida
         *(DirReg+i)=1;
         
      if (fPAA && *(DirReg+i)==1)  //si presiona aceptar alarma y esta en estado 1
         *(DirReg+i)=2;            //modifico a aceptada
         
      if (*(DirReg+i)==2 && !bit_test(rAlarmas,i))   //si esta aceptada y desaparece
         *(DirReg+i)=0;                              //pongo estado 0 normal
   }
   
}

//Logica de la alarma, modifica el estado de los leds
//hace titilar o fijo dependiendo el estado de la alarma
void LogicaAl(void){
int temp=0;
temp=255;
if (fPAC) fCampana=0,output_high(pled);
if (!fCampana){
     if(fPAA)  //si pulso aceptar alarma cargo 0 en temp
        temp=0;
}    
    temp = temp & rMaux;   //borro alarma recien aparecida
    rMaux = temp | rFpAlTemp;    //reviso si aparecio nueva alarma
    rFpAlTemp=0;
    
    if (f05seg)            //para que titilen alarmas recien
      temp=255;            //aparecidas
    else
      temp=0;
    
    rTempLed=temp & rMaux;    //enmascaro para titilar si hay alarma sin reconocer
    
    temp = rEstado & ~rMaux;
    
    rLeds = rtempLed | temp;
   
   if (fPPL)               //si pulso ppl pongo todo a 1 para encender leds
      temp=255;
   else
      temp=0;
      
    rLeds=temp | rLeds;

   if (fCampana)output_high(pAlarma);
   else output_low(pAlarma);
}

//Interpreta y ejecuta comando recibido por modbus
//1.- Acepta campana
//2.- Acepta alarma
Void Comando(){
int16 tempCMD=0;
if (rComando!=0){
   tempCMD=rComando;
   rComando=0;//pone comando a cero
   switch (tempCMD){
      case 1:fMdbPAC=1;
             break;
      case 2:fMdbPAA=1;
             break;
      default: break;
   }
}
}


//RECUPERA DATOS DE LA EEPROM
Void Lee_EEPROM (void){
int temp=0;
//Dir mem 0 direccion modbus
//si dir > de 127 vel modbus 19200 
//si dir < 127 vel = 9600
temp= read_EEPROM(kAddress);
if (temp>127){
   ModbusAddress=temp-127;
   setup_uart(19200);
}
else {
   ModbusAddress=temp;
   setup_uart(9600);
}
rpA1=read_EEPROM(krpA1);
rpA2=read_EEPROM(krpA2);
rpA3=read_EEPROM(krpA3);
rpA4=read_EEPROM(krpA4);
rpD1=read_EEPROM(krpD1);
rpD2=read_EEPROM(krpD2);
rpD3=read_EEPROM(krpD3);
rpD4=read_EEPROM(krpD4);
rSpTrAl=read_EEPROM(kTrAl);
rPreBTAl=read_EEPROM(kPreBTAl);
}

int8 norPort(int8 Dato){
int8 temp=0;
//A1
if(bit_test(Dato,3)) bit_set(temp,0);
else bit_clear(temp,0);
//A2
if(bit_test(Dato,2)) bit_set(temp,1);
else bit_clear(temp,1);
//A3
if(bit_test(Dato,1)) bit_set(temp,2);
else bit_clear(temp,2);
//A4
if(bit_test(Dato,0)) bit_set(temp,3);
else bit_clear(temp,3);
//D1
if(bit_test(Dato,7)) bit_set(temp,4);
else bit_clear(temp,4);
//D2
if(bit_test(Dato,6)) bit_set(temp,5);
else bit_clear(temp,5);
//D3
if(bit_test(Dato,5)) bit_set(temp,6);
else bit_clear(temp,6);
//D4
if(bit_test(Dato,4)) bit_set(temp,7);
else bit_clear(temp,7);
return temp;
}

int8 norDir(int8 Dir){
int8 temp=0;
//A1
if(bit_test(Dir,7)) bit_set(temp,0);
else bit_clear(temp,0);
//A2
if(bit_test(Dir,6)) bit_set(temp,1);
else bit_clear(temp,1);
//A3
if(bit_test(Dir,5)) bit_set(temp,2);
else bit_clear(temp,2);
//A4
if(bit_test(Dir,4)) bit_set(temp,3);
else bit_clear(temp,3);
//D1
if(bit_test(Dir,3)) bit_set(temp,4);
else bit_clear(temp,4);
//D2
if(bit_test(Dir,2)) bit_set(temp,5);
else bit_clear(temp,5);
//D3
if(bit_test(Dir,1)) bit_set(temp,6);
else bit_clear(temp,6);
//D4
if(bit_test(Dir,0)) bit_set(temp,7);
else bit_clear(temp,7);
return temp;
}

//RECUPERA DATOS DE LA EEPROM
Void Esc_EEPROM (void){
write_EEPROM(krpA1,rpA1 );
write_EEPROM(krpA2,rpA2);
write_EEPROM(krpA3,rpA3);
write_EEPROM(krpA4,rpA4);
write_EEPROM(krpD1,rpD1);
write_EEPROM(krpD2,rpD2);
write_EEPROM(krpD3,rpD3);
write_EEPROM(krpD4,rpD4);
write_EEPROM(kTrAl,rSpTrAl);
write_EEPROM(kPreBTAl,rPreBTAl);
}
