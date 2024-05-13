
//PROJEK WAHANA CERDAS - SAVI
//TEKNIK ELEKTRO UNIVERSITAS PADJADJARAN
//2023

#include <ArduinoJson.h>
#include <DHT.h>
#include <Wire.h>

//time milis
unsigned long timer = 0;

// Inisialisasi port BLDC

int EL_Roda1=8;  //EL 
int Signal_Roda1=3;   // Signal - Hall sensor
int ZF_Roda1=10;  // ZF 
int VR_Roda1=11;    //VR 

int EL_Roda2=4;  //EL 
int Signal_Roda2=2;   // Signal - Hall sensor
int ZF_Roda2=5;  // ZF 
int VR_Roda2=6;    //VR'

// Relay PIN
int RELAY_PIN = 7;

// Inisialisasi port DHT11
DHT dht(13, DHT11);
float suhu = 0;

// Define value variable 
int pos1=0;
int steps1=0;
int speed1=0;
float velo1=0;

int pos2=0;
int steps2=0;
int speed2=0;
float velo2=0;

String direction1; 
String direction2;
String direct;
String mode;
String control;
String informasi;
String kode;

unsigned long startTime;
unsigned long endTime;
unsigned long responseTime;

//RPM 
float revolutions=0;
int rpm=0; // max value 32,767 16 bit
float revolutions2=0;
int rpm2=0; // max value 32,767 16 bit
long  startTime_rpm=0;
long  elapsedTime;

//Kecepatan linear
float V1=0;
float V2=0;
float ave_v;

//GPS (dummy)
float lat= -6.923987;
float longi = 107.773354;

// Ulson
//sensor depan
const int trig_1 = 30; 
const int echo_1 = 31;
//sensor kiri
const int trig_2 = 32;
const int echo_2 = 33;
//sensor kanan
const int trig_3 = 36;
const int echo_3 = 37;

int thres_depan = 0;
int thres_samping = 0;


unsigned int durasi_1, durasi_2, durasi_3; // variabel untuk pengukuran waktu
int jarak_1, jarak_2, jarak_3; // variabel untuk pengukuran jarak
unsigned int durasi_11, durasi_12, durasi_21, durasi_31;
int jarak_11, jarak_12, jarak_21, jarak_31;


void interruptFunction() //interrupt service routine
{  
  revolutions++;
}

void interruptFunction2() //interrupt service routine
{  
  revolutions2++;
}

void setup() {
  Serial.begin(115200);
  Serial.println(F("Proses Kalibrasi, tunggu sebentar!"));

  //relay
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN,LOW);           //defalt - high = mati, low = hidup
  
  //Setup Roda 1
  pinMode(EL_Roda1, OUTPUT);              //stop/start - EL 
  pinMode(Signal_Roda1, INPUT_PULLUP);           //plus       - Signal  
  pinMode(ZF_Roda1, OUTPUT);              //direction  - ZF
  digitalWrite(EL_Roda1,LOW);             //default    - stop
  
  //Setup Roda 2
  pinMode(EL_Roda2, OUTPUT);              //stop/start - EL 
  pinMode(Signal_Roda2, INPUT_PULLUP);           //plus       - Signal  
  pinMode(ZF_Roda2, OUTPUT);              //direction  - ZF
  digitalWrite(EL_Roda2,LOW);             //default    - stop

  //Hall sensor detection - Count steps
  attachInterrupt(digitalPinToInterrupt(Signal_Roda1),interruptFunction,RISING);
  attachInterrupt(digitalPinToInterrupt(Signal_Roda2),interruptFunction2,RISING);

  //sensor suhu - temp&hum
  dht.begin();

  // Pin mode for Input: SensorDepan
  pinMode(trig_1, OUTPUT);       // set pin trig menjadi OUTPUT
  pinMode(echo_1, INPUT);           // set pin echo menjadi INPUT
  // Pin mode for Input: SensorKiri
  pinMode(trig_2, OUTPUT);       // set pin trig menjadi OUTPUT
  pinMode(echo_2, INPUT); 
  // Pin mode for Input: SensorKanan
  pinMode(trig_3, OUTPUT);       // set pin trig menjadi OUTPUT
  pinMode(echo_3, INPUT); 

  //Respon milis
  startTime = 0;
  endTime = 0;
  responseTime = 0;

  Wire.begin();
  kalibrasiroda();
  Serial.print("Selesai!\n");
  //Serial.print("--------------------------\n");
  //Serial.println("Urutan serial monitor : yaw, speed1, speed2, suhu, baterai, lat, long, ulson");
}

void kalibrasiroda(){
  Forward(0,0);
  delay(1000);
  Stop();
}

void infor(){
  if(informasi =="temp")
  {
    float suhu = dht.readTemperature();
    //float suhu = 30;
    Serial.print(suhu);
    Serial.print(",");
  }

  if (informasi == "hum")
  {
    float kelembapan = dht.readHumidity();
    //float kelembapan = 20;
    Serial.print(kelembapan);
    Serial.print(",");
  }

  if (informasi == "infospeed")
  {
    //float ave_v = random(600, 1201) / 1000.0;
    Serial.print(ave_v);
    Serial.print(",");
  }
  
  // Data dummy
  if (informasi == "baterai")
  {
    float baterai = random(50, 100); //data dummy
    Serial.print(baterai);
    Serial.print(",");
  }

  if (informasi == "gps")
  {
    float lat= -6.923987;
    float longi = 107.773354;
    Serial.print(lat,6);
    Serial.print(",");
    Serial.print(longi,6);
    Serial.print(",");
  }
}
 

//kontrol robot tanpa step
void kontrol(){
  // {"control":"lambat"}
  // {"control":"sedang"}
  // {"control":"cepat"}  
  // {"control":"maju"}
  // {"control":"maju_ges"}
  // {"control":"mundur"}
  // {"control":"stop"}
  // {"control":"hidup"}
  // {"control":"mati"}
  // {"control":"kiri"}
  // {"control":"kanan"}
  // {"control":"putar_balik"}
  // {"control":"speed"}

  // change speed serial monitor arduino, tapi harus gerak dulu
  // {"control":"speed_turn","velo1":"2.0","velo2":"2.0"}

  //Tanpa beban
  // 0.75 = normal, diatas 1 udah cepet

  if(control =="maju")
  {
    input_v(0.6,0.6);
    Forward(speed1,speed2);
    Serial.print("Robot Maju");
    Serial.print(",");
  }

  if(control =="maju_ges")          //maju untuk gesture
  {
    input_v(0.5,0.5);
    Forward(speed1,speed2);
    Serial.print("Robot Maju");
    Serial.print(",");
  }

  if(control =="mundur")
  {
    input_v(0.5,0.5);
    Backward(speed1,speed2);
    Serial.print("Robot Mundur");
    Serial.print(",");
  }

   if(control =="stop")
  {
    input_v(0,0);
    Stop();
    Serial.print("Robot Berhenti");
    Serial.print(",");
  }
  
  if(control == "kanan")
  {
    input_v(0.6,0.6);
    right(speed1,speed2);
    Serial.print("Robot Belok Kanan");
    Serial.print(",");
  }

  if(control == "kiri")
  {
    input_v(0.6,0.6);
    left(speed1,speed2);
    Serial.print("Robot Belok Kiri");
    Serial.print(",");
  }

  if(control == "putar_balik")
  {
    input_v(0.6,0.6);
    Forward(speed1,speed2);
    Serial.print("Robot Putar balik");
    Serial.print(",");
  }
  
  
  if (control == "lambat")
  {
    input_v(0.5,0.5);
    analogWrite(VR_Roda1, speed1);
    analogWrite(VR_Roda2, speed2);
  }

  if (control == "sedang")
  {
    input_v(1.5,1.5);
    analogWrite(VR_Roda1, speed1);
    analogWrite(VR_Roda2, speed2);
  }

  if (control == "cepat")
  {
    input_v(0.8,0.8);
    analogWrite(VR_Roda1, speed1);
    analogWrite(VR_Roda2, speed2);
  }

  //{"control":"speed","velo1":"2.0","velo2":"2.0"}
  if (control == "speed")
  {
    //float receivedValue = Serial.parseFloat();
    //float receivedValue = 3.5;
    //input_v(receivedValue,receivedValue);
    input_v(velo1,velo2);
    Serial.print(velo1);
    Serial.print(",");
    Serial.print(speed1);
    Serial.print(",");
    Serial.print(speed2);
    Serial.print(",");
    analogWrite(VR_Roda1, speed1);
    analogWrite(VR_Roda2, speed2);
  }

  if (control == "speed_turn")
  {
    input_v(velo1,velo2);
    Serial.print(velo1);
    Serial.print(",");
    Serial.print(velo2);
    Serial.print(",");
    analogWrite(VR_Roda1, speed1);
    analogWrite(VR_Roda2, speed2);
  }
}

void Forward(int speed1, int speed2){
  
  analogWrite(VR_Roda1, speed1);
  analogWrite(VR_Roda2, speed2);
  
  //change_speed_Forward(speed1, speed2);
  
  digitalWrite(EL_Roda1,LOW);
  digitalWrite(EL_Roda2,LOW);
  //delay(1000);
  digitalWrite(ZF_Roda1,HIGH);
  digitalWrite(ZF_Roda2,LOW);
  //delay(1000);
  digitalWrite(EL_Roda1,HIGH);
  digitalWrite(EL_Roda2,HIGH);

}

void Backward(int speed1, int speed2){
  analogWrite(VR_Roda1, speed1);
  analogWrite(VR_Roda2, speed2);
  digitalWrite(EL_Roda1,LOW);
  digitalWrite(EL_Roda2,LOW);
  //delay(1000);
  digitalWrite(ZF_Roda1,LOW);
  digitalWrite(ZF_Roda2,HIGH);
  //delay(1000);
  digitalWrite(EL_Roda1,HIGH);
  digitalWrite(EL_Roda2,HIGH);
}

void left(int speed1, int speed2){
  
  analogWrite(VR_Roda1, speed1);
  analogWrite(VR_Roda2, speed2);
  
  //change_speed_Forward(speed1, speed2);
  
  digitalWrite(EL_Roda1,LOW);
  digitalWrite(EL_Roda2,LOW);
  //delay(1000);
  digitalWrite(ZF_Roda1,HIGH);
  digitalWrite(ZF_Roda2,LOW);
  //delay(1000);
  digitalWrite(EL_Roda1,HIGH);
  digitalWrite(EL_Roda2,HIGH);

}

void right(int speed1, int speed2){
  
  analogWrite(VR_Roda1, speed1);
  analogWrite(VR_Roda2, speed2);
  
  //change_speed_Forward(speed1, speed2);
  
  digitalWrite(EL_Roda1,LOW);
  digitalWrite(EL_Roda2,LOW);
  //delay(1000);
  digitalWrite(ZF_Roda1,HIGH);
  digitalWrite(ZF_Roda2,LOW);
  //delay(1000);
  digitalWrite(EL_Roda1,HIGH);
  digitalWrite(EL_Roda2,HIGH);

}

void rotate(int speed1, int speed2){
  
  analogWrite(VR_Roda1, speed1);
  analogWrite(VR_Roda2, speed2);
  
  //change_speed_Forward(speed1, speed2);
  
  digitalWrite(EL_Roda1,LOW);
  digitalWrite(EL_Roda2,LOW);
  //delay(1000);
  digitalWrite(ZF_Roda1,HIGH);
  digitalWrite(ZF_Roda2,LOW);
  //delay(1000);
  digitalWrite(EL_Roda1,HIGH);
  digitalWrite(EL_Roda2,HIGH);
}

void Stop(){
  digitalWrite(EL_Roda1,LOW);
  digitalWrite(EL_Roda2,LOW);
}

void relay(){
  if (control =="hidup")
  {
    //Serial.println("SAVION");
    digitalWrite(RELAY_PIN, LOW); 
  }
  if (control == "mati")
  {
    //Serial.println("SAVIOFF");
    Stop();
    digitalWrite(RELAY_PIN, HIGH);
  }
}

void input_v(float velocity1, float velocity2){
  if (velocity1 >= 5.85 || velocity2 >= 5.85){
    velocity1 = 5.82;
    velocity2 = 5.82;
  }

  if (velocity1 >= 5.85){
    velocity1 = 5.82;
  }
  
  if (velocity2 >= 5.85){
    velocity2 = 5.82;
  }
  
  rpm = (60*velocity1*2.5)/(2*3.14*0.0762);
  rpm2 = (60*velocity2*2.5)/(2*3.14*0.0762); 
  speed1=map(rpm,0,735,0,255);
  speed2=map(rpm2,0,735,0,255);
  //Serial.print(speed1);
  //Serial.print(",");
  //Serial.print(speed2);
  //Serial.print(",");
}

void rpm_lur(){
  revolutions=0; rpm=0;
  revolutions2=0; rpm2=0;
  startTime_rpm=millis();         
  
  delay(1000);
  detachInterrupt(Signal_Roda1);
                
  
  //now let's see how many counts we've had from the hall effect sensor and calc the RPM
  elapsedTime=millis()-startTime_rpm;     //finds the time, should be very close to 1 sec
  
  if(revolutions>0 || revolutions2>0)
  {

    rpm = (revolutions * 60000) / (elapsedTime * 44.5);        //calculates rpm
    rpm2 = (revolutions2 * 60000) / (elapsedTime * 44.5);        //calculates rpm
    V1=(2 * 3.14 * 0.0762 * rpm/60);
    V2=(2 * 3.14 * 0.0762 * rpm2/60);
    ave_v=((V1+V2)/2);
  }
  //Serial.print("Revolutions 1:");
  //Serial.print(revolutions);
  //Serial.print(",");
  //Serial.print("Revolutions 2:");
  //Serial.print(revolutions2);
  //Serial.print(",");
  //Serial.print("Elapsed time");
  //Serial.print(elapsedTime);
  //Serial.println(",");
  /*Serial.print("RPM 1:");
  Serial.println(rpm);
  Serial.print("RPM 2:");
  Serial.println(rpm2);
  Serial.print("Velocity 1:");
  Serial.println(V1);
  Serial.print("Velocity 2:");
  Serial.println(V2);*/
}

void jarakSensor () {
  // Menghapus kondisi trigPin
  digitalWrite(trig_1, LOW);
  // Menyetel trigPin TINGGI (AKTIF)
  digitalWrite(trig_1, HIGH);
  digitalWrite(trig_1, LOW); 

  // menghitung jarak sensor Kiri
  durasi_1 = pulseIn(echo_1, HIGH); // menerima suara ultrasonic
  jarak_1 = (durasi_1 * 0.034) / 2;  // mengubah durasi menjadi jarak (cm)

    // Menghapus kondisi trigPin
  digitalWrite(trig_2, LOW);
  
  // Menyetel trigPin TINGGI (AKTIF)
  digitalWrite(trig_2, HIGH);
  
  digitalWrite(trig_2, LOW); 
  

  // menghitung jarak sensor Kiri
  durasi_2 = pulseIn(echo_2, HIGH); // menerima suara ultrasonic
  jarak_2 = (durasi_2 * 0.034) / 2;  // mengubah durasi menjadi jarak (cm)

  // Menghapus kondisi trigPin
  digitalWrite(trig_3, LOW);
  
  // Menyetel trigPin TINGGI (AKTIF)
  digitalWrite(trig_3, HIGH);

  digitalWrite(trig_3, LOW); 


  // menghitung jarak sensor Kiri
  durasi_3 = pulseIn(echo_3, HIGH); // menerima suara ultrasonic
  jarak_3 = (durasi_3 * 0.034) / 2;  // mengubah durasi menjadi jarak (cm)
}

void jarakSensor1() {
  // Menghapus kondisi trigPin
  digitalWrite(trig_1, LOW);
 
  // Menyetel trigPin TINGGI (AKTIF)
  digitalWrite(trig_1, HIGH);

  digitalWrite(trig_1, LOW); 
  

  // menghitung jarak sensor Kiri
  durasi_11 = pulseIn(echo_1, HIGH); // menerima suara ultrasonic
  jarak_11 = (durasi_11 * 0.034) / 2;  // mengubah durasi menjadi jarak (cm)

  // Menghapus kondisi trigPin
  digitalWrite(trig_1, LOW);

  // Menyetel trigPin TINGGI (AKTIF)
  digitalWrite(trig_1, HIGH);

  digitalWrite(trig_1, LOW); 


  // menghitung jarak sensor Kiri
  durasi_12 = pulseIn(echo_1, HIGH); // menerima suara ultrasonic
  jarak_12 = (durasi_12 * 0.034) / 2;  // mengubah durasi menjadi jarak (cm)
}

void stop_ulson(int depan, int samping){
  if (jarak_1 <= depan || jarak_2 <= samping || jarak_3 <= samping){
    Stop();
  }
}


void loop() {

  /*
  if((millis()-timer)>2000){ // print data every 10ms
    kode =' ';

    if (suhu >= 32){
      kode ='s';

    }
    if (jarak_1 <= 5){
      kode ='t';

    }
    if (daya <= 2){
      kode ='d';

    }
    Serial.print(kode);
    Serial.print(",");
    Serial.print(suhu);
    Serial.print(",");
    Serial.print(daya);
    Serial.print(",");
    Serial.print(kelembapan);
    Serial.print(",");
    Serial.print(lat,6);
    Serial.print(",");
    Serial.print(longi,6);
    Serial.print(",");
    Serial.print(jarak_1);
    Serial.println(",");
    Serial.flush();
    //infor();
    
    timer = millis(); 
  }*/ 
  rpm_lur();
  float suhu = dht.readTemperature();
  int relaystatus = digitalRead(RELAY_PIN);
  /*if (relaystatus == HIGH){
      Serial.println("SAVIOFF");     
  }
  if (relaystatus == LOW){
      Serial.println("SAVION");     
  }
  #delay(500);*/

  if (V1 < 1.2 || V2 < 1.2){
    int thres_depan = 135;
    int thres_samping = 80;
  }
  else if (V1 >= 1.2 || V2 >= 1.2){
    int thres_depan = 200;
    int thres_samping = 145;
  }

  //stop_ulson(thres_depan, thres_samping);
  
  //jarakSensor();
  //jarakSensor1();



  
  if (Serial.available()>0) 
  {
    startTime = millis();
    String command=Serial.readString();
    DynamicJsonBuffer jsonBuffer;
    JsonObject& root= jsonBuffer.parseObject(command);
    if (root.success()) {
      control = root["control"].asString(); 
      informasi = root["informasi"].asString(); 
      mode = root["mode"].asString();
      direction1 = root["direction1"].asString();
      direct = root["direct"].asString();
      direction2 = root["direction2"].asString();
      velo1 = atof(root["velo1"]);
      velo2 = atof(root["velo2"]);
      relay();
      if (relaystatus == LOW)
      {
        kontrol();
        infor();     
      }
      endTime = millis(); // Catat waktu akhir
      responseTime = endTime - startTime;
      Serial.println(responseTime);
     }
  }
  
  
}
