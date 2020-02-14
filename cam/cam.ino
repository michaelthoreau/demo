
#define CHUNK_SIZE 500
#define SERIAL_BUFFER_LEN 1000
char serialBuffer[SERIAL_BUFFER_LEN];

// define our commands
const char RESET_COMMAND[] = {0x56, 0x00, 0x26, 0x00};
char DEFAULT_COMPRESSION_COMMAND[] = {0x56, 0x00, 0x31, 0x05, 0x01, 0x01, 0x12, 0x04, 0x36};
const char RES_640x480_COMMAND[] = {0x56, 0x00, 0x31, 0x05, 0x04, 0x01, 0x00, 0x19, 0x00};
const char RES_320x240_COMMAND[] = {0x56, 0x00, 0x31, 0x05, 0x04, 0x01, 0x00, 0x19, 0x11};
const char CAPTURE_IMAGE_COMMAND[] = {0x56, 0x00, 0x36, 0x01, 0x00};
const char READ_DATALEN_COMMAND[] = {0x56, 0x00, 0x34, 0x01, 0x00};

// 56 00 32 0C 00 0D 00 00 XX XX 00 00 YY YY 00 0A
char READ_DATA_COMMAND[] = {0x56, 0x00, 0x32, 0x0C, 0x00, 0x0A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x10, 0x00};

const char STOP_CAPTURE_COMMAND[] = {0x56, 0x00, 0x36, 0x01, 0x03};

const char SET_115200_BAUD_COMMAND[] = {0x56, 0x00, 0x24, 0x03, 0x01, 0x0D, 0xA6};

void printHexBuf(char* buf, size_t numBytes) {
  for (uint i = 0; i < numBytes; i++) {
    Serial.print(buf[i], HEX);
    Serial.print(" ");
  }
  Serial.print("\n\r");
}

void initCamera() {

  // write a reset to the camera
  Serial1.write(RESET_COMMAND, sizeof(RESET_COMMAND));
  memset(serialBuffer, 0x00, SERIAL_BUFFER_LEN);
  size_t numBytes = Serial1.readBytes(serialBuffer, SERIAL_BUFFER_LEN);
  Serial.println("Sending reset");
  printHexBuf(serialBuffer, numBytes);
      
  //set resolution
  Serial1.write(RES_320x240_COMMAND, sizeof(RES_320x240_COMMAND));
  memset(serialBuffer, 0x00, SERIAL_BUFFER_LEN);
  numBytes = Serial1.readBytes(serialBuffer, SERIAL_BUFFER_LEN);
  Serial.println("Setting resolution");
  printHexBuf(serialBuffer, numBytes);

  // set compression level
  DEFAULT_COMPRESSION_COMMAND[8] = 150;
  Serial1.write(DEFAULT_COMPRESSION_COMMAND, sizeof(DEFAULT_COMPRESSION_COMMAND));
  memset(serialBuffer, 0x00, SERIAL_BUFFER_LEN);
  numBytes = Serial1.readBytes(serialBuffer, SERIAL_BUFFER_LEN);
  Serial.println("Setting compression level");
  printHexBuf(serialBuffer, numBytes);
   


}


void setup()  
{
  // Open serial communications and wait for port to open:
  Serial.begin(115200);
  
  Serial1.begin(38400);
  Serial1.setTimeout(200);

  delay(2000);
  initCamera();
  
  // set baud rate
  Serial1.write(SET_115200_BAUD_COMMAND, sizeof(SET_115200_BAUD_COMMAND));
  memset(serialBuffer, 0x00, SERIAL_BUFFER_LEN);
  size_t numBytes = Serial1.readBytes(serialBuffer, SERIAL_BUFFER_LEN);
  Serial.println("Setting baud");
  printHexBuf(serialBuffer, numBytes);
  
  Serial1.begin(115200);

  

}




void loop() // run over and over
{
  // capture image
  Serial1.write(CAPTURE_IMAGE_COMMAND, sizeof(CAPTURE_IMAGE_COMMAND));
  memset(serialBuffer, 0x00, SERIAL_BUFFER_LEN);
  size_t numBytes = Serial1.readBytes(serialBuffer, SERIAL_BUFFER_LEN);
  Serial.println("Capture image command sent");
  printHexBuf(serialBuffer, numBytes);


  // get length of data
  Serial1.write(READ_DATALEN_COMMAND, sizeof(READ_DATALEN_COMMAND));
  memset(serialBuffer, 0x00, SERIAL_BUFFER_LEN);
  numBytes = Serial1.readBytes(serialBuffer, SERIAL_BUFFER_LEN);
  Serial.println("reading length of data");
  printHexBuf(serialBuffer, numBytes);

  char high_len = serialBuffer[7];
  char low_len = serialBuffer[8];

  
  uint16_t dataLen = (high_len << 8) + low_len;


  // read data in chunks
  Serial.print("bytes to read: ");
  Serial.print(dataLen);
  Serial.print("\n\r");

  // flush buffer
  Serial1.readBytes(serialBuffer, SERIAL_BUFFER_LEN);
  memset(serialBuffer, 0x00, SERIAL_BUFFER_LEN);

  
  size_t actual_chunk_size = CHUNK_SIZE;
  for (uint16_t i = 0; i < dataLen; i += CHUNK_SIZE) {

    // set start address
    READ_DATA_COMMAND[8] = (char)(i >> 8);
    READ_DATA_COMMAND[9] = (char)(i & 0xFF);

    // set read amount
    if (dataLen - i < CHUNK_SIZE){
      actual_chunk_size = dataLen - i;
    }
    READ_DATA_COMMAND[12] = (char)(actual_chunk_size >> 8);
    READ_DATA_COMMAND[13] = (char)(actual_chunk_size & 0xFF);

    // flush buffer
//    Serial1.readBytes(serialBuffer, SERIAL_BUFFER_LEN);
//    memset(serialBuffer, 0x00, SERIAL_BUFFER_LEN);
    
    Serial1.write(READ_DATA_COMMAND, sizeof(READ_DATA_COMMAND));

    // read and discard header/footer (5 bytes)
    Serial1.readBytes(serialBuffer, 5);
    memset(serialBuffer, 0x00, 5);
  
    numBytes = Serial1.readBytes(serialBuffer, actual_chunk_size);
    Serial.print("read image data: ");
    Serial.print(i);
    Serial.print(" ");
    Serial.print(actual_chunk_size);
    Serial.print(" ");
    Serial.print(numBytes);
    Serial.print("\n\r");
    
//    printHexBuf(serialBuffer, numBytes);
    
    // read and discard header/footer (5 bytes)
    Serial1.readBytes(serialBuffer, 5);
    memset(serialBuffer, 0x00, 5);
  }
  
  // send stop capture command
  Serial1.write(STOP_CAPTURE_COMMAND, sizeof(STOP_CAPTURE_COMMAND));
  memset(serialBuffer, 0x00, SERIAL_BUFFER_LEN);
  numBytes = Serial1.readBytes(serialBuffer, SERIAL_BUFFER_LEN);
  Serial.println("stop capture");
  printHexBuf(serialBuffer, numBytes);






  
}
