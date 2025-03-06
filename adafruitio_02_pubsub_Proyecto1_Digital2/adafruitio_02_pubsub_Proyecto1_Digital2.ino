// Adafruit IO Publish & Subscribe Example
//
// Adafruit invests time and resources providing this open source code.
// Please support Adafruit and open source hardware by purchasing
// products from Adafruit!
//
// Written by Todd Treece for Adafruit Industries
// Copyright (c) 2016 Adafruit Industries
// Licensed under the MIT license.
//
// All text above must be included in any redistribution.

/************************** Configuration ***********************************/

// edit the config.h tab and enter your Adafruit IO credentials
// and any additional configuration needed for WiFi, cellular,
// or ethernet clients.
#include "config.h"
#include <stdlib.h>
#include <string>

//#include "driver/uart.h"  // Control avanzado de UART (necesario para interrupciones)
//#include "esp_intr_alloc.h"

#define RXD2 16  // Pin de recepción desde el Arduino
#define TXD2 17  // Pin de transmisión al Arduino

/*#define BUF_SIZE 1024  // Tamaño del buffer de recepción

volatile bool dataReady = false;
uart_isr_handle_t handle;  // Manejador de interrupción

void IRAM_ATTR uart_isr(void* arg) {
  uart_disable_rx_intr(UART_NUM_2);  // Desactivar interrupción para evitar llamadas repetidas
  dataReady = true;
}*/

/************************ Example Starts Here *******************************/

// this int will hold the current count for our sketch
char Pulse = 0x64;
char Temperature = 0x1B;
char Distance = 0x1E;
int DC = 1;
int Servo = 0;
int cycle;
bool new_data = true;
String motores = "00";


// Track time of last published messages and limit feed->save events to once
// every IO_LOOP_DELAY milliseconds.
//
// Because this sketch is publishing AND subscribing, we can't use a long
// delay() function call in the main loop since that would prevent io.run()
// from being called often enough to receive all incoming messages.
//
// Instead, we can use the millis() function to get the current time in
// milliseconds and avoid publishing until IO_LOOP_DELAY milliseconds have
// passed.
#define IO_LOOP_DELAY 3000
unsigned long lastUpdate = 0;

// set up the 'counter' feed
AdafruitIO_Feed *feedPulse = io.feed("pulse");
AdafruitIO_Feed *feedDistance = io.feed("distance");
AdafruitIO_Feed *feedTemperature = io.feed("temperature");
AdafruitIO_Feed *feedServo = io.feed("servo");
AdafruitIO_Feed *feedDC = io.feed("dc");
AdafruitIO_Feed *feedControlServo = io.feed("controlservo");
AdafruitIO_Feed *feedControlDC = io.feed("controldc");

void setup() {

  
  
  // Configurar la interrupción de UART
  /*uart_config_t uart_config = {
    .baud_rate = 9600,
    .data_bits = UART_DATA_8_BITS,
    .parity = UART_PARITY_DISABLE,
    .stop_bits = UART_STOP_BITS_1,
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    .source_clk = UART_SCLK_APB,
  };

  uart_param_config(UART_NUM_2, &uart_config);
  uart_driver_install(UART_NUM_2, BUF_SIZE, 0, 0, NULL, 0);

  // Registrar la interrupción de UART
  esp_intr_alloc(ETS_UART2_INTR_SOURCE, ESP_INTR_FLAG_IRAM, uart_isr, NULL, &handle);
  uart_enable_rx_intr(UART_NUM_2);  // Habilita la interrupción por recepción*/

  // start the serial connection
  Serial.begin(115200);
  delay(1000);  // Esperar para evitar problemas de inicialización
  Serial.println("ESP32 Iniciado");


  // Configurar UART2
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);

  // wait for serial monitor to open
  //while(! Serial);

  Serial.print("Connecting to Adafruit IO");

  // connect to io.adafruit.com
  io.connect();

  // set up a message handler for the count feed.
  // the handleMessage function (defined below)
  // will be called whenever a message is
  // received from adafruit io.
  //counter->onMessage(handleMessage);
  feedControlServo->onMessage(handleMessageServo);
  feedControlDC->onMessage(handleMessageDC);

  // wait for a connection
  while(io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  // we are connected
  Serial.println();
  Serial.println(io.statusText());
  feedPulse->get();
  feedDistance->get();
  feedTemperature->get();
  feedServo->get();
  feedDC->get();

}

void loop() {

  // io.run(); is required for all sketches.
  // it should always be present at the top of your loop
  // function. it keeps the client connected to
  // io.adafruit.com, and processes any incoming data.
  io.run();

  /*if (dataReady) {
    dataReady = false;  // Reseteamos la bandera de interrupción

    uint8_t data[BUF_SIZE];  // Buffer de recepción
    int len = uart_read_bytes(UART_NUM_2, data, BUF_SIZE, 100 / portTICK_RATE_MS);
    
    if (len > 0) {
        data[len] = '\0';  // Convertir a string
        Serial.println((char*)data);
        Pulse = data[0];
        Distance = data[1];
        Temperature = data[2];
    }
    uart_enable_rx_intr(UART_NUM_2);  // Reactivar la interrupción
  }*/

  
  if (Serial2.available()){
    char incomingByte = Serial2.read();
    if (incomingByte == '<'){
      String data = Serial2.readStringUntil('>');
      data.trim();
      if (data.length() == 5){
        Serial.println("Recibido: " + data);
        Distance = data[0];
        Temperature = data[1];
        Pulse = data[2];
        DC = (int)data[3];
        Servo = (int)data[4];
        new_data = true;
      }
    } else {
      Serial.println("No data");
    }
  }

  /*if (new_data){
    Serial.println("True");
  } else {
    Serial.println("False");
  }*/

  if ((millis() > (lastUpdate + IO_LOOP_DELAY))&&(new_data == true)) {
    // save count to the 'counter' feed on Adafruit IO
    
    if (cycle == 0){
      //Pulse = rand() % 100 + 1;
      Serial.print("sending to Pulse -> ");
      Serial.println((int)Pulse);
      feedPulse->save(Pulse);
    } else if (cycle == 1){
      //Distance = rand() % 100 + 1;
      Serial.print("sending to Distance -> ");
      Serial.println((int)Distance);
      feedDistance->save(Distance);
    } else if (cycle == 2){
      //Temperature = rand() % 100 + 1;
      Serial.print("sending to Temperature -> ");
      Serial.println((int)Temperature);
      feedTemperature->save(Temperature);
    } else if (cycle == 3){
      Serial.print("sending to Servo -> ");
      Serial.println(Servo);
      feedServo->save(Servo);
    } else if (cycle == 4){
      Serial.print("sending to DC -> ");
      Serial.println(DC);
      feedDC->save(DC);
      new_data = true;
    }
    // increment the count by 1
    if (cycle <= 3){
      cycle++;
    } else {
      cycle = 0;
    }
    // after publishing, store the current time
    lastUpdate = millis();
    
  }
  Serial2.println(motores);
}

void handleMessageServo(AdafruitIO_Data *data) {
  //Serial.print("Received from Servo <-");
  //Serial.println(data->value());
  int valor = (int)data->value();
  if ((valor)==1){
    motores[1] = '1';
  } else {
    motores[1] = '0';
  }
}

void handleMessageDC(AdafruitIO_Data *data) {
  //Serial.print("Received from DC <-");
  //Serial.println(data->value());
  int valor = (int)data->value();
  if ((valor)==1){
    motores[0] = '1';
  } else {
    motores[0] = '0';
  }
}
