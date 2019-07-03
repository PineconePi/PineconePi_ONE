long randNumber;
void setup() {
    Serial.begin(9600);
randomSeed(analogRead(0));
}

void loop() {
  // put your main code here, to run repeatedly:
helloICKEY();
Serial.println(" ");
Serial.println(" ");
delay(5000);
}
void helloICKEY(void)
{
  int sum1;
  float sum2;
  int a=12345,b=56789;
  float aa=12345.678,bb=56789.012;
  int n=1;
  int i;
  unsigned long  time_start;
  unsigned long  time1;

  randNumber = random(1,10);
  sum1=randNumber;
  time_start=micros();
  for(i=0;i<n;i++)
  {
    sum1=a+b;
   // sum1+=a;
    
  }
  time1=micros()-time_start;
  Serial.print("int add test 1m times: ");
  Serial.print(time1);
  Serial.println(" us");
    Serial.println(sum1);

  randNumber = random(100,200);
  sum2=(float)randNumber/1000.0;
  time_start=micros();
  for(i=0;i<n;i++)
  {
    sum2=aa+bb;
    //sum2+=aa;
  }
  time1=micros()-time_start;
  Serial.print("float add test 1m times: ");
  Serial.print(time1);
  Serial.println(" us");
Serial.println(sum2);

   randNumber = random(11,20);
  sum1=randNumber;
  time_start=micros();
  for(i=0;i<n;i++)
  {
    sum1=a*b;
  }
  time1=micros()-time_start;
  Serial.print("int mul test 1m times: ");
  Serial.print(time1);
  Serial.println(" us");
Serial.println(sum1);

  randNumber = random(200,300);
  sum2=(float)randNumber/1000.0;
  time_start=micros();
  for(i=0;i<n;i++)
  {
    sum2=aa*bb;
  }
  time1=micros()-time_start;
  Serial.print("float mul test 1m times: ");
  Serial.print(time1);
  Serial.println(" us");
Serial.println(sum2);
  
  randNumber = random(300,400);
  sum2=(float)randNumber/1000.0;
  time_start=micros();
  for(i=0;i<n;i++)
  {
    sum2=aa/bb;
  }
  time1=micros()-time_start;
  Serial.print("float div test 1m times: ");
  Serial.print(time1);
  Serial.println(" us");
  Serial.println(sum2);
  
  sum1++;
  sum2++;
}


