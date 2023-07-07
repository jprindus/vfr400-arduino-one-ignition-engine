/*este programa esta realizado para ejecutarse como unidad de encendido electronico
   en una HONDA VFR 400 NC24 modelo 87 con rueda fonica de 12-1 dientes,en un arduino
   NANO o UNO o en un ATmega 328p realizando la adaptacion en lenguaje c
*/


#define sensor1 2
#define sensor2 4
#define salidaRPM 10
#define cilindro1  5
#define cilindro2  6
#define cilindro3  7
#define cilindro4  8
#define diente     11
volatile int numeroDIENTE = 0;   //defino como variable global
float rpm = 0;            //defino como variable
unsigned int TiempoEntreDientes = 0;
char sensor2estado = 0;
char sensor1estado = 0;
int retrasovector[] = {255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 217, 200, 186, 174, 163, 153, 145, 137, 125, 110, 102, 94,  87,  80,  74,  68,  63,  58,  54,  50,  46,  42,  38,  35,  32,  29,  26,  23,  21,  21,  21,  21,  21,  21,  21,  21,  21,  21,  21,  20,  19,  18,  17,  16,  15,  14,  14,  13,  12,  12,  11,  10,  9, 9, 8, 8, 7, 6 , 6, 7, 7, 7, 8, 8, 9, 9, 10,  10,  10,  10,  9, 9, 8, 8, 7, 7, 7, 6, 6, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4 , 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,};
int retraso = 255;
int posicion_retraso_vector = 0;
int bobinaON[] = {1, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 10, 10, 10, 10};
int rpm1 = 1;
int NumeroDientesAntesDeDispararseDondeEnciendeBobina = 1;
int rpm2 = 100;
volatile int cont = 0;
char dientefalta = 1;






void setup() {


  pinMode(sensor1, INPUT_PULLUP);
  pinMode(sensor2, INPUT_PULLUP);
  pinMode(cilindro1, OUTPUT);
  pinMode(cilindro2, OUTPUT);
  pinMode(cilindro3, OUTPUT);
  pinMode(cilindro4, OUTPUT);
  pinMode(salidaRPM, OUTPUT);
  pinMode(diente, OUTPUT);
  attachInterrupt(0, encendido, FALLING);
  MCUCR |= (1 << ISC01) | (0 << ISC00);



  // Configuracion del timer1 en modo normal (para medir rpm)
  cli();                                         //desactiva las interrupciones para poder configurar sin problemas

  TCCR1A = 0;                                    // El registro de control A queda todo en 0, pines OC1A y OC1B deshabilitados
  TCCR1B = 0;                                    // limpia el registrador
  TCCR1B |= (1 << CS11) | (1 << CS10) ;          // configura prescaler en 64: CS11 = 1 y cs10=1,me da un tiempo entre 4 microsegundos a 262 milisegundos a
  TCNT1 = 0x00;                                  // inicia timer para desbordamiento en 0

  sei();







}
void loop() {
  for (;;) {

    sensor2estado = digitalRead(sensor2);
    //lectura sensor 2 para comparar en la funcion encendido

    PORTB &= ~(1 << PB3);                                       //digitalWrite(diente, LOW);

    
    if (cont != 0) {
      TiempoEntreDientes = TCNT1;                                 //toma el valor del timer1
      TCNT1 = 0;                                                  //reinicia el timer1
      PORTB |= (1 << PB3);                                        //digitalWrite(diente, HIGH);


      ////////////////////////////////////////////////////////////////////CONTADOR DE DIENTES

      if (sensor2estado == LOW) {                                //verifica si por sensor 2 esta pasando el diente faltante y asi reinicia conteo
        numeroDIENTE++;
      }                                                          // suma 1 al diente anerior
      else {
        numeroDIENTE = 0;
      }                                                          //si sensor 2 esta en alto reseteo el numero de diente para q arranque el conteo de nuevo





      /////////////////////////////////////////////////////////////////////////AVANCE SEGUN RPM

      if (rpm >= 100) {
        rpm2 = ceil (rpm);
        rpm2 = rpm2 / 100;
        posicion_retraso_vector = ceil (rpm2);       //redondea valor de rpm/100....osea 1000rpm seria posicion 10 en el vector y 14000 la posicion 140
      }
      if (rpm <= 1200) {                               //maximo retraso
        
        switch  (numeroDIENTE) {                      //segun el diente que se estee ejecutando apaga la bobina correspondiente
          case (4):                                   //al ser maximo retraso lo apaga un diente mas tarde...osea 10 grados de avance
            PORTD &= ~(1 << PD5);                     //digitalWrite(cilindro1, LOW);
            break;
          case (7):
            PORTD &= ~(1 << PD6);                       //digitalWrite(cilindro2, LOW);
            break;
          case (10):
            PORTD &= ~(1 << PD7);                       //digitalWrite(cilindro3, LOW);
            break;
          case (2):
            PORTB &= ~(1 << PB0);                      //digitalWrite(cilindro4, LOW);
            break;
        }
      }
      else if ( rpm >= 10000  && rpm <= 14000) {             //minimo retraso
        
        switch  (numeroDIENTE) {                             //segun el diente que se estee ejecutando apaga la bobina correspondiente
          case (3):                                          //al ser minimo retraso lo apaga instaneamente.. osea 40 grados de avance
            PORTD &= ~(1 << PD5);                            //digitalWrite(cilindro1, LOW);
            break;
          case (6):
            PORTD &= ~(1 << PD6);                           //digitalWrite(cilindro2, LOW);
            break;
          case (9):
            PORTD &= ~(1 << PD7);                          //digitalWrite(cilindro3, LOW);
            break;
          case (1):
            PORTB &= ~(1 << PB0);                           //digitalWrite(cilindro4, LOW);
            break;
        }
      }
      else if (rpm >= 14001) {
        
        retraso = 8 ;                                               //este valor me genera un avance de 30 grados para hacer un corte
      }                                                            //de rpm suave...mas rpm mas avance

      else {
        retraso = retrasovector [posicion_retraso_vector];                            //retraso tma el valor cargado en array

        ////////////////////////////////////////////////////////////////////////TIMER MODO CTC

        //Configuração do TIMER0 modo CTC comparacion (para retraso de encendido)

        TCCR0A = 0;                     // El registro de control A queda todo en 0
        TCCR0B = 0;                     // limpia registrador
        TCNT0  = 0;                     // Inicializa el temporizador
        TCCR0A |= (1 << WGM01);         // modo ctc
        TCCR0B |= (1 << CS02);          // prescaler de 256
        TIMSK0 |= (1 << OCIE0A);        // habilita interrupción por igualdade de comparación
        OCR0A = retraso;                // carga el registrador de comparación (dato de tiempo de retraso)dearranque seria el tiemp maximo

      }



      ///////////////////////////////////////////////////////////////////////RETRASO BOBINA ON
      
        if (NumeroDientesAntesDeDispararseDondeEnciendeBobina == 1) {                //indica cuantos dientes antes se enciende labobina segun rpms

          //cuando el retraso de bobinaON e 1 se activa un diente antes
          if (numeroDIENTE == 2) {

            PORTD |= (1 << PD5);             //digitalWrite(cilindro1, HIGH);
          }
          else if (numeroDIENTE == 5) {
            PORTD |= (1 << PD6);             //digitalWrite(cilindro2, HIGH);
          }
          else if (numeroDIENTE == 8) {
            PORTD |= (1 << PD7);               //digitalWrite(cilindro3, HIGH);
          }
          else if (numeroDIENTE == 0) {
            PORTB |= (1 << PB0);              //digitalWrite(cilindro4, HIGH);
          }
        }
        else  if (NumeroDientesAntesDeDispararseDondeEnciendeBobina == 2) {

          //cuando el retraso de bobinaON e 1 se activa un diente antes
          if (numeroDIENTE == 2) {

            PORTD |= (1 << PD5);                       //digitalWrite(cilindro1, HIGH);
          }
          else if (numeroDIENTE == 4) {
            PORTD |= (1 << PD6);                      //digitalWrite(cilindro2, HIGH);
          }
          else if (numeroDIENTE == 7) {
            PORTD |= (1 << PD7);                     //digitalWrite(cilindro3, HIGH);
          }
          else if (numeroDIENTE == 10) {
            PORTB |= (1 << PB0);                     //digitalWrite(cilindro4, HIGH);
          }
        }
        else  if (NumeroDientesAntesDeDispararseDondeEnciendeBobina == 3) {

          //cuando el retraso de bobinaON e 1 se activa un diente antes
          if (numeroDIENTE == 1) {

            PORTD |= (1 << PD5);          //digitalWrite(cilindro1, HIGH);
          }
          else if (numeroDIENTE == 3) {
            PORTD |= (1 << PD6);          //digitalWrite(cilindro2, HIGH);
          }
          else if (numeroDIENTE == 6) {
            PORTD |= (1 << PD7);           //digitalWrite(cilindro3, HIGH);
          }
          else if (numeroDIENTE == 9) {
            PORTB |= (1 << PB0);           //digitalWrite(cilindro4, HIGH);
          }
        }
        else if (NumeroDientesAntesDeDispararseDondeEnciendeBobina == 4) {

          //cuando el retraso de bobinaON e 1 se activa un diente antes
          if (numeroDIENTE == 10) {

            PORTD |= (1 << PD5);                //digitalWrite(cilindro1, HIGH);
          }
          else if (numeroDIENTE == 2) {
            PORTD |= (1 << PD6);                 //digitalWrite(cilindro2, HIGH);
          }
          else if (numeroDIENTE == 5) {
            PORTD |= (1 << PD7);                //digitalWrite(cilindro3, HIGH);
          }
          else if (numeroDIENTE == 8) {
            PORTB |= (1 << PB0);                //digitalWrite(cilindro4, HIGH);
          }
        }
        else  if (NumeroDientesAntesDeDispararseDondeEnciendeBobina == 5) {

          //cuando el retraso de bobinaON e 1 se activa un diente antes
          if (numeroDIENTE == 9) {

            PORTD |= (1 << PD5);                //digitalWrite(cilindro1, HIGH);
          }
          else if (numeroDIENTE == 2) {
            PORTD |= (1 << PD6);                 //digitalWrite(cilindro2, HIGH);
          }
          else if (numeroDIENTE == 4) {
            PORTD |= (1 << PD7);                 //digitalWrite(cilindro3, HIGH);
          }
          else if (numeroDIENTE == 6) {
            PORTB |= (1 << PB0);                 //digitalWrite(cilindro4, HIGH);
          }
        }
        else  if (NumeroDientesAntesDeDispararseDondeEnciendeBobina == 6) {

          //cuando el retraso de bobinaON e 1 se activa un diente antes
          if (numeroDIENTE == 8) {

            PORTD |= (1 << PD5);                 //digitalWrite(cilindro1, HIGH);
          }
          else if (numeroDIENTE == 1) {
            PORTD |= (1 << PD6);                 //digitalWrite(cilindro2, HIGH);
          }
          else if (numeroDIENTE == 3) {
            PORTD |= (1 << PD7);                //digitalWrite(cilindro3, HIGH);
          }
          else if (numeroDIENTE == 5) {
            PORTB |= (1 << PB0);               //digitalWrite(cilindro4, HIGH);
          }
        }
        else  if (NumeroDientesAntesDeDispararseDondeEnciendeBobina == 7) {

          //cuando el retraso de bobinaON e 1 se activa un diente antes
          if (numeroDIENTE == 7) {

            PORTD |= (1 << PD5);               //digitalWrite(cilindro1, HIGH);
          }
          else if (numeroDIENTE == 10) {
            PORTD |= (1 << PD6);              //digitalWrite(cilindro2, HIGH);
          }
          else if (numeroDIENTE == 2) {
            PORTD |= (1 << PD7);                    //digitalWrite(cilindro3, HIGH);
          }
          else if (numeroDIENTE == 4) {
            PORTB |= (1 << PB0);                   //digitalWrite(cilindro4, HIGH);
          }
        }
        else if (NumeroDientesAntesDeDispararseDondeEnciendeBobina == 8) {

          //cuando el retraso de bobinaON e 1 se activa un diente antes
          if (numeroDIENTE == 6) {

            PORTD |= (1 << PD5);            //digitalWrite(cilindro1, HIGH);
          }
          else if (numeroDIENTE == 9) {
            PORTD |= (1 << PD6);           //digitalWrite(cilindro2, HIGH);
          }
          else if (numeroDIENTE == 2) {
            PORTD |= (1 << PD7);           //digitalWrite(cilindro3, HIGH);
          }
          else if (numeroDIENTE == 3) {
            PORTB |= (1 << PB0);           //digitalWrite(cilindro4, HIGH);
          }
        }
        else  if (NumeroDientesAntesDeDispararseDondeEnciendeBobina == 9) {

          //cuando el retraso de bobinaON e 1 se activa un diente antes
          if (numeroDIENTE == 5) {

            PORTD |= (1 << PD5);                 //digitalWrite(cilindro1, HIGH);
          }
          else if (numeroDIENTE == 8) {
            PORTD |= (1 << PD6);                  //digitalWrite(cilindro2, HIGH);
          }
          else if (numeroDIENTE == 1) {
            PORTD |= (1 << PD7);                  //digitalWrite(cilindro3, HIGH);
          }
          else if (numeroDIENTE == 2) {
            PORTB |= (1 << PB0);                 //digitalWrite(cilindro4, HIGH);
          }
        }
        else  if (NumeroDientesAntesDeDispararseDondeEnciendeBobina == 10) {

          //cuando el retraso de bobinaON e 1 se activa un diente antes
          if (numeroDIENTE == 4) {

            PORTD |= (1 << PD5);                 //digitalWrite(cilindro1, HIGH);
          }
          else if (numeroDIENTE == 7) {
            PORTD |= (1 << PD6);                 //digitalWrite(cilindro2, HIGH);
          }
          else if (numeroDIENTE == 10) {
            PORTD |= (1 << PD7);                 //digitalWrite(cilindro3, HIGH);
          }
          else if (numeroDIENTE == 2) {
            PORTB |= (1 << PB0);                //digitalWrite(cilindro4, HIGH);
          }
        }
      



      ////////////////////////////////////////////////////////////////////////

      cont = 0;                                    //permite que el programa solamente entre al if cuando 'cont' cambia de estado


    }

    ////////////////////////////////////////////////////////////////////////MEDIR RPM

    if (numeroDIENTE == 3 ) {
      rpm = 60 / (0.000024 * TiempoEntreDientes);         //calcula rpm entre dientes 2 y 3,al ser doble el espacio uso la mitad del tiempo para el calculo
    }
    else {
      rpm = 60 / (0.000048 * TiempoEntreDientes);         //calcula rpm con espacio normal entre dientes
    }



    //////////////////////////////////////////////////////////////
    if (numeroDIENTE == 0) {                                  //cuando el contador de dientes se pone en 0 hasta valor 1 , envio pulso para medir rpm con otro controlador
      PORTB |= (1 << PB2);                                          //digitalWrite(salidaRPM, HIGH);
      ///////////////////////////////////////////////////////////////////NUMERO DIENTE PARA ENCENDER BOBINA


      if (rpm <= 1000) {
        NumeroDientesAntesDeDispararseDondeEnciendeBobina = 1;
      }
      else {
        rpm1 = rpm / 1000;
        rpm1 = ceil (rpm1);                                                 //obtengo valor rpm1 para vector bobinaON una sola vez por vuelta
        NumeroDientesAntesDeDispararseDondeEnciendeBobina = bobinaON[rpm1];         // seria el numero de dientes antes de dispararse donde debe encender la bobina
      }
    }
    else {
      PORTB &= ~(1 << PB2);       //digitalWrite(salidaRPM, LOW);
    }

  }

}


void encendido() {
  cli();
  cont ++;
  sei();
}



ISR(TIMER0_COMPA_vect) {                   //interrupcion generada por timer0 cuando se cumple tiempo de retardo
  cli();
  switch  (numeroDIENTE) {                //segun el diente que se estee ejecutando apaga la bobina correspondiente
    case (3):                             //para generar la chispa
      PORTD &= ~(1 << PD5);               //digitalWrite(cilindro1, LOW);
      break;
    case (6):
      PORTD &= ~(1 << PD6);               //digitalWrite(cilindro2, LOW);
      break;
    case (9):
      PORTD &= ~(1 << PD7);                 //digitalWrite(cilindro3, LOW);
      break;
    case (1):
      PORTB &= ~(1 << PB0);                 //digitalWrite(cilindro4, LOW);
      break;

  }

  TCCR0A = 0;                     // El registro de control A queda todo en 0
  TCCR0B = 0;                     // limpia registrador
  sei();

}
