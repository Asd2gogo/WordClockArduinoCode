#include <Arduino.h>
#line 1 "c:\\Users\\Sven Weinert\\OneDrive - Sennheiser electronic GmbH & Co. KG\\Projekte\\WordClock\\Arduino\\LEDController\\LEDController.ino"


#include <StandardCplusplus.h>
#include <vector>
#include <FastLED.h>
#include <arduino.h>




class LEDController
  {
#define NUM_LEDS 225

CRGB leds[NUM_LEDS];

  public:
  LEDController()
  {
   
  }

  void setup(){
    FastLED.addLeds<NEOPIXEL, 6>(leds, NUM_LEDS);
  }

  void turnOn(int index, CRGB color)
  {
    leds[index] = color;
  }

  void turnOff(int index)
  {
    leds[index] = CRGB::Black;
  }

  void turnAllOn(CRGB color)
  {
    for (int x = 0; x < NUM_LEDS; x++)
    {
      leds[x] = color;
    }
  }

  void turnAllOff(){
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
  { // todo check richtung .. manchmal erster buchstabe ist großer als letzer

    std::vector<CRGB *> returnVector;

    if (startIndex < endIndex)
    { // Normale Orientierung (ungerade Reihen)

      for (int x = startIndex - 1; x <= endIndex - 1; x++)
      {
        returnVector.push_back(&leds[x]);
      }
    }
    else // Invertiert Orientierung (gerade Reihen)
    {
      for (int x = endIndex - 1; x >= startIndex - 1; x--)
      {
        returnVector.push_back(&leds[x]);
      }
    }
    return returnVector;
  }

  void show()
  {
    FastLED.show();
  }
};

class Word
{
  std::vector<CRGB *> wordLeds;
  CRGB color = CRGB::Yellow;
  int mDigitsToRemove = 0;

public:
  Word(std::vector<CRGB *> leds)
  {

    for (CRGB *led : leds)
    {
      wordLeds.push_back(led);
    }
  }

  void makeSingular(int digitsToRemove)
  {
    mDigitsToRemove = digitsToRemove;
    int size = wordLeds.size();
    for (int x = 0; x < digitsToRemove; x++)
    {
      *wordLeds.at(size - (1 + x)) = CRGB::Black;
    }
  }

  void turnOn(CRGB col = CRGB::White)
  {

    for (CRGB *led : wordLeds)
    {
      *led = color;
    }
  }

  void turnOff()
  {
    for (CRGB *led : wordLeds)
    {
      *led = CRGB::Black;
    }
  }

  void setColor(CRGB c)
  {
    color = c;
  }
};

class WordConfiguration
{

public:
  Word *word;
  int digitsToRemove = 0; // digits to turn off from push_back
  CRGB color;

  WordConfiguration(Word *w, int digits = 0, CRGB c = CRGB::White)
  {
    word = w;
    digitsToRemove = digits;
    color = c;
  }
};

class WordController
{

  std::vector<Word *> allWords;
  std::vector<Word *> basicWords;
  std::vector<Word *> minutesFirstDigitWords;
  std::vector<Word *> minutesSecondDigitWords;
  std::vector<Word *> quaterWords;
  std::vector<Word *> andWords;
  std::vector<Word *> beforeAfterWords;
  std::vector<Word *> hoursFirstDigitWords;
  std::vector<Word *> hoursSecondDigitWords;
  std::vector<Word *> fillWords;

public:
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

  void addToAllWords(Word *word)
  {
    allWords.push_back(word);
  }

  void addfillWord(Word *word)
  {
    fillWords.push_back(word);
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
        if (zehner != 1)
        {
          returnVector.push_back(WordConfiguration(minutesFirstDigitWords.at(0))); // "eine"
        }
        if (zehner > 1)
        {
          returnVector.at(0).digitsToRemove = 1; // convert "eine" zu "ein"
          returnVector.push_back(WordConfiguration(andWords.at(0)));
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
    }

    switch (zehner)
    {
    case 0:

      break;
    case 1:
      returnVector.push_back(WordConfiguration(minutesFirstDigitWords.at(9)));
      break;
    default:
      returnVector.push_back(WordConfiguration(minutesSecondDigitWords.at(zehner - 2)));
      break;
    }

    return returnVector;
  }

  std::vector<WordConfiguration> getHourWords(int hour)
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
      returnVector.at(0).digitsToRemove = 1;
    }
    else if (hour <= 12)
    {
      returnVector.push_back(WordConfiguration(hoursFirstDigitWords.at(hour - 1))); //jeweilige stunde
    }
    else if (hour == 16)
    {
      returnVector.push_back(WordConfiguration(hoursFirstDigitWords.at(5))); // sech
      returnVector.at(0).digitsToRemove = 1;
    }
    else if (hour == 17)
    {
      returnVector.push_back(WordConfiguration(hoursFirstDigitWords.at(6))); //7
      returnVector.at(0).digitsToRemove = 2;
    }
    else if (hour <= 20)
    {
      returnVector.push_back(WordConfiguration(hoursFirstDigitWords.at(hour - 11))); // jeweilige erster teil der stunde 3 / 4 / 5 / 6 / 8 / 9
    }
    else if (hour == 21)
    {
      returnVector.push_back(WordConfiguration(hoursFirstDigitWords.at(0))); //ein
      returnVector.at(0).digitsToRemove = 1;
    }
    else if (hour > 21)
    {
      returnVector.push_back(WordConfiguration(hoursFirstDigitWords.at(hour - 21))); //jeweilige stunde
    }

    if (hour < 20 && hour > 12)
    {
      returnVector.push_back(WordConfiguration(andWords.at(1)));             //und
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

    returnVector.push_back(timeQuaterWords);
    returnVector.push_back(timeQuaterEastWords);
    returnVector.push_back(timeMinutesPastWords);
    returnVector.push_back(timeMinutesBeforeWords);
    returnVector.push_back(timeOClockWords);
    returnVector.push_back(timeMiutesBeforeAfterHalfWords);

    if (minute == 0)
    { // oClock
      for (WordConfiguration wordConfiguration : getHourWords(hour))
      {
        timeOClockWords.push_back(wordConfiguration);
      }
      timeOClockWords.push_back(WordConfiguration(basicWords.at(3)));
    }

    if (minute != 0)
    { // before next hour / after current hour

      //Minutes before next Hour "fuenf zehn Minuten vor zehn"
      int minutesBeforeNextHour = (60 - minute);
      for (WordConfiguration wordConfiguration : getMinutsWords(minutesBeforeNextHour))
      { // x (Minuten)
        timeMinutesBeforeWords.push_back(wordConfiguration);
      }
      timeMinutesBeforeWords.push_back(WordConfiguration(basicWords.at(2))); // Minuten
      if (minutesBeforeNextHour == 1)                                        //todo minutes to minute

        timeMinutesBeforeWords.push_back(WordConfiguration(beforeAfterWords.at(0))); // vor

      int nextHour = (hour + 1) % 24;
      int twelfHourFormat = (nextHour % 13) + floor(nextHour / 13); // 10 11 12 1 2 3
      for (WordConfiguration wordConfiguration : getHourWords(twelfHourFormat))
      {
        timeMinutesBeforeWords.push_back(wordConfiguration); // add all Words nessesarry for the hour ("zwei und zwanzig")
      }
      //timeMinutesBeforeWords.push_back(basicWords.at(3)); // Uhr

      // after current hour "fuenf und viertig zehn Minuten nach neun"
      for (WordConfiguration wordConfiguration : getMinutsWords(minute))
      { // x (Minuten)
        timeMinutesPastWords.push_back(wordConfiguration);
      }
      timeMinutesPastWords.push_back(WordConfiguration(basicWords.at(2))); // Minuten

      timeMinutesPastWords.push_back(WordConfiguration(beforeAfterWords.at(1))); // nach

      twelfHourFormat = (hour % 13) + floor(hour / 13); // 10 11 12 1 2 3
      for (WordConfiguration wordConfiguration : getHourWords(twelfHourFormat))
      {
        timeMinutesPastWords.push_back(wordConfiguration); // add all Words nessesarry for the hour ("zwei und zwanzig")
      }
      // timeMinutesPastWords.push_back(basicWords.at(3)); // Uhr
    }

    if (minute == 15)
    { // viertel nach current hour // viertel nextHour

      //"viertel nach eins"
      timeQuaterWords.push_back(WordConfiguration(quaterWords.at(0))); // viertel

      timeQuaterWords.push_back(WordConfiguration(beforeAfterWords.at(1))); // nach

      int twelfHourFormat = (hour % 13) + floor(hour / 13); // 10 11 12 1 2 3

      for (WordConfiguration wordConfiguration : getHourWords(twelfHourFormat))
      {
        timeQuaterWords.push_back(wordConfiguration);
      }

      // "Viertel zwei"
      timeQuaterEastWords.push_back(WordConfiguration(quaterWords.at(0))); // viertel

      int nextHour = hour + 1;
      twelfHourFormat = (nextHour % 13) + floor(nextHour / 13); // 10 11 12 1 2 3

      for (WordConfiguration wordConfiguration : getHourWords(twelfHourFormat))
      {
        timeQuaterWords.push_back(wordConfiguration);
      }
    }

    if (minute == 30)
    { // halb
      //"Halb drei"
      timeQuaterWords.push_back(WordConfiguration(quaterWords.at(1))); // halb
      int nextHour = hour + 1;
      int twelfHourFormat = (nextHour % 13) + floor(nextHour / 13); // 10 11 12 1 2 3

      for (WordConfiguration wordConfiguration : getHourWords(twelfHourFormat))
      {
        timeQuaterWords.push_back(wordConfiguration);
      }
    }

    if (minute == 45)
    { // viertel vor next hour // dreiviertel nextHour

      timeQuaterWords.push_back(WordConfiguration(quaterWords.at(0))); // viertel

      timeQuaterWords.push_back(WordConfiguration(beforeAfterWords.at(0))); // vor

      int nextHour = hour + 1;
      int twelfHourFormat = (nextHour % 13) + floor(nextHour / 13); // 10 11 12 1 2 3

      for (WordConfiguration wordConfiguration : getHourWords(twelfHourFormat))
      {
        timeQuaterWords.push_back(wordConfiguration);
      }

      // "drei viertel zwei"

      timeQuaterEastWords.push_back(WordConfiguration(minutesFirstDigitWords.at(2))); // drei
      timeQuaterEastWords.push_back(WordConfiguration(quaterWords.at(0)));            // viertel

      twelfHourFormat = (nextHour % 13) + floor(nextHour / 13); // 10 11 12 1 2 3

      for (WordConfiguration wordConfiguration : getHourWords(twelfHourFormat))
      {
        timeQuaterWords.push_back(wordConfiguration);
      }
    }

    if (minute >= 15 && minute <= 45)
    { // minutes before / after halb

      //"zehn Minuten vor Halb drei"
      int deltaMinutes = abs(minute - 30);
      for (WordConfiguration wordConfiguration : getMinutsWords(deltaMinutes))
      {
        timeMiutesBeforeAfterHalfWords.push_back(wordConfiguration); // drei, vier ....
      }
      timeMiutesBeforeAfterHalfWords.push_back(WordConfiguration(basicWords.at(2), deltaMinutes == 1 ? 1 : 0)); // "Minuten" // check for singular

      timeMiutesBeforeAfterHalfWords.push_back(WordConfiguration(beforeAfterWords.at(minute > 30 ? 1 : 0))); // "vor" / "nach" // check for vor or nach

      int nextHour = hour + 1;
      int twelfHourFormat = (nextHour % 13) + floor(nextHour / 13); // 10 11 12 1 2 3

      for (WordConfiguration wordConfiguration : getHourWords(twelfHourFormat))
      {
        timeMiutesBeforeAfterHalfWords.push_back(wordConfiguration); // "eins" // "zwei"
      }
    }

    for (std::vector<WordConfiguration> vector : returnVector)
    {

      if (vector.size() != 0)
      {
        vector.push_back(WordConfiguration(basicWords.at(0))); //Es
        vector.push_back(WordConfiguration(basicWords.at(1))); //ist
      }
      /*
      else{
        std::vector<std::vector<WordConfiguration>>::iterator itr = std::find(returnVector.begin(), returnVector.end(), vector); // find vector in returnVector
        returnVector.erase(itr); // delete WordConfigurationVector if Empty (currently no matching time for that)
      }
      */
    }
    return returnVector;
  }
};

LEDController myLedController;
WordController myWordController;

#line 522 "c:\\Users\\Sven Weinert\\OneDrive - Sennheiser electronic GmbH & Co. KG\\Projekte\\WordClock\\Arduino\\LEDController\\LEDController.ino"
void setupWordControllerWithWords();
#line 601 "c:\\Users\\Sven Weinert\\OneDrive - Sennheiser electronic GmbH & Co. KG\\Projekte\\WordClock\\Arduino\\LEDController\\LEDController.ino"
void setLedForTime(int minute, int hour, int seconds);
#line 630 "c:\\Users\\Sven Weinert\\OneDrive - Sennheiser electronic GmbH & Co. KG\\Projekte\\WordClock\\Arduino\\LEDController\\LEDController.ino"
void setup();
#line 647 "c:\\Users\\Sven Weinert\\OneDrive - Sennheiser electronic GmbH & Co. KG\\Projekte\\WordClock\\Arduino\\LEDController\\LEDController.ino"
void loop();
#line 522 "c:\\Users\\Sven Weinert\\OneDrive - Sennheiser electronic GmbH & Co. KG\\Projekte\\WordClock\\Arduino\\LEDController\\LEDController.ino"
void setupWordControllerWithWords()
{
  myWordController.addbasicWord(new Word(myLedController.getLeds(211, 212))); // Es
  myWordController.addbasicWord(new Word(myLedController.getLeds(214, 216))); // ist
  myWordController.addbasicWord(new Word(myLedController.getLeds(91, 97)));   // Minuten
  myWordController.addbasicWord(new Word(myLedController.getLeds(13, 15)));   // Uhr

  myWordController.addminutesFirstDigitWord(new Word(myLedController.getLeds(181, 184))); // Eine
  myWordController.addminutesFirstDigitWord(new Word(myLedController.getLeds(218, 221))); // Zwei
  myWordController.addminutesFirstDigitWord(new Word(myLedController.getLeds(209, 206))); // drei
  myWordController.addminutesFirstDigitWord(new Word(myLedController.getLeds(186, 189))); // vier
  myWordController.addminutesFirstDigitWord(new Word(myLedController.getLeds(204, 200))); // fuenf
  myWordController.addminutesFirstDigitWord(new Word(myLedController.getLeds(178, 174))); //sechs
  myWordController.addminutesFirstDigitWord(new Word(myLedController.getLeds(151, 156))); //sieben
  myWordController.addminutesFirstDigitWord(new Word(myLedController.getLeds(192, 195))); //acht
  myWordController.addminutesFirstDigitWord(new Word(myLedController.getLeds(222, 225))); //neun
  myWordController.addminutesFirstDigitWord(new Word(myLedController.getLeds(157, 160))); //zehn
  myWordController.addminutesFirstDigitWord(new Word(myLedController.getLeds(198, 196))); //elf
  myWordController.addminutesFirstDigitWord(new Word(myLedController.getLeds(173, 168))); //zwölf

  myWordController.addminutesSecondDigitWord(new Word(myLedController.getLeds(120, 114))); // zwanzig
  myWordController.addminutesSecondDigitWord(new Word(myLedController.getLeds(129, 135))); // dreißig
  myWordController.addminutesSecondDigitWord(new Word(myLedController.getLeds(148, 142))); // vierzig
  myWordController.addminutesSecondDigitWord(new Word(myLedController.getLeds(121, 128))); // fünfzig

  myWordController.addquaterWord(new Word(myLedController.getLeds(148, 142))); //viertel
  myWordController.addquaterWord(new Word(myLedController.getLeds(139, 136))); //halb

  myWordController.addandWord(new Word(myLedController.getLeds(163, 165))); // und minutes
  myWordController.addandWord(new Word(myLedController.getLeds(18, 16)));   // und hours

  myWordController.addbeforeAfterWord(new Word(myLedController.getLeds(103, 105))); // vor
  myWordController.addbeforeAfterWord(new Word(myLedController.getLeds(99, 102)));  // nach

  myWordController.addhoursFirstDigitWord(new Word(myLedController.getLeds(60, 57))); // eins
  myWordController.addhoursFirstDigitWord(new Word(myLedController.getLeds(63, 66))); // zwei
  myWordController.addhoursFirstDigitWord(new Word(myLedController.getLeds(55, 52))); // drei
  myWordController.addhoursFirstDigitWord(new Word(myLedController.getLeds(90, 87))); // vier
  myWordController.addhoursFirstDigitWord(new Word(myLedController.getLeds(80, 76))); // fuenf
  myWordController.addhoursFirstDigitWord(new Word(myLedController.getLeds(51, 47))); // sechs
  myWordController.addhoursFirstDigitWord(new Word(myLedController.getLeds(70, 75))); // sieben
  myWordController.addhoursFirstDigitWord(new Word(myLedController.getLeds(85, 82))); // acht
  myWordController.addhoursFirstDigitWord(new Word(myLedController.getLeds(33, 36))); // neun
  myWordController.addhoursFirstDigitWord(new Word(myLedController.getLeds(26, 23))); // zehn
  myWordController.addhoursFirstDigitWord(new Word(myLedController.getLeds(67, 69))); // elf
  myWordController.addhoursFirstDigitWord(new Word(myLedController.getLeds(40, 45))); // zwölf

  myWordController.addhoursSecondDigitWord(new Word(myLedController.getLeds(3, 9))); // zwanzig

  myWordController.addfillWord(new Word(myLedController.getLeds(213, 213))); // L
  myWordController.addfillWord(new Word(myLedController.getLeds(217, 217))); // O
  myWordController.addfillWord(new Word(myLedController.getLeds(210, 210))); // R
  myWordController.addfillWord(new Word(myLedController.getLeds(216, 216))); // D
  myWordController.addfillWord(new Word(myLedController.getLeds(199, 199))); // L
  myWordController.addfillWord(new Word(myLedController.getLeds(185, 185))); // T
  myWordController.addfillWord(new Word(myLedController.getLeds(190, 191))); // CD
  myWordController.addfillWord(new Word(myLedController.getLeds(180, 179))); // UL
  myWordController.addfillWord(new Word(myLedController.getLeds(190, 191))); // CH
  myWordController.addfillWord(new Word(myLedController.getLeds(161, 162))); // VI
  myWordController.addfillWord(new Word(myLedController.getLeds(150, 149))); // DR
  myWordController.addfillWord(new Word(myLedController.getLeds(141, 140))); // Fo
  myWordController.addfillWord(new Word(myLedController.getLeds(106, 106))); // O
  myWordController.addfillWord(new Word(myLedController.getLeds(98, 98)));   // L
  myWordController.addfillWord(new Word(myLedController.getLeds(86, 86)));   // I
  myWordController.addfillWord(new Word(myLedController.getLeds(81, 81)));   // E
  myWordController.addfillWord(new Word(myLedController.getLeds(61, 62)));   // EI
  myWordController.addfillWord(new Word(myLedController.getLeds(56, 56)));   // X
  myWordController.addfillWord(new Word(myLedController.getLeds(46, 46)));   // D
  myWordController.addfillWord(new Word(myLedController.getLeds(31, 32)));   // MI
  myWordController.addfillWord(new Word(myLedController.getLeds(37, 39)));   // ZUM
  myWordController.addfillWord(new Word(myLedController.getLeds(30, 27)));   // AUCH
  myWordController.addfillWord(new Word(myLedController.getLeds(22, 19)));   // BIND
  myWordController.addfillWord(new Word(myLedController.getLeds(1, 2)));     // IN
  myWordController.addfillWord(new Word(myLedController.getLeds(10, 12)));   // NUI
}

int lastMinute = -1;
std::vector<std::vector<WordConfiguration>> possibleSentences;

void setLedForTime(int minute, int hour, int seconds)
{

  int sentencesCount = 0;
  int secondsForEachSentence = 60 / sentencesCount;
  int sentenceID = floor(seconds / secondsForEachSentence);
  int lastSentenceID = -1;

  if (lastMinute != minute)
  {
    possibleSentences = myWordController.getWordsForTime(minute, hour);

    for (std::vector<WordConfiguration> wConfigVec : possibleSentences)
    {
      sentencesCount++;
    }
  }
  if (lastSentenceID != sentenceID)
  {
    myLedController.turnAllOff();
    for (WordConfiguration wConfig : possibleSentences.at(sentenceID))
    {
        Word* word =  wConfig.word;
        word->turnOn(wConfig.color);
        word->makeSingular(wConfig.digitsToRemove);
    }
  }
}

void setup()
{

  Serial.begin(9600);
  
  myLedController.setup(); // disable for debug // Add leds to FastLED Library
  setupWordControllerWithWords(); // Vectoren mit Wörten füllen (mappen von led indexen)
  myLedController.turnAllOn(CRGB::White); // show that every Led is working
  Serial.println("send");
  
  myLedController.show();
  
  
  
  // put your setup code here, to run once:
}

void loop()
{

  //myLedController.show();
  
  // put your main code here, to run repeatedly:
}

#line 1 "c:\\Users\\Sven Weinert\\OneDrive - Sennheiser electronic GmbH & Co. KG\\Projekte\\WordClock\\Arduino\\LEDController\\WordController.ino"


