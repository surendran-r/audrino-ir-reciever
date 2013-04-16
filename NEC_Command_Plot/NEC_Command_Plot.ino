/**
 *   This program prints a single NEC protocol based IR command recieved over 
 *   the Arduino Pin 4 that is connected to a IR reciever. The out of the program
 *   is a simple list of (curTime, value) pairs and is presented in a gnuplot firendly 
 *   format where we note down the edge curTime timings.
 */ 

// We need to record a single NEC command, which is made up of the following pules
// 1. Start of Command marker               [1 sample]
// 2. Start of Command spacer               [1 sample]
// 3. Destination Address 8-bits            [ 8 * (2 sample per bit) = 16 sample]
// 4. Inverse of Destination Address 8-bits [ 8 * (2 sample per bit) = 16 sample]
// 5. Command 8-bits                        [ 8 * (2 sample per bit) = 16 sample]
// 6. Inverse of Command 8-bits             [ 8 * (2 sample per bit) = 16 sample]
// 7. End of Command pulse                  [ 2 samples , to make sure we note down both the start & the end of the pulse as well]
// Totally 68 samples
#define NUM_SAMPLES  68

int IRInputpin = 2;
unsigned long time[NUM_SAMPLES];
char data[NUM_SAMPLES];
byte change_count;
unsigned long curTime, startcurTime;

#define INVERT_ASCII(X) ((X=='0')?'1':'0')

void setup() {
  Serial.begin(115200);
  Serial.println("Analyze IR Remote");      
  pinMode(IRInputpin, INPUT);
}

void loop() {
  
  Serial.println("Waiting for IR signal...");
  change_count = 0;
  //We have a active low IR receiver and we have the IRpin pulled up by default.
  //We wait for the signal from the receiver
  while(digitalRead(IRInputpin) == HIGH) {}                                 
  
  //Note down the initial egde curTime
  time[change_count] = micros();
  data[change_count++] = '0';
  
  //Now collect the data for the single command
  while (change_count < NUM_SAMPLES) {
    if (data[change_count-1] == '0') {
      while(digitalRead(IRInputpin) == LOW) {}
      time[change_count] = micros();
      data[change_count++] = '1';
    } else {
      while(digitalRead(IRInputpin) == HIGH) {}
      time[change_count] = micros();
      data[change_count++] = '0';
    }
  }
  
  //Now that we have collected the data, we print them out to the serial port.
  //Also since our IR reciever is a active low one, we invert the data so that
  //we can easily visualize the signal
  Serial.println("Sampled Data");
  change_count = 0;
  //We offset the curTime to the startcurTime
  startcurTime = curTime = (unsigned long) time[change_count];
  Serial.print(curTime - startcurTime);
  Serial.print("\t");
  Serial.println(INVERT_ASCII(data[change_count++]));
  while (change_count < NUM_SAMPLES) {
    curTime = (unsigned long) time[change_count];
    Serial.print(curTime - startcurTime);
    Serial.print("\t");
    Serial.println(INVERT_ASCII(data[change_count-1]));
    Serial.print(curTime - startcurTime);
    Serial.print("\t");
    Serial.println(INVERT_ASCII(data[change_count++]));    
  }
}
