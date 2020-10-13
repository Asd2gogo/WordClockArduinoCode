//#include <ArduinoSTL.h>

//#define FASTLED_ALLOW_INTERRUPTS 0
//#define FASTLED_INTERRUPT_RETRY_COUNT 1
#define ANIMATION_SIZE 500
#define LED_UPDATE_DELAY 19

//overwrite PROGMEM for ESP01 to correctly save String (Char array) on External flash
#define PROGMEM ICACHE_RODATA_ATTR

//-----------------------LED-Controller-------------------------------------

//#include <zip.h>
//#include <StandardCplusplus.h>
#include <vector>

//#include <arduino.h>
#include <FastLED.h>

//------------------------WEB Server-----------------------------------------
#include <DNSServer.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include "ESPAsyncWebServer.h"

#define NUM_LEDS 225

CRGB leds[NUM_LEDS];

class AnimationController;
class Animation;
class LEDController;
class Word;
class WordConfiguration;
class WordController;

const uint8_t charList8x5[96][5] = {
    /* */ {0, 0, 0, 0, 0}, /*!*/ {0, 0, 95, 0, 0}, /*"*/ {0, 7, 0, 7, 0}, /*#*/ {20, 127, 20, 127, 20},           // 0 1 2 3
    /*$*/ {36, 42, 127, 42, 18}, /*%*/ {35, 19, 8, 100, 98}, /*&*/ {54, 73, 85, 34, 80}, /*'*/ {0, 5, 3, 0, 0},   // 4 5 6 7
    /*(*/ {0, 28, 34, 65, 0}, /*)*/ {0, 65, 34, 28, 0}, /***/ {20, 8, 62, 8, 20}, /*+*/ {8, 8, 62, 8, 8},         // 8 9 10 11
    /*,*/ {0, 80, 48, 0, 0}, /*-*/ {8, 8, 8, 8, 8}, /*.*/ {0, 96, 96, 0, 0}, /*/*/ {32, 16, 8, 4, 2},             //12 13 14 15
    /*0*/ {62, 81, 73, 69, 62}, /*1*/ {0, 4, 2, 127, 0}, /*2*/ {66, 97, 81, 73, 70}, /*3*/ {34, 65, 65, 73, 54},  //16 17 18 19
    /*4*/ {24, 20, 18, 127, 16}, /*5*/ {39, 69, 69, 69, 57}, /*6*/ {60, 74, 73, 73, 48}, /*7*/ {1, 113, 9, 5, 3}, //20 21 22 23
    /*8*/ {54, 73, 73, 73, 54}, /*9*/ {6, 73, 73, 41, 30}, /*:*/ {0, 54, 54, 0, 0}, /*;*/ {0, 86, 54, 0, 0},      //24,25,26,27
    /*<*/ {8, 20, 34, 65, 0}, /*=*/{20, 20, 20, 20, 20}, /*>*/ {0, 65, 34, 20, 8}, /*?*/ {2, 1, 81, 9, 6},        //

    /*@*/ {50, 73, 121, 65, 62}, /*A*/ {124, 18, 17, 18, 124}, /*B*/ {127, 73, 73, 73, 54}, /*C*/ {62, 65, 65, 65, 34},
    /*D*/ {127, 65, 65, 65, 62}, /*E*/ {127, 73, 73, 65, 65}, /*F*/ {127, 9, 9, 1, 1}, /*G*/ {62, 65, 73, 73, 58},
    /*H*/ {127, 8, 8, 8, 127}, /*I*/ {0, 65, 127, 65, 0}, /*J*/ {33, 65, 65, 65, 63}, /*K*/ {127, 8, 20, 34, 65},
    /*L*/ {127, 64, 64, 64, 64}, /*M*/ {127, 2, 4, 2, 127}, /*N*/ {127, 4, 8, 16, 127}, /*O*/ {62, 65, 65, 65, 62},
    /*P*/ {127, 9, 9, 9, 6}, /*Q*/ {62, 65, 81, 33, 94}, /*R*/ {127, 9, 25, 41, 70}, /*S*/ {38, 73, 73, 73, 50},
    /*T*/ {1, 1, 127, 1, 1}, /*U*/ {63, 64, 64, 64, 63}, /*V*/ {15, 48, 64, 48, 15}, /*W*/ {63, 64, 56, 64, 63},
    /*X*/ {99, 20, 8, 20, 99}, /*Y*/ {7, 8, 112, 8, 7}, /*Z*/ {97, 81, 73, 69, 67}, /*[*/ {0, 127, 65, 65, 0},
    /*\*/ {2, 4, 8, 16, 32}, /*]*/ {0, 65, 65, 127, 0}, /*^*/ {4, 2, 1, 2, 4}, /*_*/ {64, 64, 64, 64, 64},

    /*`*/ {0, 1, 2, 4, 0}, /*a*/ {32, 84, 84, 84, 120}, /*b*/ {127, 72, 68, 68, 56}, /*c*/ {56, 68, 68, 68, 32},
    /*d*/ {56, 68, 68, 72, 127}, /*e*/ {56, 84, 84, 84, 24}, /*f*/ {8, 126, 9, 1, 2}, /*g*/ {24, 164, 164, 164, 124},
    /*h*/ {127, 8, 4, 4, 120}, /*i*/ {0, 68, 125, 64, 0}, /*j*/ {32, 64, 68, 61, 0}, /*k*/ {127, 16, 40, 68, 0},
    /*l*/ {0, 65, 127, 64, 0}, /*m*/ {124, 4, 24, 4, 120}, /*n*/ {124, 8, 4, 4, 120}, /*o*/ {56, 68, 68, 68, 56},
    /*p*/ {252, 36, 36, 36, 24}, /*q*/ {24, 36, 36, 36, 252}, /*r*/ {124, 8, 4, 4, 8}, /*s*/ {72, 84, 84, 84, 36},
    /*t*/ {4, 63, 68, 32, 0}, /*u*/ {60, 64, 64, 32, 124}, /*v*/ {28, 32, 64, 32, 28}, /*w*/ {60, 64, 48, 64, 60},
    /*x*/ {68, 40, 16, 40, 68}, /*y*/ {12, 144, 160, 160, 124}, /*z*/ {68, 100, 84, 76, 68}, /*{*/ {0, 8, 54, 65, 0},
    /*|*/ {0, 0, 127, 0, 0}, /*}*/ {0, 65, 54, 8, 0}, /*~*/ {4, 2, 4, 8, 4}, /* */ {8, 28, 42, 8, 8}};

class LEDController
{
  CRGB *getLEDatXY();

public:
  LEDController()
  {
  }

  void setup()
  {
    //Serial.println("going to add Leds");

    FastLED.addLeds<NEOPIXEL, 0>(leds, NUM_LEDS);
  }

  void turnOn(int index, CRGB color)
  {
    leds[index] = color;
  }

  void turnOff(int index)
  {
    leds[index] = CRGB::Black;
  }

  void turnAllOn(CRGB color = CRGB::White)
  {
    for (int x = 0; x < NUM_LEDS; x++)
    {
      leds[x] = color;
    }
  }

  void turnAllOff()
  {
    for (int x = 0; x < NUM_LEDS; x++)
    {
      leds[x] = CRGB::Black;
    }
  }

  CRGB *getLED(int index)
  {
    return &leds[index];
  }

  std::vector<CRGB *> getLeds(int startIndex, int endIndex)
  {
    std::vector<CRGB *> returnVector;

    if (startIndex < endIndex)
    { // Normale Orientierung (ungerade Reihen)

      for (int x = startIndex - 1; x <= endIndex - 1; x++)
      {
        returnVector.push_back(&leds[x]);
      }
    }
    else if (startIndex > endIndex) // Invertiert Orientierung (gerade Reihen)
    {
      for (int x = startIndex - 1; x >= endIndex - 1; x--)
      {
        returnVector.push_back(&leds[x]);
      }
    }
    else
    { // only one LED
      returnVector.push_back(&leds[startIndex]);
    }
    return returnVector;
  }

  std::vector<CRGB *> getAllLeds()
  {
    std::vector<CRGB *> allLedsVec;
    for (int x = 0; x < NUM_LEDS; x++)
    {
      allLedsVec.push_back(&leds[x]);
    }
    return allLedsVec;
  }

  std::vector<CRGB *> getSquareLeds(int distFromCenter)
  {
    std::vector<CRGB *> returnVector;
    int maxEdgeLenght = sqrt(NUM_LEDS) - 1;
    int center = maxEdgeLenght / 2;

    int selectedEdgeLength = distFromCenter * 2 + 1;

    int leftTopEdge = center - (selectedEdgeLength - 1) / 2;
    int rightBottomEdge = center + (selectedEdgeLength - 1) / 2;

    for (int x = leftTopEdge; x <= rightBottomEdge; x++)
    {
      returnVector.push_back(getLEDatXY(x, leftTopEdge));
    }
    for (int y = leftTopEdge; y <= rightBottomEdge; y++)
    {
      returnVector.push_back(getLEDatXY(rightBottomEdge, y));
    }

    for (int x = rightBottomEdge; x >= leftTopEdge; x--)
    {
      returnVector.push_back(getLEDatXY(x, rightBottomEdge));
    }

    for (int y = rightBottomEdge; y >= leftTopEdge; y--)
    {
      returnVector.push_back(getLEDatXY(leftTopEdge, y));
    }

    return returnVector;
  }

  // Get single LED -> X/Y 0/0 is top left
  CRGB *getLEDatXY(int x, int y)
  {

    bool isRLrow = y % 2; // true if y is uneven -> Led index is from right to Left in this isRLrow
    int index;

    int x0;
    x0 = NUM_LEDS - (15 * (y + 1)); // set x0 corrosponding to current y row
    if (isRLrow)
    {
      x0 += 14; // if row index is from Right to Left -> add 14 to start to adjust x0
      index = x0 - x;
    }
    else
    {
      index = x0 + x;
    }

    return &leds[index];
  }

  // return one line on pixels (0 is top or left) orientation 0 -> horizontal 1-> vertical
  // length of line (only odd umbers) 1,3,5...
  std::vector<CRGB *> getLineLeds(int index, int orientation, int length = 15)
  {
    std::vector<CRGB *> returnVector;
    int center = 7;
    int oneSideDist = floor(length / 2);

    if (!orientation)
    { // horizontal x line

      for (int x = center - oneSideDist; x <= center + oneSideDist; x++)
      {
        returnVector.push_back(getLEDatXY(x, index));
      }
    }
    else
    { //vertical y line

      for (int y = center - oneSideDist; y <= center + oneSideDist; y++)
      {
        returnVector.push_back(getLEDatXY(index, y));
      }
    }

    return returnVector;
  }
  // get Leds for a char -> input a 5 int array and the top left xy pos of a 5 by 7 square
  std::vector<CRGB *> getLEDsForChar(int xStart, int yStart, const uint8_t charLeds[5])
  {

    std::vector<CRGB *> returnVector;

    //0 is left
    for (int line = 0; line < 5; line++)
    {

      uint8_t pixelLine = charLeds[line];

      // 0 is top and 6 is the last bottom Pixel of each line
      for (int pixel = 0; pixel < 7; pixel++)
      {

        uint8_t shiftetPixelLine = pixelLine >> pixel;
        bool activePixel = false;

        if (shiftetPixelLine & 1)
        {
          int xCoord = xStart + line;
          int yCoord = yStart + pixel;
          returnVector.push_back(getLEDatXY(xCoord, yCoord));
        }
      }
    }
    return returnVector;
  }

  void output()
  {
    FastLED.show();
  }
};

class Word
{

public:
  std::vector<CRGB *> wordLeds; // Maybe remove vector and remve classic array -> size on constuctor with vector length

  Word(std::vector<CRGB *> leds)
  {
    for (CRGB *led : leds)
    {
      wordLeds.push_back(led);
    }
  }

  void makeSingular(byte digitsToRemove)
  {
    byte size = wordLeds.size();
    for (byte x = 0; x < digitsToRemove; x++)
    {
      *wordLeds.at(size - (1 + x)) = CRGB::Black;
    }
  }

  void turnOn(CRGB col = CRGB::White)
  {
    for (CRGB *led : wordLeds)
    {
      *led = col;
    }
  }

  void turnOff()
  {
    for (CRGB *led : wordLeds)
    {
      *led = CRGB::Black;
    }
  }
};

class WordConfiguration
{

public:
  Word *word;
  byte digitsToRemove = 0; // digits to turn off from push_back
  CRGB color;

  WordConfiguration(Word *w, byte digits = 0, CRGB c = CRGB::White)
  {
    word = w;
    digitsToRemove = digits;
    color = c;
  }

  std::vector<CRGB *> getLeds(int _digitsToRemove = -1)
  {

    std::vector<CRGB *> leds;

    if (_digitsToRemove = -1)
      _digitsToRemove = digitsToRemove;

    for (int i = 0; i < word->wordLeds.size() - _digitsToRemove; i++)
    {
      leds.push_back(word->wordLeds.at(i));
    }

    return leds;
  }

  ~WordConfiguration()
  {
    /*
    delete word;
    word = NULL;
  */
  }
};

class WordController
{

public:
  std::vector<Word *> basicWords;
  std::vector<Word *> minutesFirstDigitWords;
  std::vector<Word *> minutesSecondDigitWords;
  std::vector<Word *> quaterWords;
  std::vector<Word *> andWords;
  std::vector<Word *> beforeAfterWords;
  std::vector<Word *> hoursFirstDigitWords;
  std::vector<Word *> hoursSecondDigitWords;
  std::vector<Word *> fillWords;

  int wordCount[8];
  int totalWordCnt;

  WordController()
  {
  }

  void addbasicWord(Word *word)
  {
    basicWords.push_back(word);
  }

  void addminutesFirstDigitWord(Word *word)
  {
    minutesFirstDigitWords.push_back(word);
  }

  void addminutesSecondDigitWord(Word *word)
  {
    minutesSecondDigitWords.push_back(word);
  }
  void addquaterWord(Word *word)
  {
    quaterWords.push_back(word);
  }
  void addandWord(Word *word)
  {
    andWords.push_back(word);
  }
  void addbeforeAfterWord(Word *word)
  {
    beforeAfterWords.push_back(word);
  }
  void addhoursFirstDigitWord(Word *word)
  {
    hoursFirstDigitWords.push_back(word);
  }
  void addhoursSecondDigitWord(Word *word)
  {
    hoursSecondDigitWords.push_back(word);
  }

  void addfillWord(Word *word)
  {
    fillWords.push_back(word);
  }

  Word *getRandomWord()
  {

    int randomID = random(0, totalWordCnt);
    int wordMaxID = 0;

    for (int i = 0; i < 8; i++)
    {
      if (wordMaxID <= randomID && randomID < wordMaxID + wordCount[i])
      {

        switch (i)
        {
        case 0:
          return basicWords.at(randomID - wordMaxID);
          break;
        case 1:
          return minutesFirstDigitWords.at(randomID - wordMaxID);
          break;
        case 2:
          return minutesSecondDigitWords.at(randomID - wordMaxID);
          break;
        case 3:
          return quaterWords.at(randomID - wordMaxID);
          break;
        case 4:
          return andWords.at(randomID - wordMaxID);
          break;
        case 5:
          return beforeAfterWords.at(randomID - wordMaxID);
          break;
        case 6:
          return hoursFirstDigitWords.at(randomID - wordMaxID);
          break;
        case 7:
          return hoursSecondDigitWords.at(randomID - wordMaxID);
          break;
        }
      }
      else
      {
        wordMaxID += wordCount[i];
      }
    }
  }

  Word *getRandomWordFromVector(std::vector<Word *> wordVector)
  {

    int randIndex = random(0, wordVector.size());

    return wordVector.at(randIndex);
  }

  void updateWordCnt()
  {

    wordCount[0] = basicWords.size();
    wordCount[1] = minutesFirstDigitWords.size();
    wordCount[2] = minutesSecondDigitWords.size();
    wordCount[3] = quaterWords.size();
    wordCount[4] = andWords.size();
    wordCount[5] = beforeAfterWords.size();
    wordCount[6] = hoursFirstDigitWords.size();
    wordCount[7] = hoursSecondDigitWords.size();

    totalWordCnt = 0;
    for (int i = 0; i < 8; i++)
    {
      totalWordCnt += wordCount[i];
    }
  }

  std::vector<WordConfiguration> getMinutsWords(int minute)
  {
    std::vector<WordConfiguration> returnVector;

    int zehner = floor(minute / 10);
    int einer = minute % 10;
    if (minute == 11)
    {
      returnVector.push_back(WordConfiguration(minutesFirstDigitWords.at(10))); // "elf"
    }
    else if (minute == 12)
    {
      returnVector.push_back(WordConfiguration(minutesFirstDigitWords.at(11))); // "zwölf"
    }
    else
    {
      switch (einer)
      {
      case 0:
        break;
      case 1:

        returnVector.push_back(WordConfiguration(minutesFirstDigitWords.at(0))); // "eine"

        if (zehner > 1)
        {
          returnVector.at(0).digitsToRemove = 1; // convert "eine" zu "ein"
          // returnVector.push_back(new WordConfiguration(andWords.at(0)));// "und" // commented out .. was double "und"
        }
        break;
      case 6:
        returnVector.push_back(WordConfiguration(minutesFirstDigitWords.at(5)));
        if (zehner == 1)
        {
          returnVector.at(0).digitsToRemove = 1;
        }
        break;
      case 7:
        returnVector.push_back(WordConfiguration(minutesFirstDigitWords.at(6)));
        if (zehner == 1)
        {
          returnVector.at(0).digitsToRemove = 2;
        }
        break;
      default:
        returnVector.push_back(WordConfiguration(minutesFirstDigitWords.at(einer - 1)));
        break;
      }

      switch (zehner)
      {
      case 0:

        break;
      case 1:
        returnVector.push_back(WordConfiguration(minutesFirstDigitWords.at(9)));
        break;
      default:
        if (einer != 0)
        {
          returnVector.push_back(WordConfiguration(andWords.at(0)));
        }
        returnVector.push_back(WordConfiguration(minutesSecondDigitWords.at(zehner - 2)));
        break;
      }
    }

    return returnVector;
  }

  std::vector<WordConfiguration> getHourWords(int hour, bool isOclock = false)
  {

    std::vector<WordConfiguration> returnVector;
    if (hour == 0)
    {
      returnVector.push_back(WordConfiguration(hoursFirstDigitWords.at(3)));  //vier
      returnVector.push_back(WordConfiguration(andWords.at(1)));              // und
      returnVector.push_back(WordConfiguration(hoursSecondDigitWords.at(0))); // zwanzig
    }
    else if (hour == 1)
    {
      returnVector.push_back(WordConfiguration(hoursFirstDigitWords.at(0))); //ein
      if (isOclock)
        returnVector.at(0).digitsToRemove = 1;
    }
    else if (hour <= 12)
    {
      returnVector.push_back(WordConfiguration(hoursFirstDigitWords.at(hour - 1))); //jeweilige stunde
    }
    else if (hour == 16)
    {
      returnVector.push_back(WordConfiguration(hoursFirstDigitWords.at(5), 1)); // sech
      returnVector.at(0).digitsToRemove = 1;
    }
    else if (hour == 17)
    {
      returnVector.push_back(WordConfiguration(hoursFirstDigitWords.at(6), 2)); //7
      returnVector.at(0).digitsToRemove = 2;
    }
    else if (hour <= 20)
    {
      returnVector.push_back(WordConfiguration(hoursFirstDigitWords.at(hour - 11))); // jeweilige erster teil der stunde 3 / 4 / 5 / 6 / 8 / 9
    }
    else if (hour == 21)
    {
      returnVector.push_back(WordConfiguration(hoursFirstDigitWords.at(0), 1)); //ein
      returnVector.at(0).digitsToRemove = 1;
    }
    else if (hour > 21)
    {
      returnVector.push_back(WordConfiguration(hoursFirstDigitWords.at(hour - 21))); //jeweilige stunde
    }

    if (hour < 20 && hour > 12)
    {
      returnVector.push_back(WordConfiguration(hoursFirstDigitWords.at(9))); // zehn
    }
    else if (hour > 20)
    {
      returnVector.push_back(WordConfiguration(andWords.at(1)));              //und
      returnVector.push_back(WordConfiguration(hoursSecondDigitWords.at(0))); //zwanzig
    }
    else if (hour == 20)
    {
      returnVector.push_back(WordConfiguration(hoursSecondDigitWords.at(0))); //zwanzig
    }

    return returnVector;
  }

  std::vector<std::vector<WordConfiguration>> getWordsForTime(int minute, int hour)
  { // return array of possible word combination to for given time

    std::vector<std::vector<WordConfiguration>> returnVector;

    std::vector<WordConfiguration> timeQuaterWords;
    std::vector<WordConfiguration> timeQuaterEastWords;
    std::vector<WordConfiguration> timeMinutesPastWords;
    std::vector<WordConfiguration> timeMinutesBeforeWords;
    std::vector<WordConfiguration> timeOClockWords;
    std::vector<WordConfiguration> timeMiutesBeforeAfterHalfWords;

    if (minute == 0)
    { // oClock

      std::vector<WordConfiguration> hourWords = getHourWords(hour, true);

      timeOClockWords.push_back(WordConfiguration(basicWords.at(0))); //Es
      timeOClockWords.push_back(WordConfiguration(basicWords.at(1))); //ist

      for (std::vector<WordConfiguration>::iterator pIterator = hourWords.begin(); pIterator != hourWords.end(); ++pIterator)
      {
        timeOClockWords.push_back(*pIterator);
      }
      timeOClockWords.push_back(WordConfiguration(basicWords.at(3)));
    }

    if (minute != 0)
    { // before next hour / after current hour

      //Minutes before next Hour "fuenf zehn Minuten vor zehn"

      timeMinutesBeforeWords.push_back(WordConfiguration(basicWords.at(0))); //Es
      timeMinutesBeforeWords.push_back(WordConfiguration(basicWords.at(1))); //ist

      int minutesBeforeNextHour = (60 - minute);
      std::vector<WordConfiguration> minutesWords = getMinutsWords(minutesBeforeNextHour);
      for (std::vector<WordConfiguration>::iterator pIterator = minutesWords.begin(); pIterator != minutesWords.end(); ++pIterator)
      { // x (Minuten)
        timeMinutesBeforeWords.push_back(*pIterator);
      }
      timeMinutesBeforeWords.push_back(WordConfiguration(basicWords.at(2), minutesBeforeNextHour == 1 ? 1 : 0)); // Minuten

      timeMinutesBeforeWords.push_back(WordConfiguration(beforeAfterWords.at(0))); // vor

      int nextHour = (hour + 1) % 24;
      int twelfHourFormat = (nextHour % 13) + floor(nextHour / 13); // 10 11 12 1 2 3
      std::vector<WordConfiguration> hourWords = getHourWords(twelfHourFormat);

      for (std::vector<WordConfiguration>::iterator pIterator = hourWords.begin(); pIterator != hourWords.end(); ++pIterator)
      {
        timeMinutesBeforeWords.push_back(*pIterator);
        // add all Words necessary for the hour ("zwei und zwanzig")
      }
      //timeMinutesBeforeWords.push_back(basicWords.at(3)); // Uhr

      // after current hour "fuenf und vierzig zehn Minuten nach neun"

      std::vector<WordConfiguration> minuteWords = getMinutsWords(minute);

      timeMinutesPastWords.push_back(WordConfiguration(basicWords.at(0))); //Es
      timeMinutesPastWords.push_back(WordConfiguration(basicWords.at(1))); //ist

      for (std::vector<WordConfiguration>::iterator pIterator = minuteWords.begin(); pIterator != minuteWords.end(); ++pIterator)
      { // x (Minuten)
        timeMinutesPastWords.push_back(*pIterator);
      }
      timeMinutesPastWords.push_back(WordConfiguration(basicWords.at(2))); // Minuten

      timeMinutesPastWords.push_back(WordConfiguration(beforeAfterWords.at(1))); // nach

      twelfHourFormat = (hour % 13) + floor(hour / 13); // 10 11 12 1 2 3
      hourWords = getHourWords(twelfHourFormat);

      for (std::vector<WordConfiguration>::iterator pIterator = hourWords.begin(); pIterator != hourWords.end(); ++pIterator)
      {
        timeMinutesPastWords.push_back(*pIterator);
        // add all Words nessesarry for the hour ("zwei und zwanzig")
      }
      // timeMinutesPastWords.push_back(basicWords.at(3)); // Uhr
    }

    if (minute == 15)
    { // viertel nach current hour // viertel nextHour

      //"viertel nach eins"
      timeQuaterWords.push_back(WordConfiguration(basicWords.at(0))); //Es
      timeQuaterWords.push_back(WordConfiguration(basicWords.at(1))); //ist

      timeQuaterWords.push_back(WordConfiguration(quaterWords.at(0))); // viertel

      timeQuaterWords.push_back(WordConfiguration(beforeAfterWords.at(1))); // nach

      int twelfHourFormat = (hour % 13) + floor(hour / 13); // 10 11 12 1 2 3

      std::vector<WordConfiguration> hourWords = getHourWords(twelfHourFormat);

      for (std::vector<WordConfiguration>::iterator pIterator = hourWords.begin(); pIterator != hourWords.end(); ++pIterator)
      {
        timeQuaterWords.push_back(*pIterator);
      }

      // "Viertel zwei"
      timeQuaterEastWords.push_back(WordConfiguration(basicWords.at(0))); //Es
      timeQuaterEastWords.push_back(WordConfiguration(basicWords.at(1))); //ist

      timeQuaterEastWords.push_back(WordConfiguration(quaterWords.at(0))); // viertel

      int nextHour = hour + 1;
      twelfHourFormat = (nextHour % 13) + floor(nextHour / 13); // 10 11 12 1 2 3
      hourWords = getHourWords(twelfHourFormat);

      for (std::vector<WordConfiguration>::iterator pIterator = hourWords.begin(); pIterator != hourWords.end(); ++pIterator)
      {
        timeQuaterEastWords.push_back(*pIterator);
      }
    }

    if (minute == 30)
    { // halb
      //"Halb drei"

      timeQuaterWords.push_back(WordConfiguration(basicWords.at(0))); //Es
      timeQuaterWords.push_back(WordConfiguration(basicWords.at(1))); //ist

      timeQuaterWords.push_back(WordConfiguration(quaterWords.at(1))); // halb
      int nextHour = hour + 1;
      int twelfHourFormat = (nextHour % 13) + floor(nextHour / 13); // 10 11 12 1 2 3

      std::vector<WordConfiguration> hourWords = getHourWords(twelfHourFormat);

      for (std::vector<WordConfiguration>::iterator pIterator = hourWords.begin(); pIterator != hourWords.end(); ++pIterator)
      {
        timeQuaterWords.push_back(*pIterator);
      }
    }

    if (minute == 45)
    { // viertel vor next hour // dreiviertel nextHour

      timeQuaterWords.push_back(WordConfiguration(basicWords.at(0))); //Es
      timeQuaterWords.push_back(WordConfiguration(basicWords.at(1))); //ist

      timeQuaterWords.push_back(WordConfiguration(quaterWords.at(0))); // viertel

      timeQuaterWords.push_back(WordConfiguration(beforeAfterWords.at(0))); // vor

      int nextHour = hour + 1;
      int twelfHourFormat = (nextHour % 13) + floor(nextHour / 13); // 10 11 12 1 2 3

      std::vector<WordConfiguration> hourWords = getHourWords(twelfHourFormat);

      for (std::vector<WordConfiguration>::iterator pIterator = hourWords.begin(); pIterator != hourWords.end(); ++pIterator)
      {
        timeQuaterWords.push_back(*pIterator);
      }

      // "drei viertel zwei"
      timeQuaterEastWords.push_back(WordConfiguration(basicWords.at(0))); //Es
      timeQuaterEastWords.push_back(WordConfiguration(basicWords.at(1))); //ist

      timeQuaterEastWords.push_back(WordConfiguration(minutesFirstDigitWords.at(2))); // drei
      timeQuaterEastWords.push_back(WordConfiguration(quaterWords.at(0)));            // viertel

      for (std::vector<WordConfiguration>::iterator pIterator = hourWords.begin(); pIterator != hourWords.end(); ++pIterator)
      {
        timeQuaterEastWords.push_back(*pIterator);
      }
    }

    if ((minute >= 15) && (minute <= 45) && (minute != 30))
    { // minutes before / after halb

      timeMiutesBeforeAfterHalfWords.push_back(WordConfiguration(basicWords.at(0))); //Es
      timeMiutesBeforeAfterHalfWords.push_back(WordConfiguration(basicWords.at(1))); //ist

      //"zehn Minuten vor Halb drei"
      int deltaMinutes = abs(minute - 30);

      std::vector<WordConfiguration> minuteWords = getMinutsWords(deltaMinutes);

      for (std::vector<WordConfiguration>::iterator pIterator = minuteWords.begin(); pIterator != minuteWords.end(); ++pIterator)
      {
        timeMiutesBeforeAfterHalfWords.push_back(*pIterator);
        // drei, vier ....
      }
      timeMiutesBeforeAfterHalfWords.push_back(WordConfiguration(basicWords.at(2), deltaMinutes == 1 ? 1 : 0)); // "Minuten" // check for singular

      timeMiutesBeforeAfterHalfWords.push_back(WordConfiguration(beforeAfterWords.at(minute > 30 ? 1 : 0))); // "vor" / "nach" // check for vor or nach
      timeMiutesBeforeAfterHalfWords.push_back(WordConfiguration(quaterWords.at(1)));                        // "halb"

      int nextHour = hour + 1;
      int twelfHourFormat = (nextHour % 13) + floor(nextHour / 13); // 10 11 12 1 2 3
      std::vector<WordConfiguration> hourWords = getHourWords(twelfHourFormat);

      for (std::vector<WordConfiguration>::iterator pIterator = hourWords.begin(); pIterator != hourWords.end(); ++pIterator)
      {
        timeMiutesBeforeAfterHalfWords.push_back(*pIterator);
        // "eins" // "zwei"
      }
    }

    if (timeQuaterWords.size() > 0)
      returnVector.push_back(timeQuaterWords);
    if (timeQuaterEastWords.size() > 0)
      returnVector.push_back(timeQuaterEastWords);
    if (timeMinutesPastWords.size() > 0)
      returnVector.push_back(timeMinutesPastWords);
    if (timeMinutesBeforeWords.size() > 0)
      returnVector.push_back(timeMinutesBeforeWords);
    if (timeOClockWords.size() > 0)
      returnVector.push_back(timeOClockWords);
    if (timeMiutesBeforeAfterHalfWords.size() > 0)
      returnVector.push_back(timeMiutesBeforeAfterHalfWords);

    /*
      else{
        std::vector<std::vector<WordConfiguration>>::iterator itr = std::find(returnVector.begin(), returnVector.end(), vector); // find vector in returnVector
        returnVector.erase(itr); // delete WordConfigurationVector if Empty (currently no matching time for that)
      }
      */
    return returnVector;
  }
};

class Animation
{

  CRGB _setLED;

  int startTime;
  int _endTime;
  int _duration = -1;
  int _delay = 0;
  byte lastPerzentagTimePassed = 0;
  bool _endless;
  bool didTickBefore = false;

public:
  std::vector<CRGB *> isLED;
  bool _animationDone = false;
  int iD = -1;

  Animation(std::vector<WordConfiguration *> wordConfigs, CRGB ledSetColor, int duration = 500, int delay = 0, bool endless = false)
  { // add wordsVector with Leds and set Should color or color hue delta if endless is enabled

    for (WordConfiguration *wConfig : wordConfigs)
    {
      for (CRGB *led : wConfig->getLeds(wConfig->digitsToRemove))
      {
        isLED.push_back(led);
      }
    }
    _setLED = ledSetColor;

    startTime = millis() + delay;
    _duration = duration;
    _delay = delay;
    _endTime = startTime + _duration;
    _endless = endless;
  }

  Animation(std::vector<WordConfiguration> wordConfigs, CRGB ledSetColor, int duration = 500, int delay = 0, bool endless = false)
  { // add wordsVector with Leds and set Should color or color hue delta if endless is enabled

    for (WordConfiguration wConfig : wordConfigs)
    {
      for (CRGB *led : wConfig.getLeds(wConfig.digitsToRemove))
      {
        isLED.push_back(led);
      }
    }
    _setLED = ledSetColor;

    startTime = millis() + delay;
    _duration = duration;
    _delay = delay;
    _endTime = startTime + _duration;
    _endless = endless;
  }

  Animation(std::vector<CRGB *> leds, CRGB ledSetColor, int duration = 500, int delay = 0, bool endless = false)
  {

    for (CRGB *led : leds)
    {
      isLED.push_back(led);
    }

    _setLED = ledSetColor;
    _delay = delay;
    _duration = duration;
    _endless = endless;
    startTime = millis() + _delay;
    _endTime = startTime + _duration;
  }

  Animation(CRGB *led, CRGB ledSetColor, int duration = 500, int delay = 0, bool endless = false)
  {
    isLED.push_back(led);

    _setLED = ledSetColor;
    _delay = delay;
    _duration = duration;
    _endless = endless;
    startTime = millis() + _delay;
    _endTime = startTime + _duration;
  }

  bool tick()
  {
    unsigned long currentTime = millis();
    if (currentTime <= startTime)
    { // return if animation shouldn´t start because of set delay
      return false;
    }

    if (isLED.empty())
    {
      _animationDone = true;
      return false;
    }
    int timePassed = currentTime - startTime;
    int perzentageTimePassed = (timePassed * 100) / _duration;

    if (perzentageTimePassed > lastPerzentagTimePassed)
    {

      for (int timeMissed = perzentageTimePassed - lastPerzentagTimePassed - 1; timeMissed >= 0; timeMissed--)
      {

        int perzentageTimeToGo = 100 - perzentageTimePassed - timeMissed;
        std::vector<CRGB *>::iterator isLedIter = isLED.begin(); // For each led in Animation delta color from start off animation to set value is calculated and

        for (; isLedIter != isLED.end(); isLedIter++)
        {

          if (perzentageTimeToGo <= 0)
          { // time for animation already over -> ensure set value is on led and disable animation
            **isLedIter = _setLED;

            if (!_endless)
            { // animation only once -> Prozentualer unterschied zwischen ist und soll Led CHSV und wieviel Zeit schon von der Soll Duration schon vergangen ist um neuen Teilwert der Led zu setzen
              _animationDone = true;
            }
            else
            {
              setStartEndTiming(_duration);
              _setLED += _setLED;
            }
          }
          else
          {

            CRGB isColor = (**isLedIter);
            int deltaColor[] = {(int)_setLED.r - (int)isColor.r, (int)_setLED.g - (int)isColor.g, (int)_setLED.b - (int)isColor.b};
            int totalDeltaColor[] = {(100 * deltaColor[0]) / perzentageTimeToGo, (100 * deltaColor[1]) / perzentageTimeToGo, (100 * deltaColor[2]) / perzentageTimeToGo};
            int updateColor[] = {(int)isColor.r + (totalDeltaColor[0] / 100), (int)isColor.g + (totalDeltaColor[1] / 100), (int)isColor.b + (totalDeltaColor[2] / 100)};

            CRGB newColor(updateColor[0], updateColor[1], updateColor[2]);

            **isLedIter = newColor;
          }
        }
        lastPerzentagTimePassed = perzentageTimePassed;
      }
    }

    if (didTickBefore)
    {
      return false;
    }
    else
    {
      didTickBefore = true;
      return true;
    }
  }

  bool isRunning()
  {
    return didTickBefore;
  }

  void setStartEndTiming(int duration)
  {
    startTime = millis();
    _duration = duration;
    _endTime = startTime + _duration;
  }

  bool deleteDouble(CRGB *newLed)
  {
    for (std::vector<CRGB *>::iterator currentLediter = isLED.begin(); currentLediter != isLED.end();)
    {
      if (*currentLediter == newLed)
      {
        isLED.erase(currentLediter);
        return true;
      }
      else
      {
        currentLediter++;
      }
    }
    return false;
  }
};

class AnimationController
{

  LEDController *_myLedController;
  std::vector<std::unique_ptr<Animation>> myAnimations;

public:
  AnimationController(LEDController *myLedController)
  {
    _myLedController = myLedController;
  }

  void init()
  {
    myAnimations.reserve(ANIMATION_SIZE);
    // Serial.println(myAnimations.capacity());
    // Serial.println(myAnimations.size());
  }

  void clearAllAnimations()
  {
    myAnimations.clear();
    myAnimations.shrink_to_fit();
    myAnimations.reserve(ANIMATION_SIZE);
  }

  void tick()
  {
    unsigned long currentMillis = millis();

    // tick each pixel and word animation
    // change difference slowly between set and is

    for (std::vector<std::unique_ptr<Animation>>::iterator iter = myAnimations.begin(); iter != myAnimations.end();)
    {

      if ((*iter)->_animationDone)
      {
        iter = myAnimations.erase(iter);
      }
      else
      {
        iter++;
      }
    }

    int cnt = 1;
    int cntMax = 0;
    for (std::vector<std::unique_ptr<Animation>>::iterator iter = myAnimations.begin(); iter != myAnimations.end(); cnt++, iter++)
    {
      (*iter)->iD = cnt;
      if (cntMax < cnt)
        cntMax = cnt;
      //Serial.println(iter->iD);
    }

    for (std::vector<std::unique_ptr<Animation>>::iterator tickAnimationIter = myAnimations.begin(); tickAnimationIter != myAnimations.end(); tickAnimationIter++)
    {
      ESP.wdtFeed();
      if ((*tickAnimationIter)->tick())
      { // Animation ticks -> returns true if animation is startet for the first time -> check for doubles
        // Serial.print(iter->iD);
        //Serial.println("  is a new Animation");
        for (std::vector<std::unique_ptr<Animation>>::iterator animationIterator = myAnimations.begin(); animationIterator != myAnimations.end(); animationIterator++)
        {
          //Serial.print("Going to check ID:  ");
          //Serial.println(animationIterator->iD);
          if ((*animationIterator)->iD != (*tickAnimationIter)->iD && (*animationIterator)->isRunning())
          { // check to not delete current pixels from current animation

            for (CRGB *newPixel : (*tickAnimationIter)->isLED)
            {
              // Serial.print("Going to delete Doube in: ");
              //Serial.println(animationIterator->iD);
              (*animationIterator)->deleteDouble(newPixel);
            }
          }
          //return from inner check loop and go to tick next animation in total
          if ((*animationIterator)->iD == cntMax)
          {
            break;
          }
        }
      }
    }
    //Serial.println("AnimationTick Done");
  }

  void addAnimation(Animation *anim)
  { // check if each pixel in the word is in an word or pixel animation and delete it from there

    if (myAnimations.size() < ANIMATION_SIZE - 1)
    {

      myAnimations.push_back(std::unique_ptr<Animation>(anim));
    }
    else
    {
      Serial.println("Animation full");
    }
  }

  void setToRandom(std::vector<CRGB *> leds, byte brightness)
  {
    byte randomHue = random(0L, 255L);
    for (CRGB *led : leds)
    {
      led->setHSV(randomHue, 255, brightness);
    }
  }

  void animationDone(Animation *animation)
  {

    for (std::vector<std::unique_ptr<Animation>>::iterator iter = myAnimations.begin(); iter != myAnimations.end(); iter++)
    {
      if ((*iter)->isLED == animation->isLED)
      {
        myAnimations.erase(iter);
      }
    }
  }

  int getAnimationCount()
  {
    return myAnimations.size();
  }
};

LEDController *myLedController = new LEDController();
AnimationController myAnimationController = AnimationController(myLedController);

WordController *myWordController = new WordController();

std::vector<std::vector<WordConfiguration>> possibleSentences;

void setupWordControllerWithWords()
{
  //Serial.println("start adding Words");
  myWordController->addbasicWord(new Word(myLedController->getLeds(211, 212))); // Es
  //Serial.println("first word added");

  myWordController->addbasicWord(new Word(myLedController->getLeds(214, 216))); // ist
  myWordController->addbasicWord(new Word(myLedController->getLeds(91, 97)));   // Minuten
  myWordController->addbasicWord(new Word(myLedController->getLeds(13, 15)));   // Uhr

  myWordController->addminutesFirstDigitWord(new Word(myLedController->getLeds(181, 184))); // Eine
  myWordController->addminutesFirstDigitWord(new Word(myLedController->getLeds(218, 221))); // Zwei
  myWordController->addminutesFirstDigitWord(new Word(myLedController->getLeds(209, 206))); // drei
  myWordController->addminutesFirstDigitWord(new Word(myLedController->getLeds(186, 189))); // vier
  myWordController->addminutesFirstDigitWord(new Word(myLedController->getLeds(204, 200))); // fuenf
  myWordController->addminutesFirstDigitWord(new Word(myLedController->getLeds(178, 174))); //sechs
  myWordController->addminutesFirstDigitWord(new Word(myLedController->getLeds(151, 156))); //sieben
  myWordController->addminutesFirstDigitWord(new Word(myLedController->getLeds(192, 195))); //acht
  myWordController->addminutesFirstDigitWord(new Word(myLedController->getLeds(222, 225))); //neun
  myWordController->addminutesFirstDigitWord(new Word(myLedController->getLeds(157, 160))); //zehn
  myWordController->addminutesFirstDigitWord(new Word(myLedController->getLeds(198, 196))); //elf
  myWordController->addminutesFirstDigitWord(new Word(myLedController->getLeds(173, 168))); //zwölf

  myWordController->addminutesSecondDigitWord(new Word(myLedController->getLeds(120, 114))); // zwanzig
  myWordController->addminutesSecondDigitWord(new Word(myLedController->getLeds(129, 135))); // dreißig
  myWordController->addminutesSecondDigitWord(new Word(myLedController->getLeds(113, 107))); // vierzig
  myWordController->addminutesSecondDigitWord(new Word(myLedController->getLeds(121, 128))); // fünfzig

  myWordController->addquaterWord(new Word(myLedController->getLeds(148, 142))); //viertel
  myWordController->addquaterWord(new Word(myLedController->getLeds(139, 136))); //halb

  myWordController->addandWord(new Word(myLedController->getLeds(163, 165))); // und minutes
  myWordController->addandWord(new Word(myLedController->getLeds(18, 16)));   // und hours

  myWordController->addbeforeAfterWord(new Word(myLedController->getLeds(103, 105))); // vor
  myWordController->addbeforeAfterWord(new Word(myLedController->getLeds(99, 102)));  // nach

  myWordController->addhoursFirstDigitWord(new Word(myLedController->getLeds(60, 57))); // eins
  myWordController->addhoursFirstDigitWord(new Word(myLedController->getLeds(63, 66))); // zwei
  myWordController->addhoursFirstDigitWord(new Word(myLedController->getLeds(55, 52))); // drei
  myWordController->addhoursFirstDigitWord(new Word(myLedController->getLeds(90, 87))); // vier
  myWordController->addhoursFirstDigitWord(new Word(myLedController->getLeds(80, 76))); // fuenf

  myWordController->addhoursFirstDigitWord(new Word(myLedController->getLeds(51, 47))); // sechs
  myWordController->addhoursFirstDigitWord(new Word(myLedController->getLeds(70, 75))); // sieben
  myWordController->addhoursFirstDigitWord(new Word(myLedController->getLeds(85, 82))); // acht

  myWordController->addhoursFirstDigitWord(new Word(myLedController->getLeds(33, 36))); // neun
  myWordController->addhoursFirstDigitWord(new Word(myLedController->getLeds(26, 23))); // zehn
  myWordController->addhoursFirstDigitWord(new Word(myLedController->getLeds(67, 69))); // elf
  myWordController->addhoursFirstDigitWord(new Word(myLedController->getLeds(40, 45))); // zwölf

  myWordController->addhoursSecondDigitWord(new Word(myLedController->getLeds(3, 9))); // zwanzig
                                                                                       /*
  myWordController->addfillWord(new Word(myLedController->getLeds(213, 213))); // L
  myWordController->addfillWord(new Word(myLedController->getLeds(217, 217))); // O
  myWordController->addfillWord(new Word(myLedController->getLeds(210, 210))); // R
  myWordController->addfillWord(new Word(myLedController->getLeds(216, 216))); // D
  myWordController->addfillWord(new Word(myLedController->getLeds(199, 199))); // L
  myWordController->addfillWord(new Word(myLedController->getLeds(185, 185))); // T
  myWordController->addfillWord(new Word(myLedController->getLeds(190, 191))); // CD
  myWordController->addfillWord(new Word(myLedController->getLeds(180, 179))); // UL
  myWordController->addfillWord(new Word(myLedController->getLeds(190, 191))); // CH
  myWordController->addfillWord(new Word(myLedController->getLeds(161, 162))); // VI
  myWordController->addfillWord(new Word(myLedController->getLeds(150, 149))); // DR
  myWordController->addfillWord(new Word(myLedController->getLeds(141, 140))); // Fo
  myWordController->addfillWord(new Word(myLedController->getLeds(106, 106))); // O
  myWordController->addfillWord(new Word(myLedController->getLeds(98, 98)));   // L
  myWordController->addfillWord(new Word(myLedController->getLeds(86, 86)));   // I
  myWordController->addfillWord(new Word(myLedController->getLeds(81, 81)));   // E
  myWordController->addfillWord(new Word(myLedController->getLeds(61, 62)));   // EI
  myWordController->addfillWord(new Word(myLedController->getLeds(56, 56)));   // X
  myWordController->addfillWord(new Word(myLedController->getLeds(46, 46)));   // D
  myWordController->addfillWord(new Word(myLedController->getLeds(31, 32)));   // MI
  myWordController->addfillWord(new Word(myLedController->getLeds(37, 39)));   // ZUM
  myWordController->addfillWord(new Word(myLedController->getLeds(30, 27)));   // AUCH
  myWordController->addfillWord(new Word(myLedController->getLeds(22, 19)));   // BIND
  myWordController->addfillWord(new Word(myLedController->getLeds(1, 2)));     // IN
  myWordController->addfillWord(new Word(myLedController->getLeds(10, 12)));   // NUI
  */

  myWordController->updateWordCnt();
}

int lastMinute = -1;
int lastSentenceID = -1;
int sentencesCount;
int animationMode = 0;
bool displayNormalNumbers = false;
bool forceReadingMode = false;
CRGB backgroundColor = CRGB::DarkRed;
CRGB wordColor = CRGB::White;

void setLedForTime(int minute, int hour, int seconds, bool forceUpdate = false)
{
  std::vector<Word> digitSentence;
  bool updateNormalNumber = false;
 
  
  if ((lastMinute != minute) || forceUpdate)
  {

    if (displayNormalNumbers)
  {
    digitSentence.clear();

    animationMode = 6;
    int zehnerMinute = floor(minute / 10);
    int einerMinute = minute % 10;

    int zehnerStunde = floor(hour / 10);
    int einerStunde = hour % 10;

    int zehnerSekunde = floor(seconds / 10);
    int einerSekunde = seconds % 10;
    
    digitSentence.push_back(Word(myLedController->getLEDsForChar(0, 0, charList8x5[zehnerStunde + 16]))); // first digit Hour  // 16 is in array 0
    digitSentence.push_back(Word(myLedController->getLEDsForChar(6, 0, charList8x5[einerStunde + 16])));  // second digit Hour  // 16 is in array 0
    digitSentence.push_back(Word(myLedController->getLEDsForChar(0, 8, charList8x5[zehnerMinute + 16]))); // first digit minute  // 16 is in array 0
    digitSentence.push_back(Word(myLedController->getLEDsForChar(6, 8, charList8x5[einerMinute + 16])));  // second digit minute  // 16 is in array 0
    digitSentence.push_back(Word(myLedController->getLEDsForChar(11, 0, charList8x5[26])));               // ":"
    updateNormalNumber = true;
    Serial.print(zehnerStunde);
    Serial.print(einerStunde);
    Serial.print(":");
    Serial.print(zehnerMinute);
    Serial.println(einerMinute);

  }


    sentencesCount = 0;
    lastMinute = minute;
    possibleSentences = myWordController->getWordsForTime(minute, hour);

    for (std::vector<WordConfiguration> wConfigVec : possibleSentences)
    {
      sentencesCount++;
    }
  }

  int secondsForEachSentence = 60 / sentencesCount;
  int sentenceID = floor(seconds / secondsForEachSentence);
  int cnt = 0;

  if ((lastSentenceID != sentenceID)&& !displayNormalNumbers || forceUpdate || updateNormalNumber)
  {
    lastSentenceID = sentenceID;

    std::vector<WordConfiguration> activeSentence = possibleSentences.at(sentenceID);

    // check if sentence is "Minuten vor / nach Halb" -> force reading mode
    forceReadingMode = false;
    for (WordConfiguration config : activeSentence)
    {
      if (config.word == myWordController->basicWords.at(2))
      { // "minutes" is included
        // now check again for also "halb"
        for (WordConfiguration secondConfig : activeSentence)
        {
          if (secondConfig.word == myWordController->quaterWords.at(1))
          { // halb is included
            forceReadingMode = true;
          }
        }
      }
    }

    if (!displayNormalNumbers) // only use different animations for "normal" word cenarios
    {
      if (forceReadingMode)
      {
        animationMode = 1;
      }
      else
      {
        //animationMode = random(0, 6); // 0 to 4
        animationMode++;
        if (animationMode >= 6)
          animationMode = 0;
      }
    }

    
      Serial.println(animationMode);
    switch (animationMode)
    {

    case (0): // "normal" mode for each sentence all Words are faded in, old will fade out // TODO remove or dont display "x minuten vor/ nach halb" da nicht möglich anzuzeigen
    {
      myAnimationController.clearAllAnimations();
      myAnimationController.addAnimation(new Animation(myLedController->getAllLeds(), backgroundColor, 1000));
      myAnimationController.addAnimation(new Animation(activeSentence, wordColor, 1000, 500));
    }
    break;

    case (1): // "reading" mode -> sentence is played in loop until next sentence and within loop, each word will show and dimm in reading order
    {
      myAnimationController.clearAllAnimations();
      int wordCnt = activeSentence.size();
      int readingTimeForWord = 600;
      int fadeOutDelay = 500;
      myAnimationController.addAnimation(new Animation(myLedController->getAllLeds(), backgroundColor, fadeOutDelay));

      byte sentenceRepetitions = floor((secondsForEachSentence * 1000) / (wordCnt * readingTimeForWord));

      int timeForWord = (secondsForEachSentence * 1000) / (sentenceRepetitions * wordCnt);

      for (int repetition = 0; repetition < sentenceRepetitions; repetition++)
      {

        int cnt = 0;

        for (std::vector<WordConfiguration>::iterator wordIter = activeSentence.begin(); wordIter < activeSentence.end(); wordIter++, cnt++)
        {

          int delay = cnt * timeForWord;
          delay += ((secondsForEachSentence * 1000) / sentenceRepetitions) * repetition;
          myAnimationController.addAnimation(new Animation((*wordIter).getLeds((*wordIter).digitsToRemove), wordColor, 80, delay + fadeOutDelay));
          myAnimationController.addAnimation(new Animation((*wordIter).getLeds((*wordIter).digitsToRemove), backgroundColor, 100, delay + timeForWord - 200 + fadeOutDelay));
        }
      }
    }

    break;

    case (2): // "Snake" ->  One Pixel is moving across the screen (circular pattern) and pixel with correspondig word are kept on
    {
      int outerDist = 7;
      int cnt = 0;
      uint8_t r = random8();
      uint8_t g = random8();
      uint8_t b = random8();
      myAnimationController.clearAllAnimations();
      for (int x = outerDist; x >= 0; x--)
      {
        std::vector<CRGB *> leds = myLedController->getSquareLeds(x);
        leds.pop_back(); // remove last LED -> a snake is not a square
        leds.shrink_to_fit();

        int index = 0;
        for (std::vector<CRGB *>::iterator ledIter = leds.begin(); ledIter != leds.end(); ledIter++)
        {
          int delay = cnt * 10;
          int isLedOfSetWord = false;
          myAnimationController.addAnimation(new Animation(*ledIter, CRGB(r, g, b), 10, delay));

          // check if current led is part of a new word, so it shouldnt turn off, but to the set wordColor
          for (WordConfiguration wordConfig : activeSentence)
          {
            if (isLedOfSetWord)
              break; // if led is already found in activeSentence -> continue
            for (CRGB *led : wordConfig.getLeds())
            {
              if (led == *ledIter)
              {
                isLedOfSetWord = true;
                break;
              }
            }
          }

          if (isLedOfSetWord)
          {
            myAnimationController.addAnimation(new Animation(*ledIter, wordColor, 300, delay + 400));
          }
          else
          {
            myAnimationController.addAnimation(new Animation(*ledIter, backgroundColor, 300, delay + 400));
          }

          cnt++;
          index++;
        }
      }
    }
    break;

    case (3): // "Square" -> square gets bigger from center and clears all Leds, when get smaller to center again, new sentence stay on
    {
      myAnimationController.clearAllAnimations();
      uint8_t r = random8();
      uint8_t g = random8();
      uint8_t b = random8();
      for (int i = 0; i < 8; i++)
      {
        std::vector<CRGB *> squareLeds = myLedController->getSquareLeds(i);
        myAnimationController.addAnimation(new Animation(squareLeds, CRGB(r, g, b), 100, i * 100));

        int isLedOfSetWord = false;

        for (CRGB *squareLed : squareLeds)
        {

          for (WordConfiguration wordConfig : activeSentence)
          {

            for (CRGB *led : wordConfig.getLeds())
            {
              if (led == squareLed)
              {
                myAnimationController.addAnimation(new Animation(squareLed, wordColor, 400, (i * 100) + 100));
                goto continueWithNextSquareLed;
              }
            }
          }

          myAnimationController.addAnimation(new Animation(squareLed, backgroundColor, 400, (i * 100) + 100));

        continueWithNextSquareLed:;
        }
      }
    }
    break;

    case (4): // "line from left to right / top bot / right left...."
    {
      myAnimationController.clearAllAnimations();
      int delay = 100;
      int randOrientation = random(0, 2);
      for (int x = 0; x < 15; x++)
      {
        std::vector<CRGB *> ledsNotPartOfSentence;
        ledsNotPartOfSentence.clear();

        std::vector<CRGB *> lineLeds = myLedController->getLineLeds(x, randOrientation);
        // add animation for each line to fade to word color
        myAnimationController.addAnimation(new Animation(lineLeds, wordColor, 500, delay * x));
        for (CRGB *lineLed : lineLeds)
        {
          // now check each line led if it  is part of the new sentence and should fade to background color or stay in wordcolor
          for (WordConfiguration wordConfig : activeSentence)
          {

            for (CRGB *led : wordConfig.getLeds())
            {
              if (led == lineLed)
              {
                // led is part of a new sentence -> fade from above should stay.
                //myAnimationController.addAnimation(new Animation(lineLed, wordColor, 400, (x * 200) + 500));
                goto continueWithNextLineLed;
              }
            }
          }

          //myAnimationController.addAnimation(new Animation(lineLed, backgroundColor, 400, (x * 200) + 500));
          ledsNotPartOfSentence.push_back(lineLed);
        continueWithNextLineLed:;
        }
        // all leds that aren´t part of a Word can fade to backgroundColor
        myAnimationController.addAnimation(new Animation(ledsNotPartOfSentence, backgroundColor, 400, (x * 200) + 500));
      }
    }
    break;

    case (5): // "Random words" ->  Random words turn on in random color, but only right sentence words turn on in wordColor
    {
      myAnimationController.clearAllAnimations();
      myAnimationController.addAnimation(new Animation(myLedController->getAllLeds(), backgroundColor, 500));

      int delay = 800;
      int randomWordsCnt = random(5, 10);
      for (int x = 0; x < randomWordsCnt; x++)
      {
        Word *randomWord = myWordController->getRandomWord();

        // check if random word is in current active sentence -> get new Word
        for (WordConfiguration activeWord : activeSentence)
        {
          while (activeWord.word == randomWord)
          {
            randomWord = myWordController->getRandomWord();
          }
        }

        int randomDelay = delay * x + random(200, 700);
        myAnimationController.addAnimation(new Animation(randomWord->wordLeds, CRGB(random8(), random8(), random8()), 500, randomDelay));
        myAnimationController.addAnimation(new Animation(randomWord->wordLeds, backgroundColor, 800, randomDelay + random(1200, 2000)));
      }

      int cnt = 0;
      for (std::vector<WordConfiguration>::iterator wordIter = activeSentence.begin(); wordIter < activeSentence.end(); wordIter++, cnt++)
      {

        int randomDelay = delay * cnt + random(10, 500);
        myAnimationController.addAnimation(new Animation((*wordIter).getLeds(), CRGB(random8(), random8(), random8()), 500, randomDelay));
        myAnimationController.addAnimation(new Animation((*wordIter).getLeds(), wordColor, 800, randomDelay + random(1200, 2000)));
      }
    }

    break;

    case (6): // "display normal digits-> use digitSentence -> each entry is a Word with the leds for a digit (1-9) display also ":" and maybe seconds with single pixels
    {
      myAnimationController.clearAllAnimations();
      myAnimationController.addAnimation(new Animation(myLedController->getAllLeds(), backgroundColor, 200));

      for (Word wordDigit : digitSentence)
      {
        myAnimationController.addAnimation(new Animation(wordDigit.wordLeds, wordColor, 600, 100));
      }
    }

    break;
    
    }

    //trigger Animation with WconfigVector and decide wich Animation
  }
}

void printLEDDataSerial()
{

  for (int x = 0; x < 15; x++)
  {

    int firstIndex = NUM_LEDS - (x + 1) * 15;
    int y;

    if (((x + 1) % 2) == 1)
    { // L->R
      for (int z = 0; z < 15; z++)
      {

        y = myLedController->getLED(firstIndex + z)->r;
        //Serial.print(firstIndex+z);
        Serial.print(" ");
        Serial.print(y / 100);
        Serial.print((y / 10) % 10);
        Serial.print(y % 10);
      }
    }
    else
    { // R ->L
      for (int z = 0; z < 15; z++)
      {

        y = myLedController->getLED(firstIndex + 14 - z)->r;
        Serial.print(" ");
        Serial.print(y / 100);
        Serial.print((y / 10) % 10);
        Serial.print(y % 10);

        // Serial.print(firstIndex+14-z);
      }
    }
    Serial.println();
  }

  Serial.println();
  Serial.println();
}

//--------Variables for Handling the current Time -> will be increased in loop every 1000 millis---------
static int h = random(1, 12);
static int m = random(0, 60);
static int s = 0;

void setNewTime(int newH, int newM)
{
  s = 0;
    setLedForTime(newM, newH, s, true);
  h = newH;
  m = newM;
}

//-------------------------------WebServer-----------------------

bool newTime = false;
DNSServer dnsServer;
AsyncWebServer webServer(80);

struct tcp_pcb;
extern struct tcp_pcb *tcp_tw_pcbs;
extern "C" void tcp_abort(struct tcp_pcb *pcb);

void tcpCleanup(void)
{
  while (tcp_tw_pcbs)
    tcp_abort(tcp_tw_pcbs);
}

String splitString(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++)
  {
    if (data.charAt(i) == separator || i == maxIndex)
    {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }

  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void handleTimeForm(String time, String _wordColor, String _backgroundColor)
{
  //String s = webServer.arg("clockTime");

  String hstring = splitString(time, ':', 0);
  String mstring = splitString(time, ':', 1);

  int hNumber = hstring.toInt();
  int mNumber = mstring.toInt();

  //String _wordColor = webServer.arg("wordColor");
  //String _backgroundColor = webServer.arg("backgroundColor");

  //wordColor = CRGB((int)strtoll(&_wordColor[1], NULL, 16));
  //backgroundColor = CRGB((int)strtoll(&_backgroundColor[1], NULL, 16));

  setNewTime(hNumber, mNumber);
}

String clockTimeParamString, wordColorString, backgroundColorString;

class CaptiveRequestHandler : public AsyncWebHandler
{
public:
  CaptiveRequestHandler() {}
  virtual ~CaptiveRequestHandler() {}

  bool canHandle(AsyncWebServerRequest *request)
  {
    //request->addInterestingHeader("ANY");
    return true;
  }

  void handleRequest(AsyncWebServerRequest *request)
  {


    int params = request->params();
    /*
    for (int i = 0; i < params; i++)
    {
      AsyncWebParameter *p = request->getParam(i);
      if (p->isFile())
      { //p->isPost() is also true
        Serial.printf("FILE[%s]: %s, size: %u\n", p->name().c_str(), p->value().c_str(), p->size());
      }
      else if (p->isPost())
      {
        Serial.printf("POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
      }
      else
      {
        Serial.printf("GET[%s]: %s\n", p->name().c_str(), p->value().c_str());
      }
    }
    */
    int hNumber = -1;
    int mNumber = -1;
    //get all the URL parameters from request and if all are present update time and colors

    if (request->hasParam("clockTime"))
    {
      AsyncWebParameter *clockTimeParam = request->getParam("clockTime");
      if (request->hasParam("backgroundColor"))
      {
        AsyncWebParameter *_backgroundColor = request->getParam("backgroundColor");
        if (request->hasParam("wordColor"))
        {

          if (request->hasParam("checkBoxNormalClock") && params > 0)
          {
            displayNormalNumbers = true;
          }
          else
          {
            displayNormalNumbers = false;
          }
          AsyncWebParameter *_wordColor = request->getParam("wordColor");

          clockTimeParamString = clockTimeParam->value().c_str();
          backgroundColorString = _backgroundColor->value().c_str();
          wordColorString = _wordColor->value().c_str();

          String hstring = splitString(clockTimeParamString, ':', 0);
          String mstring = splitString(clockTimeParamString, ':', 1);

          hNumber = hstring.toInt();
          mNumber = mstring.toInt();

          //String _wordColor = webServer.arg("wordColor");
          //String _backgroundColor = webServer.arg("backgroundColor");

          backgroundColor = CRGB((int)strtoll(&backgroundColorString[1], NULL, 16));
          wordColor = CRGB((int)strtoll(&wordColorString[1], NULL, 16));

          newTime = true;
        }
      }
    }

    AsyncResponseStream *response = request->beginResponseStream("text/html");

    response->print("<!DOCTYPE html><html lang='de'><head>");
    response->print("<meta name='viewport' content='width=device-width'>");
    response->print("<style>");
    response->print("li{");
    response->print("list-style-type: none;");
    response->print("border-color: white;");
    response->print("border-style: solid;");
    response->print("text-align: center;");
    response->print("}");
    response->print("</style>");
    response->print("</head>");
    response->print("<body>");
    response->print("<form action=\"/timeentered\">");
    response->print("<ul>");
    response->print("<li>");
    response->print("<h1>WordClock Configurator</h1>");
    response->print("<label for=\"timeform\">Uhrzeit einstellen:  </label>");
    response->printf("<input type=\"time\" id=\"timeform\" name=\"clockTime\" value=\"%02i:%02i\" autofocus required>", hNumber == -1 ? h : hNumber, mNumber == -1 ? m : mNumber);
    response->print("</li>");
    response->print("<li>");
    response->print("<label for=\"background\">Hintergrund Farbe:  </label>");
    response->printf("<input type= \"color\" id = \"backgroundColor\" name=\"backgroundColor\" value =\"#%02X%02X%02X\" required>", backgroundColor.raw[0], backgroundColor.raw[1], backgroundColor.raw[2]);
    response->print("</li>");
    response->print("<li>");
    response->print("<label for=\"wordColor\">Woerter Farbe:  </label>");
    response->printf("<input type= \"color\" id = \"wordColor\" name=\"wordColor\" value=\"#%02X%02X%02X\" required>", wordColor.raw[0], wordColor.raw[1], wordColor.raw[2]);
    response->print("</li>");
    response->print(" <li>");
    response->print("<label for=\"checkbox\">Normale Ziffern anzeigen:  </label>");
    response->printf("<input type= \"checkbox\" id = \"setNormalClock\" name=\"checkBoxNormalClock\" value=\"displayNormalClock\" %s>", displayNormalNumbers ? "checked" : "");
    response->print("</li>");
    response->print(" <li>");
    response->print("<input type=\"submit\" id =\"button\" onclick = \"setTimeout()\"  value=\"Ok\">");
    response->print("</li>");
    response->print("</ul>");
    response->print("</form>");
    response->print("<script>");
    response->print("function setTimeout() {");
    response->print("document.getElementById(\"button\").disabled=true;");
    response->print("setTimeout(\"document.getElementById(\"button\").disabled=false;\",2000);");
    response->print("}");
    response->print("</script>");
    response->print("</body></html>");

    request->send(response);
  }
};

void setupAndStartWifi()
{

  const String WSSID = "WordClockSetup";
  const String PASS = "12345677654321";
  WiFi.persistent(false);
  WiFi.mode(WIFI_AP);
  WiFi.softAP(WSSID, PASS, 1, true, 1);
  // reply to all ip requests with ip of this device
  dnsServer.start(53, "*", WiFi.softAPIP());
  webServer.addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER); //only when requested from AP

  webServer.begin();
}

//-----------------------Standard Arduino --------------------------------

void setup()
{

  Serial.begin(115200);

  setupAndStartWifi();

  myLedController->setup(); // disable for debug // Add leds to FastLED Library
  //Serial.println("Setup Done");
  setupWordControllerWithWords(); // Vectoren mit Wörten füllen (mappen von led indexen)
  //Serial.println("Words created");
  myAnimationController.init();
  myLedController->turnAllOn(CRGB::DarkGreen); // show that every Led is working
  FastLED.setBrightness(200);

  // delay(200);
  //Serial.println("booted");
}

unsigned long lastIncrease = 0;

int xpos = 7;
int ypos = 7;

void loop()
{
  static unsigned long lastLedUpdate = 0;

  if (lastIncrease + 1000 < millis())
  {

    lastIncrease = millis();
    s++;
    if (s > 59)
    {
      s = 0;
      m++;
      if (m > 59)
      {
        h++;
        m = 0;
        if (h > 23)
        {
          h = 0;
        }
      }
    }
    setLedForTime(m, h, s);
  }

  if (newTime)
  {
    newTime = false;
    handleTimeForm(clockTimeParamString, wordColorString, backgroundColorString);
  }

  
  //   Serial.print("Heap:  ");

  //Serial.println(ESP.getFreeHeap());
  /*
  Serial.print("  HeapFragmentation:  ");
  Serial.print(ESP.getHeapFragmentation());
  Serial.print("  Heap MaxBlockSize:  ");
  Serial.print(ESP.getMaxFreeBlockSize());

  Serial.print("  Stack:  ");
  Serial.println(ESP.getFreeContStack());
*/
  if (millis() > lastLedUpdate + LED_UPDATE_DELAY)
  {
    lastLedUpdate = millis();
    myLedController->output();
  }

  myAnimationController.tick();

  dnsServer.processNextRequest();
  tcpCleanup();
}
