#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define PIN_X A0
#define PIN_Y A1

int basic_x, basic_y, x_read, y_read, x, y;
byte paper[45][23], votes[122], message[32];
LiquidCrystal_I2C lcd(0x27, 16, 2);

byte shi[8] = {
  B00000,
  B00001,
  B00001,
  B00010,
  B10010,
  B01100,
  B00100,
  B00000,
};
byte fou[8] = {
  B00000,
  B00000,
  B10001,
  B01010,
  B00100,
  B01010,
  B10001,
  B00000,
};
byte nan[8] = {
  B00100,
  B01110,
  B10101,
  B01110,
  B10001,
  B10001,
  B10001,
  B01110,
};
byte nv[8] = {
  B01110,
  B10001,
  B10001,
  B10001,
  B01110,
  B00100,
  B11111,
  B00100,
};

void setup() {
  xy_init();
  paper_reset();
  lcd_init();
}

void loop() {
  xy_read();

  if (xy_move()) {
    byte ascii = check_position();
    if (ascii != 32) {
      message_add(ascii);
    }
  }

  lcd_display();

  delay(100);
}

void xy_init() {
  int sum_x = 0, sum_y = 0;

  for (int i = 0; i < 10; i++) {
    sum_x += analogRead(PIN_X);
    sum_y += analogRead(PIN_Y);
    delay(10);
  }

  basic_x = sum_x / 10;
  basic_y = sum_y / 10;

  x = 22;
  y = 11;
}

void paper_reset() {
  for (int i = 0; i < 122; i++) {
    votes[i] = 0;
  }

  for (int i = 0; i < 45; i++) {
    for (int j = 0; j < 23; j++) {
      paper[i][j] = 32;
    }
  }

  paper_add(2, 7, 4, 4, 0);
  paper_add(2, 12, 4, 4, 1);
  paper_add(39, 7, 4, 4, 2);
  paper_add(39, 12, 4, 4, 3);
  paper_add(11, 2, 3, 2, 116);
  paper_add(16, 2, 3, 2, 115);
  paper_add(21, 2, 3, 2, 121);
  paper_add(26, 2, 3, 2, 109);
  paper_add(31, 2, 3, 2, 113);
  byte ascii = 65;
  for (int i = 6; i <= 15; i += 3) {
    for (int j = 9; j <= 33; j += 4, ascii++) {
      if ((i == 12 || i == 15) && j == 21) {
        j += 4;
      }
      paper_add(j, i, 3, 2, ascii);
    }
  }
  ascii = 49;
  for (int i = 8; i <= 32; i += 3, ascii++) {
    paper_add(i, 19, 2, 2, ascii);
  }
  paper_add(35, 19, 2, 2, 48);
}

void paper_add(int x_min, int y_min, int x_size, int y_size, byte ascii) {
  for (int i = x_min; i < x_min + x_size; i++) {
    for (int j = y_min; j < y_min + y_size; j++) {
      paper[i][j] = ascii;
    }
  }
}

void lcd_init() {
  for (int i = 0; i < 32; i++) {
    message[i] = 32;
  }

  lcd.init();
  lcd.backlight();
  lcd.createChar(0, shi);
  lcd.createChar(1, fou);
  lcd.createChar(2, nan);
  lcd.createChar(3, nv);
}

void xy_read() {
  x_read = analogRead(PIN_X);
  y_read = analogRead(PIN_Y);
}

bool xy_move() {
  if (x_read == basic_x && y_read == basic_y) {
    return false;
  }

  if (x_read > basic_x && x < 44) {
    x += 1;
  } else if (x_read < basic_x && x > 0) {
    x -= 1;
  }

  if (y_read > basic_y && y < 22) {
    y += 1;
  } else if (y_read < basic_y && y > 0) {
    y -= 1;
  }

  return true;
}

byte check_position() {
  byte ascii = paper[x][y];

  if (ascii != 32) {
    votes[ascii] += 1;
    paper[x][y] = 32;
  }

  if (votes[ascii] >= 2) {
    paper_reset();
    return ascii;
  }

  return 32;
}

void message_add(byte ascii) {
  if (message[31] == 32) {
    for (int i = 0; i < 32; i++) {
      if (message[i] == 32) {
        message[i] = ascii;
        break;
      }
    }
  } else {
    for (int i = 0; i < 31; i++) {
      message[i] = message[i + 1];
    }
    message[31] = ascii;
  }
}

void lcd_display() {
  lcd.clear();

  for (int i = 0; i < 16; i++) {
    lcd.write(message[i]);
  }
  lcd.setCursor(0, 1);
  for (int i = 16; i < 32; i++) {
    lcd.write(message[i]);
  }
}
