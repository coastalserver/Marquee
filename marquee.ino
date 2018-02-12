 
#include "U8glib.h"
  
U8GLIB_SSD1306_128X64 u8g(12, 11, 10, 9, 8); // SW SPI Com: SCK (d0)= 12, MOSI (d1) = 11, CS = 10, A0 (dc)= 9, rst = 8

// setup input buffer
#define LINE_MAX 17
uint8_t line_buf[LINE_MAX] = "Marquee";
uint8_t line_pos = 0;

// setup a text screen to support scrolling
#define ROW_MAX 1

uint8_t screen[ROW_MAX][LINE_MAX];
uint8_t rows, cols;

// line height, which matches the selected font (5x7)
#define LINE_PIXEL_HEIGHT 14

// clear entire screen, called during setup
void clear_screen(void) {
  uint8_t i, j;
  for( i = 0; i < ROW_MAX; i++ )
    for( j = 0; j < LINE_MAX; j++ )
      screen[i][j] = 0;  
}

// append a line to the screen, scroll up
void add_line_to_screen(void) {
  uint8_t i, j;
  for( j = 0; j < LINE_MAX; j++ )
    screen[rows-1][j] = line_buf[j];
}

// U8GLIB draw procedure: output the screen
void draw(void) {
  uint8_t i, y;
  // graphic commands to redraw the complete screen are placed here    
  y = 20;       // reference is the top left -1 position of the string
  y--;           // correct the -1 position of the drawStr 
  for( i = 0; i < rows; i++ )
  {
    u8g.drawStr( 0, y, (char *)(screen[i]));
    y += u8g.getFontLineSpacing();
  }

//   u8g.drawFrame(0,30,128,34);
 
}

void exec_line(void) {
  // echo line to the serial monitor
  //   virtual  serdata.println((const char *)line_buf);
  //  Serial.println((const char *)line_buf);
  // add the line to the screen
  add_line_to_screen();
  
  // U8GLIB picture loop
  u8g.firstPage();  
  do {
    draw();
  } while( u8g.nextPage() );
}

// clear current input buffer
void reset_line(void) { 
      line_pos = 0;
      line_buf[line_pos] = '\0';  
}

// add a single character to the input buffer 
void char_to_line(uint8_t c) {
      line_buf[line_pos] = c;
      line_pos++;
      line_buf[line_pos] = '\0';  
}

// check serial in and handle the character
void read_line(void) {
  if ( Serial.available() )
  {
    uint8_t c;
    c = Serial.read();

  if ( c == '\r' ) {
     exec_line();
     reset_line();
     }

    else {
      char_to_line(c);
    }
  }
}

// Arduino master setup
void setup(void) { 
 
  // set font for the console window

   u8g.setFont(u8g_font_gdr12);
    
  // set upper left position for the string draw procedure
  u8g.setFontPosTop();
  
  // calculate the number of rows for the display
  rows = u8g.getHeight() / u8g.getFontLineSpacing();
  if ( rows > ROW_MAX )
    rows = ROW_MAX; 
  
  // estimate the number of columns for the display

 cols = 16; 
  
  clear_screen();               // clear screen
  delay(1000);                  // do some delay
  Serial.begin(9600);        // init serial
  exec_line();                    // place the input buffer into the screen
  reset_line();                   // clear input buffer
}

// Arduino main loop
void loop(void) {
     
   read_line();
  
}

