#include <WiFi.h>
#include <string>
#include <ArduinoJson.h>
const char* ssid     = "ChinaNet-vpgE";
const char* password = "9byavc8y";
const int httpPort = 80;
const unsigned int esp8266RxBufferLength = 600;
char esp8266RxBuffer[esp8266RxBufferLength];

const char * hostdomain = "api.heclouds.com";     //不需要修改
const char * host = "http://api.heclouds.com/devices/509447574";     //不需要修改
char device_id[] = "509447574";    //修改为自己的设备ID
char API_KEY[] = "K9GNElQbbm5T8RBaBZd1ngFku9A=";    //修改为自己的API_KEY
char sensor_id1[] = "temp1";
//char sensor_id2[] = "HUMI";
/*
{
 "datastreams":[{
         "id":"sys_time",
         "datapoints":[{
                 "value":"21"
                      }]
               }]
}
*/
int ConstructJson(String *attrname,int *array,int arraysize,String &strout)
{
  StaticJsonBuffer<500> jsonBuffer;
  JsonObject & outObj = jsonBuffer.createObject();
  JsonArray& datastreams = outObj.createNestedArray("datastreams");

  for (int i = 0;i < arraysize;i++)
  {
    JsonObject & middle = datastreams.createNestedObject();
    middle["id"]=attrname[i];
    JsonArray& datapoints = middle.createNestedArray("datapoints");
    JsonObject & inner = datapoints.createNestedObject();
    inner["value"]=String(array[i], DEC);
    datapoints[0]=inner;
  }
  
  //outObj.printTo(Serial);
  outObj.printTo(strout);
  //Serial.println(jsonBuffer.size());
  int jsonbuffersize=jsonBuffer.size();
  Serial.println(jsonbuffersize);
  jsonBuffer.clear();
  return jsonbuffersize;
}

int ConstructJson2(String attrname,int value,String &strout)
{
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject & outObj = jsonBuffer.createObject();
  JsonArray& datastreams = outObj.createNestedArray("datastreams");


  JsonObject & middle = datastreams.createNestedObject();
  middle["id"]=attrname;
  JsonArray& datapoints = middle.createNestedArray("datapoints");
  JsonObject & inner = datapoints.createNestedObject();
  inner["value"]=String(value, DEC);
  datapoints[0]=inner;
  
  //outObj.printTo(Serial);
  outObj.printTo(strout);
  //Serial.println(jsonBuffer.size());
  int jsonbuffersize=jsonBuffer.size();
  Serial.println(jsonbuffersize);
  jsonBuffer.clear();
  return jsonbuffersize;
}

void setup() {
  Serial.begin(115200);
  delay(10);
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

/*
  void loop() {
  Serial.print("connecting to ");
  Serial.println(host);
  WiFiClient client;

  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }
  delay(10);
  //String recv = readLine();//将从串口接收的数据发送到服务器，readLine()方法可以自行设计
  //while (recv != "0")
  while(1)
  {
    if (recv.length() && recv != "0" && recv != "-1200"){//与我的项目相匹配的判断条件
      String data = (String)"{\"heartbeat\":\"" + 100+"\"}";
      int length = data.length();

      String postRequest =(String)("POST ") + "/ HTTP/1.1\r\n" +
          "Content-Type: application/json;charset=utf-8\r\n" +
          "Host: " + host + ":" + httpPort + "\r\n" +
          "Content-Length: " + length + "\r\n" +
          "Connection: Keep Alive\r\n\r\n" +
          data+"\r\n";
       Serial.println(postRequest);
       client.print(postRequest);
       delay(100);
       break;
  }
    recv = readLine();
  }
  client.stop();
  }
*/

void loop() {
  Serial.print("connecting to ");
  Serial.println(hostdomain);
  WiFiClient client;

  if (!client.connect(hostdomain, httpPort)) {
    Serial.println("connection failed");
    return;
  }
  delay(1000);
  //String recv = readLine();//将从串口接收的数据发送到服务器，readLine()方法可以自行设计
  //while (recv != "0")
  while (1)
  {
    delay(1000);
    String data = (String)"{\"heartbeat\":\"" + 100 + "\"}";
    int length = data.length();
    
    String requestMethod = (String)("POST ") + (String)("/devices/") + (String)(device_id) + (String)("/datapoints HTTP/1.1\r\n");
    String requestAPIkey = (String)("api-key:") + (String)(API_KEY) + (String)("\r\n");
    String requestHost = (String)("Host:") + (String)hostdomain + (String)("\r\n");
    //"{\"sensor\":\"gps\",\"time\":1351824120,\"data\":[48.756080,2.302038]}";
    String ss = String(random(10,99), DEC);
    String postbody = (String)("{\"datastreams\":[{\"id\":\"pm25\",\"datapoints\":[{\"value\":");
    postbody += ss+(String)("}]}]}");


    String attrname[]={"PM10","pm1_0","pm25"};
    String out;
    int value[] ={12,30,60};
    //int bodysize =ConstructJson(attrname,value,sizeof(value)/sizeof(value[0]),out);
    for (int i =0;i<sizeof(value)/sizeof(value[0]);i++)
    {
        String out="";
        String postRequest = "";
        int bodysize =ConstructJson2(attrname[i],value[i],out);
        Serial.println("bodysize=");
        Serial.println(bodysize);
        String requestlength = (String)("Content-Length:") + String(bodysize,DEC) + (String)("\r\n\r\n");
        //postRequest += requestMethod + requestAPIkey + requestHost + requestlength + postbody;
        postRequest += requestMethod + requestAPIkey + requestHost + requestlength + out;
        Serial.println(postRequest);
        client.print(postRequest);
        delay(8000);
        
        String line = client.readStringUntil('\n');
        while(line.length() != 0)
        {
            Serial.println(line);
            line = client.readStringUntil('\n');
        }
        Serial.println(line);
        delay(2000);
    }
  }
  client.stop();
}


String readLine()
{
  String str;
  while (Serial.available())
  {
    char temp = Serial.read();
    if (temp != '\n') {
      str += char(temp);
      delay(2);
    }//delete temp;
    else break;//delete temp;
  }
  return str;
}
