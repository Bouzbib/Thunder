//0 --> toggle off/on
//1 SIMULTANOEUS DUTY FR --> select a simultaneous 1 or alternating 0 signal, its DUTY % and frequency FR
//2 SIMULTANOEUS DUTY FR REPETIOTIONS--> select a simultaneous 1 or alternating 0 signal, its DUTY % and frequency FR, and number pulses to do REPETIOTIONS


/////////////////////OPERATIONS DO NOT MUTATE THE VARIABLE///////////////////////////
/* a=target variable, b=bit number to act upon 0-n */   /////////////////////////////
#define BIT_SET(a,b) ((a) | (1ULL<<(b)))                /////////////////////////////
#define BIT_CLEAR(a,b) ((a) & ~(1ULL<<(b)))             /////////////////////////////
#define BIT_FLIP(a,b) ((a) ^ (1ULL<<(b)))               /////////////////////////////
#define BIT_CHECK(a,b) (!!((a) & (1ULL<<(b))))          /////////////////////////////
                                                        /////////////////////////////
#define BITMASK_SET(x, mask) ((x) | (mask))             /////////////////////////////
#define BITMASK_CLEAR(x, mask) ((x) & (~(mask)))        /////////////////////////////
#define BITMASK_FLIP(x, mask) ((x) ^ (mask))            /////////////////////////////
#define BITMASK_CHECK_ALL(x, mask) (!(~(x) & (mask)))   /////////////////////////////
#define BITMASK_CHECK_ANY(x, mask) ((x) & (mask))       /////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

//circular bit shifting to the left
inline uint32_t ROTATE_LEFT(uint32_t n,unsigned int cutoff, unsigned int b){
  return (n << b) | (n >> (cutoff - b)); // works upto 32 bits -> (n << b) | (n >> (32 - b));
}

//circular bit shifting to the right
inline uint32_t ROTATE_RIGHT(uint32_t n,unsigned int cutoff, unsigned int b){
  return (n >> b) | (n << (cutoff - b)); // works upto 32 bits -> (n >> b) | (n << (32 - b));
}

//to deal with gpio20 not being available, shift bit 9 of the mask to the left
//probably there is a better way of doing this
inline uint32_t customBitMaskSet(uint32_t mask){
  return BIT_CHECK(mask,8)>0?(BIT_SET(BIT_CLEAR(mask,8),9)<<12):(BIT_CLEAR(BIT_CLEAR(mask,8),9)<<12);
  //more readable version below
  /*
  if (BIT_CHECK(mask,8)){
    return (BIT_SET(BIT_CLEAR(mask,8),9)<<12);
  }else{
    return (BIT_CLEAR(BIT_CLEAR(mask,8),9)<<12);
  }
  */
}

//print a number of bits of var
void printBits(int numberOfBits,unsigned long var)
{
  for (int b = numberOfBits-1; b >= 0; b--) {
    byte bit = bitRead(var, b);
    Serial.print(bit);
  }
  Serial.println();
}

//True GPIO Mask
uint32_t mask = 0b0000000000000000000000;

//We use gpios 12 to 19, and 21
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t aux =  0b1011111111000000000000;//DO NOT REMOVE THE 6 zeros after the ones, CANNOT USE THOSE PINS //
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

uint32_t pseudoRegister = 0b000000000;

const int Output1 = 12;
const int Output2 = 13;
const int Output3 = 14;
const int Output4 = 15;
const int Output5 = 16;
const int Output6 = 17;
const int Output7 = 18;
const int Output8 = 19;
const int Output9 = 21;

const int Outputs[] = {Output1,Output2,Output3,Output4,Output5,Output6,Output7,Output8,Output9};
const int NumOutputs = sizeof(Outputs)/sizeof(int);
//physical setup should be like this
//     |    column 1    |    column 2    |    column 3
//-----+----------------+----------------+--------------
//row 1|     Output1    |     Output2    |     Output3   
//-----+----------------+----------------+--------------
//row 2|     Output8    |     Output9    |     Output4        
//-----+----------------+----------------+--------------
//row 3|     Output7    |     Output6    |     Output5    
//------------------------------------------------------

const int OutputsSquare[] = {Output1,Output2,Output3,Output4,Output5,Output6,Output7,Output8};
const int NumOutputsSquare = sizeof(OutputsSquare)/sizeof(int);

const int OutputsCross[] = {Output1,Output9,Output5,Output3,Output9,Output7};
const uint16_t OutputsCrossBinary[] = {0b000000001,0b100000000,0b000010000,0b000000100,0b100000000,0b001000000};
const int NumOutputsCross = sizeof(OutputsCross)/sizeof(int);

const int OutputsTriangleTopLeft[] = {Output1,Output2,Output3,Output9,Output7,Output8};
//const int OutputsTriangleTopRight[] = {Output1,Output2,Output3,Output4,Output5,Output9};
//const int OutputsTriangleBottomLeft[] = {Output1,Output9,Output5,Output6,Output7,Output8};
//const int OutputsTriangleBottomRight[] = {Output3,Output4,Output5,Output6,Output7,Output9};
const uint16_t OutputsTriangleTopLeftBinary[] = {0b000000001,0b000000010,0b000000100,0b100000000,0b001000000,0b010000000};
//const uint16_t OutputsTriangleTopRightBinary[] = {0b000000001,0b000000010,0b000000100,0b000001000,0b000010000,0b100000000};
//const uint16_t OutputsTriangleBottomLeftBinary[] = {0b000000001,0b100000000,0b000010000,0b000100000,0b001000000,0b010000000};
//const uint16_t OutputsTriangleBottomRightBinary[] = {0b000000100,0b000001000,0b000010000,0b000100000,0b001000000,0b100000000};
const int NumOutputsTriangle = sizeof(OutputsTriangleTopLeft)/sizeof(int);

const int maxCustomPatternLength = 64;
uint16_t customPattern[maxCustomPatternLength];
int customPatternLength = 0;

//int** advancedCustomPattern;
//advancedCustomPattern = (int**)calloc(1, sizeof(int*)); //[[pattern],duration,delayBeforeNext]


int simultaneous = HIGH;
int enable = HIGH;
long mapped = 0;
float mapped2 = 0.0;
int serial = 1;
long delay1 = 0;
long delay1aux = 0;
long delay2 = 0;
long delay2aux = 0;
long wait = 0;
long waitaux = 0;
int delaytype1 = 0;
int delaytype2 = 0;
int delaytype3 = 0;

long mappedaux = 0;
float mapped2aux = 0.0;
long delay1_2 = 0;
long delay1aux_2 = 0;
long delay2_2 = 0;
long delay2aux_2 = 0;
long wait_2 = 0;
long waitaux_2 = 0;
int delaytype1aux = 0;
int delaytype2aux = 0;
int delaytype3aux = 0;

int delaytypeBetween = 0;

int limit = 0;
int limitTimes = 0;
int limitTimesaux = 0;
int continious = 0;
int continiousaux = 0;
int pathCreation = 0;
int patterns = 0;
int patterns2 = 0;
int pattern = 0;
int delayBetweenStimuli = 0;
int direction = 0;
int numberOfSimultaneousCoils = 0;
int customPatternSet = 0;

int temporalAcuity = 0;
int temporalAcuityDelay = 0;
int limitTimes2 = 0;
int waited = 0;

void delayaux(long delay){ //custom delay function to go around limitations on longest delayMicroseconds(not very accurate but works)
  int times = delay/16380;
  unsigned int remainder = delay%16380;
  for(int i=0;i<times;i++){
    delayMicroseconds(16380);
  }
  if(remainder >0){
    delayMicroseconds(remainder);
  }
}

void setup() {
  Serial.begin(115200);
  for(int i = 0; i<NumOutputs;i++){
    pinMode(Outputs[i],OUTPUT);
    digitalWrite(Outputs[i],LOW);
  }
  //USE BELOW FOR MANUAL SETTING OF PINS AS OUTPUT
  /*
  gpio_config_t io_conf;

  io_conf.mode = GPIO_MODE_OUTPUT;
  io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
  io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
  io_conf.pin_bit_mask = aux;

  gpio_config(&io_conf);
  */
}

void loop() {
  //0 --> toggle off/on
  //1 SIMULTANOEUS DUTY FR --> select a simultaneous 1 or alternating 0 signal, its DUTY % and frequency FR
  //2 SIMULTANOEUS DUTY FR REPETIOTIONS--> select a simultaneous 1 or alternating 0 signal, its DUTY % and frequency FR, and number pulses to do REPETIOTIONS
  if (Serial.available()) {
    serial = 1;
    int command = Serial.parseInt();
    if(command == 0){//DOESN'T FULLY WORK//toggle off/on outputs only works for pulses and continious generation, DOES NOT WORK FOR PATTERNS yet, I need to add the if statement
      enable = !enable;
      Serial.println("enable 0");
      //while ( Serial.read() != '\n');
    }else if(command == 1){//WORKS///continious generation I don't remember need to test again
      simultaneous = Serial.parseInt()%2;
      float duty = Serial.parseFloat();
      float fr = Serial.parseFloat();
      limit = 0;
      continious = 1;
      pathCreation = 0;
      temporalAcuity = 0;
      //while ( Serial.read() != '\n'); //skipt until finding the new line character
    
      mapped = (1/fr)*1000000;
      if (simultaneous){
        mapped2 = duty/100;
      }else{
        mapped2 = duty/100 >= 50 ? 50 : duty/100;
      }
      
      delaytype1 = 0;
      delaytype2 = 0;
      delaytype3 = 0;
      
      delay1 = mapped*mapped2;
      delay1aux = delay1/1000;
      delay2 = mapped*(1-mapped2);
      delay2aux = delay2/1000;
      wait = (abs(delay2-delay1))/2;
      waitaux = wait/1000;

      if (delay1 < 16380){ //check if it is higher than unsigned int max value
        delaytype1 = 1;
      }else{
        delaytype1 = 0;
      }
      if (delay2 < 16380){ //check if it is higher than unsigned int max value
        delaytype2 = 1;
      }else{
        delaytype2 = 0;
      }

      if (wait < 16380){ //check if it is higher than unsigned int max value
        delaytype3 = 1;
      }else{
        delaytype3 = 0;
      }

      Serial.println(",,,,,,,,,,,,,");
      Serial.print("mapped: ");
      Serial.println(mapped);
      Serial.print("mapped2: ");
      Serial.println(mapped2);
      Serial.print("delay1: ");
      Serial.println(delay1);
      Serial.println(delay1aux);
      Serial.print("delay2: ");
      Serial.println(delay2);
      Serial.println(delay2aux);
      Serial.print("delay wait: ");
      Serial.println(wait);
      Serial.println(waitaux);
      Serial.print("delay types: ");
      Serial.println(delaytype1);
      Serial.println(delaytype2);
      Serial.println(delaytype3);
    }else if(command == 2){//ARBITRARY AMOUNT OF Pulses of specified frequency and intensity example: 4 1 0.05 50 500         explanation --> 4 1 0.05=INTENSITY(percentage of pulsewidth fed into the coil, be careful it depends on the frequency) 50=FREQUENCY(frequency of the pulse fed into the coil) 500=PULSES(number of pulses)
      simultaneous = Serial.parseInt()%2;
      float duty = Serial.parseFloat();
      float fr = Serial.parseFloat();
      limitTimes = Serial.parseInt();
      limit = 1;
      continious = 0;
      pathCreation = 0;
      temporalAcuity = 0;
      //while ( Serial.read() != '\n'); //skipt until finding the new line character
    
      mapped = (1/fr)*1000000;
      if (simultaneous){
        mapped2 = duty/100;
      }else{
        mapped2 = duty/100 >= 50 ? 50 : duty/100;
      }
      
      delaytype1 = 0;
      delaytype2 = 0;
      delaytype3 = 0;
      
      delay1 = mapped*mapped2;
      delay1aux = delay1/1000;
      delay2 = mapped*(1-mapped2);
      delay2aux = delay2/1000;
      wait = (abs(delay2-delay1))/2;
      waitaux = wait/1000;

      if (delay1 < 16380){ //check if it is higher than unsigned int max value
        delaytype1 = 1;
      }else{
        delaytype1 = 0;
      }
      if (delay2 < 16380){ //check if it is higher than unsigned int max value
        delaytype2 = 1;
      }else{
        delaytype2 = 0;
      }

      if (wait < 16380){ //check if it is higher than unsigned int max value
        delaytype3 = 1;
      }else{
        delaytype3 = 0;
      }

      Serial.print("mapped: ");
      Serial.println(mapped);
      Serial.print("mapped2: ");
      Serial.println(mapped2);
      Serial.print("delay1: ");
      Serial.println(delay1);
      Serial.println(delay1aux);
      Serial.print("delay2: ");
      Serial.println(delay2);
      Serial.println(delay2aux);
      Serial.print("delay wait: ");
      Serial.println(wait);
      Serial.println(waitaux);
      Serial.print("delay types: ");
      Serial.println(delaytype1);
      Serial.println(delaytype2);
      Serial.println(delaytype3);
    }else if(command == 3){//MAY BE BUGGED HAVE TO DOUBLECHECK //CONTINIOUS pulses of specified frequency and intensity example: 3 1 40 50 500         explanation --> 4 1 40=INTENSITY(microseconds of pulsewidth fed into the coil) 50=FREQUENCY(frequency of the pulse fed into the coil) 500=PULSES(number of pulses)
      simultaneous = Serial.parseInt()%2;
      float onTime = (int)Serial.parseFloat();
      float fr = Serial.parseFloat();
      limit = 0;
      continious = 1;
      pathCreation = 0;
      temporalAcuity = 0;
      //while ( Serial.read() != '\n'); //skipt until finding the new line character
    
      mapped = (1/fr)*1000000;
      
      if (simultaneous){
        //onTime can go up to 100%
      }else{
        //ontime can go up to 50%
        onTime = (onTime > (mapped/2))? mapped/2 : onTime;
      }
      
      delaytype1 = 0;
      delaytype2 = 0;
      delaytype3 = 0;
      
      delay1 = onTime; //mapped*mapped2;
      delay1aux = delay1/1000;
      delay2 = mapped-onTime; //mapped*(1-mapped2);
      delay2aux = delay2/1000;
      wait = (abs(delay2-delay1))/2;
      waitaux = wait/1000;

      if (delay1 < 16380){ //check if it is higher than unsigned int max value
        delaytype1 = 1;
      }else{
        delaytype1 = 0;
      }
      if (delay2 < 16380){ //check if it is higher than unsigned int max value
        delaytype2 = 1;
      }else{
        delaytype2 = 0;
      }

      if (wait < 16380){ //check if it is higher than unsigned int max value
        delaytype3 = 1;
      }else{
        delaytype3 = 0;
      }

      Serial.print("mapped: ");
      Serial.println(mapped);
      Serial.print("onTime: ");
      Serial.println(onTime);
      Serial.print("delay1: ");
      Serial.println(delay1);
      Serial.println(delay1aux);
      Serial.print("delay2: ");
      Serial.println(delay2);
      Serial.println(delay2aux);
      Serial.print("delay wait: ");
      Serial.println(wait);
      Serial.println(waitaux);
      Serial.print("delay types: ");
      Serial.println(delaytype1);
      Serial.println(delaytype2);
      Serial.println(delaytype3);
    }else if(command == 4){//WORKS//ARBITRARY AMOUNT OF Pulses of specified frequency and intensity example: 4 1 40 50 500         explanation --> 4 1 40=INTENSITY(microseconds of pulsewidth fed into the coil) 50=FREQUENCY(frequency of the pulse fed into the coil) 500=PULSES(number of pulses)
      simultaneous = Serial.parseInt()%2;
      float onTime = (int)Serial.parseFloat();
      float fr = Serial.parseFloat();
      limitTimes = Serial.parseInt();
      limit = 1;
      continious = 0;
      pathCreation = 0;
      temporalAcuity = 0;
      //while ( Serial.read() != '\n'); //skipt until finding the new line character
    
      mapped = (1/fr)*1000000;
      
      if (simultaneous){
        //onTime can go up to 100%
      }else{
        //ontime can go up to 50%
        onTime = (onTime > (mapped/2))? mapped/2 : onTime;
      }
      
      delaytype1 = 0;
      delaytype2 = 0;
      delaytype3 = 0;
      
      delay1 = onTime; //mapped*mapped2;
      delay1aux = delay1/1000;
      delay2 = mapped-onTime; //mapped*(1-mapped2);
      delay2aux = delay2/1000;
      wait = (abs(delay2-delay1))/2;
      waitaux = wait/1000;

      if (delay1 < 16380){ //check if it is higher than unsigned int max value
        delaytype1 = 1;
      }else{
        delaytype1 = 0;
      }
      if (delay2 < 16380){ //check if it is higher than unsigned int max value
        delaytype2 = 1;
      }else{
        delaytype2 = 0;
      }

      if (wait < 16380){ //check if it is higher than unsigned int max value
        delaytype3 = 1;
      }else{
        delaytype3 = 0;
      }

      Serial.print("mapped: ");
      Serial.println(mapped);
      Serial.print("onTime: ");
      Serial.println(onTime);
      Serial.print("delay1: ");
      Serial.println(delay1);
      Serial.println(delay1aux);
      Serial.print("delay2: ");
      Serial.println(delay2);
      Serial.println(delay2aux);
      Serial.print("delay wait: ");
      Serial.println(wait);
      Serial.println(waitaux);
      Serial.print("delay types: ");
      Serial.println(delaytype1);
      Serial.println(delaytype2);
      Serial.println(delaytype3);
    }else if(command == 5){//UNTESTED PROBABLY DOESN'T WORK path creation -> REPEAT CONTINIOUSLY very short high frequency spark followed by a lower frequency one, to see if the low frequency sparks can be guided with ultrasound
      simultaneous = Serial.parseInt()%2;
      float onTime = (int)Serial.parseFloat();
      float onTime2 = (int)Serial.parseFloat();
      float fr = Serial.parseFloat();
      float fr2 = Serial.parseFloat();
      limitTimes = Serial.parseInt();
      limitTimesaux = Serial.parseInt();
      limit = 0;
      continious = 0;
      continiousaux = 1;
      pathCreation = 1;
      temporalAcuity = 0;
      //while ( Serial.read() != '\n'); //skipt until finding the new line character
    
      mapped = (1/fr)*1000000;
      mappedaux = (1/fr2)*1000000;

      if (simultaneous){
        //onTime can go up to 100%
      }else{
        //ontime can go up to 50%
        onTime = (onTime > (mapped/2))? mapped/2 : onTime;
      }

      if (simultaneous){
        //onTime can go up to 100%
      }else{
        //ontime can go up to 50%
        onTime2 = (onTime2 > (mappedaux/2))? mappedaux/2 : onTime2;
      }
      
      delaytype1 = 0;
      delaytype2 = 0;
      delaytype3 = 0;
      
      delaytype1aux = 0;
      delaytype2aux = 0;
      delaytype3aux = 0;

      delay1 = onTime; //mapped*mapped2;
      delay1aux = delay1/1000;
      delay2 = mapped-onTime; //mapped*(1-mapped2);
      delay2aux = delay2/1000;
      wait = (abs(delay2-delay1))/2;
      waitaux = wait/1000;

      delay1_2 = onTime2; //mapped*mapped2;
      delay1aux_2 = delay1_2/1000;
      delay2_2 = mappedaux-onTime2; //mapped*(1-mapped2);
      delay2aux_2 = delay2_2/1000;
      wait_2 = (abs(delay2_2-delay1_2))/2;
      waitaux_2 = waitaux_2/1000;

      if (delay1 < 16380){ //check if it is higher than unsigned int max value
        delaytype1 = 1;
      }else{
        delaytype1 = 0;
      }
      if (delay2 < 16380){ //check if it is higher than unsigned int max value
        delaytype2 = 1;
      }else{
        delaytype2 = 0;
      }

      if (wait < 16380){ //check if it is higher than unsigned int max value
        delaytype3 = 1;
      }else{
        delaytype3 = 0;
      }

      if (delay1_2 < 16380){ //check if it is higher than unsigned int max value
        delaytype1aux = 1;
      }else{
        delaytype1aux = 0;
      }
      if (delay2_2 < 16380){ //check if it is higher than unsigned int max value
        delaytype2aux = 1;
      }else{
        delaytype2aux = 0;
      }

      if (wait_2 < 16380){ //check if it is higher than unsigned int max value
        delaytype3aux = 1;
      }else{
        delaytype3aux = 0;
      }
      // Serial.print("mapped: ");
      // Serial.println(mapped);
      // Serial.print("onTime: ");
      // Serial.println(onTime);
      // Serial.print("delay1: ");
      // Serial.println(delay1);
      // Serial.println(delay1aux);
      // Serial.print("delay2: ");
      // Serial.println(delay2);
      // Serial.println(delay2aux);
      // Serial.print("delay wait: ");
      // Serial.println(wait);
      // Serial.println(waitaux);
      // Serial.print("delay types: ");
      // Serial.println(delaytype1);
      // Serial.println(delaytype2);
      // Serial.println(delaytype3);
    }else if(command == 6){//UNTESTED PROBABLY DOESN'T WORK path creation -> ARBITRARY AMOUNT OF very short high frequency spark followed by a lower frequency one, to see if the low frequency sparks can be guided with ultrasound
      simultaneous = Serial.parseInt()%2;
      float onTime = (int)Serial.parseFloat();
      float onTime2 = (int)Serial.parseFloat();
      float fr = Serial.parseFloat();
      float fr2 = Serial.parseFloat();
      limitTimes = Serial.parseInt();
      limitTimesaux = Serial.parseInt();
      limit = 0;
      continious = 0;
      continiousaux = 0;
      pathCreation = 1;
      temporalAcuity = 0;
      //while ( Serial.read() != '\n'); //skipt until finding the new line character
    
      mapped = (1/fr)*1000000;
      mappedaux = (1/fr2)*1000000;

      if (simultaneous){
        //onTime can go up to 100%
      }else{
        //ontime can go up to 50%
        onTime = (onTime > (mapped/2))? mapped/2 : onTime;
      }

      if (simultaneous){
        //onTime can go up to 100%
      }else{
        //ontime can go up to 50%
        onTime2 = (onTime2 > (mappedaux/2))? mappedaux/2 : onTime2;
      }
      
      delaytype1 = 0;
      delaytype2 = 0;
      delaytype3 = 0;
      
      delaytype1aux = 0;
      delaytype2aux = 0;
      delaytype3aux = 0;

      delay1 = onTime; //mapped*mapped2;
      delay1aux = delay1/1000;
      delay2 = mapped-onTime; //mapped*(1-mapped2);
      delay2aux = delay2/1000;
      wait = (abs(delay2-delay1))/2;
      waitaux = wait/1000;

      delay1_2 = onTime2; //mapped*mapped2;
      delay1aux_2 = delay1_2/1000;
      delay2_2 = mappedaux-onTime2; //mapped*(1-mapped2);
      delay2aux_2 = delay2_2/1000;
      wait_2 = (abs(delay2_2-delay1_2))/2;
      waitaux_2 = waitaux/1000;

      if (delay1 < 16380){ //check if it is higher than unsigned int max value
        delaytype1 = 1;
      }else{
        delaytype1 = 0;
      }
      if (delay2 < 16380){ //check if it is higher than unsigned int max value
        delaytype2 = 1;
      }else{
        delaytype2 = 0;
      }

      if (wait < 16380){ //check if it is higher than unsigned int max value
        delaytype3 = 1;
      }else{
        delaytype3 = 0;
      }

      if (delay1_2 < 16380){ //check if it is higher than unsigned int max value
        delaytype1aux = 1;
      }else{
        delaytype1aux = 0;
      }
      if (delay2_2 < 16380){ //check if it is higher than unsigned int max value
        delaytype2aux = 1;
      }else{
        delaytype2aux = 0;
      }

      if (wait_2 < 16380){ //check if it is higher than unsigned int max value
        delaytype3aux = 1;
      }else{
        delaytype3aux = 0;
      }
      // Serial.print("mapped: ");
      // Serial.println(mapped);
      // Serial.print("onTime: ");
      // Serial.println(onTime);
      // Serial.print("delay1: ");
      // Serial.println(delay1);
      // Serial.println(delay1aux);
      // Serial.print("delay2: ");
      // Serial.println(delay2);
      // Serial.println(delay2aux);
      // Serial.print("delay wait: ");
      // Serial.println(wait);
      // Serial.println(waitaux);
      // Serial.print("delay types: ");
      // Serial.println(delaytype1);
      // Serial.println(delaytype2);
      // Serial.println(delaytype3);
    }else if(command == 7){//NEED TO FIX OUTPUT ORDER WAS CHANGED Patterns without simulatenous coils nor direct port manipulation
      simultaneous = Serial.parseInt()%2;
      float onTime = (int)Serial.parseFloat();
      float fr = Serial.parseFloat();
      limitTimes = Serial.parseInt();
      pattern = Serial.parseInt();
      direction = Serial.parseInt();
      delayBetweenStimuli = (int)Serial.parseFloat();
      limit = 0;
      continious = 0;
      pathCreation = 0;
      patterns = 1;
      temporalAcuity = 0;
      //while ( Serial.read() != '\n'); //skipt until finding the new line character
    
      mapped = (1/fr)*1000000;
      
      if (simultaneous){
        //onTime can go up to 100%
      }else{
        //ontime can go up to 50%
        onTime = (onTime > (mapped/2))? mapped/2 : onTime;
      }
      
      delaytype1 = 0;
      delaytype2 = 0;
      delaytype3 = 0;
      delaytypeBetween = 0;
      
      delay1 = onTime; //mapped*mapped2;
      delay1aux = delay1/1000;
      delay2 = mapped-onTime; //mapped*(1-mapped2);
      delay2aux = delay2/1000;
      wait = (abs(delay2-delay1))/2;
      waitaux = wait/1000;

      if (delay1 < 16380){ //check if it is higher than unsigned int max value
        delaytype1 = 1;
      }else{
        delaytype1 = 0;
      }
      if (delay2 < 16380){ //check if it is higher than unsigned int max value
        delaytype2 = 1;
      }else{
        delaytype2 = 0;
      }

      if (wait < 16380){ //check if it is higher than unsigned int max value
        delaytype3 = 1;
      }else{
        delaytype3 = 0;
      }

      if (delayBetweenStimuli < 16380){ //check if it is higher than unsigned int max value
        delaytypeBetween = 1;
      }else{
        delaytypeBetween = 0;
      }
    }else if(command == 8){//patterns port maniplation  example: 8 1 500000 0.5 1 0 0 100000 1         explanation--> 8 1 onTime frequency pulses pattern[0=square,1=cross,2=triangle] direction[0=left,1=right] delayBetweenStimuli numberOfSimultaneousCoils 
      simultaneous = Serial.parseInt()%2;
      float onTime = (int)Serial.parseFloat();
      float fr = Serial.parseFloat();
      limitTimes = Serial.parseInt();
      pattern = Serial.parseInt();
      direction = Serial.parseInt();
      delayBetweenStimuli = (int)Serial.parseFloat();
      numberOfSimultaneousCoils = Serial.parseInt();
      limit = 0;
      continious = 0;
      pathCreation = 0;
      patterns = 0;
      patterns2 = 1;
      temporalAcuity = 0;
      //while ( Serial.read() != '\n'); //skipt until finding the new line character
    
      mapped = (1/fr)*1000000;
      
      if (simultaneous){
        //onTime can go up to 100%
      }else{
        //ontime can go up to 50%
        onTime = (onTime > (mapped/2))? mapped/2 : onTime;
      }
      
      delaytype1 = 0;
      delaytype2 = 0;
      delaytype3 = 0; 
      delaytypeBetween = 0;
      
      delay1 = onTime; //mapped*mapped2;
      delay1aux = delay1/1000;
      delay2 = mapped-onTime; //mapped*(1-mapped2);
      delay2aux = delay2/1000;
      wait = (abs(delay2-delay1))/2;
      waitaux = wait/1000;

      if (delay1 < 16380){ //check if it is higher than unsigned int max value
        delaytype1 = 1;
      }else{
        delaytype1 = 0;
      }
      if (delay2 < 16380){ //check if it is higher than unsigned int max value
        delaytype2 = 1;
      }else{
        delaytype2 = 0;
      }

      if (wait < 16380){ //check if it is higher than unsigned int max value
        delaytype3 = 1;
      }else{
        delaytype3 = 0;
      }

      if (delayBetweenStimuli < 16380){ //check if it is higher than unsigned int max value
        delaytypeBetween = 1;
      }else{
        delaytypeBetween = 0;
      }
    }else if(command == 9){//set custom pattern   example: 9 1 500000 0.5 1 99 1 100000 1 PATTERN 1,2,3,4,5,6,7,8      explanation  --> 9 1 500000=INTENSITY(microseconds of pulsewidth fed into the coil) 0.5=FREQUENCY(frequency of the pulse fed into the coil) 1=PULSES(number of pulses) ..finish...
      Serial.println("command 9");
      simultaneous = Serial.parseInt()%2;
      float onTime = (int)Serial.parseFloat();
      float fr = Serial.parseFloat();
      limitTimes = Serial.parseInt();
      pattern = Serial.parseInt();
      pattern = -1;
      direction = Serial.parseInt();
      delayBetweenStimuli = (int)Serial.parseFloat();
      numberOfSimultaneousCoils = Serial.parseInt();

      customPatternLength = 0;
      int read = -1;
      while(read != 0 & customPatternLength < maxCustomPatternLength){
        read = Serial.parseInt();
        customPattern[customPatternLength] = 0b000000001 << (read-1);
        customPatternLength += 1;
      }
      if (customPatternLength < 64){
        customPatternLength -= 1; //last char was /n so parseInt returns 0, we decrement the max size
      }
      
      customPatternSet = 1;

      limit = 0;
      continious = 0;
      pathCreation = 0;
      patterns = 0;
      patterns2 = 0;
      temporalAcuity = 0;
      //while ( Serial.read() != '\n'); //skipt until finding the new line character
    
      mapped = (1/fr)*1000000;
      
      if (simultaneous){
        //onTime can go up to 100%
      }else{
        //ontime can go up to 50%
        onTime = (onTime > (mapped/2))? mapped/2 : onTime;
      }
      
      delaytype1 = 0;
      delaytype2 = 0;
      delaytype3 = 0;
      delaytypeBetween = 0;

      delay1 = onTime; //mapped*mapped2;
      delay1aux = delay1/1000;
      delay2 = mapped-onTime; //mapped*(1-mapped2);
      delay2aux = delay2/1000;
      wait = (abs(delay2-delay1))/2;
      waitaux = wait/1000;

      if (delay1 < 16380){ //check if it is higher than unsigned int max value
        delaytype1 = 1;
      }else{
        delaytype1 = 0;
      }
      if (delay2 < 16380){ //check if it is higher than unsigned int max value
        delaytype2 = 1;
      }else{
        delaytype2 = 0;
      }

      if (wait < 16380){ //check if it is higher than unsigned int max value
        delaytype3 = 1;
      }else{
        delaytype3 = 0;
      }

      if (delayBetweenStimuli < 16380){ //check if it is higher than unsigned int max value
        delaytypeBetween = 1;
      }else{
        delaytypeBetween = 0;
      }
      Serial.println(delaytypeBetween);
      Serial.println(delayBetweenStimuli);      
    }else if(command == 10){//play custom pattern that has been previously set
      if(customPatternSet){
        pattern = 99;
        patterns2 = 1;

        limit = 0;
        continious = 0;
        pathCreation = 0;
        patterns = 0;
        temporalAcuity = 0;
      }else{
        Serial.println("please set a custom pattern before trying to play it");
      }
    }else if(command == 11){//for temporal acuity. 2 idetical stimuli of specified frequency and intensity separated by arbitrary delay(ms) 11 1 30 50 137 500 explanation  --> 11 1 30=INTENSITY(us of pulsewidth) 50=FREQUENCY 137=Number of pulses 500=Delay between the 2 stimuli
      simultaneous = Serial.parseInt()%2;
      float onTime = (int)Serial.parseFloat();
      float fr = Serial.parseFloat();
      limitTimes = Serial.parseInt();
      limitTimes2 = limitTimes;
      temporalAcuityDelay = Serial.parseInt();
      limit = 0;
      continious = 0;
      pathCreation = 0;
      temporalAcuity = 1;
      //while ( Serial.read() != '\n'); //skipt until finding the new line character
    
      mapped = (1/fr)*1000000;
      
      if (simultaneous){
        //onTime can go up to 100%
      }else{
        //ontime can go up to 50%
        onTime = (onTime > (mapped/2))? mapped/2 : onTime;
      }
      
      delaytype1 = 0;
      delaytype2 = 0;
      delaytype3 = 0;
      
      delay1 = onTime; //mapped*mapped2;
      delay1aux = delay1/1000;
      delay2 = mapped-onTime; //mapped*(1-mapped2);
      delay2aux = delay2/1000;
      wait = (abs(delay2-delay1))/2;
      waitaux = wait/1000;

      if (delay1 < 16380){ //check if it is higher than unsigned int max value
        delaytype1 = 1;
      }else{
        delaytype1 = 0;
      }
      if (delay2 < 16380){ //check if it is higher than unsigned int max value
        delaytype2 = 1;
      }else{
        delaytype2 = 0;
      }

      if (wait < 16380){ //check if it is higher than unsigned int max value
        delaytype3 = 1;
      }else{
        delaytype3 = 0;
      }

      Serial.print("mapped: ");
      Serial.println(mapped);
      Serial.print("onTime: ");
      Serial.println(onTime);
      Serial.print("delay1: ");
      Serial.println(delay1);
      Serial.println(delay1aux);
      Serial.print("delay2: ");
      Serial.println(delay2);
      Serial.println(delay2aux);
      Serial.print("delay wait: ");
      Serial.println(wait);
      Serial.println(waitaux);
      Serial.print("delay types: ");
      Serial.println(delaytype1);
      Serial.println(delaytype2);
      Serial.println(delaytype3);
    }
    while ( Serial.read() != '\n' & Serial.available() != 0);
  }else{
    if(limit){
      if(limitTimes>0){
        if(simultaneous){
          if(enable){
            digitalWrite(Output1,HIGH);
            digitalWrite(Output2,HIGH);

          }


          if(delaytype1==1){
            delayMicroseconds(delay1);
          }else{
            delayaux(delay1);
          }

          if(enable){
            digitalWrite(Output1,LOW);
            digitalWrite(Output2,LOW);

          }


          if(delaytype2==1){
            delayMicroseconds(delay2);
          }else{
            delayaux(delay2);
          }
        }else{
          //int half = mapped/2;
          
          if(enable){
            digitalWrite(Output1,HIGH);
          }


          if(delaytype1==1){
            delayMicroseconds(delay1);
          }else{
            delayaux(delay1);
          }

          if(enable){
            digitalWrite(Output1,LOW);
          }

          if(delaytype3==1){
            delayMicroseconds(wait);
          }else{
            delayaux(wait);
          }

          if(enable){
            digitalWrite(Output2,HIGH);
          }

          if(delaytype1==1){
            delayMicroseconds(delay1);
          }else{
            delayaux(delay1);
          }

          if(enable){
            digitalWrite(Output2,LOW);
          }

          if(delaytype3==1){
            delayMicroseconds(wait);
          }else{
            delayaux(wait);
          }
        }
        limitTimes--;
      }
      digitalWrite(Output1,LOW);
      digitalWrite(Output2,LOW);
    }else if(continious){
      //Serial.println("continious");
      if(simultaneous){
        if(enable){
          digitalWrite(Output1,HIGH);
          digitalWrite(Output2,HIGH);
        }


        if(delaytype1==1){
          delayMicroseconds(delay1);
        }else{
          delayaux(delay1);
        }

        if(enable){
          digitalWrite(Output1,LOW);
          digitalWrite(Output2,LOW);

        }


        if(delaytype2==1){
          delayMicroseconds(delay2);
        }else{
          delayaux(delay2);
        }
      }else{
        //int half = mapped/2;
        
        if(enable){
            digitalWrite(Output1,HIGH);
          }


        if(delaytype1==1){
          delayMicroseconds(delay1);
        }else{
          delayaux(delay1);
        }

        if(enable){
          digitalWrite(Output1,LOW);
        }

        if(delaytype3==1){
          delayMicroseconds(wait);
        }else{
          delayaux(wait);
        }

        if(enable){
          digitalWrite(Output2,HIGH);
        }

        if(delaytype1==1){
          delayMicroseconds(delay1);
        }else{
          delayaux(delay1);
        }

        if(enable){
          digitalWrite(Output2,LOW);
        }

        if(delaytype3==1){
          delayMicroseconds(wait);
        }else{
          delayaux(wait);
        }
      }
    }else if(pathCreation){// untested
      Serial.println("path creation");
      if(continiousaux){
        if(simultaneous){
          if(enable){
            digitalWrite(Output1,HIGH);
            digitalWrite(Output2,HIGH);

          }


          if(delaytype1==1){
            delayMicroseconds(delay1);
          }else{
            delayaux(delay1);
          }

          if(enable){
            digitalWrite(Output1,LOW);
            digitalWrite(Output2,LOW);

          }


          if(delaytype2==1){
            delayMicroseconds(delay2);
          }else{
            delayaux(delay2);
          }
        }else{
          //int half = mapped/2;
          
          if(enable){
            digitalWrite(Output1,HIGH);
          }


          if(delaytype1==1){
            delayMicroseconds(delay1);
          }else{
            delayaux(delay1);
          }

          if(enable){
            digitalWrite(Output1,LOW);
          }

          if(delaytype3==1){
            delayMicroseconds(wait);
          }else{
            delayaux(wait);
          }

          if(enable){
            digitalWrite(Output2,HIGH);
          }

          if(delaytype1==1){
            delayMicroseconds(delay1);
          }else{
            delayaux(delay1);
          }

          if(enable){
            digitalWrite(Output2,LOW);
          }

          if(delaytype3==1){
            delayMicroseconds(wait);
          }else{
            delayaux(wait);
          }
        }
        digitalWrite(Output1,LOW);
        digitalWrite(Output2,LOW);
        if(simultaneous){
          if(enable){
            digitalWrite(Output1,HIGH);
            digitalWrite(Output2,HIGH);

          }


          if(delaytype1aux==1){
            delayMicroseconds(delay1aux);
          }else{
            delayaux(delay1aux);
          }

          if(enable){
            digitalWrite(Output1,LOW);
            digitalWrite(Output2,LOW);

          }


          if(delaytype2aux==1){
            delayMicroseconds(delay2aux);
          }else{
            delayaux(delay2aux);
          }
        }else{
          //int half = mapped/2;
          
          if(enable){
            digitalWrite(Output1,HIGH);
          }


          if(delaytype1aux==1){
            delayMicroseconds(delay1aux);
          }else{
            delayaux(delay1aux);
          }

          if(enable){
            digitalWrite(Output1,LOW);
          }

          if(delaytype3aux==1){
            delayMicroseconds(waitaux);
          }else{
            delayaux(waitaux);
          }

          if(enable){
            digitalWrite(Output2,HIGH);
          }

          if(delaytype1aux==1){
            delayMicroseconds(delay1aux);
          }else{
            delayaux(delay1aux);
          }

          if(enable){
            digitalWrite(Output2,LOW);
          }

          if(delaytype3aux==1){
            delayMicroseconds(waitaux);
          }else{
            delayaux(waitaux);
          }
        }
      }else{
        if(limitTimes > 0){
          if(simultaneous){
            if(enable){
              digitalWrite(Output1,HIGH);
              digitalWrite(Output2,HIGH);

            }


            if(delaytype1==1){
              delayMicroseconds(delay1);
            }else{
              delayaux(delay1);
            }

            if(enable){
              digitalWrite(Output1,LOW);
              digitalWrite(Output2,LOW);

            }


            if(delaytype2==1){
              delayMicroseconds(delay2);
            }else{
              delayaux(delay2);
            }
          }else{
            //int half = mapped/2;
            
            if(enable){
              digitalWrite(Output1,HIGH);
            }


            if(delaytype1==1){
              delayMicroseconds(delay1);
            }else{
              delayaux(delay1);
            }

            if(enable){
              digitalWrite(Output1,LOW);
            }

            if(delaytype3==1){
              delayMicroseconds(wait);
            }else{
              delayaux(wait);
            }

            if(enable){
              digitalWrite(Output2,HIGH);
            }

            if(delaytype1==1){
              delayMicroseconds(delay1);
            }else{
              delayaux(delay1);
            }

            if(enable){
              digitalWrite(Output2,LOW);
            }

            if(delaytype3==1){
              delayMicroseconds(wait);
            }else{
              delayaux(wait);
            }
          }
          limitTimes -= 1;
        }
        digitalWrite(Output1,LOW);
        digitalWrite(Output2,LOW);
        if(limitTimesaux > 0){
          if(simultaneous){
            if(enable){
              digitalWrite(Output1,HIGH);
              digitalWrite(Output2,HIGH);

            }


            if(delaytype1aux==1){
              delayMicroseconds(delay1aux);
            }else{
              delayaux(delay1aux);
            }

            if(enable){
              digitalWrite(Output1,LOW);
              digitalWrite(Output2,LOW);

            }


            if(delaytype2aux==1){
              delayMicroseconds(delay2aux);
            }else{
              delayaux(delay2aux);
            }
          }else{
            //int half = mapped/2;
            
            if(enable){
              digitalWrite(Output1,HIGH);
            }


            if(delaytype1aux==1){
              delayMicroseconds(delay1aux);
            }else{
              delayaux(delay1aux);
            }

            if(enable){
              digitalWrite(Output1,LOW);
            }

            if(delaytype3aux==1){
              delayMicroseconds(waitaux);
            }else{
              delayaux(waitaux);
            }

            if(enable){
              digitalWrite(Output2,HIGH);
            }

            if(delaytype1aux==1){
              delayMicroseconds(delay1aux);
            }else{
              delayaux(delay1aux);
            }

            if(enable){
              digitalWrite(Output2,LOW);
            }

            if(delaytype3aux==1){
              delayMicroseconds(waitaux);
            }else{
              delayaux(waitaux);
            }
          }
          limitTimesaux -= 1;
        }
      }
    }else if(patterns){
      Serial.println("patterns no simul");
      if(pattern == 0){//square
        if(direction == 0){ // left?
          for(int i = 0;i<NumOutputsSquare;i++){
            for(int j = 0;j<limitTimes;j++){
              digitalWrite(Outputs[i],HIGH);

              if(delaytype1==1){
                delayMicroseconds(delay1);
              }else{
                delayaux(delay1);
              }

              digitalWrite(Outputs[i],LOW);

              if(delaytype2==1){
                delayMicroseconds(delay2);
              }else{
                delayaux(delay2);
              }
            }
            if(delaytypeBetween==1){
              delayMicroseconds(delayBetweenStimuli);
            }else{
              delayaux(delayBetweenStimuli);
            }
          }
        }else if(direction == 1){
          for(int i = NumOutputsSquare-1;i>=0;i--){
            for(int j = limitTimes;j>0;j--){
              digitalWrite(Outputs[i],HIGH);

              if(delaytype1==1){
                delayMicroseconds(delay1);
              }else{
                delayaux(delay1);
              }

              digitalWrite(Outputs[i],LOW);

              if(delaytype2==1){
                delayMicroseconds(delay2);
              }else{
                delayaux(delay2);
              }
            }
            if(delaytypeBetween==1){
              delayMicroseconds(delayBetweenStimuli);
            }else{
              delayaux(delayBetweenStimuli);
            }
          }
        }
      }else if (pattern == 1){//cross
        if(direction == 0){ // left?
          for(int i = 0;i<NumOutputsCross;i++){
            for(int j = 0;j<limitTimes;j++){
              digitalWrite(Outputs[i],HIGH);

              if(delaytype1==1){
                delayMicroseconds(delay1);
              }else{
                delayaux(delay1);
              }

              digitalWrite(Outputs[i],LOW);

              if(delaytype2==1){
                delayMicroseconds(delay2);
              }else{
                delayaux(delay2);
              }
            }
            if(delaytypeBetween==1){
              delayMicroseconds(delayBetweenStimuli);
            }else{
              delayaux(delayBetweenStimuli);
            }
          }
        }else if(direction == 1){ //right?
          for(int i = NumOutputsCross-1;i>=0;i--){
            for(int j = limitTimes;j>0;j--){
              digitalWrite(Outputs[i],HIGH);

              if(delaytype1==1){
                delayMicroseconds(delay1);
              }else{
                delayaux(delay1);
              }

              digitalWrite(Outputs[i],LOW);

              if(delaytype2==1){
                delayMicroseconds(delay2);
              }else{
                delayaux(delay2);
              }
            }
            if(delaytypeBetween==1){
              delayMicroseconds(delayBetweenStimuli);
            }else{
              delayaux(delayBetweenStimuli);
            }
          }
        }
      }
      patterns = 0;
    }else if(patterns2){
      Serial.println("patterns2 port manipulation");
      if(pattern == 0){//square
        pseudoRegister = 0b011111111>>(NumOutputsSquare-numberOfSimultaneousCoils);//set register to start with the numberOfSimultaneousCoils
        if(direction == 0){ // left?
          for(int i = 0;i<NumOutputsSquare;i++){
            mask = customBitMaskSet((uint8_t)pseudoRegister);//set mask
            for(int j = 0;j<limitTimes;j++){
              GPIO.out_w1ts = mask;  //SET HIGH pins
              GPIO.out_w1tc = aux & ~mask; //SET LOW pins
              if(delaytype1==1){
              delayMicroseconds(delay1);
              }else{
                delayaux(delay1);
              }
              GPIO.out_w1ts = 0b0;  //SET HIGH pins
              //GPIO.out_w1tc = aux & ~mask; //SET LOW pins
              if(delaytype2==1){
              delayMicroseconds(delay2);
              }else{
                delayaux(delay2);
              } 
            }
            if(delaytypeBetween==1){
              delayMicroseconds(delayBetweenStimuli);
            }else{
              delayaux(delayBetweenStimuli);
            }
            if(i>NumOutputsSquare-numberOfSimultaneousCoils-1){
              pseudoRegister = pseudoRegister << 1;
            }else{
              pseudoRegister = ROTATE_LEFT((uint8_t)pseudoRegister,8,1);//rotate register with cutoff at 8 bits
            }
          }
        }else if(direction == 1){
          for(int i = 0;i<NumOutputsSquare;i++){
            mask = customBitMaskSet((uint8_t)pseudoRegister);//set mask
            for(int j = 0;j<limitTimes;j++){
              GPIO.out_w1ts = mask;  //SET HIGH pins
              GPIO.out_w1tc = aux & ~mask; //SET LOW pins
              if(delaytype1==1){
              delayMicroseconds(delay1);
              }else{
                delayaux(delay1);
              }
              GPIO.out_w1ts = 0b0;  //SET HIGH pins
              //GPIO.out_w1tc = aux & ~mask; //SET LOW pins 
              if(delaytype2==1){
              delayMicroseconds(delay2);
              }else{
                delayaux(delay2);
              }
            }
            if(delaytypeBetween==1){
              delayMicroseconds(delayBetweenStimuli);
            }else{
              delayaux(delayBetweenStimuli);
            }
            printBits(9,pseudoRegister);
            Serial.println(i);
            Serial.println(NumOutputsSquare-numberOfSimultaneousCoils-1);
            Serial.println(NumOutputsSquare-i+numberOfSimultaneousCoils);
            if(i>NumOutputsSquare-numberOfSimultaneousCoils-1){
              pseudoRegister = BIT_CLEAR(pseudoRegister,NumOutputsSquare-i+numberOfSimultaneousCoils-1);
            }else{
              pseudoRegister = ROTATE_RIGHT((uint8_t)pseudoRegister,8,1);//rotate register with cutoff at 8 bits
            }
            printBits(9,pseudoRegister);
            Serial.println("---------");
          }
        }
      }else if (pattern == 1){//cross
        pseudoRegister = 0b000000000;
        int skipClear = 0;
        if(direction == 0){ // left?
          for(int i=0;i<numberOfSimultaneousCoils;i++){ //can change this to avoid the modulus on the botoom of direction==1
            pseudoRegister |= OutputsCrossBinary[i];
          }
          for(int i = 0;i<NumOutputsCross;i++){
            mask = customBitMaskSet(pseudoRegister);//set mask
            for(int j = 0;j<limitTimes;j++){
              GPIO.out_w1ts = mask;  //SET HIGH pins
              GPIO.out_w1tc = aux & ~mask; //SET LOW pins 
              
              if(delaytype1==1){
              delayMicroseconds(delay1);
              }else{
                delayaux(delay1);
              }
              GPIO.out_w1ts = 0b0;  //SET HIGH pins
              //GPIO.out_w1tc = aux & ~mask; //SET LOW pins 
              
              if(delaytype2==1){
              delayMicroseconds(delay2);
              }else{
                delayaux(delay2);
              }
            }
            
            if(delaytypeBetween==1){
              delayMicroseconds(delayBetweenStimuli);
            }else{
              delayaux(delayBetweenStimuli);
            }
            if(i+numberOfSimultaneousCoils<NumOutputsCross){
              pseudoRegister |= OutputsCrossBinary[i+numberOfSimultaneousCoils];//add next coil to the register
              if(OutputsCrossBinary[i] == OutputsCrossBinary[i+numberOfSimultaneousCoils]){
                skipClear = 1;
              }else{
                skipClear = 0;
              }
            }
            if(!skipClear){
              pseudoRegister = BITMASK_CLEAR(pseudoRegister,OutputsCrossBinary[i]);//remove current coil from the register
            }
          }
        }else if(direction == 1){ //right?
          for(int i=0;i<numberOfSimultaneousCoils;i++){ //can change this to avoid the modulus on the botoom of direction==1
            pseudoRegister |= OutputsCrossBinary[NumOutputsCross-1-i];
          }
          for(int i = NumOutputsCross-1;i>=0;i--){
            mask = customBitMaskSet(pseudoRegister);//set mask
            for(int j = 0;j<limitTimes;j++){
              GPIO.out_w1ts = mask;  //SET HIGH pins
              GPIO.out_w1tc = aux & ~mask; //SET LOW pins 
              
              if(delaytype1==1){
              delayMicroseconds(delay1);
              }else{
                delayaux(delay1);
              }
              GPIO.out_w1ts = 0b0;  //SET HIGH pins
              //GPIO.out_w1tc = aux & ~mask; //SET LOW pins 
              
              if(delaytype2==1){
              delayMicroseconds(delay2);
              }else{
                delayaux(delay2);
              }
            }
            
            if(delaytypeBetween==1){
              delayMicroseconds(delayBetweenStimuli);
            }else{
              delayaux(delayBetweenStimuli);
            }
            
            if(i-numberOfSimultaneousCoils >= 0){
              pseudoRegister |= OutputsCrossBinary[i-numberOfSimultaneousCoils];//add next coil to the register
              if(OutputsCrossBinary[i] == OutputsCrossBinary[i-numberOfSimultaneousCoils]){
                skipClear = 1;
              }else{
                skipClear = 0;
              }
            }
            if(!skipClear){
              pseudoRegister = BITMASK_CLEAR(pseudoRegister,OutputsCrossBinary[i]);//BIT_CLEAR(pseudoRegister,(i+numberOfSimultaneousCoils)%6);//remove current coil from the register
            }
          }
        }
      }else if (pattern == 2){//triangle
        pseudoRegister = 0b000000000;
        for(int i=0;i<numberOfSimultaneousCoils;i++){ //can change this to avoid the modulus on the botoom of direction==1
          pseudoRegister |= OutputsTriangleTopLeftBinary[i];
        }
        if(direction == 0){ // left?
          for(int i = 0;i<NumOutputsTriangle;i++){

            mask = customBitMaskSet(pseudoRegister);//set mask
            Serial.print("mask: ");
            printBits(23,mask);
            for(int j = 0;j<limitTimes;j++){
              GPIO.out_w1ts = mask;  //SET HIGH pins
              GPIO.out_w1tc = aux & ~mask; //SET LOW pins 
              
              if(delaytype1==1){
              delayMicroseconds(delay1);
              }else{
                delayaux(delay1);
              }
              Serial.print("mask: ");
              printBits(23,~mask);

              GPIO.out_w1ts = 0b0;  //SET HIGH pins
              //GPIO.out_w1tc = aux; //SET LOW pins 
              
              if(delaytype2==1){
              delayMicroseconds(delay2);
              }else{
                delayaux(delay2);
              }
            }
            if(delaytypeBetween==1){
              delayMicroseconds(delayBetweenStimuli);
            }else{
              delayaux(delayBetweenStimuli);
            }
            if(i+numberOfSimultaneousCoils<NumOutputsTriangle){
              pseudoRegister |= OutputsTriangleTopLeftBinary[i+numberOfSimultaneousCoils];//add next coil to the register
            }
            pseudoRegister = BITMASK_CLEAR(pseudoRegister,OutputsTriangleTopLeftBinary[i]);//remove previous coil from the register
          }
        }else if(direction == 1){ //right?
          for(int i = NumOutputsTriangle-1;i>=0;i--){
            mask = customBitMaskSet(pseudoRegister);//set mask
            for(int j = 0;j<limitTimes;j++){
              GPIO.out_w1ts = mask;  //SET HIGH pins
              GPIO.out_w1tc = aux & ~mask;//SET LOW pins
       
              if(delaytype1==1){
              delayMicroseconds(delay1);
              }else{
                delayaux(delay1);
              }

              GPIO.out_w1ts = 0b0;//SET HIGH pins
              //GPIO.out_w1tc = aux & ~mask; //SET LOW pins 
              
              if(delaytype2==1){
                delayMicroseconds(delay2);
              }else{
                delayaux(delay2);
              }
            }
            
            if(delaytypeBetween==1){
              delayMicroseconds(delayBetweenStimuli);
            }else{
              delayaux(delayBetweenStimuli);
            }
            if(i>=numberOfSimultaneousCoils){
              pseudoRegister |= OutputsTriangleTopLeftBinary[i];//add next coil to the register
            }
            pseudoRegister = BITMASK_CLEAR(pseudoRegister,OutputsTriangleTopLeftBinary[(i+numberOfSimultaneousCoils)%NumOutputsTriangle]);//remove current coil from the register
          }
        }
      }else if (pattern == 99){//custom pattern
        Serial.println("pattern99 custom");
        pseudoRegister = 0b000000000;
        if(direction == 0){ // left?
        for(int i=0;i<numberOfSimultaneousCoils;i++){
          pseudoRegister |= customPattern[i];
        }
          for(int i = 0;i<customPatternLength;i++){
            mask = customBitMaskSet(pseudoRegister);//set mask
            for(int j = 0;j<limitTimes;j++){
              GPIO.out_w1ts = mask;  //SET HIGH pins
              GPIO.out_w1tc = aux & ~mask; //SET LOW pins 
              
              if(delaytype1==1){
              delayMicroseconds(delay1);
              }else{
                delayaux(delay1);
              }


              GPIO.out_w1ts = 0b0;  //SET HIGH pins
              GPIO.out_w1tc = aux;// //SET LOW pins 

              if(delaytype2==1){
              delayMicroseconds(delay2);
              }else{
                delayaux(delay2);
              }
            }
           
            if(delaytypeBetween==1){
              delayMicroseconds(delayBetweenStimuli);
            }else{
              delayaux(delayBetweenStimuli);
            }
            if(i+numberOfSimultaneousCoils<customPatternLength){
              pseudoRegister |= customPattern[i+numberOfSimultaneousCoils];//add next coil to the register
            }
            pseudoRegister = BITMASK_CLEAR(pseudoRegister,customPattern[i]);//remove previous coil from the register
          }
        }else if(direction == 1){ //right?
          for(int i=customPatternLength-1;i>=customPatternLength-numberOfSimultaneousCoils;i--){
            pseudoRegister |= customPattern[i];
          }
          for(int i = customPatternLength-1;i>=0;i--){
            mask = customBitMaskSet(pseudoRegister);//set mask
            for(int j = 0;j<limitTimes;j++){
              GPIO.out_w1ts = mask;  //SET HIGH pins
              GPIO.out_w1tc = aux & ~mask;//SET LOW pins

              if(delaytype1==1){
              delayMicroseconds(delay1);
              }else{
                delayaux(delay1);
              }

              GPIO.out_w1ts = 0b0;//SET HIGH pins
              GPIO.out_w1tc = aux; //SET LOW pins 

              if(delaytype2==1){
                delayMicroseconds(delay2);
              }else{
                delayaux(delay2);
              }
            }
            if(delaytypeBetween==1){
              delayMicroseconds(delayBetweenStimuli);
            }else{
              delayaux(delayBetweenStimuli);
            }
            if(i>=numberOfSimultaneousCoils){
              pseudoRegister |= customPattern[i-numberOfSimultaneousCoils];//add next coil to the register
            }
            pseudoRegister = BITMASK_CLEAR(pseudoRegister,customPattern[i]);
          }
        }
      }
      GPIO.out_w1ts = 0b0; //SET HIGH pins
      GPIO.out_w1tc = aux; //SET LOW pins
      patterns2 = 0;
    }else if(temporalAcuity){
      if(limitTimes > 0){
        if(simultaneous){
          if(enable){
            digitalWrite(Output1,HIGH);
            digitalWrite(Output2,HIGH);

          }


          if(delaytype1==1){
            delayMicroseconds(delay1);
          }else{
            delayaux(delay1);
          }

          if(enable){
            digitalWrite(Output1,LOW);
            digitalWrite(Output2,LOW);

          }
        }
        limitTimes -= 1;
      }

      delay(temporalAcuityDelay);
      
      if(limitTimes == 0 & limitTimes2 > 0){
        if(simultaneous){
          if(enable){
            digitalWrite(Output1,HIGH);
            digitalWrite(Output2,HIGH);

          }


          if(delaytype1==1){
            delayMicroseconds(delay1);
          }else{
            delayaux(delay1);
          }

          if(enable){
            digitalWrite(Output1,LOW);
            digitalWrite(Output2,LOW);

          }
        }
        limitTimes2 -= 1;
      }
      digitalWrite(Output1,LOW);
      digitalWrite(Output2,LOW);
    }
  }
}
