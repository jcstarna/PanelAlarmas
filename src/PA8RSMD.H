////////////////////////////////////////////////////////////////////////////////
////                   PANEL DE ALARMAS PA8R SMD
//                            VARIABLES
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

//PUERTO A
#DEFINE  pLed     PIN_A1   //LED DE ENCENDIDO/CONFIG
#DEFINE  pMax485  PIN_A3   //AUTORIZACION DE TRANSMISION
#DEFINE  pAlarma  PIN_A5   //SALIDA RELE DE ALARMA


//PUERTO B
//PUERTO MULTIPLEXADO

//PUERTO C
#define  pMux1    PIN_C0
#define  pMux2    PIN_C1
#define  pG       PIN_C2
#DEFINE  pSER     PIN_C3
#define  pRCK     PIN_C4
#define  pSCK     PIN_C5

int const k10ms=10;
int const k05s=24;//50

//direcciones datos en EEPROM
int const kAddress=0;
int const krpA1=1;
int const krpA2=2;
int const krpA3=3;
int const krpA4=4;
int const krpD1=5;
int const krpD2=6;
int const krpD3=7;
int const krpD4=8;
int const kEstAl=9;  //estado del panel
int const kTrAl=10;  //tiempo de autoreset alarma 0 - no autoreset
int const kPreBTAl=11;

//VARIABLES
//FLAGS
int1  f05seg=0;//pulso de 1/2 segundo
int1  fProg=0;
int1  fPAC=0;  //Paralelo pulsadores
int1  fPAA=0;  //IDEM
int1  fPPL=0;  //idem
int1  fMdbPAA=0;  //PAA via modbus
int1  fMdbPAC=0;  //PAC via modbus
int1  fBusInact=0;
int1  fCampana=0;
int1  fAutoRes=0;
int1  fEscEEPROM=0;

//Tiempos
int rTmr10ms=0;//contador para llegar a 10ms
int rTmr05s=0; //contador para llegar a 500ms
int rTiempos=0;//registros con bits de tiempo
int rBTAl=1;   //Base de tiempo para alarma  
int rAuxBTAl=0;//auxiliar

//De Comunicacion
//INT RX_buff[10];
//INT TX_buff[10];
int RX_index=0;
int TX_index=0;
int rTmrBus=0; //tiempo de inactividad del bus
INT rSpTmrBus=5;  //tiempo de inact del bus, depende del baud rate


//De alarmas
int rAlarmas=0;//rAlarmas
int rEstAntAl=0;
int rFpAl=0;
int rFpAlTemp=0;
int rMAux=0;//marcas auxiliares para leds
int rLeds=0;//registro imagen de leds del frente
int rTempLed=0;
int rA1=0;
int rA2=0;
int rA3=0;
int rA4=0;
int rD1=0;
int rD2=0;
int rD3=0;
int rD4=0;
int rTrAl=0;

//Registros Modbus
int16 rEstado=1;     // rEstado
int16 rComando=2;    // 40002 
int16 rpA1=10;       // 40003
int16 rpA2=10;       // 40004
int16 rpA3=10;       // 40005
int16 rpA4=10;       // 40006
int16 rpD1=10;       // 40007
int16 rpD2=10;       // 40008
int16 rpD3=10;       // 40009
int16 rpD4=10;       // 40010
int eA1=0;           // 40011        
int eA2=0;           // 
int eA3=0;           // 40012
int eA4=0;           // 
int eA5=0;           // 40013
int eA6=0;           // 
int eA7=0;           // 40014
int eA8=0;           // 
int16 rSpTrAl=0;     // 40015
int16 rPreBTAl=0;    // 40016
int16 rBtn=0;        //




//bit alarmas (lectura fisica filtrada)
#bit     pD1   =  rAlarmas.4  //2
#bit     pD2   =  rAlarmas.5  //1
#bit     pD3   =  rAlarmas.6  //0
#bit     pD4   =  rAlarmas.7  //7  
#bit     pA1   =  rAlarmas.0  //3
#bit     pA2   =  rAlarmas.1  //4
#bit     pA3   =  rAlarmas.2  //5
#bit     pA4   =  rAlarmas.3  //6

//bits en palabra estado
#bit     fD1   =  rEstado.4
#bit     fD2   =  rEstado.5
#bit     fD3   =  rEstado.6
#bit     fD4   =  rEstado.7
#bit     fA1   =  rEstado.0
#bit     fA2   =  rEstado.1
#bit     fA3   =  rEstado.2
#bit     fA4   =  rEstado.3

//bits flancos positivos
#bit     fpD1   =  rFpAl.4
#bit     fpD2   =  rFpAl.5
#bit     fpD3   =  rFpAl.6
#bit     fpD4   =  rFpAl.7
#bit     fpA1   =  rFpAl.0
#bit     fpA2   =  rFpAl.1
#bit     fpA3   =  rFpAl.2
#bit     fpA4   =  rFpAl.3
