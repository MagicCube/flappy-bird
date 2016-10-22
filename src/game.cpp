#include <Arduino.h>

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "pitches.h"
#include "resources.h"

Adafruit_SSD1306 display = Adafruit_SSD1306(4);

#define BUZZER_PIN 5
#define BUTTON_PIN 9

#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 64

unsigned long clock = 0;
unsigned long timerDelay = 0;
unsigned long fallTimer = 0;

uint8_t score = 0;

uint8_t highScore = 0;

uint8_t buttonState = 0;
boolean buttonActive = true;
int boost = 0;
uint8_t height = 10;
uint8_t fallSpeed = 0;

uint8_t flap[] = {0, 2, 3, 3, 2, 1, 1};

int sewer1 = 84;
int sewer2 = 126;
int sewerSpeed = 2;

int gap1 = 7;
int gap2 = 14;

uint8_t hills[DISPLAY_WIDTH];
uint8_t hillSlope = 1;
uint8_t hillTop = 26;

uint8_t song = 0;

void died();

uint8_t centerX(uint8_t width)
{
    return (DISPLAY_WIDTH - width) / 2;
}

uint8_t centerY(uint8_t height)
{
    return (DISPLAY_HEIGHT - height) / 2;
}

void beep()
{
    digitalWrite(BUZZER_PIN, HIGH);
    delay(10);
    digitalWrite(BUZZER_PIN, LOW);
    delay(10);
}

void background() {
    display.fillRect(0, 50, DISPLAY_WIDTH, 2, WHITE);

    uint8_t last = DISPLAY_WIDTH - 1;

    if (hills[last] < hillTop)
    {
        hillSlope = 0;
        hillTop = random(10, 40);
    }
    if (hills[last] > 45)
    {
        hillSlope = 1;
    }
    if (hillSlope == 1)
    {
        hills[last] = hills[last] - 1;
    }
    if (hillSlope == 0)
    {
        hills[last] = hills[last] + 1;
    }

    for (int i = 0; i < last; i++) {
        hills[i] = hills[i + 1];
        display.drawPixel(i + 1, hills[i], WHITE);
        display.drawFastVLine(i + 1, hills[i], 3, WHITE);
        if (i % 2 == 0)
        {
            display.drawFastVLine(i + 1, hills[i], 6, WHITE);
        }
    }
}

void buzz(int targetPin, long frequency, long length) {
    long delayValue = 1000000 / frequency / 2;
    long numCycles = frequency * length / 1000;
    for (long i = 0; i < numCycles; i++) {
        digitalWrite(targetPin, HIGH);
        delayMicroseconds(delayValue);
        digitalWrite(targetPin, LOW);
        delayMicroseconds(delayValue);
    }
}

void sing(int s) {

    song = s;

    int size = sizeof(melody) / sizeof(int);
    for (int thisNote = 0; thisNote < size; thisNote++) {
        int noteDuration = 1000 / tempo[thisNote];
        buzz(BUZZER_PIN, melody[thisNote], noteDuration);
        int pauseBetweenNotes = noteDuration * .4;
        delay(pauseBetweenNotes);
        buttonState = digitalRead(BUTTON_PIN);
        buzz(BUZZER_PIN, 0, noteDuration);
        display.clearDisplay();
        background();
        display.fillRect(46, 30, 29, 14, BLACK);
        display.drawBitmap(centerX(48), 0, splash, 48, 48, 1);
        display.display();
        buttonState = digitalRead(BUTTON_PIN);
        if (buttonState == LOW) {
            break;
        }
    }
}


int bullet1 = 3;
int bullet2 = 36;
int bulletTravel1 = 150;
int bulletTravel2 = 250;

void bullets() {

    if (bulletTravel1 < 0) {
        bulletTravel1 = 150;
        bullet1 = random(1, 6);
    }
    if (bulletTravel2 < 0) {
        bulletTravel2 = 150;
        bullet2 = random(35, 39);
    }

    bulletTravel1 = bulletTravel1 - 4;
    bulletTravel2 = bulletTravel2 - 4;

    if (bulletTravel1 >= 84 && bulletTravel1 <= 88) {
        for (int i = 300; i < 450; i++) {
            tone(BUZZER_PIN, i, 100);
        }
    }
    if (bulletTravel2 >= 84 && bulletTravel2 <= 88) {
        for (int i = 300; i < 450; i++) {
            tone(BUZZER_PIN, i, 100);
        }
    }

    display.drawBitmap(bulletTravel1, bullet1, bullet, 8, 8, 1);
    display.drawBitmap(bulletTravel2, bullet2, bullet, 8, 8, 1);
}

void sewers() {

    if (sewer1 < 0) {
        sewer1 = 84;
        gap1 = random(5, 20);
    }
    if (sewer2 < 0) {
        sewer2 = 84;
        gap2 = random(5, 20);
    }

    if (score == 4) {
        sewerSpeed = 3;
    }

    sewer1 = sewer1 - sewerSpeed;
    sewer2 = sewer2 - sewerSpeed;

    display.fillRect(sewer1 - 1, 0, 8, gap1 - 4, BLACK);         // Top Tube
    display.fillRect(sewer1 - 2, gap1 - 4, 10, 4, BLACK);        // Top Cap

    display.fillRect(sewer1 - 2, gap1 + 20, 10, 4, BLACK);       // Bottom Cap
    display.fillRect(sewer1 - 1, gap1 + 24, 8, 20, BLACK);       // Bottom Tube

    display.fillRect(sewer2 - 1, 0, 8, gap2 - 4, BLACK);         // Top Tube
    display.fillRect(sewer2 - 2, gap2 - 4, 10, 4, BLACK);        // Top Cap

    display.fillRect(sewer2 - 2, gap2 + 20, 10, 4, BLACK);       // Bottom Cap
    display.fillRect(sewer2 - 1, gap2 + 24, 8, 20, BLACK);       // Bottom Tube

    display.drawRect(sewer1 - 1, 0, 8, gap1 - 4, WHITE);         // Top Tube
    display.drawRect(sewer1 - 2, gap1 - 4, 10, 4, WHITE);        // Top Cap

    display.drawRect(sewer1 - 2, gap1 + 20, 10, 4, WHITE);       // Bottom Cap
    display.drawRect(sewer1 - 1, gap1 + 24, 8, 20, WHITE);       // Bottom Tube

    display.drawRect(sewer2 - 1, 0, 8, gap2 - 4, WHITE);         // Top Tube
    display.drawRect(sewer2 - 2, gap2 - 4, 10, 4, WHITE);        // Top Cap

    display.drawRect(sewer2 - 2, gap2 + 20, 10, 4, WHITE);       // Bottom Cap
    display.drawRect(sewer2 - 1, gap2 + 24, 8, 20, WHITE);       // Bottom Tube
}

void scores() {
    if ((sewer1 <= 11 && sewer1 >= 9) || (sewer2 <= 11 && sewer2 >= 9)) {
        score++;
        for (int i = 1500; i > 1000; i--) {
            tone(BUZZER_PIN, i, 50);
        }
    }
    display.fillRect(110, 0, 18, 9, WHITE);
    display.setCursor(113, 1);
    display.println(score);
}

void collision() {
    if (height + 3 < gap1 && sewer1 <= 24 && sewer1 >= 12) {
        died();
    }
    else if (height + 10 > gap1 + 20 && sewer1 <= 24 && sewer1 >= 12) {
        died();
    }
    else if (height + 3 < gap2 && sewer2 <= 24 && sewer2 >= 12) {
        died();
    }
    else if (height + 10 > gap2 + 20 && sewer2 <= 24 && sewer2 >= 12) {
        died();
    }
    else if (height < bullet1 + 6 && bulletTravel1 <= 24 && bulletTravel1 >= 12) {
        died();
    }
    else if (height > bullet2 && bulletTravel2 <= 24 && bulletTravel2 >= 12) {
        died();
    }
    else if (height + 8 > 42) {
        // Fall down
        died();
    }
}

void died() {

    buttonActive = false;
    tone(BUZZER_PIN, 500, 100);
    delay(200);
    tone(BUZZER_PIN, 500, 100);
    delay(100);
    tone(BUZZER_PIN, 300, 500);

    if (score > highScore) {
        highScore = score;
    }
    while (height < 39) {
        clock = millis();
        fallSpeed = ((clock - fallTimer) / 200) + 1;
        height = height + fallSpeed;
        display.clearDisplay();
        background();
        sewers();
        bullets();
        display.fillCircle(23, height + 4, 3, BLACK);
        if (fallSpeed > 2) {
            display.drawBitmap(18, height, bird[5], 8, 8, 1);
        }
        else if (fallSpeed > 1) {
            display.drawBitmap(18, height, bird[4], 8, 8, 1);
        }
        else {
            display.drawBitmap(18, height, bird[1], 8, 8, 1);
        }
        scores();
        display.display();
        delay(100);
    }
    display.fillRect(centerX(112), centerY(52), 112, 52, WHITE);
    display.drawRect(centerX(108), centerY(48), 108, 48, BLACK);

    display.setCursor(16, 18);
    display.setTextSize(2);
    display.print("GAMEOVER");

    display.setCursor(42, 40);
    display.setTextSize(1);
    display.print("SCORE ");
    display.print(score);

    display.display();

    buttonState = digitalRead(BUTTON_PIN);
    while (buttonState == HIGH) {
        buttonState = digitalRead(BUTTON_PIN);
        delay(40);
    }
    score = 0;
    buttonActive = true;
    buttonState = 0;
    sewer1 = 84;
    sewer2 = 126;
    sewerSpeed = 2;
    bulletTravel1 = 150;
    bulletTravel2 = 250;
    bullet1 = random(1, 4);
    bullet2 = random(35, 39);
    boost = 0;
    height = 10;
    fallSpeed = 0;
    clock = millis();
    fallTimer = clock;
}

void gamePlay() {
    Serial.println("gameplay");
    int flapHeight[] = {height, height, height - 5, height - 7, height - 9, height - 7, height - 6};
    buttonState = analogRead(A1) < 460 ? LOW : HIGH;

    if (buttonState == LOW && buttonActive == true) {
        for (int i = 900; i < 1400; i++) {
            tone(BUZZER_PIN, i, 20);
        }
        for (int x = 0; x < 7; x++) {
            display.clearDisplay();
            background();
            sewers();
            bullets();
            display.fillCircle(23, (flapHeight[x] - boost) + 4, 3, BLACK);
            display.drawBitmap(18, flapHeight[x] - boost, bird[flap[x]], 8, 8, 1);
            scores();
            display.display();
            delay(100);
            buttonState = analogRead(A1) < 460 ? LOW : HIGH;
            height = flapHeight[x] - boost;
            collision();
            if (x > 2) {
                if (buttonState == LOW && buttonActive) {
                    x = 0;
                    boost = boost + 7;
                    for (int i = 900; i < 1400; i++) {
                        tone(BUZZER_PIN, i, 20);
                    }
                }
            }
        }
        height = flapHeight[5] - boost;
        clock = millis();
        fallTimer = clock;
        boost = 0;
    }
    else {
        fallSpeed = ((clock - fallTimer) / 400) + 1;
        if (fallSpeed > 3) {
            fallSpeed = 3;
        }
        height = height + fallSpeed;

        if (height > 39) {
            height = 39;
        }
        display.clearDisplay();

        background();
        sewers();
        bullets();

        display.fillCircle(23, height + 4, 3, BLACK);

        if (fallSpeed > 2) {
            display.drawBitmap(18, height, bird[5], 8, 8, 1);
        }
        else if (fallSpeed > 1) {
            display.drawBitmap(18, height, bird[4], 8, 8, 1);
        }
        else {
            display.drawBitmap(18, height, bird[1], 8, 8, 1);
        }

        scores();

        display.display();
        delay(100);
        collision();
    }
}
















// ----------------------------------------- SETUP -----------------------------------------

void setup() {
    delay(1000);
    pinMode(5, OUTPUT);

    display.begin();
    display.setTextColor(BLACK);
    display.setTextSize(1);
    display.clearDisplay();
    display.display();

    for (int i = 0; i <= DISPLAY_WIDTH - 1; i++)
    {
        hills[i] = 47;
    }

    beep();

    while (hills[0] > 30)
    {
        background();
    }

    buttonState = digitalRead(BUTTON_PIN);
    while (buttonState == HIGH)
    {
        sing(1);
    }

    Serial.println("511");

    beep();
}

// --------------------------------------- MAIN LOOP ---------------------------------------

void loop() {
    clock = millis();
    gamePlay();
}
