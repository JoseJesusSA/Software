#include <18F4550.h>
#device adc=10
//#fuses HSPLL,NOPUT,NOWDT,NOBROWNOUT,NOLVP,NODEBUG,USBDIV,VREGEN,PLL5,NOPBADEN,CPUDIV1
#fuses HS,NOWDT,NOPROTECT,NOPUT,NOLVP,NOCPD,NODEBUG,NOWRT
#use delay(clock=20000000)
#use i2c(Master,sda=PIN_B0,scl=PIN_B1)
#use rs232(baud=115200, parity=N, xmit=PIN_C6, rcv=PIN_C7)
#include <lcd.c>   
#include <kbd_lib.c>
#include <DS1307_2.c>
#define eeprom_rojo 254
#define eeprom_blanco 255
#define bt 0
#define ene 1
#define o_acento 2
#define bloqueo 3
#define signo 4
#include <stdlib.h>

int amanecer,anochecer,diah,diam,nocheh,nochem;
int hr,min,sec,dia,sem,mes,anio;
int blanco=0;
int rojo=0;
int salida=0;
int transicion=0;
int16 num_incrementos,total_incrementos;
float incremento,total,incremento_r,incremento_b,total_r,total_b;
int Modo_BT=0;
int PWM[3];
int Envio[20];
int Reg[500];
int Aux0[2],Aux1[2],Aux2[2],Aux3[2],Aux4[2],Aux5[2];
int16 Valor_PWM;
int Valor_PWM1;

int control=0;
int confirmacion=0;
unsigned int a1;
unsigned int a2;

int s=0;
int i,j,k,l,m,n,o,p,q=0,env,r,t;
int16 v=1;
int w=0;
int a,b,c,d=0;
char dato;
int Cantidad_Eventos[3];
int limite=0;
int Reloj[6];
int pantalla=0;
int lock=0;

void lock_cont()
{
   tiempo(hr,min,sec,dia,sem,mes,anio);
   q=min;
   q=q+2;
   if(min==59)
   {
      q=1;
   }
}

void lcd_caracteres_especiales()
{
lcd_send_byte(0,0x40); // escribimos en la cgram
lcd_send_byte(1,0X06); // | | |x|x| |
lcd_send_byte(1,0x15); // |x| |x| |x|
lcd_send_byte(1,0x0E); // | |x|x|x| |
lcd_send_byte(1,0x04); // | | |x| | |
lcd_send_byte(1,0x0E); // | |x|x|x| |
lcd_send_byte(1,0x15); // |x| |x| |x|
lcd_send_byte(1,0x06); // | | |x|x| |
lcd_send_byte(1,0x00); // | | | | | |
/*-------------------------------------------------------------------*/
lcd_send_byte(0,0x48); // escribimos en la cgram
lcd_send_byte(1,0x0E); // | |x|x|x| |
lcd_send_byte(1,0x00); // | | | | | |
lcd_send_byte(1,0x16); // |x| |x|x| |
lcd_send_byte(1,0x19); // |x|x| | |x|
lcd_send_byte(1,0x11); // |x| | | |x|
lcd_send_byte(1,0x11); // |x| | | |x|
lcd_send_byte(1,0x11); // |x| | | |x|
lcd_send_byte(1,0x00); // | | | | | |
/*-------------------------------------------------------------------*/
lcd_send_byte(0,0x50); // escribimos en la cgram
lcd_send_byte(1,0x02); // | | | |x| |
lcd_send_byte(1,0x04); // | | |x| | |
lcd_send_byte(1,0x00); // | | | | | |
lcd_send_byte(1,0x0E); // | |x|x|x| |
lcd_send_byte(1,0x11); // |x| | | |x|
lcd_send_byte(1,0x11); // |x| | | |x|
lcd_send_byte(1,0x0E); // | |x|x|x| |
lcd_send_byte(1,0x00); // | | | | | |
/*-------------------------------------------------------------------*/
lcd_send_byte(0,0x58); // escribimos en la cgram
lcd_send_byte(1,0x04); // | | |x| | |
lcd_send_byte(1,0x0A); // | |x| |x| |
lcd_send_byte(1,0x0A); // | |x| |x| |
lcd_send_byte(1,0x1F); // |x|x|x|x|x|
lcd_send_byte(1,0x1B); // |x|x| |x|x|
lcd_send_byte(1,0x1B); // |x|x| |x|x|
lcd_send_byte(1,0x1F); // |x|x|x|x|x|
lcd_send_byte(1,0x00); // | | | | | |
/*-------------------------------------------------------------------*/
lcd_send_byte(0,0x60); // escribimos en la cgram
lcd_send_byte(1,0x04); // | | |x| | |   xxx
lcd_send_byte(1,0x00); // | | | | | |  x   x
lcd_send_byte(1,0x04); // | | |x| | |      x
lcd_send_byte(1,0x08); // | |x| | | |     x 
lcd_send_byte(1,0x10); // |x| | | | |    x 
lcd_send_byte(1,0x11); // |x| | | |x|
lcd_send_byte(1,0x0E); // | |x|x|x| |    x
lcd_send_byte(1,0x00); // | | | | | |
}

void guardar()
{
   tiempo(hr,min,sec,dia,sem,mes,anio);
      if(s==21)
   {
      s=0;
      w=0;
      write_eeprom(253,s);
      write_eeprom(252,w);
   }
   w++;
   write_eeprom(w+11,dia);
   w++;
   write_eeprom(w+11,mes);
   w++;
   write_eeprom(w+11,anio);
   w++;
   write_eeprom(w+11,hr);
   w++;
   write_eeprom(w+11,min);
   
   write_eeprom(253,s);
   write_eeprom(252,w);

}

int porcentaje_pwm(int porcentaje)
{
   float a;
   int b;
   a=porcentaje*2.55;
   b=a;
return (b);
}

void SecuenciaPWM(int secuencia)//0-Amanecer 1-Anochecer
{ 
int16 T,tiempo_transicion;
int i;
float R,b_porcentaje,r_porcentaje;
tiempo(hr,min,sec,dia,sem,mes,anio); //Lee tiempo del DS1307

   if(min == q)
    {
      lock=1;
    }
lcd_putc('\f');

   blanco=read_eeprom(eeprom_blanco);
   rojo=read_eeprom(eeprom_rojo);

   if (secuencia==0)//amanecer
   { 
      printf(lcd_putc,"Amaneciendo...");
      tiempo_transicion=read_eeprom(9);
      }
   else
   {
      printf(lcd_putc,"Anocheciendo...");
      tiempo_transicion=read_eeprom(10);
      }
   if (tiempo_transicion<=1)
   {
      R=((tiempo_transicion*60)/0.255);
      T=R;
      //lcd_gotoxy(10,1);
      //printf(lcd_putc,"t=%lu",T);
      if (secuencia==0) //amanecer
         {
            float z=255;
            incremento_r=rojo/z;
            incremento_b=(255-blanco)/z;
         }
      else
         {
            float z=255;
            incremento_r=(255-rojo)/z;
            incremento_b=blanco/z;
         }
      total_r=rojo;
      total_b=blanco;
      
      for(i=0;i<=254;i++)
      {
        if (Secuencia==0)//amanecer
        { 
            total_r=total_r-incremento_r;
            total_b=total_b+incremento_b;
            rojo=total_r;
            blanco=total_b;
            r_porcentaje=rojo/2.55;
            b_porcentaje=blanco/2.55;
            //Valor_PWM1=blanco;  
            set_pwm1_duty(blanco);
            set_pwm2_duty(rojo);
            }
         else//anochecer
         {
            total_r=total_r+incremento_r;
            total_b=total_b-incremento_b;
            rojo=total_r;
            blanco=total_b;
            r_porcentaje=rojo/2.55;
            b_porcentaje=blanco/2.55;
            //Valor_PWM1=blanco;
            set_pwm1_duty(blanco);
            set_pwm2_duty(rojo);
            }
      lcd_gotoxy(2,2);
      printf(lcd_putc,"b=%f, r=%f",b_porcentaje,r_porcentaje);
      delay_ms(T);
      }
      
      if (secuencia==0)//dia
      { 
         rojo=0;
         blanco=255;
         write_eeprom(eeprom_blanco,blanco);
         write_eeprom(eeprom_rojo,rojo);
         s++;
         w++;
         write_eeprom(w+11,99);//12
         guardar();
         Valor_PWM1=255;
         set_pwm1_duty(255);
         set_pwm2_duty(0);
         }
      else//noche
      {
         rojo=255;
         blanco=0;
         write_eeprom(eeprom_blanco,blanco);
         write_eeprom(eeprom_rojo,rojo);
         s++;
         w++;
         write_eeprom(w+11,98);//18
         guardar();
         Valor_PWM1=0;
         set_pwm1_duty(0);
         set_pwm2_duty(255);
         }
    }
 else
 {
   float a;
   total_incrementos=(tiempo_transicion*60);
   a=total_incrementos;
   incremento=(255/a);
   Total=0;
   num_incrementos=0;
   
   total_r=rojo;
   total_b=blanco;
   Valor_PWM1=blanco;
   if (secuencia==0)
   { //amanecer
      transicion=1;
      incremento_r=rojo/a;
      incremento_b=(255-blanco)/a;
      }
   else{
      transicion=2;
      incremento_r=(255-rojo)/a;
      incremento_b=blanco/a;
      }
 }
}

void conversion1()
{
   int event=0;
   int event_aux;
   int event_eeprom=12;
   int limite_aux;
   
   for(event=0; event < s; event++)
   {
      event_aux=(event*11);
      Reg[event_aux]=read_eeprom(event_eeprom);
      event_eeprom=(event_eeprom+6);
      //printf(lcd_putc,"\f evento: %d\n", event_aux);
      //printf(lcd_putc,"\n eeprom %d",event_eeprom);
      //delay_ms(1500);
   }
   
   limite_aux=((s*5)+(s-1)+13);
   //printf(lcd_putc,"\f limite %d", limite_aux);
   //delay_ms(1500);
   
   t=13;
   v=1;
   
    for(t=13; t < limite_aux; t++)
         {
            if(read_eeprom(t) < 10)
            {
               Reg[v]=0;
               Reg[v+1]=read_eeprom(t);
            }
            else
            {
               Reg[v]=(read_eeprom(t)*(.1));
               Reg[v+1]=((read_eeprom(t))-(Reg[v]*10));
            }
            
            //printf(lcd_putc,"\f t=%d  v=%ld\n",t,v);
            //printf(lcd_putc,"\n Reg_V:%d  V+1:%d", Reg[v], Reg[v+1]);
            //delay_ms(1500);
            v=(v+2);
            
            if(t==17)
            {
               t=18;
               v=12;
            }
            if(t==23)
            {
               t=24;
               v=23;
            }
            if(t==29)
            {
               t=30;
               v=34;
            }
            if(t==35)
            {
               t=36;
               v=45;
            }
            if(t==41)
            {
               t=42;
               v=56;
            }
            if(t==47)
            {
               t=48;
               v=67;
            }
            if(t==53)
            {
               t=54;
               v=78;
            }
            if(t==59)
            {
               t=60;
               v=89;
            }
            if(t==65)
            {
               t=66;
               v=100;
            }
            
            if(t==71)
            {
               t=72;
               v=111;
            }
            if(t==77)
            {
               t=78;
               v=122;
            }
            if(t==83)
            {
               t=84;
               v=133;
            }
            if(t==89)
            {
               t=90;
               v=144;
            }
            if(t==95)
            {
               t=96;
               v=155;
            }
            if(t==101)
            {
               t=102;
               v=166;
            }
            if(t==107)
            {
               t=108;
               v=177;
            }
            if(t==113)
            {
               t=114;
               v=188;
            }
            if(t==119)
            {
               t=120;
               v=199;
            }
            if(t==125)
            {
               t=126;
               v=210;
            }
            if(t==131)
            {
               t=132;
               v=221;
            }
            
         }
         
}

void conversion_reloj()
{

   tiempo(hr,min,sec,dia,sem,mes,anio);
   
   if(hr < 10)
   {
      Reloj[0]=0;
      Reloj[1]=hr;
   }
   else
   {
      Reloj[0]=(hr*.1);
      Reloj[1]=(hr-(Reloj[0]*10));
   }
   if(min < 10)
   {
      Reloj[2]=0;
      Reloj[3]=min;
   }
   else
   {
      Reloj[2]=(min*.1);
      Reloj[3]=(min-(Reloj[2]*10));
   }
   
   if(sec < 10)
   {
      Reloj[4]=0;
      Reloj[5]=sec;
   }
   else
   {
      Reloj[4]=(sec*.1);
      Reloj[5]=(sec-(Reloj[4]*10));
   }

   printf("%d%d", Reloj[0], Reloj[1]);
   printf("%d%d", Reloj[2], Reloj[3]);
  // printf("%d%d", Reloj[4], Reloj[5]);
}

 void Conversion()
 {
        
         for(a=0; a < 12; a++)
         {
            if(read_eeprom(b) < 10)
            {
               Envio[a]=0;
               Envio[a+1]=read_eeprom(b);
            }
            else
            {
                Envio[a]=(read_eeprom(b)*(.1));
                Envio[a+1]=((read_eeprom(b))-(Envio[a]*10));
            }
           a++;
           b++;
         }
         
         if(Valor_PWM1 > 99)
         {
            Envio[12]=(Valor_PWM1*(.01));
            Envio[13]=(((Valor_PWM1)-(Envio[12]*(100)))*(.1));
            Envio[14]=((Valor_PWM1)-((Envio[12]*(100))+(Envio[13]*(10))));
         }
         else
         {
            Envio[12]=0;
            Envio[13]=(Valor_PWM1*(.1));
            Envio[14]=(Valor_PWM1-(Envio[13]*(10)));
         }
         env=0;
 }
 
 void Envio_Config()
{
   a=0;
   b=5;
   Modo_BT=1;
   Conversion();
   printf("%d%d%d",Envio[12],Envio[13],Envio[14]);
   for(env=0; env < 12; env++)
      {
         printf("%d",Envio[env]);
      }

}

 void Recibe_Config()
{
      for (j=0 ; j<2 ; j++)
   {
      Aux0[j] = getchar();
   }
      for (k=0 ; k<2 ; k++)
   {
      Aux1[k] = getchar();
   }
   for (l=0 ; l<2 ; l++)
   {
      Aux2[l] = getchar();
   }
   for (m=0 ; m<2 ; m++)
   {
      Aux3[m] = getchar();
   }
   for (n=0 ; n<2 ; n++)
   {
      Aux4[n] = getchar();
   }
   for (o=0 ; o<2 ; o++)
   {
      Aux5[o] = getchar();
   }

diah = ((Aux0[0]-48)*10)+((Aux0[1]-48)*1);
diam = ((Aux1[0]-48)*10)+((Aux1[1]-48)*1);
nocheh = ((Aux2[0]-48)*10)+((Aux2[1]-48)*1);
nochem = ((Aux3[0]-48)*10)+((Aux3[1]-48)*1);
amanecer = ((Aux4[0]-48)*10)+((Aux4[1]-48)*1);
anochecer = ((Aux5[0]-48)*10)+((Aux5[1]-48)*1);

     write_eeprom(5,diah);
     write_eeprom(6,diam);
     write_eeprom(7,nocheh);
     write_eeprom(8,nochem);
     write_eeprom(9,amanecer);
     write_eeprom(10,anochecer);

}
 
 void Recibe_Slider_PWM()
{

   for (p=0 ; p<3 ; p++) 
   {
      PWM[p] = getchar();
   }
   Valor_PWM = ((PWM[0]-48)*100)+((PWM[1]-48)*10)+((PWM[2]-48)*1);
    
      Valor_PWM1 = Valor_PWM;
      Valor_PWM = (Valor_PWM*4);
      write_eeprom(eeprom_blanco,Valor_PWM1);
      set_pwm1_duty(Valor_PWM);
   
}
 
 void Registro()
{ 
  
   c=12;
   conversion1();
   
  r=0;
  if(s < 10)
  {
  Cantidad_Eventos[0]=0;
  Cantidad_Eventos[1]=s;
  }
  else
  {
  Cantidad_Eventos[0]=(s*.1);
  Cantidad_Eventos[1]=(s-(Cantidad_Eventos[0]*10));
  }
  
/* Cantidad_Eventos=s;
 printf(lcd_putc,"\f Cant_event: %d",Cantidad_Eventos);
 delay_ms(1500);
 
printf("%d", Cantidad_Eventos);*/
 printf("%d%d", Cantidad_Eventos[0],Cantidad_Eventos[1]);
limite=(s*11);
 if(Cantidad_Eventos != 0 )
 { 
   for (r=0 ; r < limite ; r++) 
   {
  printf("%d", Reg[r]);
   }
 }
 s=0;
 w=0;
    write_eeprom(253,s);
   write_eeprom(252,w);
}

 #INT_RDA
void rda_isr()
{
dato = getchar();
   if (dato == 'A')
   {
      Envio_Config();
   }
   if (dato == 'B')
   {
      Recibe_Config();
   }
   if (dato == 'C')
   {
      Recibe_Slider_PWM();
   }
   if (dato == 'D')
   {
      Registro();
   }
   if (dato == 'U')
   {
      Modo_BT=0; 
   }
   if (dato == 'R')
   {
      conversion_reloj();
      
   }

}

int kbrd()
   {
   char k; 
   int  x,w;
   w=1;
   while(w==1)
   {
      k=kbd_getc(); 
      x=k-48;                             //Conversión numérica
      if(k!=0)
      w=0;
   }
   return(x);
   }

void contrasena(int modo)//0-normal 1-Cambio 2-inicio
{ 
char data[4], clave[4];  //Matrices para guardar clave y datos
int k,a,b;
int i;
a=0;
d=1;
  
while(a==0)
{

   lcd_putc('\f');
   pantalla=1;
   i=0;
   if (modo==0)
   {  
         printf(lcd_putc,"\f  Contrasena");
         lcd_gotoxy(11,1);
         lcd_send_byte(1,ene);   
   }
   if (modo==1)
   {
         lcd_gotoxy(1,1);
         printf(lcd_putc,"\fVerificacion de");
         lcd_gotoxy(11,1);
         lcd_send_byte(1,o_acento);
         lcd_gotoxy(3,3);
         printf(lcd_putc,"Contrasena");
         lcd_gotoxy(11,2);
         lcd_send_byte(1,ene);
         delay_ms(700);
         printf(lcd_putc,"\f  Contrasena");
         lcd_gotoxy(11,1);
         lcd_send_byte(1,ene);
         }
   if (modo==2)
   {  
         printf(lcd_putc,"\fNueva contrasena");
         lcd_gotoxy(15,1);
         lcd_send_byte(1,ene);
   }

   b=0;
   //while(b==0){
   while(i<=3)
      {                //Para tres datos
         enable_interrupts(INT_TIMER0);
          lcd_gotoxy(2,1);
          k=kbrd(); 
          data[i]=k;          
          i++;               
          lcd_gotoxy(i+6,2);
          printf(lcd_putc,"*"); 
      }
    if (modo==2)
       {
           for (i=0;i<=3;i++) 
            {             //Pasa datos de eeprom a la matriz clave
                write_eeprom(i,data[i]+48);
            }
               //b=1;
         /*tiempo(hr,min,sec,dia,sem,mes,anio);
         q=min;
         q=q+2;
         if(min==59)
         {
            q=2;
          }*/
          lock_cont();
         pantalla=0;
         a=1;
         lock=1;
        }
     if (modo==0||modo==1)
          {
               for (i=0;i<=3;i++) 
               {             
                  clave[i]=read_eeprom(i)-48;
               }
               if ((data[0]==clave[0])&&(data[1]==clave[1])&&(data[2]==clave[2])&&(data[3]==clave[3]))
               {
                   if(modo==0)
                   {
                      //b=1;
                      lock_cont();
                      
                     /* tiempo(hr,min,sec,dia,sem,mes,anio);
                      q=min;
                      q=q+2;
                      if(min==59)
                        {
                           q=2;
                         }*/
                      pantalla=0;
                      lock=0;
                      a=1;
                      
                   }
                   if(modo==1)
                   {
                      //b=1;
                      a=0;
                      modo=2;
                   }
               }
               else
               {      
                  d--;
                  if(d == 0)
                  {
                     a=1;
                     d=1;
                     pantalla=0;
                  }     
                  printf(lcd_putc,"\f  Incorrecto");  //Clave erronea
                  delay_ms(200);
                  lock=1;
               }
            }
   //}
}
}

#INT_TIMER0           //Interrupción de alta prioridad del TMR1
void timer0_isr()
{
   tiempo(hr,min,sec,dia,sem,mes,anio); //Lee tiempo del DS1307

   if(min == q)
    {
      lock=1;
    }

   if(pantalla == 0)
   {
      lcd_putc('\f');
      lcd_gotoxy(2,1);
      printf(lcd_putc,"Bioterio   %dE",s);
      lcd_gotoxy(2,2);
      printf(lcd_putc,"%2d:%2d %2d/%2d/%2d",hr,min,dia,mes,anio);  //Visualiza en LCD la hora
   }
   
   if (lock == 1)
   {
      lcd_gotoxy(16,1);
      lcd_send_byte(1,bloqueo);
   }
 
   if (Modo_BT == 1)
   {
      lcd_gotoxy(15,1);
      lcd_send_byte(1,bt);
   }
  
   if(Modo_BT == 1 && lock == 1)
   {
      lcd_gotoxy(15,1);
      lcd_send_byte(1,bt);
      lcd_gotoxy(16,1);
      lcd_send_byte(1,bloqueo);
   }

if (transicion==0)
{
   if ((hr==diah)&(min==diam))
   {
      w++;
      write_eeprom(w+11,97);//24
      guardar();   
      s++;
      SecuenciaPWM(0); //Amanecer
   }
   if ((hr==nocheh)&(min==nochem)) //hora anochecer
   {
      w++;
      write_eeprom(w+11,96);//30
      guardar();
      s++;
      SecuenciaPWM(1); //Anochecer
   }   

}

if (transicion==1||transicion==2)
{
   float b_porcentaje,r_porcentaje,a,b;
   int i;
   printf(lcd_putc,"\f");
      if (num_incrementos!=total_incrementos)
      {
         i=total;
         a=num_incrementos;
         b=total_incrementos;
            if (transicion==1)
            { //amanecer
               total_r=total_r-incremento_r;
               total_b=total_b+incremento_b;
               rojo=total_r;
               blanco=total_b;
               r_porcentaje=rojo/2.55;
               b_porcentaje=blanco/2.55;
               set_pwm1_duty(blanco);
               set_pwm2_duty(rojo);
               write_eeprom(eeprom_blanco,blanco);
               write_eeprom(eeprom_rojo,rojo);
               printf(lcd_putc,"Amaneciendo...");
               //Valor_PWM1=blanco;
    
               }
            else
            {
              total_r=total_r+incremento_r;
               total_b=total_b-incremento_b;
               rojo=total_r;
               blanco=total_b;
               r_porcentaje=rojo/2.55;
               b_porcentaje=blanco/2.55;
               set_pwm1_duty(blanco);
               set_pwm2_duty(rojo);
               write_eeprom(eeprom_blanco,blanco);
               write_eeprom(eeprom_rojo,rojo);
               printf(lcd_putc,"Anocheciendo...");
               //Valor_PWM1=blanco;
      
               }
         lcd_gotoxy(1,2);
         printf(lcd_putc,"b=%f, r=%f",b_porcentaje,r_porcentaje);
         //lcd_gotoxy(1,1);
         //printf(lcd_putc,"t=%f, i=%f",total,incremento);
         total=incremento+total;
         num_incrementos++;
      }
   else{
         if (transicion==1)
         { //amanecer
            set_pwm1_duty(255);
            set_pwm2_duty(0);
            pantalla=0;
            rojo=0;
            blanco=255;
            s++;
            w++;
            write_eeprom(w+11,99);//12
            guardar();
            Valor_PWM1=blanco;
            write_eeprom(eeprom_blanco,blanco);
            write_eeprom(eeprom_rojo,rojo);
   
            }
         else
         {
            set_pwm1_duty(0);
            set_pwm2_duty(255);
            pantalla=0;
            rojo=255;
            blanco=0;
            s++;
            w++;
            write_eeprom(w+11,98);//12
            guardar();
            Valor_PWM1=blanco;
            write_eeprom(eeprom_blanco,blanco);
            write_eeprom(eeprom_rojo,rojo);
           }
         transicion=0;
      }
}
set_timer0(255);         //Precarga el TNMR1 antes de salir
}
              
int kb_parpadeo(int x,int y,int max,int min,int actual,int decimales)
{
   int espera=1;
   int decimal=0;
   int kb_t,a;
   salida=0;
   a=actual;
      while (espera==1)
      {
         kb_t=kbrd();
           if (kb_t==-6)
           {
               decimal=0;
               a=min;
               }
            if (kb_t==18)
            {
               salida=1;
               espera=0;
               kb_t=17;
               }
            if (kb_t==17)
               espera=0;
            else
            {
                  decimal++;
               if (decimal==1)
                  a=kb_t;
               if (decimal==2)
               {
                  a=(a*10)+kb_t;
                  if (decimales==2)
                  decimal=0;
               }
               if (decimal==3)
               {
                  a=(a*100)+kb_t;
                  decimal=0;
                  }
               if (a>=max)
               {
                  a=max;
                  decimal=0;
                  }
               if (a<=min)
               {
                  a=min;
                  decimal=0;
                  }
                lcd_gotoxy(x,y);
              if(decimales==2)
                printf(lcd_putc,"%2u",a);
              if(decimales==3)
                printf(lcd_putc,"%3u",a);  
             }
      }    
      return(a);
 }
 
void reloj_modificacion(int hrm,int minm,int diam,int mesm,int aniom)
{
int control=0;
    printf(lcd_putc,"\f Config Reloj");
    delay_ms(500);
switch(control){
   case 0:{ //hora
   lcd_gotoxy(2,2);
   printf(lcd_putc,"  :%2d %2d/%2d/%2d",minm,diam,mesm,aniom);  //Visualiza en LCD la hora
   hrm=kb_parpadeo(2,2,23,0,hrm,2);
   control=control+1;
   }
   case 1:{ //minuto
   lcd_gotoxy(2,2);
   printf(lcd_putc,"%2d:   %2d/%2d/%2d",hrm,diam,mesm,aniom);  //Visualiza en LCD la hora
   minm=kb_parpadeo(5,2,59,0,minm,2);
   control=control+1;
   }
   case 2:{ //dia
   lcd_gotoxy(2,2);
   printf(lcd_putc,"%2d:%2d   /%2d/%2d",hrm,minm,mesm,aniom);  //Visualiza en LCD la hora
   diam=kb_parpadeo(8,2,31,0,diam,2);
   control=control+1;
   }
   case 3:{ //mes
   lcd_gotoxy(2,2);
   printf(lcd_putc,"%2d:%2d %2d/  /%2d",hrm,minm,diam,aniom);  //Visualiza en LCD la hora
   mesm=kb_parpadeo(11,2,12,0,mesm,2);
   control=control+1;
   }
   case 4:{ //año
   lcd_gotoxy(2,2);
   printf(lcd_putc,"%2d:%2d %2d/%2d/  ",hrm,minm,diam,mesm);  //Visualiza en LCD la hora
   aniom=kb_parpadeo(14,2,99,0,aniom,2);
   control=control+1;
   }
   break;
   }
  escribir_reloj(hrm,minm,diam,mesm,aniom);
}

void leer_configuracion()
{
         diah=read_eeprom(5);
         diam=read_eeprom(6);
         nocheh=read_eeprom(7);
         nochem=read_eeprom(8);
         amanecer=read_eeprom(9);
         anochecer=read_eeprom(10);
         }

void Imprimir_configuracion()
{
     leer_configuracion();
         lcd_putc('\f');
         lcd_gotoxy(2,1);
         printf(lcd_putc,"Configuracion");
         lcd_gotoxy(5,2);
         printf(lcd_putc,"Actual");
        
        delay_ms(700);
         
         lcd_putc('\f');
         lcd_gotoxy(3,1);
         printf(lcd_putc,"Inicio Dia");
         lcd_gotoxy(6,2);
         printf(lcd_putc,"%2d:%2d",diah,diam);  //Visualiza en LCD la hora
         delay_ms(700);
         
         lcd_putc('\f');
         lcd_gotoxy(3,1);
         printf(lcd_putc,"Inicio Noche");
         lcd_gotoxy(6,2);
         printf(lcd_putc,"%2d:%2d",nocheh,nochem);  //Visualiza en LCD la hora
         delay_ms(700);
         
         lcd_putc('\f');
         lcd_gotoxy(3,1);
         printf(lcd_putc,"T Amanecer");
         lcd_gotoxy(7,2);
         printf(lcd_putc,"0:%2d",amanecer);  //Visualiza en LCD la hora
         delay_ms(700);
         
         lcd_putc('\f');
         lcd_gotoxy(3,1);
         printf(lcd_putc,"T Anochecer");
         lcd_gotoxy(7,2);
         printf(lcd_putc,"0:%2d",anochecer);  //Visualiza en LCD la hora
         delay_ms(700);
         pantalla=0;
}

void comprobar1()
{
int error=0;
a1=diah;
a2=nocheh;

if((a1 == 0)&&(a2 == 23))
{
   a1=24;
}
if((a2 == 0)&&(a1 == 23))
{
   a2=24;
}


if((a1 == a2) && (diam==nochem))
{
   error=1;
}
else
{
   error=2;
}

if(a1 == (a2-1))
{
   a1=diam+amanecer;
   a2= 60 + nochem;
   if(a1 < a2)
   {
      error=2;
   }
   else
   {
      error=1;
   }
  
}
if(a2 == (a1-1))
{
   a1=nochem + anochecer;
   a2= 60 + diam;
   if(a1 < a2)
   {
      error=2;
   }
   else
   {
      error=1;
   }
}
if(a1 == a2)
{
   if(diam < nochem)
   {
      a1 = diam + amanecer;
      if(a1 < nochem)
      {
         error=2;
      }
      else
      {
         error=1;
      }
   }
   if(nochem < diam)
   {
      a1 = nochem + anochecer;
      if(a1 < diam)
      {
         error=2;
      }
      else
      {
         error=1;
      }
   }
}

if(a1 == 255 || a2 == 255 || diam == 255 || amanecer == 255 || nocheh == 255 || nochem == 255 || anochecer == 255 )
{
   error=1;
}


if(error==1)
{
         printf(lcd_putc,"\f!Error en datos!");
        delay_ms(800);
        confirmacion=0;
        control=0;
}
if(error==2)
{
         lcd_putc('\f');
         printf(lcd_putc," Datos Correctos?");  
         lcd_gotoxy(1,1);
         lcd_send_byte(1,signo);
         lcd_gotoxy(3,2);
         printf(lcd_putc,"A=Si Otros=No");
         confirmacion=kbrd();
         if (confirmacion==17)
            confirmacion=1;
         else
         {
         confirmacion=0;
         control=0;
         }
}

}

void secuencias()
{
 control=0;
 confirmacion=0;
   lcd_putc('\f');
   lcd_gotoxy(1,1);
    printf(lcd_putc,"Tiempo Secuencia");
    delay_ms(500);
while(confirmacion==0)
{
switch(control){
   case 0:{ // dia hora
   lcd_putc('\f');
   lcd_gotoxy(3,1);
   printf(lcd_putc,"Inicio Dia");
   lcd_gotoxy(5,2);
   printf(lcd_putc,"  :%2d",diam);  //Visualiza en LCD la hora
   diah=kb_parpadeo(5,2,23,0,diah,2);
   
   control=control+1;
   }
   case 1:{ // dia minuto
   lcd_gotoxy(5,2);
   printf(lcd_putc,"%2d:  ",diah);  //Visualiza en LCD la hora
   diam=kb_parpadeo(8,2,59,0,diam,2);
   
   control=control+1;
   }
   case 2:{ //noche hora
   lcd_putc('\f');
   lcd_gotoxy(3,1);
   printf(lcd_putc,"Inicio Noche");
   lcd_gotoxy(5,2);
   printf(lcd_putc,"  :%2d",nochem);  //Visualiza en LCD la hora
   nocheh=kb_parpadeo(5,2,23,0,nocheh,2);
   
   control=control+1;
   }
   case 3:{ //noche minuto
   lcd_gotoxy(5,2);
   printf(lcd_putc,"%2d:  ",nocheh);  //Visualiza en LCD la hora
   nochem=kb_parpadeo(8,2,59,0,nochem,2);
   
   control=control+1;
   }
   case 4:{ //amanecer
   lcd_putc('\f');
   lcd_gotoxy(3,1);
   printf(lcd_putc,"Amanecer");
   lcd_gotoxy(5,2);
   printf(lcd_putc," 0:%2d",amanecer);  //Visualiza en LCD la hora
   amanecer=kb_parpadeo(8,2,59,0,amanecer,2);
   
   control=control+1;
   }
   case 5:{ //anochecer
   lcd_putc('\f');
   lcd_gotoxy(3,1);
   printf(lcd_putc,"Anochecer");
   lcd_gotoxy(5,2);
   printf(lcd_putc," 0:%2d",anochecer);  //Visualiza en LCD la hora
   anochecer=kb_parpadeo(8,2,59,0,anochecer,2);
   
   control=control+1;
   }
   comprobar1();
   break;
   }
     
     write_eeprom(5,diah);
     write_eeprom(6,diam);
     write_eeprom(7,nocheh);
     write_eeprom(8,nochem);
     write_eeprom(9,amanecer);
     write_eeprom(10,anochecer);
}
Imprimir_configuracion();
}

void Recuperacion_luminocidad(int retardo)
{
int i;
float b_porcentaje,r_porcentaje;
float z=255;
blanco=read_eeprom(eeprom_blanco);
rojo=read_eeprom(eeprom_rojo);
lcd_putc('\f');
lcd_gotoxy(2,1);
printf(lcd_putc,"Recuperacion");
lcd_gotoxy(12,1);
lcd_send_byte(1,o_acento);
incremento_r=rojo/z;
incremento_b=blanco/z;
total_r=0;
total_b=0;
for(i=0;i<=254;i++)
      {
      total_r=total_r+incremento_r;
      total_b=total_b+incremento_b;
      blanco=total_b;
      rojo=total_r;
      r_porcentaje=rojo/2.55;
      b_porcentaje=blanco/2.55;
      set_pwm1_duty(blanco);
      set_pwm2_duty(rojo);
      lcd_gotoxy(2,2);
      printf(lcd_putc,"b=%f, r=%f",b_porcentaje,r_porcentaje);
      delay_ms(retardo);
      }
      Valor_PWM1=blanco;
   set_pwm1_duty(blanco);
   set_pwm2_duty(rojo);
}

void configuracion_inicial()
{
int a;
a=read_eeprom(0);
if (a==255)
{
   blanco=1;
   rojo=1;
   set_pwm1_duty(blanco);
   set_pwm2_duty(rojo);
   write_eeprom(eeprom_blanco,blanco);
   write_eeprom(eeprom_rojo,rojo);
   lcd_putc('\f');
   lcd_gotoxy(4,1);
   printf(lcd_putc,"Bienvenido");
   delay_ms(200);
   contrasena(2);
   tiempo(hr,min,sec,dia,sem,mes,anio); //Lee tiempo del DS1307
   reloj_modificacion(hr,min,dia,mes,anio);
   secuencias();
   }
   else
   {
      Recuperacion_luminocidad(1);
      contrasena(0);
      leer_configuracion();
   }
}

void modo_manual()
{
int b,r,blanco1,rojo1;
int control=0;
blanco=read_eeprom(eeprom_blanco);
rojo=read_eeprom(eeprom_rojo);
salida=0;
    lcd_putc('\f');
    lcd_gotoxy(3,1);
    printf(lcd_putc,"Modo Manual");
    delay_ms(500);
    lcd_putc('\f');
   lcd_gotoxy(1,1);
   printf(lcd_putc,"Blanco");
   lcd_gotoxy(1,2);
   printf(lcd_putc,"Rojo");
   blanco1=blanco/2.55;
   rojo1=rojo/2.55;
while(salida==0){
switch(control){
   case 0:{ // blanco
   lcd_gotoxy(15,1);
   printf(lcd_putc,"-");  //Visualiza en LCD la hora
   lcd_gotoxy(15,2);
   printf(lcd_putc," ");  //Visualiza en LCD la hora
   blanco1=kb_parpadeo(8,1,100,0,blanco1,3);
   control=control+1;
   b=porcentaje_pwm(blanco1);
   blanco=b;
   set_pwm1_duty(b);
   Valor_PWM1=b;
   write_eeprom(eeprom_blanco,blanco);
   }
   case 1:{ // rojo
   lcd_gotoxy(15,2);
   printf(lcd_putc,"-");  //Visualiza en LCD la hora
   lcd_gotoxy(15,1);
   printf(lcd_putc," ");  //Visualiza en LCD la hora
   rojo1=kb_parpadeo(8,2,100,0,rojo1,3);
   control=control-1;
   r=porcentaje_pwm(rojo1);
   rojo=r;
   set_pwm2_duty(r);
   write_eeprom(eeprom_rojo,rojo);
   }
   break;
   }
}
}
 
void main() 
{
   rtc_init();
   lcd_init();
   lcd_caracteres_especiales();
   s=read_eeprom(253);
   w=read_eeprom(252);
   if(s==255)
   {
      s=0;
   }
   if(w==255)
   {
      w=0;
   }
   lcd_putc('\f');
   lcd_gotoxy(4,1);
   printf(lcd_putc,"Bienvenido");
   leer_configuracion();
   delay_ms(200);
   kbd_init();
   port_b_pullups(TRUE);   //Pull-up para RB0  
   setup_timer_2(T2_DIV_BY_4, 255, 1);
   setup_ccp1(CCP_PWM);
   setup_ccp2(CCP_PWM);
   Valor_PWM1=25;
   set_pwm1_duty(25);
   set_pwm2_duty(25);
   delay_ms(20);
   configuracion_inicial();
   SETUP_TIMER_0(RTCC_DIV_1|RTCC_EXT_L_TO_H | RTCC_8_BIT);
   set_timer0(255);  
   //setup_vref(FALSE);
   enable_interrupts(INT_TIMER0);
   enable_interrupts(GLOBAL);    //Habilita int. general
  enable_interrupts(INT_RDA);

while(1)
{  
   int a;
     
   a=kbrd();
   
   if(Modo_BT==1)
   {
      a=100;  
   }
   if(lock==1)
   {
      a=243;
   }
   if(lock==1 && Modo_BT==1)
   {
      a=100;
   }

   if (transicion==0)
      {
      disable_interrupts(INT_TIMER0);
      if (a==17)
      {
         lock_cont();
         Imprimir_configuracion();
      }
      if (a==18)
      {
         lock_cont();
         secuencias();
      }
      if (a==19)
      {
         lock_cont();
         reloj_modificacion(hr,min,dia,mes,anio);
      }
      if (a==20)
         contrasena(1);
      if (a==243)
         contrasena(0);
      if (a==250)
      {
         lock_cont();
         modo_manual();
      }
     
      set_timer0(255);         //Precarga el TNMR1 antes de salir
      enable_interrupts(INT_TIMER0);
      }
      else
   set_timer0(255);         //Precarga el TNMR1 antes de salir

   
}
}


