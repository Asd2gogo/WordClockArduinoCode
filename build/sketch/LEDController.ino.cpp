#include <Arduino.h>
#line 1 "c:\\Users\\Sven Weinert\\OneDrive - Sennheiser electronic GmbH & Co. KG\\Projekte\\WordClock\\Arduino\\LEDController\\LEDController.ino"
//#define FASTLED_ALLOW_INTERRUPTS 0#

//overwrite PROGMEM for ESP01 to correctly save String (Char array) on External flash
#define PROGMEM ICACHE_RODATA_ATTR

//-----------------------LED-Controller-------------------------------------
//#include <StandardCplusplus.h>
//#include <zip.h>
#include <vector>
#include <FastLED.h>
#include <arduino.h>

//------------------------WEB Server-----------------------------------------
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>


#define NUM_LEDS 225

CRGB leds[NUM_LEDS];

class AnimationController;
class Animation;
class LEDController;
class Word;
class WordConfiguration;
class WordController;

class LEDController
{
   CRGB* getLEDatXY();

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
  { // todo check richtung .. manchmal erster buchstabe ist großer als letzer

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

  std::vector<CRGB*> getSquareLeds(int distFromCenter){
    std::vector<CRGB*> returnVector;
    int maxEdgeLenght = sqrt(NUM_LEDS)-1;
    int center = maxEdgeLenght / 2;

    int selectedEdgeLength = distFromCenter * 2+1;

    int leftTopEdge = center - (selectedEdgeLength-1)/2;
    int rightBottomEdge = center + (selectedEdgeLength-1)/2;

    for(int x = leftTopEdge; x <= rightBottomEdge; x++){
      
      if(x == leftTopEdge || x == rightBottomEdge){  // first or Last Row -> take all y -> left or right edge of square
        for(int y = leftTopEdge; y <= rightBottomEdge; y++){
          returnVector.push_back(getLEDatXY(x,y));
        }
      }else{ // somewhere in center -> only top and bottom y -> Top / bottom maxEdgeLenght
        returnVector.push_back(getLEDatXY(x, leftTopEdge));
        returnVector.push_back(getLEDatXY(x, rightBottomEdge));
      }
    }
    return returnVector;
  }

  // Get single LED -> X/Y 0/0 is top left
  CRGB* getLEDatXY(int x, int y){

      bool isRLrow = y%2; // true if y is uneven -> Led index is from right to Left in this isRLrow
      int index;

      int x0;
       x0 = NUM_LEDS -(15*(y + 1)); // set x0 corrosponding to current y row
      if(isRLrow){
        x0 += 14; // if row index is from Right to Left -> add 14 to start to adjust x0
        index = x0-x;
      }else{
        index = x0+x;
      }

    return &leds[index];
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
  CRGB color = CRGB::White;

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

  void setColor(CRGB c)
  {
    color = c;
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

  std::vector<CRGB *> getLeds(int _digitsToRemove = 0)
  {

    std::vector<CRGB *> leds;
    for (int i = 0; i < word->wordLeds.size() - _digitsToRemove; i++)
    {
      leds.push_back(word->wordLeds.at(i));
    }

    return leds;
  }

  ~WordConfiguration()
  {

    word = NULL;
    delete word;
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

  std::vector<WordConfiguration *> getMinutsWords(int minute)
  {
    std::vector<WordConfiguration *> returnVector;

    int zehner = floor(minute / 10);
    int einer = minute % 10;
    if (minute == 11)
    {
      returnVector.push_back(new WordConfiguration(minutesFirstDigitWords.at(10))); // "elf"
    }
    else if (minute == 12)
    {
      returnVector.push_back(new WordConfiguration(minutesFirstDigitWords.at(11))); // "zwölf"
    }
    else
    {
      switch (einer)
      {
      case 0:
        break;
      case 1:
        
          returnVector.push_back(new WordConfiguration(minutesFirstDigitWords.at(0))); // "eine"
        
        if (zehner > 1)
        {
          returnVector.at(0)->digitsToRemove = 1; // convert "eine" zu "ein"
          returnVector.push_back(new WordConfiguration(andWords.at(0)));
        }
        break;
      case 6:
        returnVector.push_back(new WordConfiguration(minutesFirstDigitWords.at(5)));
        if (zehner == 1)
        {
          returnVector.at(0)->digitsToRemove = 1;
        }
        break;
      case 7:
        returnVector.push_back(new WordConfiguration(minutesFirstDigitWords.at(6)));
        if (zehner == 1)
        {
          returnVector.at(0)->digitsToRemove = 2;
        }
        break;
      default:
        returnVector.push_back(new WordConfiguration(minutesFirstDigitWords.at(einer - 1)));
        break;
      }
    

    switch (zehner)
    {
    case 0:

      break;
    case 1:
      returnVector.push_back(new WordConfiguration(minutesFirstDigitWords.at(9)));
      break;
    default:
      if(einer != 0){
       returnVector.push_back(new WordConfiguration(andWords.at(0)));
      }
      returnVector.push_back(new WordConfiguration(minutesSecondDigitWords.at(zehner - 2)));
      break;
    }
    }

    return returnVector;
  }

  std::vector<WordConfiguration *> getHourWords(int hour, bool isOclock = false)
  {

    std::vector<WordConfiguration *> returnVector;
    if (hour == 0)
    {
      returnVector.push_back(new WordConfiguration(hoursFirstDigitWords.at(3)));  //vier
      returnVector.push_back(new WordConfiguration(andWords.at(1)));              // und
      returnVector.push_back(new WordConfiguration(hoursSecondDigitWords.at(0))); // zwanzig
    }
    else if (hour == 1)
    {
      returnVector.push_back(new WordConfiguration(hoursFirstDigitWords.at(0))); //ein
      if(isOclock) returnVector.at(0)->digitsToRemove = 1;
    }
    else if (hour <= 12)
    {
      returnVector.push_back(new WordConfiguration(hoursFirstDigitWords.at(hour - 1))); //jeweilige stunde
    }
    else if (hour == 16)
    {
      returnVector.push_back(new WordConfiguration(hoursFirstDigitWords.at(5))); // sech
      returnVector.at(0)->digitsToRemove = 1;
    }
    else if (hour == 17)
    {
      returnVector.push_back(new WordConfiguration(hoursFirstDigitWords.at(6))); //7
      returnVector.at(0)->digitsToRemove = 2;
    }
    else if (hour <= 20)
    {
      returnVector.push_back(new WordConfiguration(hoursFirstDigitWords.at(hour - 11))); // jeweilige erster teil der stunde 3 / 4 / 5 / 6 / 8 / 9
    }
    else if (hour == 21)
    {
      returnVector.push_back(new WordConfiguration(hoursFirstDigitWords.at(0))); //ein
      returnVector.at(0)->digitsToRemove = 1;
    }
    else if (hour > 21)
    {
      returnVector.push_back(new WordConfiguration(hoursFirstDigitWords.at(hour - 21))); //jeweilige stunde
    }

    if (hour < 20 && hour > 12)
    {
      returnVector.push_back(new WordConfiguration(hoursFirstDigitWords.at(9))); // zehn
    }
    else if (hour > 20)
    {
      returnVector.push_back(new WordConfiguration(andWords.at(1)));              //und
      returnVector.push_back(new WordConfiguration(hoursSecondDigitWords.at(0))); //zwanzig
    }
    else if (hour == 20)
    {
      returnVector.push_back(new WordConfiguration(hoursSecondDigitWords.at(0))); //zwanzig
    }

    return returnVector;
  }

  std::vector<std::vector<WordConfiguration *>> getWordsForTime(int minute, int hour)
  { // return array of possible word combination to for given time

    std::vector<std::vector<WordConfiguration *>> returnVector;

    std::vector<WordConfiguration *> timeQuaterWords;
    std::vector<WordConfiguration *> timeQuaterEastWords;
    std::vector<WordConfiguration *> timeMinutesPastWords;
    std::vector<WordConfiguration *> timeMinutesBeforeWords;
    std::vector<WordConfiguration *> timeOClockWords;
    std::vector<WordConfiguration *> timeMiutesBeforeAfterHalfWords;

    if (minute == 0)
    { // oClock

      std::vector<WordConfiguration *> hourWords = getHourWords(hour, true);
      for (std::vector<WordConfiguration *>::iterator pIterator = hourWords.begin(); pIterator != hourWords.end(); ++pIterator)
      {
        timeOClockWords.push_back(*pIterator);
      }
      timeOClockWords.push_back(new WordConfiguration(basicWords.at(3)));
    }

    if (minute != 0)
    { // before next hour / after current hour

      //Minutes before next Hour "fuenf zehn Minuten vor zehn"
      int minutesBeforeNextHour = (60 - minute);
      std::vector<WordConfiguration *> minutesWords = getMinutsWords(minutesBeforeNextHour);
      for (std::vector<WordConfiguration *>::iterator pIterator = minutesWords.begin(); pIterator != minutesWords.end(); ++pIterator)
      { // x (Minuten)
        timeMinutesBeforeWords.push_back(*pIterator);
      }
      timeMinutesBeforeWords.push_back(new WordConfiguration(basicWords.at(2), minutesBeforeNextHour == 1 ? 1 : 0)); // Minuten

      timeMinutesBeforeWords.push_back(new WordConfiguration(beforeAfterWords.at(0))); // vor

      int nextHour = (hour + 1) % 24;
      int twelfHourFormat = (nextHour % 13) + floor(nextHour / 13); // 10 11 12 1 2 3
      std::vector<WordConfiguration *> hourWords = getHourWords(twelfHourFormat);

      for (std::vector<WordConfiguration *>::iterator pIterator = hourWords.begin(); pIterator != hourWords.end(); ++pIterator)
      {
        timeMinutesBeforeWords.push_back(*pIterator);
        // add all Words necessary for the hour ("zwei und zwanzig")
      }
      //timeMinutesBeforeWords.push_back(basicWords.at(3)); // Uhr

      // after current hour "fuenf und vierzig zehn Minuten nach neun"

      std::vector<WordConfiguration *> minuteWords = getMinutsWords(minute);

      for (std::vector<WordConfiguration *>::iterator pIterator = minuteWords.begin(); pIterator != minuteWords.end(); ++pIterator)
      { // x (Minuten)
        timeMinutesPastWords.push_back(*pIterator);
      }
      timeMinutesPastWords.push_back(new WordConfiguration(basicWords.at(2))); // Minuten

      timeMinutesPastWords.push_back(new WordConfiguration(beforeAfterWords.at(1))); // nach

      twelfHourFormat = (hour % 13) + floor(hour / 13); // 10 11 12 1 2 3
      hourWords = getHourWords(twelfHourFormat);

      for (std::vector<WordConfiguration *>::iterator pIterator = hourWords.begin(); pIterator != hourWords.end(); ++pIterator)
      {
        timeMinutesPastWords.push_back(*pIterator);
        // add all Words nessesarry for the hour ("zwei und zwanzig")
      }
      // timeMinutesPastWords.push_back(basicWords.at(3)); // Uhr
    }

    if (minute == 15)
    { // viertel nach current hour // viertel nextHour

      //"viertel nach eins"
      timeQuaterWords.push_back(new WordConfiguration(quaterWords.at(0))); // viertel

      timeQuaterWords.push_back(new WordConfiguration(beforeAfterWords.at(1))); // nach

      int twelfHourFormat = (hour % 13) + floor(hour / 13); // 10 11 12 1 2 3

      std::vector<WordConfiguration *> hourWords = getHourWords(twelfHourFormat);

      for (std::vector<WordConfiguration *>::iterator pIterator = hourWords.begin(); pIterator != hourWords.end(); ++pIterator)
      {
        timeQuaterWords.push_back(*pIterator);
      }

      // "Viertel zwei"
      timeQuaterEastWords.push_back(new WordConfiguration(quaterWords.at(0))); // viertel

      int nextHour = hour + 1;
      twelfHourFormat = (nextHour % 13) + floor(nextHour / 13); // 10 11 12 1 2 3
      hourWords = getHourWords(twelfHourFormat);

      for (std::vector<WordConfiguration *>::iterator pIterator = hourWords.begin(); pIterator != hourWords.end(); ++pIterator)
      {
        timeQuaterWords.push_back(*pIterator);
      }
    }

    if (minute == 30)
    { // halb
      //"Halb drei"
      timeQuaterWords.push_back(new WordConfiguration(quaterWords.at(1))); // halb
      int nextHour = hour + 1;
      int twelfHourFormat = (nextHour % 13) + floor(nextHour / 13); // 10 11 12 1 2 3

      std::vector<WordConfiguration *> hourWords = getHourWords(twelfHourFormat);

      for (std::vector<WordConfiguration *>::iterator pIterator = hourWords.begin(); pIterator != hourWords.end(); ++pIterator)
      {
        timeQuaterWords.push_back(*pIterator);
      }
    }

    if (minute == 45)
    { // viertel vor next hour // dreiviertel nextHour

      timeQuaterWords.push_back(new WordConfiguration(quaterWords.at(0))); // viertel

      timeQuaterWords.push_back(new WordConfiguration(beforeAfterWords.at(0))); // vor

      int nextHour = hour + 1;
      int twelfHourFormat = (nextHour % 13) + floor(nextHour / 13); // 10 11 12 1 2 3

      std::vector<WordConfiguration *> hourWords = getHourWords(twelfHourFormat);

      for (std::vector<WordConfiguration *>::iterator pIterator = hourWords.begin(); pIterator != hourWords.end(); ++pIterator)
      {
        timeQuaterWords.push_back(*pIterator);
      }

      // "drei viertel zwei"

      timeQuaterEastWords.push_back(new WordConfiguration(minutesFirstDigitWords.at(2))); // drei
      timeQuaterEastWords.push_back(new WordConfiguration(quaterWords.at(0)));            // viertel

      for (std::vector<WordConfiguration *>::iterator pIterator = hourWords.begin(); pIterator != hourWords.end(); ++pIterator)
      {
        timeQuaterEastWords.push_back(*pIterator);
      }
    }

    if ((minute >= 15) && (minute <= 45))
    { // minutes before / after halb

      //"zehn Minuten vor Halb drei"
      int deltaMinutes = abs(minute - 30);

      std::vector<WordConfiguration *> minuteWords = getMinutsWords(deltaMinutes);

      for (std::vector<WordConfiguration *>::iterator pIterator = minuteWords.begin(); pIterator != minuteWords.end(); ++pIterator)
      {
        timeMiutesBeforeAfterHalfWords.push_back(*pIterator);
        // drei, vier ....
      }
      timeMiutesBeforeAfterHalfWords.push_back(new WordConfiguration(basicWords.at(2), deltaMinutes == 1 ? 1 : 0)); // "Minuten" // check for singular

      timeMiutesBeforeAfterHalfWords.push_back(new WordConfiguration(beforeAfterWords.at(minute > 30 ? 1 : 0))); // "vor" / "nach" // check for vor or nach
      timeMiutesBeforeAfterHalfWords.push_back(new WordConfiguration(quaterWords.at(1))); // "halb"

      int nextHour = hour + 1;
      int twelfHourFormat = (nextHour % 13) + floor(nextHour / 13); // 10 11 12 1 2 3
      std::vector<WordConfiguration *> hourWords = getHourWords(twelfHourFormat);

      for (std::vector<WordConfiguration *>::iterator pIterator = hourWords.begin(); pIterator != hourWords.end(); ++pIterator)
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

    for (int index = 0; index < returnVector.size(); index++)
    {
      std::vector<WordConfiguration *> *vector = &returnVector.at(index);
      vector->push_back(new WordConfiguration(basicWords.at(0))); //Es
      vector->push_back(new WordConfiguration(basicWords.at(1))); //ist
    }

    /*
      else{
        std::vector<std::vector<WordConfiguration>>::iterator itr = std::find(returnVector.begin(), returnVector.end(), vector); // find vector in returnVector
        returnVector.erase(itr); // delete WordConfigurationVector if Empty (currently no matching time for that)
      }
      */
    return returnVector;
  }
};

class PixelAnimation
{
  std::vector<CRGB *> isLED;
  std::vector<CHSV> _setLED;
  int startTime;
  int duration;

  void tick()
  {
  }
};

class Animation
{

  CRGB _setLED;
  
  int startTime;
  int _endTime;
  int _duration = -1;
  int _delay = 0;
  int lastPerzentagTimePassed = 0;
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

  bool tick()
  {
    int currentTime = millis();
    if(currentTime <= startTime){ // return if animation shouldnt start because of set delay
      return false;
    }

    if(isLED.empty()){
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
              _setLED += _setLED ;
            }
          }
          else
          {

            CRGB isColor = (**isLedIter);
            int deltaColor[] = {(int)_setLED.r - (int)isColor.r, (int)_setLED.g - (int)isColor.g, (int)_setLED.b - (int)isColor.b};
            int totalDeltaColor[] = {(100 * deltaColor[0]) / perzentageTimeToGo, (100 * deltaColor[1]) / perzentageTimeToGo, (100 * deltaColor[2]) / perzentageTimeToGo};
            int updateColor[] = {(int)isColor.r + (totalDeltaColor[0] / 100), (int)isColor.g + (totalDeltaColor[1] / 100), (int)isColor.b + (totalDeltaColor[2] / 100)};
            /*
      Serial.print("time To Go: ");
      Serial.println(perzentageTimeToGo);
      Serial.print("isColor: ");
      Serial.println(isColor.val);

      Serial.print("setColor: ");
      Serial.println(_setLED.val);

      

      Serial.print("deltaColor: ");
      Serial.println(deltaColor[2]);
      Serial.print("totalDeltaColor: ");
      Serial.println(totalDeltaColor[2]);
      Serial.print("updateColor: ");
      Serial.println(updateColor[2]);
      */
            CRGB newColor(updateColor[0], updateColor[1], updateColor[2]);

            **isLedIter = newColor;
          }
        }
        lastPerzentagTimePassed = perzentageTimePassed;
      }
    }

    if(didTickBefore){
       return false;
    }else{
      didTickBefore = true;
      return true;
    }
  
  }

  bool isRunning(){
    return didTickBefore;
  }

  void setStartEndTiming(int duration)
  {
    startTime = millis();
    _duration = duration;
    _endTime = startTime + _duration;
  }

  bool deleteDouble(CRGB* newLed){
    for(std::vector<CRGB*>::iterator currentLediter = isLED.begin(); currentLediter != isLED.end();){
      if(*currentLediter == newLed){
        isLED.erase(currentLediter);
        isLED.shrink_to_fit();
        return true;
      }else
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
  std::vector<Animation> myAnimations;

  public:
  AnimationController(LEDController *myLedController)
  {
    _myLedController = myLedController;
  }

  void tick()
  {
    int currentMillis = millis();
    // tick each pixel and word animation
    // change difference slowly between set and is
     
      for (std::vector<Animation>::iterator iter = myAnimations.begin(); iter != myAnimations.end();)
    {
    
      if ((*iter)._animationDone)
      {
        iter = myAnimations.erase(iter);
      }else{
        iter++;
      }
    }
      myAnimations.shrink_to_fit();

    for (std::vector<Animation>::iterator iter = myAnimations.begin(); iter != myAnimations.end();iter++)
    {
        if((*iter).tick()){// Animation ticks -> returns true if animation is startet for the first time -> check for doubles

          for(std::vector<Animation>::iterator animationIterator = myAnimations.begin(); animationIterator != myAnimations.end(); animationIterator++){
            
            if(animationIterator->iD != iter->iD && animationIterator->isRunning()){ // check to not delete current pixels from current animation
                
              for(CRGB* newPixel : iter->isLED){
                animationIterator->deleteDouble(newPixel);
              }
            }

          }
        }
    }

  }

  void addAnimation(Animation anim)
  { // check if each pixel in the word is in an word or pixel animation and delete it from there
    anim.iD = myAnimations.size();
    myAnimations.push_back(anim);
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

    for (std::vector<Animation>::iterator iter = myAnimations.begin(); iter != myAnimations.end(); iter++)
    {
      if (iter->isLED == animation->isLED)
      {
        myAnimations.erase(iter);
      }
    }
  }
};

LEDController *myLedController = new LEDController();
AnimationController myAnimationController = AnimationController(myLedController);

WordController *myWordController = new WordController();


std::vector<std::vector<WordConfiguration *>> possibleSentences;

#line 901 "c:\\Users\\Sven Weinert\\OneDrive - Sennheiser electronic GmbH & Co. KG\\Projekte\\WordClock\\Arduino\\LEDController\\LEDController.ino"
void setupWordControllerWithWords();
#line 989 "c:\\Users\\Sven Weinert\\OneDrive - Sennheiser electronic GmbH & Co. KG\\Projekte\\WordClock\\Arduino\\LEDController\\LEDController.ino"
void setLedForTime(int minute, int hour, int seconds);
#line 1041 "c:\\Users\\Sven Weinert\\OneDrive - Sennheiser electronic GmbH & Co. KG\\Projekte\\WordClock\\Arduino\\LEDController\\LEDController.ino"
void printLEDDataSerial();
#line 1089 "c:\\Users\\Sven Weinert\\OneDrive - Sennheiser electronic GmbH & Co. KG\\Projekte\\WordClock\\Arduino\\LEDController\\LEDController.ino"
void setNewTime(int newH, int newM);
#line 1163 "c:\\Users\\Sven Weinert\\OneDrive - Sennheiser electronic GmbH & Co. KG\\Projekte\\WordClock\\Arduino\\LEDController\\LEDController.ino"
void getFormatedHTMLResponse(char* string, int _h, int _m , uint8_t _wordColor[], uint8_t _backgroundColor[]);
#line 1169 "c:\\Users\\Sven Weinert\\OneDrive - Sennheiser electronic GmbH & Co. KG\\Projekte\\WordClock\\Arduino\\LEDController\\LEDController.ino"
void setupAndStartWifi();
#line 1195 "c:\\Users\\Sven Weinert\\OneDrive - Sennheiser electronic GmbH & Co. KG\\Projekte\\WordClock\\Arduino\\LEDController\\LEDController.ino"
void handleTimeForm();
#line 1217 "c:\\Users\\Sven Weinert\\OneDrive - Sennheiser electronic GmbH & Co. KG\\Projekte\\WordClock\\Arduino\\LEDController\\LEDController.ino"
String splitString(String data, char separator, int index);
#line 1238 "c:\\Users\\Sven Weinert\\OneDrive - Sennheiser electronic GmbH & Co. KG\\Projekte\\WordClock\\Arduino\\LEDController\\LEDController.ino"
void setup();
#line 1258 "c:\\Users\\Sven Weinert\\OneDrive - Sennheiser electronic GmbH & Co. KG\\Projekte\\WordClock\\Arduino\\LEDController\\LEDController.ino"
void loop();
#line 901 "c:\\Users\\Sven Weinert\\OneDrive - Sennheiser electronic GmbH & Co. KG\\Projekte\\WordClock\\Arduino\\LEDController\\LEDController.ino"
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
}

int lastMinute = -1;
int lastSentenceID = -1;
int sentencesCount;
int animationMode = 0;
CRGB backgroundColor = CRGB::Gray;
CRGB wordColor = CRGB::Red;

void setLedForTime(int minute, int hour, int seconds)
{
  if (lastMinute != minute)
  {
    sentencesCount = 0;
    lastMinute = minute;
    possibleSentences = myWordController->getWordsForTime(minute, hour);

    for (std::vector<WordConfiguration *> wConfigVec : possibleSentences)
    {
      sentencesCount++;
    }
  }

  int secondsForEachSentence = 60 / sentencesCount;
  int sentenceID = floor(seconds / secondsForEachSentence);
  int cnt = 0;

  if (lastSentenceID != sentenceID)
  {
    lastSentenceID = sentenceID;

    switch(animationMode){

      case(0): // "normal" mode for each sentence all Words are faded in, old will fade out // TODO remove or dont display "x minuten vor/ nach halb" da nicht möglich anzuzeigen
        myAnimationController.addAnimation(Animation(myLedController->getAllLeds(), backgroundColor,2000));
        myAnimationController.addAnimation(Animation(possibleSentences.at(sentenceID),wordColor, 2000));
      break;

      case(1): // "reading" mode -> sentence is played in loop until next sentence and within loop, each word will show and dimm in reading order

      break;

      case(2): // "Raining" ->  Words that should turn on, are falling from top in random order

      break;
      case(3): // "Snake" ->  One Pixel is moving across the screen (circular pattern) and pixel with correspondig word are kept on

      break;

      case(4): // "Square" -> square gets bigger from center and clears all Leds, when get smaller to center again, new sentence stay on

      break;
    }

    

    //trigger Animation with WconfigVector and decide wich Animation
    //myAnimationController.startAnimation(possibleSentences.at(sentenceID), AnimationController::ANIMATION_ALL_ON_OTHER_OFF, 1000);
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
int h = 8;
int m = 0;
int s = 0;

void setNewTime(int newH, int newM){
  h = newH;
  m = newM;
  s = 0;
  setLedForTime(m, h, s);
}



//-------------------------------WebServer-----------------------




const byte DNS_PORT = 53;
IPAddress apIP(172, 217, 28, 1);
DNSServer dnsServer;
ESP8266WebServer webServer(80);

   static const char responseHTML[] =
  /*
                      "<!DOCTYPE html><html lang='de'><head>"
                      "<meta name='viewport' content='width=device-width'>"
                      "<title>Uhrzeit einstellen</title></head><body>"
                      "<form action=\"/timeentered\">"
                      "<label for=\"appt\">Uhrzeit einstellen:  </label>"
                      "<input type=\"time\" id=\"timeform\" name=\"clockTime\" autofocus required>"
                      "<input type=\"submit\" value=\"Ok\">"
                      "</form>"
                      "</body></html>";
*/
  "<!DOCTYPE html><html lang='de'><head>"
    "<meta name='viewport' content='width=device-width'>"
    "<style>"
        "li{"
            "list-style-type: none;"
            "border-color: white;"
            "border-style: solid;"
            "text-align: center;"
     "   }"
    "</style>"
    "</head>"
    "<body>"
    "<form action=\"/timeentered\">"
    "<ul>"
    "<li>"
        "<h1>WordClock Configurator</h1>"
        "<label for=\"timeform\">Uhrzeit einstellen:  </label>"
        "<input type=\"time\" id=\"timeform\" name=\"clockTime\" value=\"%02i:%02i\" autofocus required>"
    "</li>"
    "<li>"
        "<label for=\"background\">Hintergrund Farbe:  </label>"
        "<input type= \"color\" id = \"backgroundColor\" name=\"backgroundColor\" value =\"#%02X%02X%02X\" required>"
    "</li>"
    "<li>"
        "<label for=\"wordColor\">Wörter Farbe:  </label>"
        "<input type= \"color\" id = \"wordColor\" name=\"wordColor\" value=\"#%02X%02X%02X\" required>"
    "</li>"
    /*
    "<li>"
    <label for="animationMode">AnimationsArt:</label>
      <select id="animationMode" name="mode">
        <option value="1">Synchron Fade all</option>
        <option value="2">Fade after each other</option>
      </select>
    "</li>"
    */
   " <li>"
        "<input type=\"submit\" value=\"Ok\">"
    "</li>"
    "</ul>"
    "</form>"
    "</body></html>";

void getFormatedHTMLResponse(char* string, int _h, int _m , uint8_t _wordColor[], uint8_t _backgroundColor[]){
  
  sprintf(string, responseHTML, _h,_m,_backgroundColor[0],_backgroundColor[1],_backgroundColor[2],_wordColor[0],_wordColor[1],_wordColor[2]);
  Serial.println(string);
}  

void setupAndStartWifi(){

  const String WSSID = "WordClockSetup";
  const String PASS = "12345677654321";
  
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP(WSSID,PASS,1, true, 1);

  // reply to all ip requests with ip of this device
  dnsServer.start(DNS_PORT, "*", apIP);

  //replay to all requests with same HTML
  webServer.onNotFound([]() {
    char formatedString[sizeof(responseHTML)];
    getFormatedHTMLResponse(formatedString,h,m, wordColor.raw, backgroundColor.raw);
    webServer.send(200, "text/html", formatedString);
  });
  webServer.on("/timeentered", handleTimeForm);
    
  webServer.begin();

}



void handleTimeForm(){
  String s = webServer.arg("clockTime");
  
  String hstring = splitString(s, ':', 0);
  String mstring = splitString(s, ':', 1);

  int hNumber = hstring.toInt();
  int mNumber = mstring.toInt();
  setNewTime(hNumber, mNumber);
 

  String _wordColor = webServer.arg("wordColor");
  String _backgroundColor = webServer.arg("backgroundColor");

  wordColor = CRGB((int) strtoll(&_wordColor[1],NULL, 16));
  backgroundColor = CRGB((int) strtoll(&_backgroundColor[1],NULL, 16));

  char formatedString[sizeof(responseHTML)];
    getFormatedHTMLResponse(formatedString,h,m, wordColor.raw, backgroundColor.raw);
  webServer.send(200, "text/html", formatedString);
}

String splitString(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length()-1;

  for(int i=0; i<=maxIndex && found<=index; i++){
    if(data.charAt(i)==separator || i==maxIndex){
        found++;
        strIndex[0] = strIndex[1]+1;
        strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }

  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
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
  myLedController->turnAllOn(CRGB::Black); // show that every Led is working
  FastLED.setBrightness(200);

  delay(3000);
  // put your setup code here, to run once:
}


int lastIncrease = 0;
void loop()
{

  if(lastIncrease + 1100 < millis()){
    lastIncrease = millis();
    s++;
    if(s >59){
      s = 0;
      m++;
      if(m>59){
        m=0;
        h++;
        if(h>23){
          h=0;
        }
      }
    }
   
   /*
    Serial.print("Heap:  ");
    Serial.print(ESP.getFreeHeap());
    Serial.print("  Stack:  ");
    Serial.println(ESP.getFreeContStack());
    */
    //setLedForTime(m, h, s);

    uint8_t r = random8();
    uint8_t g = random8();
    uint8_t b = random8();
    for(int i = 0 ; i < 8; i++){
       myAnimationController.addAnimation(Animation(myLedController->getSquareLeds(i), CRGB(r,g,b), 100, i*100));
       myAnimationController.addAnimation(Animation(myLedController->getSquareLeds(i), CRGB::Black, 400, (i*100)+100));
    }

    //printLEDDataSerial();
  }

  
  



  if(millis()%15 == 0){
  myLedController->output();
  } 
  myAnimationController.tick();
  

  dnsServer.processNextRequest();
  webServer.handleClient();
}

