/*
 * test_proj.c
 *
 * Created: 4/14/2020 6:21:50 PM
 * Author : kerickson
 */

//#include <avr/io.h>




#include "avr.h"

#include "lcd.h"

int ret;
int aret;
struct statistic{
   int current;
   int min;
   int max;
   long sum;
   int cnt;
} stats;



void update_stats(int s){
   stats.current = s;
   if(stats.cnt==0){
      stats.min = s;
      stats.max = s;
   }

   if (stats.min > s){
      stats.min = s;
   }
   if (stats.max < s){
      stats.max = s;
   }
   stats.sum += s;
   stats.cnt ++;
}

void clear_stats(){
   //stats.current = 0;
   stats.sum =  0;
   stats.cnt = 0;
   stats.min = 0;
   stats.max = 0;
}

int get_sample(){
   CLR_BIT(ADMUX, 7);
   SET_BIT(ADMUX, 6);

   CLR_BIT(ADMUX, 3);
   CLR_BIT(ADMUX, 2);
   CLR_BIT(ADMUX, 0);

   SET_BIT(ADCSRA, 7);
   SET_BIT(ADCSRA, 6);
   while((GET_BIT(ADCSRA,6)));
   aret = ADC;
   return;
}

int get_diffsample(){
   CLR_BIT(ADMUX, 7);
   SET_BIT(ADMUX, 6);

   CLR_BIT(ADMUX, 4);
   CLR_BIT(ADMUX, 3);
   CLR_BIT(ADMUX, 2);
   CLR_BIT(ADMUX, 1);
   SET_BIT(ADMUX, 0);

   SET_BIT(ADCSRA, 7);
   SET_BIT(ADCSRA, 6);
   while((GET_BIT(ADCSRA,6)));

   //int ret = ADC;
   ret = ADC;
   return;
}

void vdisplay(){
   float t;
   float m;
   char buf[8];
   char bufb[8];
   char bufc[9];
   char bufd[9];
   lcd_clr();
   t = (stats.current/1024.0)*5.0;
   sprintf(buf, "C:%4.2f", t);
   m = ((stats.sum/1024.0)*5.0)/stats.cnt;

   sprintf(bufb, "A:%4.2f", m);
   lcd_pos(0,0);
   lcd_puts(buf);
   lcd_pos(1,0);
   lcd_puts(bufb);
   t = (stats.min/1024.0)*5.0;
   m = (stats.max/1024.0)*5.0;
   sprintf(bufc, "min:%4.2f", t);
   sprintf(bufd, "max:%4.2f", m);
   lcd_pos(0,7);
   lcd_puts(bufc);
   lcd_pos(1,7);
   lcd_puts(bufd);
   //do other parameters: avg, min, max...
   return;

}

void ndisplay(){
   char buf[8];
   char bufb[8];
   char bufc[9];
   char bufd[9];

   float t = (stats.current/1024.0)*5.0;
   sprintf(buf, "C:%4.2f", t);

   sprintf(bufb, "A:--.--");
   sprintf(bufc, "min:--.--");
   sprintf(bufd, "max:--.--");
   lcd_clr();
   lcd_pos(0,0);
   lcd_puts(buf);
   lcd_pos(1,0);
   lcd_puts(bufb);
   lcd_pos(0,7);
   lcd_puts(bufc);
   lcd_pos(1,7);
   lcd_puts(bufd);
   return;
}

void ddisplay(int n, int m){
   char charbuf[17];
   char numbuf [10];
   lcd_clr();
   float t = (n/1024.0)*5.0;
   float u = (m/1024.0)*5.0;
   sprintf(numbuf, "A: %4.2f", t);
   sprintf(charbuf, "B: %4.2f", u);
   //sprintf(charbuf, "DIFFERENTIAL V:");
   lcd_pos(0,0);
   lcd_puts(charbuf);
   lcd_pos(1,5);
   lcd_puts(numbuf);


}

void modavr_wait(unsigned short nsec){
   TCCR0 = 2;
   while (nsec--) {
      TCNT0 = (unsigned char)(256 - (XTAL_FRQ / 8) * 0.00002);
      SET_BIT(TIFR, TOV0);
      while (!GET_BIT(TIFR, TOV0));
   }
   TCCR0 = 0;
}


/*void wait(){
	volatile int i;
	for(i = 0; i<10000; ++i){
 //pass
	}
 }*/

void
myavr_wait(unsigned short msec)
{
   TCCR0 = 3;
   while (msec--) {
      TCNT0 = (unsigned char)(256 - (XTAL_FRQ / 64) * 0.001);
      SET_BIT(TIFR, TOV0);
      while (!GET_BIT(TIFR, TOV0));
   }
   TCCR0 = 0;
}


int is_pressed(int r, int cc){
   //set all pins to N/C
   DDRC = 0;
   PORTC = 0;
   //int check = 0;
   /*for(check = 0; check<r; ++check){
    SET_BIT(PORTB, 0);
    myavr_wait(500);
    CLR_BIT(PORTB, 0);
    myavr_wait(500);
    }
    myavr_wait(2000);*/

   int row = 7 -r;
   SET_BIT(DDRC, row);
   CLR_BIT(PORTC, row);

   CLR_BIT(DDRC, cc);
   SET_BIT(PORTC, cc);
   avr_wait(1);
   if (GET_BIT(PINC, cc)){
      //if NOT 0, return NOT PRESSED
      return 0;
   }
   return 1;



   //set r pin to strong 0 , set c pin to weak 1
   //read PIN @c
}


int get_key(void){
   int r, co;
   /*SET_BIT(PORTB, 0);
    myavr_wait(500);
    CLR_BIT(PORTB, 0);
    myavr_wait(500);*/
   //r = 3; r>-1; --r
   for (r = 3; r>-1; --r){
      for(co = 0; co<4; ++co){
         if(is_pressed(r,co)){
            return (co*4)+(3-r)+1;
         }
      }
   }
   return 0;
}


/*void
 myavr_wait(unsigned short msec)
 {
	TCCR0 = 3;
	while (msec--) {
 TCNT0 = (unsigned char)(256 - (XTAL_FRQ / 64) * 0.001);
 SET_BIT(TIFR, TOV0);
 while (!GET_BIT(TIFR, TOV0));
	}
	TCCR0 = 0;
 }*/




/*void display_dt(struct dt *dt){
	char buf[17];
	char bufb[17];
	sprintf(buf, "%02d/%02d/%04d", dt->month, dt->day, dt->year);
	sprintf(bufb, "%02d:%02d:%02d", dt->hour, dt->minute, dt->second);
	lcd_clr();
	lcd_pos(0,3);
	lcd_puts(buf);
	lcd_pos(1,0);
	lcd_puts(bufb);
 }*/








int main(void){
   lcd_init();
   lcd_clr();
   ndisplay(); //implement to display dashes...
   int mykey = 0;
   stats.cnt = 0;

   while(1){
      mykey = get_key();
      if (mykey==4){
         //RESET mode
         int tr = 0;
         while(tr!=8 && tr!=12){
            tr = get_key();
            lcd_clr();
            int an = get_sample();
            update_stats(an);
            ndisplay();
            clear_stats();
            //clear data from stats...
            avr_wait(500);
         }

         continue;

      }
      else if( mykey==8){
         int trackkey = 0;
         while(trackkey!=4 && trackkey!=12){
            trackkey = get_key();
            if (trackkey==4||trackkey==12){
               continue;
            }
            else{
               int newsamp = get_sample();
               update_stats(newsamp);
               vdisplay();
               avr_wait(500);
            }
         }
         continue;
      }
      else if(mykey==12){
         //differential mode...
         int track = 0;
         while(track!=4 && track!=8){
            track = get_key();
            if (track==4||track==8){
               continue;
            }
            else{
               //int newa = get_sample();
               //int newb = get_diffsample();
               get_sample();
               get_diffsample();
               //int newdiff = newa-newb;
               
               ddisplay(ret, aret);
               avr_wait(500);
            }
         }
         continue;
      }
      else{
         continue;
      }
   }
}
