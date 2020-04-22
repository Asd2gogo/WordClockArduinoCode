#include <app_api.h>
#include <avr8-stub.h>

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

  void turnOn(bool resetSingular = false)
  {

    for (CRGB *led : wordLeds)
    {
      *led = color;
    }
    if (resetSingular)
    {
      mDigitsToRemove = 0;
    }
    else
    {
      makeSingular(mDigitsToRemove);
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



void setup() {
  // put your setup code here, to run once:
debug_init();
LEDController myLedController;
WordController myWordController;
//myLedController.setup();

}

void loop() {
  // put your main code here, to run repeatedly:

}
