
//FRONT_PROG
//DO NOW
//FIRST PRIORITY
//MAKE SO IT DOESNT HAVE ONE WALL OR NO WALL AND IT JUST GOES STRAIGHT (while going one cell at a atime)
// and do random move without front wall

//SECONDARY PRIORITIES
//MAKE PID WITH I AND D 
//MAKE ERROR CATCH CATCH MORE ERRORS
//MAKE PID ON AND OFF
//STATE MACHINES 
//ENUM

//general notes
//LET IT HIT THE WALL TO STRAIGHTEN OUT
//CALIBRATION PID DOESNT HELP


bool first_check = true;
int left_offset = 20; // last value was 15 , 5 , 30, 10

//GLOBAL for pid
int permReading_left;
int permReading_middle;
int permReading_right;
int base_speed =150;// 200,150,125
double kp = 0.30;//0.50,0.25
double kd = 0.30;
double error;
double prev_error;
double d_control;

double motor_left = 125;
double motor_right = 125;

int left_ir_low_bound = 994;
int left_ir_high_bound = 1007;

//Motor 
int motor_1_logic_1 = 3;
int motor_1_logic_2 = 4;

int motor_2_logic_1 = 5;
int motor_2_logic_2 = 10;

int turn_on_en_1 = 22;
int turn_on_en_2 = 23;


//encoder
int RH_ENCODER_A = 20; 
int RH_ENCODER_B = 21;

int LH_ENCODER_A = 0;
int LH_ENCODER_B = 1;

unsigned long left_count=0;
unsigned long right_count=0;



//IRLED

//LEFT
int sensor_left = A2;
int sensorReading_left;
int sensor_left_power = 2; 


//MIDDLE
int sensor_middle =A4 ;
int sensorReading_middle;
int sensor_middle_power = 7;


//RIGHT
int sensor_right = A1;
int sensorReading_right;
int sensor_right_power = 12;

//prev led values 
int prev_sensorReading_right;
int prev_sensorReading_middle;
int prev_sensorReading_left;

//int val;




void setup() {
  attachInterrupt(digitalPinToInterrupt(LH_ENCODER_A),left_encoder_event,CHANGE);
  attachInterrupt(digitalPinToInterrupt(LH_ENCODER_B),left_encoder_event,CHANGE);
  
  attachInterrupt(digitalPinToInterrupt(RH_ENCODER_A),right_encoder_event,CHANGE);
  attachInterrupt(digitalPinToInterrupt(RH_ENCODER_B),right_encoder_event,CHANGE);
  
  Serial.begin(9600);
  //calibrate

   
  //irled
  //Serial.begin(9600);
   
  Serial.begin(9600);
  pinMode(sensor_left,INPUT);
  pinMode(sensor_middle,INPUT);
  pinMode(sensor_right,INPUT);
  
  pinMode(sensor_left_power,OUTPUT);
  pinMode(sensor_middle_power,OUTPUT);
  pinMode(sensor_right_power,OUTPUT);
  digitalWrite(sensor_left_power, HIGH);
  
  digitalWrite(sensor_middle_power, HIGH);
  
  digitalWrite(sensor_right_power, HIGH);

  //motors
  // pinMode(turn_on_en_input,INPUT);
  pinMode(turn_on_en_1,OUTPUT);
  pinMode(turn_on_en_2,OUTPUT);
  
  pinMode(motor_1_logic_1,OUTPUT);
  
  pinMode(motor_1_logic_2,OUTPUT);
  
  pinMode(motor_2_logic_1,OUTPUT);
  
  pinMode(motor_2_logic_2,OUTPUT);
  
  digitalWrite(turn_on_en_1,LOW);
  digitalWrite(turn_on_en_2,LOW);



  //sensorReading_middle = 0;
  //readIR();
  
  calibrate_pid();

  wait_until_start_hand();

  //}
}

void wait_until_start_hand(){
  
  while(first_check){
  //readIR();
  readIR_map();
  //delay(500);

  if (hasfrontwall()){
    first_check = false;
    delay(500);
    ////*
    //unsigned long curr = millis();
    //while (millis() - curr < 500){
    //nothing (just waiting for 500 seconds to pass
    //}
   }
   //*/
   //delay(300);
  }
}

//encoder and turns
void left_encoder_event() {
left_count ++;
}

 
// encoder event for the interrupt call
void right_encoder_event() {
right_count ++;
}



void print_encoder_count(){
  
  Serial.print("left_count");
  Serial.println(left_count);
  Serial.print("right_count");
  Serial.println(right_count);
}


void forward_until(int left_speed,int right_speed,int stop_time){
   
  unsigned long curr = millis();
  while (millis() - curr < stop_time){
    digitalWrite(turn_on_en_1,HIGH);
    digitalWrite(turn_on_en_2,HIGH);
  
    analogWrite(motor_1_logic_2,left_speed);
    digitalWrite(motor_1_logic_1,LOW);
    analogWrite(motor_2_logic_2,right_speed);
    digitalWrite(motor_2_logic_1,LOW);
  } 
  
}


void left_turn_until(){
  unsigned long curr = left_count;
  while( left_count - curr < 340){//380 330 ,280,290,300,310,320,330
    left_turn();
  }
}
//use left_count
void right_turn_until(){
  unsigned long curr = left_count;
  while( left_count - curr < 260){ //320,340, 370,300,290,280,270
    right_turn();
  }
}



void reverse_turn_until(){
  unsigned long curr_l = left_count;
  //use left_count instead of right_count
  unsigned long curr_r = left_count;
  
 if (sensorReading_left < sensorReading_right){
  while( left_count - curr_l < 850){ //800,830, 860,870,790,800,810,840
    left_turn();
 }
 
}

  else{//use left_count instead of right_count
    while( left_count - curr_r < 790){ //800,830, 860,870
      right_turn();
    }
       
  }
 
}

void go_one_cell(){
  unsigned long curr = left_count;
  while(left_count-curr < 945){ // 450,600, 800,950,900,960,955,945,950
    readIR_map();
    /*
    
    if (hasleftwall()!= true || hasrightwall()!= true){
      motor_left = base_speed;
      motor_right= base_speed-10;
      forward(motor_left,motor_right);
      continue;
    }
    */
  /*
    if (abs(sensorReading_left - sensorReading_right) > 300){
      motor_left = base_speed;
      motor_right= base_speed-10;
      forward(motor_left,motor_right);
      continue;
      
    }
 */
    pid_control();
    forward(motor_left,motor_right);
  }
}

//maybe delete this 
void go_one_cell_no_wall(){
  unsigned long curr = left_count;
  while(left_count-curr < 950){ // 450,600, 800,950,900,960,955,945
    //pid_control();
    //forward(150,150);
    forward(motor_left,motor_right);
  }
}

void halt_until(int stop_time ){
 unsigned long curr = millis();
  while (millis() - curr < stop_time){
    halt();
  } 
}

//CALIBRATION
void calibrate_pid(){
/*
permReading_left = analogRead(sensor_left);
permReading_middle= analogRead(sensor_middle);
permReading_right= analogRead(sensor_right);

//map left and right
permReading_left = map(permReading_left,993,1009,0,200);
permReading_right = map(permReading_right,180,820,0,200);
*/
///*
permReading_left = 0;
permReading_middle= 0;
permReading_right= 0;
//*/

//trying to get readings to be the same 

//permReading_left -= left_offset;


}



//CHOICES
bool hasfrontwall(){
  if (sensorReading_middle >360){ //300 , 350,370,400,275,330,360
    return true;
  }
  return false;
}

bool hasleftwall(){                                      //350 maybe a bit too high
  if (sensorReading_left >500){ //100 ,500,300,250,275,400,350,310,320,343,410,420,550
      return true;
  }
  return false;
}


bool hasrightwall(){
  if (sensorReading_right >340){//350 ,500,300,250,275 ,280,320,344
      return true;
  }
  return false;
}

//possibly change so in main loop this only runs if hasfrontwall returns true 
void random_move(){
  int random_move;
  readIR_map();
  //has to turn cases
    
    if (hasleftwall() && hasrightwall() && !hasfrontwall()){
      return;
    }
    //if (abs(sensorReading_left - sensorReading_right) < 300){
    //  return;
    //}
    if (hasleftwall() && hasrightwall() && hasfrontwall()){
      reverse_turn_until();
      return;
    }
    else if (hasleftwall() && !hasrightwall() && hasfrontwall()){
      right_turn_until();
      return;
    }
    //this isnt running //hopefully it runs now //STILL NOT RUNNING //It WORKS NOW
    else if (hasrightwall()  && !hasleftwall() && hasfrontwall()){//|| (hasfronwall() &&(hasrightwall - hasleftwall > 200) ){//
      left_turn_until();
      return;
    }

    //random choice cases 

    // 
    
    else if (!hasleftwall()&& !hasfrontwall() && hasrightwall())  {
      random_move = random(millis()) % 2;
      if (random_move == 1){
        left_turn_until();
        return;
      }
      else{
      //goes straight
        return;
        
      }
    } 

    
    else if (hasleftwall()&& !hasfrontwall() && !hasrightwall() ) {
      random_move = random(millis()) % 2;
      if (random_move == 1){
        right_turn_until();
        return;
      }
      else{
      //goes straight
        return;
        
      }
    } 

    else if (!hasleftwall()&& hasfrontwall() && !hasrightwall())  {
      random_move = random(millis()) % 2;
      if (random_move == 1){
        right_turn_until();
        return;
      }
      else{
        left_turn_until();
        return;
        
      }
    }


//in case of errors 
    else{
      random_move = random(millis()) % 2;
      if (random_move == 1){
        right_turn_until();
        return;
      }
      else{
        left_turn_until();
        return;
        
      }
    }
    return;
  
}
  
  
  
  
void loop(){
  ///*
  //readIR_map();
  //pid_control();
  
  //pid_control();
  //forward(motor_left,motor_right);
  //readIR_map();
 
  go_one_cell();
  halt_until(800);
  
  
  
  readIR_map();

  
  
  //if (hasfrontwall()){
  if (hasfrontwall()){
   halt_until(700);
   forward_until(125,125,400);
   halt_until(600);
   random_move(); 
   halt_until(700);
  }

  error_catch();
  //*/
  //forward(base_speed,base_speed-10);  
}



//IR 
void readIR(){
  
  sensorReading_left = analogRead(sensor_left);
  sensorReading_middle = analogRead(sensor_middle);
  sensorReading_right = analogRead(sensor_right);
  
  Serial.print("Sensor Reading: ");
  Serial.println(sensorReading_left);
  Serial.println(sensorReading_middle);
  Serial.println(sensorReading_right);
  Serial.println();
} 
  
  
void readIR_map(){
  prev_sensorReading_left = sensorReading_left;
  prev_sensorReading_middle = sensorReading_middle;
  prev_sensorReading_right = sensorReading_right;
    
  sensorReading_left = analogRead(sensor_left);
  sensorReading_middle = analogRead(sensor_middle);
  sensorReading_right = analogRead(sensor_right);


  //sensorReading_left = map(sensorReading_left,993,1009,0,200);

  //CURRENT TRY TO MAP LEFT READING TO RIGHT SENSOR READING RANGE (180 - 820)
  //sensorReading_left = map(sensorReading_left,991,1005,180,820);
                                             //991,992
  sensorReading_left = map(sensorReading_left,left_ir_low_bound,left_ir_high_bound,0,200);
  sensorReading_left -= left_offset;
  
  //sensorReading_right = map(sensorReading_right,180,820,0,200);


  Serial.print("Sensor Reading: ");
  Serial.print( "LEFT : ");
  Serial.println(sensorReading_left);
  Serial.print( "MIDDLE : ");
  Serial.println(sensorReading_middle);
  Serial.print( "RIGHT : ");
  Serial.println(sensorReading_right);
  Serial.println();

}



//PID / ERROR_CATCHING 

void error_catch(){
  if (prev_sensorReading_left== sensorReading_left && prev_sensorReading_middle== sensorReading_middle && prev_sensorReading_right== sensorReading_right){
    int rand_num = random(millis()%2);
    if (rand_num == 1){
      left_turn_until();
    }
    else{
      right_turn_until();
    }
  }
}

void regulateSensorL(){
  /*
    
   readIR_map();
  if (!hasleftwall || !hasrightwall){
    forward(prev_motor_left,prev_motor_right);
    return;
  }
  */
  sensorReading_left = analogRead(sensor_left);
  
  //sensorReading_left = map(sensorReading_left,993,1009,0,200);
  //sensorReading_left = map(sensorReading_left,991,1005,0,200);
  sensorReading_left = map(sensorReading_left,left_ir_low_bound,left_ir_high_bound,0,200);
  sensorReading_left -= left_offset;
  
  sensorReading_left = sensorReading_left- permReading_left;
  if (analogRead(sensor_left) - permReading_left < 0){
    sensorReading_left = ~sensorReading_left + 1;
  }  
  
} 

void regulateSensorR(){
  /*
  readIR_map();
  if (!hasleftwall || !hasrightwall){
    forward(prev_motor_left,prev_motor_right);
    return;
  }
  */
  sensorReading_right = analogRead(sensor_right);
  
  sensorReading_right = map(sensorReading_right,180,820,0,200);
  
  sensorReading_right = sensorReading_right- permReading_right;
  if (analogRead(sensor_right) - permReading_right < 0){
    sensorReading_right = ~sensorReading_right + 1;
  }
}



void pid_control(){
  //if no wall on either side
  //TRY TO FIX IF NO WALL ON EITHER SIDE DO DEFAULT VALUES for motor
  /*
  readIR_map();
  if ((sensorReading_left - sensorReading_right > 200) || (sensorReading_right- sensorReading_left > 200)){
      motor_left = base_speed;
      motor_right= base_speed-10;
      return;
    }
  //end of added 
  */    
  regulateSensorL();
  regulateSensorR();


  
  //too close left
  if (sensorReading_left > sensorReading_right){
    
    prev_error = error;
    error = sensorReading_left - sensorReading_right;
    d_control = (error -prev_error)*kd;
    
    
    //forward(base_speed,base_speed - (( sensorReading_left - sensorReading_right) * kp));
    motor_left = base_speed;
    motor_right = base_speed - (( sensorReading_left - sensorReading_right) * kp);
    return;
  }
  
  //too close right
  if (sensorReading_right > sensorReading_left){

    prev_error = error;
    error = sensorReading_left - sensorReading_right;
    d_control = (error -prev_error)*kd;
    
    //forward(base_speed - ((sensorReading_right - sensorReading_left) * kp),base_speed);
    motor_left = base_speed - ((sensorReading_right - sensorReading_left) * kp);
    motor_right = base_speed;
    return;
  }
  //( sensorReading_left - sensorReading_right)
//(sensorReading_right - sensorReading_left)
} 
  
  
//MOTORS

void reverse(){
  digitalWrite(turn_on_en_1,HIGH);
  digitalWrite(turn_on_en_2,HIGH);
  
  digitalWrite(motor_1_logic_1,HIGH);
  digitalWrite(motor_1_logic_2,LOW);
  
  digitalWrite(motor_2_logic_1,HIGH);
  digitalWrite(motor_2_logic_2,LOW);
  
} 
  

void forward(){
  
  digitalWrite(turn_on_en_1,HIGH);
  digitalWrite(turn_on_en_2,HIGH);
  
  digitalWrite(motor_1_logic_1,LOW);
  digitalWrite(motor_1_logic_2,HIGH);
  
  digitalWrite(motor_2_logic_1,LOW);
  digitalWrite(motor_2_logic_2,HIGH);
  
}



void halt(){
  digitalWrite(turn_on_en_1,LOW);
  digitalWrite(turn_on_en_2,LOW);
  
  digitalWrite(motor_1_logic_1,LOW);
  digitalWrite(motor_1_logic_2,LOW);
  digitalWrite(motor_2_logic_1,LOW);
  digitalWrite(motor_2_logic_2,LOW);
    

}



void reverse_turn(){
  digitalWrite(turn_on_en_1,HIGH);
  digitalWrite(turn_on_en_2,HIGH);
    
  digitalWrite(motor_1_logic_1,100);
  digitalWrite(motor_1_logic_2,LOW);
  digitalWrite(motor_2_logic_1,LOW);
  digitalWrite(motor_2_logic_2,100);

}


void left_turn(){
  digitalWrite(turn_on_en_1,HIGH);
  digitalWrite(turn_on_en_2,HIGH);

  
  digitalWrite(motor_1_logic_1,100);
  digitalWrite(motor_1_logic_2,LOW);
  digitalWrite(motor_2_logic_1,LOW);
  digitalWrite(motor_2_logic_2,100);
}


void right_turn(){
  digitalWrite(turn_on_en_1,HIGH);
  digitalWrite(turn_on_en_2,HIGH);

  
  digitalWrite(motor_1_logic_2,HIGH);
  digitalWrite(motor_2_logic_1,HIGH);
  digitalWrite(motor_1_logic_1,LOW);
  digitalWrite(motor_2_logic_2,LOW);
}


void forward(int left_speed,int right_speed){
  digitalWrite(turn_on_en_1,HIGH);
  digitalWrite(turn_on_en_2,HIGH);
  
  analogWrite(motor_1_logic_2,left_speed);
  digitalWrite(motor_1_logic_1,LOW);
  analogWrite(motor_2_logic_2,right_speed);
  digitalWrite(motor_2_logic_1,LOW);
}



