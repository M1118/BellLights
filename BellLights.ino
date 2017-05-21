

#define BELL1    2
#define BELL2    3
#define BELL3    11
#define LED1     5
#define LED2     6
#define LED3     7
#define DURATION A0

#define MAX_DURATION  5000

#define FAST_RATE   100  // 10Hz
#define SLOW_RATE   200  // 5Hz

typedef enum {
  LEDOff, LEDOn, LEDFastBlink, LEDSlowBlink
} LEDState;

struct {
  LEDState       state;
  unsigned long  nextChange;
  int            pin;
  bool           isOn;
  unsigned long  blinkTick;
} led[] = {
  { LEDOff, 0, LED1, false, 0 },
  { LEDOff, 0, LED2, false, 0 },
  { LEDOff, 0, LED3, false, 0 }
};

extern unsigned long millis();

unsigned long state_duration;

void setup() {
  Serial.begin(9600);

  pinMode(BELL1, INPUT);
  pinMode(BELL2, INPUT);
  pinMode(BELL3, INPUT);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);

  readDuration();
}

void readDuration()
{
  int duration = analogRead(DURATION);
  unsigned long percent = (duration * 10) / 102;
  state_duration = (MAX_DURATION / 100) * percent;
}

void updateLEDs()
{
  unsigned long tick = millis();
  int i;
  for (i = 0; i < 3; i++)
  {
    if (led[i].nextChange < tick && led[i].state != LEDOff)
    {
      led[i].nextChange = tick + state_duration;
      switch (led[i].state)
      {
        case LEDOn:
          led[i].state = LEDFastBlink;
          led[i].isOn = false;
          led[i].blinkTick = tick + FAST_RATE;
          break;
        case LEDFastBlink:
          led[i].state = LEDSlowBlink;
          led[i].isOn = false;
          led[i].blinkTick = tick + SLOW_RATE;
          break;
        case LEDSlowBlink:
          led[i].state = LEDOff;
          break;
      }
    }
    if (led[i].blinkTick < tick && (led[i].state == LEDFastBlink || led[i].state == LEDSlowBlink))
    {
      led[i].isOn = led[i].isOn ? false : true;
      led[i].blinkTick = tick + (led[i].state == LEDFastBlink ? FAST_RATE : SLOW_RATE);
    }
    else if (led[i].state == LEDOn)
    {
      led[i].isOn = true;
    }

    if (led[i].state == LEDOff)
    {
      digitalWrite(led[i].pin, LOW);
    }
    else if (led[i].isOn)
    {
      digitalWrite(led[i].pin, HIGH);
    }
    else if (! led[i].isOn)
    {
      digitalWrite(led[i].pin, LOW);
    }
  }
}

void ledOn(int ledNo)
{
  LEDState prevState = led[ledNo].state;
  led[ledNo].state = LEDOn;
  led[ledNo].nextChange = millis() + state_duration;
  led[ledNo].isOn = true;
  digitalWrite(led[ledNo].pin, HIGH);
  // Force all the other indicators to the next state
  if (prevState != LEDOn)
  {
    for (int i = 0; i < 3; i++)
    {
      if (i != ledNo)
      {
        led[i].nextChange = millis() + 10;
      }
    }
  }
}

void readBells()
{
  if (! digitalRead(BELL1))
    ledOn(0);
  if (! digitalRead(BELL2))
    ledOn(1);
  if (! digitalRead(BELL3))
    ledOn(2);
}

void loop() {
  readBells();
  updateLEDs();

  if ((millis() % 1000) == 0)
    readDuration();
}
