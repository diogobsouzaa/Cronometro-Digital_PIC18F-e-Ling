 //Projeto 2 Cronômetro Digital com Timer e Interrupção usando PIC18F e Ling. C
 //Diogo Barboza de Souza -- NUSP: 12745657
 
 
 
 
 //Valores iniciais
 int inicio = 0;
 int botao = 0;
 signed char ucContador = -1;        // variavel global para a função incremento
 unsigned char FlagAux = 0;

void Incremento(unsigned char Contador)   // Bloco de incremento
{
     switch (Contador){
            case 0:{ latd = 0b00111111; break;}   // 0 no display de 7seg.
            case 1:{ latd = 0b00000110; break;}   // 1 no display de 7seg.
            case 2:{ latd = 0b01011011; break;}   // 2 no display de 7seg.
            case 3:{ latd = 0b01001111; break;}   // 3 no display de 7seg.
            case 4:{ latd = 0b01100110; break;}   // 4 no display de 7seg.
            case 5:{ latd = 0b01101101; break;}   // 5 no display de 7seg.
            case 6:{ latd = 0b01111101; break;}   // 6 no display de 7seg.
            case 7:{ latd = 0b00000111; break;}   // 7 no display de 7seg.
            case 8:{ latd = 0b01111111; break;}   // 8 no display de 7seg.
            case 9:{ latd = 0b01101111; break;}   // 9 no display de 7seg.
            default:{ PORTD =0; ucContador = -1; break;} // zera todo o PORTD (apaga os leds do display) e
            //reincia o contador
     }
}

void ConfigMCU()   // Configurações iniciais do microcontrolador
{
  // Configurando os pinos como digitais
  ADCON1 |= 0x0F;
  
  //ligando o display de 7 seg. do kit
   TRISA =0;
   PORTA =0b00001111;

  // Config. das portas
   TRISB.RB1 = 1;  // configura pino RB1 como entrada
   PORTB.RB1=1;
   TRISB.RB0 = 1;  // configura pino RB0 como entrada
   PORTB.RB0=1;
   TRISD=0; //define RD como saida
}

void timer0(){
     T0CON.TMR0ON = 0; // timer off para configurar
     if (botao == 0){
        //calculo --> ciclo de maq. * prescaler * (modo_8_16_bits - valor inicial)
       //1000,000us = 0.5 us * 128 *  (65536-x)
       //Xinicial = 49911
       //C2F7h em hexadecimal
       // valores iniciais carregados no timer para contagem até 1s na razão 128 e modo 16 bits PARA 1 SEGUNDO
     
       TMR0L = 0xF7;           //configurando temporizador para 1s
       TMR0H = 0xC2;
     }
     if (botao == 1){
      // valores iniciais carregados no timer para contagem até 1s na razão 128 e modo 16 bits  PARA 0,25 SEGUNDOS
     //ciclo de maq. * prescaler * (modo_8_16_bits - valor inicial)
     //250,000us = 0.5 us * 128 *  (65536-x)
     //Xinicial de 61629,75 --> ~61630 aproximadamente
     //F0BE em hexadecimal
     
       TMR0L = 0xBE;           //configurando temporizador para 0,25s
       TMR0H = 0xF0;
     }
     T0CON.TMR0ON = 1; // liga timer
}

void interrupt_high() iv 0x0008 ics ICS_AUTO {
    if(INTCON.INT0IF == 1 && FlagAux==0){
       botao = 0;
       timer0();
       //TMR0L = 0xF7;           //configurando temporizador
       //TMR0H = 0xC2;

       FlagAux=1;
     }
     
     if(INTCON.INT0IF == 1 && FlagAux==1)//certificação que a tecla já foi apertada
     {
          FlagAux=0;

     }
        
    if(INTCON3.INT1IF == 1 && FlagAux==0){

       botao = 1;
       timer0();
       //TMR0L = 0xBE;
       //TMR0H = 0xF0;

       FlagAux=1;
     }
     if(INTCON3.INT1IF == 1 && FlagAux==1)//certificação que a tecla já foi apertada
     {
      FlagAux=0;
     }
     
     //Reiniciando as flags de interrupção de int0 e int1
     INTCON.INT0IF = 0;
     INTCON3.INT1IF = 0;
}


void ConfigTIMER()
{
 // preparando TMR0

    T0CON = 0B00000110;
//  não usa borda, uso do prescaler (ativo em 0), e razão do prescaler: 110 (128)

    INTCON.TMR0IF = 0 ; //Flag de overflow zerada
}

void main() {

// chamada das fuções de configuração e preparo
ConfigMCU();
ConfigTIMER();

  INTCON.GIEH = 1; //habilitando alta prioridade

  RCON.IPEN = 1;  //habilitando níveis de prioridade

  INTCON.INT0IF = 0;    //Flag de interrupção int0 zerada
  INTCON.INT0IE = 1;  //Habilita a interrupção para INT0


  INTCON3.INT1IE = 1;       // habilita interrupção para INT1
  INTCON3.INT1IP = 1;       // colocando INT1 como alta prioridade
  INTCON3.INT1IF = 0;       //zera a flag de interrupção de INT1


  INTCON2.INTEDG1 = 1 ; //  borda de subida para INT1
  INTCON2.INTEDG0 = 1;  //  borda de subida para INT0

 //(pooling) aguardando a interrupção e o inicio da contagem
 while(1){
   // sinalizando que já foi apertado o botão pela primeira fez e o tempo de um periodo já se passou (flag = 1)
   if (INTCON.TMR0IF == 1){
          timer0();
          Incremento(++ucContador);  // chamada da função para a contagem
          INTCON.TMR0IF = 0; // zera o flag de overflow da contagem, para reiniciar o periodo
          
   }
 }
}