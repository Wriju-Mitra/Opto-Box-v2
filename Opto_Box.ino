/*
 *Opto_Box
 *
 * This program controls an optogenetic setup designed to perform optogenetic manipulations
 * in groups of flies held in vials. 
 * 
 * Requires the FastLED library by Daniel Garcia. The zip file is provided in case the libary is unavailable in the future 
 * 
 * (Working)
 * 
 * Wriju 
 * Chouhan Lab
 * TIFR-DBS
 * Nov 2023
 */



#include <FastLED.h>


#define NUM_LEDS 40               //Number of LEDS per per panel. There are four panels with 5 modules(8 LEDS each) daisychained. Each panel connects to the arduino independently(Total no. of LEDS divided by four)

#define DATA_PIN 3



CRGB leds[NUM_LEDS];

CRGB color(0,0,0);               //create an array of CRGB class to store color values

String Command;

const int Fan_Pin = 7;
//const int Status_LED = 13;
//const int LED_PIN = 10;
const int buzzer_Pin = 8;


unsigned long Stim_Dur = 0;                     // defining the datatype as int causes problems
unsigned int Stim_Freq = 0;
unsigned int time_period = 0;



 int RED_Value;
 int GREEN_Value;
 int BLUE_Value;

bool fan_State = true;
bool Start =false;
unsigned long start_time = 0;
bool foreverOn = false;
bool Strobe = true;




void setup() 
{

  FastLED.addLeds<WS2812, DATA_PIN, GRB>(leds, NUM_LEDS);  // GRB ordering is typical
   // FastLED.addLeds<WS2812B, DATA_PIN, RGB>(leds, NUM_LEDS);  // GRB ordering is typical

 
  //FastLED.setMaxRefreshRate(0);
 
 
 
 
 pinMode(Fan_Pin, OUTPUT);
 //pinMode(LED_PIN, OUTPUT);
 //pinMode(Status_LED, OUTPUT); 
 pinMode(buzzer_Pin, OUTPUT);
 
 
 Serial.begin(9600);
 buzz(1000);
 delay(500);
 Serial.println("Starting......!!!");
 Serial.setTimeout(60000);
 delay(1000);






if(Serial)                                                        //Recieve user inputs
  {
   // digitalWrite(Status_LED, HIGH);
    
    Serial.println("Enter Programming Mode? y/n");                //just for the sake of it
    while(true)
       {        
        Command = Serial.readStringUntil('\n');
        if(Command.equals("y"))
          buzz(100);
          break;
        }   
     
     while(true)                                                                                     //total duration of stimulation
        {
         Serial.println(" Enter Stimulation Duration(in minutes): Enter zero for continuous stim");
      
         Command = Serial.readStringUntil('\n');
         if(Command.toInt()> 0)
        {
          Stim_Dur = Command.toInt();   
        //   Serial.println(Stim_Dur);  
          buzz(100);
          break;
        }
         
        else if(Command.toInt() == 0)
        {
          foreverOn = true;
          buzz(100);
          break;
        }
         
         else
         {
          Serial.println(" Wrong Input! ");
          buzz(100);
         }
        
        }
      while(true)                                                                                                // frequency at which the LEDs would be strobed. Try to be reasonable
        {
         Serial.println(" Enter Stimulation Frequency (in hertz)(Enter zero for continuous stimulation): ");
      
         Command = Serial.readStringUntil('\n');
         if(Command.toInt())  
       {
          Stim_Freq = Command.toInt();
          
          time_period = 1000/Stim_Freq;                   // calculating time_period in approx millisecs to avoid having to do floating point math    
          
          buzz(100);
          break;
        }

          else if(Command.toInt()== 0)  
       {
         Strobe = false;
          buzz(100);
          break;
        }
         
         
         else
         {
          Serial.println(" Wrong Input! ");
          buzz(100);
         }

        }

             
       while(true)
        {
         Serial.println(" Enter RED value(1-256): ");
      
         Command = Serial.readStringUntil('\n');
         if(Command.toInt()> 0 && Command.toInt() <=256) 
        {
          RED_Value = (Command.toInt()) -1;                                            // because if .toInt returns zero then the if block will not run
          buzz(100);
          break;
        }
         else
         {
          Serial.println(" Wrong Input! ");
          buzz(100);
         }
        
        }


       while(true)
        {
         Serial.println(" Enter GREEN value(1-256): ");
      
         Command = Serial.readStringUntil('\n');
         if(Command.toInt()> 0 && Command.toInt() <=256) 
        {
          GREEN_Value = (Command.toInt())-1;
          buzz(100);
          break;
        }
         else
         {
          Serial.println(" Wrong Input! ");
          buzz(100);
         }
        
        }

        while(true)
        {
         Serial.println(" Enter BLUE value(1-256): ");
      
         Command = Serial.readStringUntil('\n');
         if(Command.toInt()> 0 && Command.toInt() <=256)                             
        {
          BLUE_Value = (Command.toInt()) - 1;
          buzz(100);
          break;
        }
         else
         {
          Serial.println(" Wrong Input! ");
          buzz(100);
         }
        
        }


       
        while(true)                                                             //At high intensities the heat generated is a concern espeially when experiments are conducted at higher temperatures. Its a good idea to keep the fan on
        {
         Serial.println("Fan ON/OFF?");
      
         Command = Serial.readStringUntil('\n');
        if(Command.equals("ON"))
         { 
          buzz(100);
          break;
         }
         else if(Command.equals("OFF"))
        {
          buzz(100);
          fan_State = false;
          break;
        }
         else
         {
          Serial.println(" Wrong Input! ");
          buzz(100);
         }
        
        }

         while(true)
        {
         Serial.println("Start? y/n");
      
         Command = Serial.readStringUntil('\n');
        if(Command.equals("y"))
         { 
          buzz(100);
          Start =true;
          
          break;
         }
        
         else
         {
          Serial.println(" Wrong Input! ");
          buzz(100);
         }
        
        }   

  }

  else
  {
    Serial.println("Issue with Serial Comms......!!");
  }

 
   

  color[0] =  RED_Value;
  color[1] = GREEN_Value;
  color[2] = BLUE_Value;                      // updating color values





  Serial.println("Experiment Details:.............");                                   //display experiment details for the record
  Serial.print("Stimulation Duration(in mins) = ");
  Serial.println(Stim_Dur);
  Serial.print("Frequency = ");
  Serial.println(Stim_Freq);
  Serial.print("Red value = ");
  Serial.println(RED_Value);
  Serial.print("Green value = ");
  Serial.println(GREEN_Value);
  Serial.print("Blue value = ");
  Serial.println(BLUE_Value);
  Serial.print("Fan state = ");
  Serial.println(fan_State);
 
 


  Stimulate();                                                                       //Start of stimulation


  
}// end of setup

void loop()
{

} //empty loop






void Stimulate()
{
                                                              
  if(fan_State)                                              //switch fan on/off
  
    digitalWrite(Fan_Pin, HIGH);                

  else

    digitalWrite(Fan_Pin, LOW);
  
   
   
 Serial.println("Starting with experiment......!!!!");
  
  
  
  
 
  buzz(1000);
  start_time = millis();
  Stim_Dur = Stim_Dur*1000*60;                        // convert mins to millisecs for millis function

//   Serial.println(Stim_Dur);
   
  while((millis() - start_time) < Stim_Dur || foreverOn)
  {

   if(Strobe)                                       // for strobe effect
   {

        fill_solid(leds, 40, color);
        FastLED.show();                            //switching on for half t

        delay(time_period/2);

        fill_solid(leds, 40, CRGB::Black);         //switching off
        FastLED.show();
   
        delay(time_period/2);
   
   
   }


   else
   {
                                        // normal lighting without strobe

    fill_solid(leds, 40, color);
    FastLED.show();

                                        
   }

    
  }// end of while
   
    
    fill_solid(leds, 40, CRGB::Black);         //switching off
    FastLED.show();
    Serial.println("end of Stimulus");
    buzz(1000);
 

  
   Serial.println("Restart or refresh serial comm to start again");

}// end of Stimulate()


void buzz(int t)
{
  digitalWrite(buzzer_Pin, HIGH);
 // digitalWrite(Status_LED, LOW);
  delay(t);
  digitalWrite(buzzer_Pin, LOW);
  //digitalWrite(Status_LED, HIGH);
}

