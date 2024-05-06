////////////////////////////////////////////////////////////////////////////////
////                   RUTINAS DE TECLADO
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
int const kInmuni=10;   //constante de tiempor para inmunidad de la tecla

int  rTempTec=0;   //imagen del teclado
int  rInmunidad=0; //tiempo que debe presionar la tecla para aceptar

int1 fTeclaOK=0;  //bandera que una tecla esta apretada    
int1 fIntPPL=0;   //ppl interno
int1 fIntPAA=0;   //paa interno
int1 fIntPAC=0;   //pac interno
int1 fExtPAA=0;   //paa externo
int1 fExtPPL=0;   //ppl externo
int1 fExtPAC=0;   //pac externo


//Scan del teclado
void ScanTeclado(void){
output_low(pMux2);
output_high(pMux1);
delay_us(10);
rTempTec=input_b();

      if (rTempTec!=0) //si=0 ninguna tecla apretada... me voy
         {
         rInmunidad--;
            if(rInmunidad==0 && fTeclaOK==0)
               {rInmunidad=kInmuni;
               switch (rTempTec){
                           case 1:fintPPL=1;
                                  fTeclaOK=1;
                                  break;
                           case 2:fintPAA=1;
                                  fTeclaOK=1;
                                  break;
                           case 4:fintPAC=1;
                                  fTeclaOK=1;
                                  break;
                           case 16:fextPPL=1;
                                   fTeclaOK=1;
                                   break;
                           case 32:fextPAA=1;
                                   fTeclaOK=1;
                                   break;
                           case 64:fextPAC=1;
                                   fTeclaOK=1;
                                   break;                                  
                           default:fTeclaOK=0;
                                   rInmunidad=kInmuni;
                                   break;
                                 }
               }
            }
      else{
      rInmunidad=kInmuni;
      fTeclaOK=0;
            fIntPPL=0;   //ppl interno
            fIntPAA=0;   //paa interno
            fIntPAC=0;   //pac interno
            fExtPAA=0;   //paa externo
            fExtPPL=0;   //ppl externo
            fExtPAC=0;   //pac externo
      }
            if(fIntPAA||fExtPAA||fMdbPAA)fPAA=1,fMdbPAA=0;
            else fPAA=0;
            if(fIntPAC||fExtPAC||fMdbPAC)fPAC=1,fMdbPAC=0;
            else fPAC=0;
            if(fIntPPL||fExtPPL)fPPL=1;
            else fPPL=0;
}
