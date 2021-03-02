#include "U8glib.h"  // Include U8glib library


// Create display and set pins:
U8GLIB_PCD8544 u8g(13, 11, 10, 9, 8);   // (CLK=13, DIN=11, CE=10, DC=9, RST=8)
const int screen_width = 75;
const int screen_height = 45;


// Icons
const uint8_t heart_bitmap[] U8G_PROGMEM = {
  0x00, 0x24, 0x7e, 0x7e, 0x3c, 0x18, 0x00, 0x00
};


//Joystick input
int VRx = A0;
int VRy = A1;
int SW = 2;

int x_pos = 0;
int y_pos = 0;
int SW_state = 0;
int map_x = 0;
int map_y = 0;


// Snake properties
const int box_width = 5;
const int box_height = 5; 
int snake_x = 0;
int snake_y = 0;
int x_dir = 1;
int y_dir = 0;
const int max_snake_length = screen_width/box_width * screen_height/box_height;
int snake_positions[max_snake_length][2] = {{snake_x, snake_y}};
int snake_length = 2;
int lives = 3;


// Apple properties
int apple_x;
int apple_y;


void setup(void){  // Start of setup
  Serial.begin(9600);

  
  pinMode(VRx, INPUT);
  pinMode(VRy, INPUT);
  pinMode(SW, INPUT_PULLUP);

  u8g.setFont(u8g_font_u8glib_4);

  spawn_snake();
  spawn_apple();
}


void loop(void){  // Start of loop
  long start = millis();

  if (lives == 0){
    lives = 3;
    snake_length = 2;
    spawn_snake();
    spawn_apple(); 
  }
  
  if (snake_x == apple_x && snake_y == apple_y){
    spawn_apple();
    snake_length++;
  }
  
  // picture loop
  u8g.firstPage();  
  do {
    for (int i=0; i<snake_length; i++){
      draw(snake_positions[i][0], snake_positions[i][1]);
    }
    for (int i=1; i<lives+1; i++){
      draw_heart(76, 2+8*i);
    }
    draw(apple_x, apple_y);
  } while( u8g.nextPage() );
  read_joystick();

  move_pos();
  check_colission();
  move_tail(snake_x, snake_y);

  long finish = millis();
  int delay_time = max(200-(finish-start), 0);
  delay(delay_time);
}

void read_joystick(){
  SW_state = digitalRead(SW);
  x_pos = analogRead(VRx);
  y_pos = analogRead(VRy);
  SW_state = digitalRead(SW);
  map_x = map(x_pos, 0, 1023, -512, 512) + 20;
  map_y = map(y_pos, 0, 1023, -512, 512) + 20;
}

void move_pos(void){
  int input_dir_x = map_x/490;
  int input_dir_y = map_y/490;
  if (input_dir_x != 0){
    if (!(input_dir_x < 0 && x_dir > 0) && !(input_dir_x > 0 && x_dir < 0)){
      x_dir = input_dir_x;
      y_dir = 0; 
    }
  }else if (input_dir_y != 0){
    if (!(input_dir_y < 0 && y_dir > 0) && !(input_dir_y > 0 && y_dir < 0)){
      y_dir = input_dir_y;
      x_dir = 0;
    }
  }

  if (snake_x+box_width >= screen_width && x_dir == 1){
    snake_x = -box_width;
  }else if(snake_x <= 0 && x_dir == -1){
    snake_x = screen_width;
  }else if(snake_y+box_height >= screen_height && y_dir == 1){
    snake_y = -box_height;
  }else if (snake_y <= 0 && y_dir == -1){
    snake_y = screen_height;
  }
  
  snake_x += x_dir*box_width;
//  Serial.println(snake_x);
  snake_y += y_dir*box_height;
//  Serial.println(snake_y);
}


void move_tail(int x, int y){
  snake_positions[0][0] = x;
  snake_positions[0][1] = y;
  int temp1[2] = {snake_positions[0][0], snake_positions[0][1]};
  int temp2[2];
  for (int i = 1; i < snake_length; i++){
    temp2[0] = snake_positions[i][0];
    temp2[1] = snake_positions[i][1];
    snake_positions[i][0] = temp1[0];
    snake_positions[i][1] = temp1[1];
    temp1[0] = temp2[0]; 
    temp1[1] = temp2[1];
  }
}


void spawn_apple(){
  bool run_status = true;
  while (run_status){
    apple_x = random(0, (screen_width-box_width)/5)*box_width;
    apple_y = random(0, (screen_height-box_height)/5)*box_height;
  
    for (int i = 0; i < snake_length; i++){
      if (snake_positions[i][0] == apple_x && snake_positions[i][1] == apple_y){
        break;
      }
      if (i == snake_length-1){
        run_status = false;
        break;
      }
    }
  }
}


void check_colission(){
  for (int i = 1; i < snake_length; i++){
    if(snake_positions[i][0] == snake_x && snake_positions[i][1] == snake_y){
      snake_length = i;
      lives--;
      break;
    }  
  }
}


void spawn_snake(){
  snake_x = random(0, (screen_width-box_width)/5)*box_width;
  snake_y = random(0, (screen_height-box_height)/5)*box_height;
}



void draw(int box_x, int box_y) {
  u8g.drawFrame(0, 0, screen_width, screen_height);
  u8g.drawBox(box_x, box_y, box_width, box_height);
}

void draw_heart(int x, int y){
  u8g.drawBitmapP(x, y, 1, 8, heart_bitmap);
}
