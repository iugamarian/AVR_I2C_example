#define F_CPU 16000000UL
#include <util/twi.h>
#include <avr/io.h>
#include <util/delay.h>
#define SET(x,y) (x|=(1<<y))
#define CLR(x,y) (x&=(~(1<<y)))
#define CHK(x,y) (x&(1<<y)) 
#define TOG(x,y) (x^=(1<<y))
 
//global variables
#define BUFLEN_RECV 12
uint8_t r_index =0;
uint8_t recv[BUFLEN_RECV]; //buffer to store received bytes

#define BUFLEN_TRAN 3
uint8_t t_index=0;
//test bytes to transmit
uint8_t tran[BUFLEN_TRAN] = {0x12, 0x34, 0x56}; 

//variable to indicate if something went horribly wrong
 uint8_t reset=0;

 //prototypes
void handleI2C();
void var_delay_us(uint16_t);
void makesnd(uint16_t);


//---------------MAIN---------------------------------------------
int main(){
  _delay_ms(300);	// shorter than master

  DDRB = 0x04; // 00000100 buzzer out
  PORTB= 0x00; // buzzer low
  //load slave address
 TWAR = (0x01<<1); //we're using address 0x01 
 //enable I2C hardware
  TWCR = (1<<TWEN)|(1<<TWEA);

 while(1){
  handleI2C();
 }
}
//-----------END MAIN---------------------------------------------

//setup the I2C hardware to ACK the next transmission
//and indicate that we've handled the last one.
#define TWACK (TWCR=(1<<TWINT)|(1<<TWEN)|(1<<TWEA))
//setup the I2C hardware to NACK the next transmission
#define TWNACK (TWCR=(1<<TWINT)|(1<<TWEN))
//reset the I2C hardware (used when the bus is in a illegal state)
#define TWRESET (TWCR=(1<<TWINT)|(1<<TWEN)|(1<<TWSTO)|(1<<TWEA))
void handleI2C(){
  //check if we need to do any software actions
  if(CHK(TWCR,TWINT)){
    switch(TW_STATUS){
//--------------Slave receiver------------------------------------
    //SLA_W received and acked, prepare for data receiving
		case 0x60:  
      TWACK;
      r_index =0;
      break;
    case 0x80:  //a byte was received, store it and 
                //setup the buffer to recieve another
      recv[r_index] = TWDR;
      r_index++;
      //don't ack next data if buffer is full
      if(r_index >= BUFLEN_RECV){
        TWNACK;
  _delay_ms(400);
  makesnd(recv[0]);	// this is int, all fractions are lost
  makesnd(recv[1]);	// this is int, all fractions are lost
  makesnd(recv[2]);	// this is int, all fractions are lost
  makesnd(recv[0]);	// this is int, all fractions are lost
  makesnd(recv[1]);	// this is int, all fractions are lost
  makesnd(recv[2]);	// this is int, all fractions are lost
  makesnd(recv[0]);	// this is int, all fractions are lost
  makesnd(recv[1]);	// this is int, all fractions are lost
  makesnd(recv[2]);	// this is int, all fractions are lost
  makesnd(recv[0]);	// this is int, all fractions are lost
  makesnd(recv[1]);	// this is int, all fractions are lost
  makesnd(recv[2]);	// this is int, all fractions are lost
  makesnd(recv[0]);	// this is int, all fractions are lost
  makesnd(recv[1]);	// this is int, all fractions are lost
  makesnd(recv[2]);	// this is int, all fractions are lost
  _delay_ms(100);
      }else {
    TWACK;
   }
   break;
    case 0x68://adressed as slave while in master mode.
              //should never happen, better reset;
      reset=1;
    case 0xA0: //Stop or rep start, reset state machine
      TWACK;
      break;
//-------------- error recovery ----------------------------------
    case 0x88: //data received  but not acked
      //should not happen if the master is behaving as expected
      //switch to not adressed mode
      TWACK;
      break;
//---------------Slave Transmitter--------------------------------
    case 0xA8:  //SLA R received, prep for transmission
		            //and load first data
      t_index=1;
      TWDR = tran[0];
      TWACK;
      break;
    case 0xB8:  //data transmitted and acked by master, load next
      TWDR = tran[t_index];
      t_index++;
      //designate last byte if we're at the end of the buffer
      if(t_index >= BUFLEN_TRAN)
      {
         TWNACK;
         _delay_ms(1000);
      }
      else TWACK;
      break;
    case 0xC8: //last byte send and acked by master
    //last bytes should not be acked, ignore till start/stop
      //reset=1;
    case 0xC0: //last byte send and nacked by master 
		//(as should be)
      TWACK;
      break;
//--------------------- bus error---------------------------------
    //illegal start or stop received, reset the I2C hardware
		case 0x00: 
      TWRESET;
      break;
    }
  }
}


void var_delay_us(uint16_t usvar)	// allow delays without constant, for buzzer
{
  while (usvar-- != 0)
    _delay_us(1);
}

void makesnd(uint16_t frequency)
{
		frequency=(frequency+50);
		uint16_t decrease = 0;
		decrease=4000/frequency; // buzzer will make sound repeated this many times
   		while(decrease){
			var_delay_us(frequency);	// buzzer frequency
			PORTB= 0x04; // 00000100
			var_delay_us(frequency);	// buzzer frequency
			PORTB= 0x00;
    	 	decrease--;
		}
    _delay_ms(200);
}
