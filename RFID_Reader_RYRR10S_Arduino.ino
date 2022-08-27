#include <SoftwareSerial.h>

SoftwareSerial NFCserial(4, 5);

int piezoPin = 11;

uint8_t response_byte;
uint8_t received_buf_pos;
uint8_t received_data[256];

uint8_t data_len;
String received_id = "";
boolean received_complete;

void init_nfc();
void serial_receive();
void scan_tag();

uint8_t info_command[2] = {0x01, 0x00}; // IDN command, request short information about CR95HF
uint8_t protocol_command_iso14443a[4] =  {0x02, 0x02, 0x02, 0x00}; // protocol select command
uint8_t detect_command_iso14443a[4]   =  {0x04, 0x02, 0x26, 0x07}; // send receive commands

void setup() {
  Serial.begin(9600);
  pinMode(piezoPin, OUTPUT);
  delay(1000);
  init_nfc();
}

void loop() {
  scan_tag();
  delay(500);
}

void init_nfc() {
  NFCserial.begin(57600);
  delay(10);

  NFCserial.write(info_command, 2);
  delay(100);
  show_serial_data();
  Serial.println("");

  delay(1000);

  Serial.println("Sending protocol settings...");
  NFCserial.write(protocol_command_iso14443a, 4);
  delay(100);
  show_serial_data();
  Serial.println("");
}

void serial_receive() {
  uint8_t received_char;

  while (NFCserial.available() != 0) {
    received_char = char (NFCserial.read());

    //the first byte of the received message is the response
    if (received_buf_pos == 0) {
      response_byte = received_char;
    }

    //second byte of the received data is the data length
    else if (received_buf_pos == 1) {
      data_len = received_char;
    }

    else if (received_buf_pos == 2) {

    }

    else if (received_buf_pos == 3) {

    }

    else {
      received_data[received_buf_pos - 4] = received_char;
      received_id += received_char; //adding to a string
    }
    received_buf_pos++;

    if (received_buf_pos >= data_len) {
      received_complete = true;
    }
  }
}

void scan_tag() {
  received_buf_pos = 0;
  received_complete = false;

  NFCserial.write(detect_command_iso14443a, 4);
  delay(200);

  if (NFCserial.available()) serial_receive();
  else return;
  if (response_byte == 0x80) {   // error code means frame correctly received

    Serial.println("Tag detected...");
    Serial.print("ID in HEX: ");

    for (int pos = 7; pos >= 0; pos--) {
      Serial.print(received_data[pos], HEX);
    }

    Serial.println("");
    Serial.print("ID in DEC: ");
    Serial.println(received_id);
    delay(1000); //this is important
  }
  else {
    Serial.println("No tag detected.");
  }

  //comparing the authorize id
  if (received_id == "4000") {
    Serial.println("Tag Detected");
    tone(piezoPin, 1000, 500);
    received_id = "";
    delay(500);
  }
  received_id = "";
}

// this display the debug message
void show_serial_data()
{
  while (NFCserial.available() != 0)      /* If data is available on serial port */
    Serial.print(NFCserial.read(), HEX);  /* Print character received on to the serial monitor */
}
