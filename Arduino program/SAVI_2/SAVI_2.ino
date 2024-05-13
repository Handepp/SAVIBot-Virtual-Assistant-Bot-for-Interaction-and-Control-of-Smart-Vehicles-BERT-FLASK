#include <Wire.h>
#include <MPU6050_light.h>
#include <Servo.h>
#include <ArduinoJson.h>

MPU6050 mpu(Wire);
unsigned long timer = 0;
int Data_z = 0;
int z = 0;
int now = 0;

int resetPin = 12;
int servoposition = 81;
int angle; // Sudut servo yang akan dikontrol
int angle_baru;
int targetangle=0;

Servo myservo;

bool isForward = false;
bool isBackward = false;
bool isStop = false;
bool isRight = false;
bool isLeft = false;
bool isUTurn = false;

String mode = "";
String control;

int servoPin = 9; // Pin yang digunakan untuk menghubungkan servo

const int trig_1 = 30;
const int echo_1 = 31;
const int trig_2 = 32;
const int echo_2 = 33;
const int trig_3 = 34;
const int echo_3 = 35;

unsigned int durasi_1, durasi_2, durasi_3; // variabel untuk pengukuran waktu
int jarak_1, jarak_2, jarak_3;// variabel untuk pengukuran 
unsigned int durasi_11, durasi_12, durasi_21, durasi_31;
int jarak_11, jarak_12, jarak_21, jarak_31;

void setup() {
  Serial.begin(115200);
  pinMode(trig_1, OUTPUT);       // set pin trig menjadi OUTPUT
  pinMode(echo_1, INPUT);           // set pin echo menjadi INPUT

  // Pin mode for Input: SensorDepan
  pinMode(trig_2, OUTPUT);
  pinMode(echo_2, INPUT);
  // Pin mode for Input: SensorKanan
  pinMode(trig_3, OUTPUT);
  pinMode(echo_3, INPUT);
  Wire.begin();
  Wire.setClock(200000); // 400kHz I2C clock. Comment this line if having compilation difficulties
  Wire.setWireTimeout(3000, true); //timeout value in uSec
  
  //Serial.print(F("MPU6050 status: "));
  //Serial.println(status);
  //while(status!=0){ } // Stop jika MPU tidak terkoneksi
  
  Serial.println(F("Proses Kalibrasi, tunggu sebentar!"));
  mpu.begin();
  delay(1000);
  // mpu.upsideDownMounting = true; // uncomment this line if the MPU6050 is mounted upside-down
  mpu.calcOffsets(); // gyro and accelero
  myservo.attach(servoPin);
  myservo.write(81); // Set posisi awal servo ke tengah (0 derajat)
  Serial.print("Selesai!\n");
}

// {"control":"kiri"}
// {"control":"maju"}

void(* resetFunc) (void) = 0; //declare reset function @ address 0

void bot(){
  if(control =="maju")
  {
    isForward = true;
    isBackward = false;
    isStop = false;
    isRight = false;
    isLeft = false;
    isUTurn = false;

    now = Data_z;
    mode = "lurus";
  }
  if(control =="mundur")
  {
    isBackward = true;
    isForward = false;
    isStop = false;
    isRight = false;
    isLeft = false;
    isUTurn = false;

    now = Data_z;
  }
  
  if(control =="kiri")
  {
    isBackward = false;
    isForward = false;
    isStop = false;
    isRight = false;
    isLeft = true;
    isUTurn = false;
    
    servoposition = 36;
    myservo.write(servoposition);
  }

  if(control =="kanan")
  {
    isBackward = false;
    isForward = false;
    isStop = false;
    isRight = true;
    isLeft = false;
    isUTurn = false;
    
    servoposition = 126;
    myservo.write(servoposition);
  }

  if(control =="stop")
  {
    isBackward = false;
    isForward = false;
    isStop = true;
    isRight = false;
    isLeft = false;
    isUTurn = false;
  }

  if(control =="putar_balik")
  {
    isBackward = false;
    isForward = false;
    isStop = false;
    isRight = false;
    isLeft = false;
    isUTurn = true;
    
    servoposition = 126;
    myservo.write(servoposition);
  }

  if(control =="reset")
  {
    resetFunc();  //call reset
  }
}



void Lurus_stabil(int Data_z, int Data_z_now){
  /*Serial.print(",");
  Serial.print("Data_z_now:");
  Serial.print(Data_z_now);*/
  if (Data_z % Data_z_now == 0) {
        angle_baru = 81;
    }
    if (Data_z > Data_z_now) {
        angle = map(Data_z, Data_z_now, (Data_z_now+90), 81, 149);
        z=(Data_z-Data_z_now)*(-4);
        angle_baru = angle-z;
        if (angle_baru >= 142){
            angle_baru = 142;
          }
        }
  // Jika yaw positif, kendalikan servo dari 90-180 derajat
    if (Data_z < Data_z_now) {
        angle = map(Data_z,(Data_z_now-90), Data_z_now, 13, 81);
        z=(Data_z-Data_z_now)*(4);
        angle_baru = angle+z;
        if (angle_baru <= 20){
            angle_baru = 20;
          }
        }
    // Kontrol servo
    myservo.write(angle_baru);
}

void Backward(int Data_z, int Data_z_now){
//
  /*Serial.print(",");
  Serial.print("Data_z_now:");
  Serial.print(Data_z_now);*/
  if (Data_z % Data_z_now == 0) {
        angle_baru = 81;
    }
    if (Data_z > Data_z_now) {
        angle = map(Data_z, Data_z_now, (Data_z_now+90), 81, 13);
        /*Serial.print(",");
        Serial.print("angel: ");
        Serial.print(angle);*/
        z=(Data_z-Data_z_now)*(-5);
        angle_baru = angle+z;
        if (angle_baru <= 20){
            angle_baru = 20;
          }
        }
  // Jika yaw positif, kendalikan servo dari 90-180 derajat
    if (Data_z < Data_z_now) {
        angle = map(Data_z,(Data_z_now-90), Data_z_now, 149, 81);
        /*Serial.print(",");
        Serial.print("angel: ");
        Serial.print(angle);*/
        z=(Data_z-Data_z_now)*(4);
        angle_baru = angle-z;
        if (angle_baru >= 142){
            angle_baru = 142;
          }
        }

    // Kontrol servo
    myservo.write(angle_baru);
}


void jaraksensor () {
  // Menghapus kondisi trigPin
  digitalWrite(trig_1, LOW);
  delayMicroseconds(10);
  // Menyetel trigPin TINGGI (AKTIF) selama 10 mikrodetik
  digitalWrite(trig_1, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig_1, LOW); 
  delayMicroseconds(10); 

  // menghitung jarak sensor Kiri
  durasi_1 = pulseIn(echo_1, HIGH); // menerima suara ultrasonic
  jarak_1 = (durasi_1 * 0.034) / 2;  // mengubah durasi menjadi jarak (cm)

  // Menghapus kondisi trigPin
  digitalWrite(trig_2, LOW);
  delayMicroseconds(10);
  // Menyetel trigPin TINGGI (AKTIF) selama 10 mikrodetik
  digitalWrite(trig_2, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig_2, LOW); 
  delayMicroseconds(10); 

  // menghitung jarak sensor Depan
  durasi_2 = pulseIn(echo_2, HIGH); // menerima suara ultrasonic
  jarak_2 = (durasi_2 * 0.034) / 2;  // mengubah durasi menjadi jarak (cm)

  // Menghapus kondisi trigPin
  digitalWrite(trig_3, LOW);
  delayMicroseconds(10);
  // Menyetel trigPin TINGGI (AKTIF) selama 10 mikrodetik
  digitalWrite(trig_3, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig_3, LOW); 
  delayMicroseconds(10); 

  // menghitung jarak sensor Kanan`
  durasi_3 = pulseIn(echo_3, HIGH); // menerima suara ultrasonic
  jarak_3 = (durasi_3 * 0.034) / 2;  // mengubah durasi menjadi jarak (cm)
}

void jarakSensor1() {
  // Menghapus kondisi trigPin
  digitalWrite(trig_1, LOW);
  delayMicroseconds(10);
  // Menyetel trigPin TINGGI (AKTIF)
  digitalWrite(trig_1, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig_1, LOW); 
  delayMicroseconds(10);

  // menghitung jarak sensor Kiri
  durasi_11 = pulseIn(echo_1, HIGH); // menerima suara ultrasonic
  jarak_11 = (durasi_11 * 0.034) / 2;  // mengubah durasi menjadi jarak (cm)
}

void stop_ulson(int depan, int samping){
  if (jarak_1 <= depan || jarak_2 <= samping || jarak_3 <= samping){
    isBackward = false;
    isForward = false;
    isStop = true;
    isRight = false;
    isLeft = false;
    isUTurn = false;
  }
}

void loop() {
  
  if((millis()-timer)>10){ // print data every 10ms
      mpu.update();
      Data_z = mpu.getAngleZ();
      /*Serial.print("Yaw : ");
      Serial.print(Data_z); // Mendapatkan sumbu Z
      Serial.print(",");  
      Serial.print("Kondisi LEFT : ");
      Serial.print(isLeft); // Kondisi left  
      Serial.print(",");  
      Serial.print("Kondisi Lurus : ");
      Serial.println(isForward); // Kondisi left*/
      //timer = millis();  
  }
  if (Serial.available()>0) 
  {
    String command=Serial.readString();
    DynamicJsonBuffer jsonBuffer;
    JsonObject& root= jsonBuffer.parseObject(command);
    if (root.success()) {
      control = root["control"].asString(); 
      bot();      
     }
  }

  if (isForward){
    if (!isLeft && mode == "kiri"){
      float pembagi = 90;
      int pengali= round(Data_z / pembagi);
      Lurus_stabil(Data_z,pengali*90);
      //Serial.print("Servo: ");
      //Serial.print(angle_baru);
      //Serial.print(",");
    }

    if (!isRight && mode == "kanan"){
      float pembagi = 90;
      int pengali= round(abs(Data_z / pembagi));
      Lurus_stabil(Data_z,pengali*(-90));
      /*Serial.print("Servo: ");
      Serial.print(angle_baru);
      Serial.print(",");*/
    }
    if (!isUTurn && mode == "putarbalik"){
      float pembagi = 180;
      int pengali= round(abs(Data_z / pembagi));
      Lurus_stabil(Data_z,pengali*(-180));
      /*Serial.print("Servo: ");
      Serial.print(angle_baru);
      Serial.print(",");*/
    }
    
    else if (mode == "lurus"){
      Lurus_stabil(Data_z,now);
      /*Serial.print("Now: ");
      Serial.print(now);
      Serial.print(",");
      Serial.print("Servo :");
      Serial.print(angle_baru);
      Serial.print(",");*/
    }

  }

  if(isBackward){
    Backward(Data_z,now);
    /*Serial.print("Now: ");
    Serial.print(now);
    Serial.print(",");
    Serial.print("Servo :");
    Serial.print(angle_baru);
    Serial.print(",");*/
  }
  if (isLeft){
    Data_z = (Data_z + 3);
    //int pengali= (Data_z / 90) + 1;
    mode = "kiri";
    if (Data_z % 90 == 0){
      //Lurus_stabil(Data_z,pengali*90);
      //Serial.print(angle_baru);
      isForward = true;
      isLeft = false;
  }
  }

  if (isRight){
    Data_z = (Data_z - 3);
    //int pengali= (Data_z / 90) + 1;
    mode = "kanan";
    if (Data_z % 90 == 0){
      //Lurus_stabil(Data_z,pengali*(-90));
      //Serial.print(angle_baru);
      isForward = true;
      isRight = false;
  }
  }
  
  if(isStop){
    servoposition = 81;
    myservo.write(servoposition);
  }
  if(isUTurn){
    Data_z = (Data_z - 3);
    //int pengali= (Data_z / 90) + 1;
    mode = "putarbalik";
    if (Data_z % 180 == 0){
      //Lurus_stabil(Data_z,pengali*(-90));
      //Serial.print(angle_baru);
      isForward = true;
      isUTurn = false;
    }
  }

  //stop_ulson(135,80);
  
 }
