//LEFT
int sensor_1 = A2;
int sensorReading_1;

//MIDDLE
int sensor_2 =A4 ;
int sensorReading_2;

//RIGHT
int sensor_3 = A1;
int sensorReading_3;

void setup() {
Serial.begin(9600);
pinMode(sensor_1,INPUT);
pinMode(sensor_2,INPUT);
pinMode(sensor_3,INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
sensorReading_1 = analogRead(sensor_1);
sensorReading_2 = analogRead(sensor_2);
sensorReading_3 = analogRead(sensor_3);

Serial.print("Sensor Reading: ");
Serial.println(sensorReading_1);
Serial.println(sensorReading_2);
Serial.println(sensorReading_3);
Serial.println();
delay(500);
}