# 1 "c:\\Users\\Sven Weinert\\OneDrive - Sennheiser electronic GmbH & Co. KG\\Projekte\\WordClock\\Arduino\\LEDController\\LEDController.ino"
//#define FASTLED_ALLOW_INTERRUPTS 0


//#include <StandardCplusplus.h>
# 6 "c:\\Users\\Sven Weinert\\OneDrive - Sennheiser electronic GmbH & Co. KG\\Projekte\\WordClock\\Arduino\\LEDController\\LEDController.ino" 2
# 7 "c:\\Users\\Sven Weinert\\OneDrive - Sennheiser electronic GmbH & Co. KG\\Projekte\\WordClock\\Arduino\\LEDController\\LEDController.ino" 2
# 8 "c:\\Users\\Sven Weinert\\OneDrive - Sennheiser electronic GmbH & Co. KG\\Projekte\\WordClock\\Arduino\\LEDController\\LEDController.ino" 2




CRGB leds[225];

class LEDController
{

  public:
  LEDController()
  { }

  void setup(){
    //Serial.println("going to add Leds");
    FastLED.addLeds<NEOPIXEL, 0>(leds, 225);
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
    for (int x = 0; x < 225; x++)
    {
      leds[x] = color;
    }
  }

  void turnAllOff(){
    for (int x = 0; x < 225; x++)
    {
      leds[x] = CRGB::Black;
    }
  }

  CRGB* getLED(int index)
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
    }else if(startIndex > endIndex) // Invertiert Orientierung (gerade Reihen)
    {
      for (int x = startIndex - 1; x >= endIndex-1; x--)
      {
        returnVector.push_back(&leds[x]);
      }
    }else{ // only one LED
      returnVector.push_back(&leds[startIndex]);
    }
    return returnVector;
  }

  std::vector<CRGB*> getAllLeds(){
    std::vector<CRGB*> allLedsVec;
    for(int x = 0; x < 225; x++){
      allLedsVec.push_back(&leds[x]);
    }
    return allLedsVec;
  }

  void output()
  {
    FastLED.show();
  }
};

class Word
{
  std::vector<CRGB *> wordLeds; // Maybe remove vector and remve classic array -> size on constuctor with vector length
  CRGB color = CRGB::White;

  public:
  Word(std::vector<CRGB* > leds)
  {
    for (CRGB* led : leds)
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

  ~WordConfiguration(){

    word = __null;
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

  std::vector<WordConfiguration*> getMinutsWords(int minute)
  {
    std::vector<WordConfiguration*> returnVector;

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
        if (zehner != 1)
        {
          returnVector.push_back(new WordConfiguration(minutesFirstDigitWords.at(0))); // "eine"
        }
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
    }

    switch (zehner)
    {
    case 0:

      break;
    case 1:
      returnVector.push_back(new WordConfiguration(minutesFirstDigitWords.at(9)));
      break;
    default:
      returnVector.push_back(new WordConfiguration(andWords.at(0)));
      returnVector.push_back(new WordConfiguration(minutesSecondDigitWords.at(zehner - 2)));
      break;
    }

    return returnVector;
  }

  std::vector<WordConfiguration*> getHourWords(int hour)
  {

    std::vector<WordConfiguration*> returnVector;
    if (hour == 0)
    {
      returnVector.push_back(new WordConfiguration(hoursFirstDigitWords.at(3))); //vier
      returnVector.push_back(new WordConfiguration(andWords.at(1))); // und
      returnVector.push_back(new WordConfiguration(hoursSecondDigitWords.at(0))); // zwanzig
    }
    else if (hour == 1)
    {
      returnVector.push_back(new WordConfiguration(hoursFirstDigitWords.at(0))); //ein
      returnVector.at(0)->digitsToRemove = 1;
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
      returnVector.push_back(new WordConfiguration(andWords.at(1))); //und
      returnVector.push_back(new WordConfiguration(hoursFirstDigitWords.at(9))); // zehn
    }
    else if (hour > 20)
    {
      returnVector.push_back(new WordConfiguration(andWords.at(1))); //und
      returnVector.push_back(new WordConfiguration(hoursSecondDigitWords.at(0))); //zwanzig
    }
    else if (hour == 20)
    {
      returnVector.push_back(new WordConfiguration(hoursSecondDigitWords.at(0))); //zwanzig
    }

    return returnVector;
  }

  std::vector<std::vector<WordConfiguration*>> getWordsForTime(int minute, int hour)
  { // return array of possible word combination to for given time

    std::vector<std::vector<WordConfiguration*>> returnVector;

    std::vector<WordConfiguration*> timeQuaterWords;
    std::vector<WordConfiguration*> timeQuaterEastWords;
    std::vector<WordConfiguration*> timeMinutesPastWords;
    std::vector<WordConfiguration*> timeMinutesBeforeWords;
    std::vector<WordConfiguration*> timeOClockWords;
    std::vector<WordConfiguration*> timeMiutesBeforeAfterHalfWords;



    if (minute == 0)
    { // oClock

       std::vector<WordConfiguration*> hourWords = getHourWords(hour);
      for (std::vector<WordConfiguration*>::iterator pIterator = hourWords.begin(); pIterator!= hourWords.end(); ++pIterator)
      {
        timeOClockWords.push_back(*pIterator);
      }
      timeOClockWords.push_back(new WordConfiguration(basicWords.at(3)));
    }

    if (minute != 0)
    { // before next hour / after current hour

      //Minutes before next Hour "fuenf zehn Minuten vor zehn"
      int minutesBeforeNextHour = (60 - minute);
       std::vector<WordConfiguration*> minutesWords = getMinutsWords(minutesBeforeNextHour);
      for (std::vector<WordConfiguration*>::iterator pIterator = minutesWords.begin(); pIterator!= minutesWords.end(); ++pIterator)
      { // x (Minuten)
        timeMinutesBeforeWords.push_back(*pIterator);
      }
      timeMinutesBeforeWords.push_back(new WordConfiguration(basicWords.at(2),minutesBeforeNextHour==1?1:0)); // Minuten

      timeMinutesBeforeWords.push_back(new WordConfiguration(beforeAfterWords.at(0))); // vor

      int nextHour = (hour + 1) % 24;
      int twelfHourFormat = (nextHour % 13) + floor(nextHour / 13); // 10 11 12 1 2 3
      std::vector<WordConfiguration*> hourWords = getHourWords(twelfHourFormat);

      for (std::vector<WordConfiguration*>::iterator pIterator =hourWords.begin(); pIterator!= hourWords.end(); ++pIterator)
      {
        timeMinutesBeforeWords.push_back(*pIterator);
         // add all Words nessesarry for the hour ("zwei und zwanzig")
      }
      //timeMinutesBeforeWords.push_back(basicWords.at(3)); // Uhr

      // after current hour "fuenf und viertig zehn Minuten nach neun"

      std::vector<WordConfiguration*> minuteWords = getMinutsWords(minute);

      for (std::vector<WordConfiguration*>::iterator pIterator = minuteWords.begin(); pIterator!= minuteWords.end(); ++pIterator)
      { // x (Minuten)
        timeMinutesPastWords.push_back(*pIterator);
      }
      timeMinutesPastWords.push_back(new WordConfiguration(basicWords.at(2))); // Minuten

      timeMinutesPastWords.push_back(new WordConfiguration(beforeAfterWords.at(1))); // nach

      twelfHourFormat = (hour % 13) + floor(hour / 13); // 10 11 12 1 2 3
      hourWords = getHourWords(twelfHourFormat);

      for (std::vector<WordConfiguration*>::iterator pIterator =hourWords.begin(); pIterator!= hourWords.end(); ++pIterator)
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

      std::vector<WordConfiguration*> hourWords = getHourWords(twelfHourFormat);

      for (std::vector<WordConfiguration*>::iterator pIterator =hourWords.begin(); pIterator!= hourWords.end(); ++pIterator)
      {
        timeQuaterWords.push_back(*pIterator);
      }

      // "Viertel zwei"
      timeQuaterEastWords.push_back(new WordConfiguration(quaterWords.at(0))); // viertel

      int nextHour = hour + 1;
      twelfHourFormat = (nextHour % 13) + floor(nextHour / 13); // 10 11 12 1 2 3
      hourWords = getHourWords(twelfHourFormat);

      for (std::vector<WordConfiguration*>::iterator pIterator =hourWords.begin(); pIterator!= hourWords.end(); ++pIterator)
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

      std::vector<WordConfiguration*> hourWords = getHourWords(twelfHourFormat);

      for (std::vector<WordConfiguration*>::iterator pIterator =hourWords.begin(); pIterator!= hourWords.end(); ++pIterator)
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

       std::vector<WordConfiguration*> hourWords = getHourWords(twelfHourFormat);

      for (std::vector<WordConfiguration*>::iterator pIterator =hourWords.begin(); pIterator!= hourWords.end(); ++pIterator)
      {
        timeQuaterWords.push_back(*pIterator);

      }

      // "drei viertel zwei"

      timeQuaterEastWords.push_back(new WordConfiguration(minutesFirstDigitWords.at(2))); // drei
      timeQuaterEastWords.push_back(new WordConfiguration(quaterWords.at(0))); // viertel

      for (std::vector<WordConfiguration*>::iterator pIterator =hourWords.begin(); pIterator!= hourWords.end(); ++pIterator)
      {
        timeQuaterWords.push_back(*pIterator);
      }
    }

    if ((minute >= 15) && (minute <= 45))
    { // minutes before / after halb

      //"zehn Minuten vor Halb drei"
      int deltaMinutes = abs(minute - 30);

      std::vector<WordConfiguration*> minuteWords = getMinutsWords(deltaMinutes);

      for (std::vector<WordConfiguration*>::iterator pIterator =minuteWords.begin(); pIterator!= minuteWords.end(); ++pIterator)
      {
        timeMiutesBeforeAfterHalfWords.push_back(*pIterator);
         // drei, vier ....
      }
      timeMiutesBeforeAfterHalfWords.push_back(new WordConfiguration(basicWords.at(2), deltaMinutes == 1 ? 1 : 0)); // "Minuten" // check for singular

      timeMiutesBeforeAfterHalfWords.push_back(new WordConfiguration(beforeAfterWords.at(minute > 30 ? 1 : 0))); // "vor" / "nach" // check for vor or nach

      int nextHour = hour + 1;
      int twelfHourFormat = (nextHour % 13) + floor(nextHour / 13); // 10 11 12 1 2 3
      std::vector<WordConfiguration*> hourWords = getHourWords(twelfHourFormat);

      for (std::vector<WordConfiguration*>::iterator pIterator =hourWords.begin(); pIterator!= hourWords.end(); ++pIterator)
      {
       timeMiutesBeforeAfterHalfWords.push_back(*pIterator);
         // "eins" // "zwei"
      }
    }


    if(timeQuaterWords.size() >0)returnVector.push_back(timeQuaterWords);
    if(timeQuaterEastWords.size() >0)returnVector.push_back(timeQuaterEastWords);
    if(timeMinutesPastWords.size() >0)returnVector.push_back(timeMinutesPastWords);
    if(timeMinutesBeforeWords.size() >0)returnVector.push_back(timeMinutesBeforeWords);
    if(timeOClockWords.size() >0)returnVector.push_back(timeOClockWords);
    if(timeMiutesBeforeAfterHalfWords.size() >0)returnVector.push_back(timeMiutesBeforeAfterHalfWords);


    for(int index = 0; index < returnVector.size(); index++){

        std::vector<WordConfiguration*>* vector = &returnVector.at(index);
        vector->push_back(new WordConfiguration(basicWords.at(0))); //Es
        vector->push_back(new WordConfiguration(basicWords.at(1))); //ist

      }

      /*

      else{

        std::vector<std::vector<WordConfiguration>>::iterator itr = std::find(returnVector.begin(), returnVector.end(), vector); // find vector in returnVector

        returnVector.erase(itr); // delete WordConfigurationVector if Empty (currently no matching time for that)

      }

      */
# 557 "c:\\Users\\Sven Weinert\\OneDrive - Sennheiser electronic GmbH & Co. KG\\Projekte\\WordClock\\Arduino\\LEDController\\LEDController.ino"
    return returnVector;
  }
};

class PixelAnimation{
  std::vector<CRGB*> isLED;
  std::vector<CHSV> _setLED;
  int startTime;
  int duration;


  void tick(){

  }


};

class WordAnimation{
  std::vector<CRGB*> isLED;
  std::vector<CHSV> _setLED;
  int startTime;
  int duration;

};


class AnimationController{

  int _animationDuration;
  int _animationStartTime;
  std::vector<CRGB*> isLED;
  std::vector<CHSV> _setLED;

  LEDController* _myLedController;
  std::vector<WordConfiguration*> _wConfigVec;
    public:
  AnimationController(LEDController* myLedController, std::vector<CRGB*> ledsToChange, int duration){
    _myLedController = myLedController;
    _animationDuration = duration;
    _animationStartTime = millis();
    isLED = ledsToChange;
    for (CRGB* led : isLED){
      _setLED.push_back(rgb2hsv_approximate(*led));
    }
  }


  void tick(){
    int currentMillis = millis();
    // tick each pixel and word domination
    // change diffrenze slowly between set and is

      for (WordConfiguration* wConfig : _wConfigVec) // id for debug // sentenceID for automation
      {
          Word* word = wConfig->word;
          word->turnOn(wConfig->color);
          word->makeSingular(wConfig->digitsToRemove);
      }
      animationDone();

  }

  void addPixelAnimation(){ // check weather pixel is already in an animation and delete is from there

  }

  void addWordAnimation(){ // check if each pixel in the word is in an word or pixel animation and delete it from there

  }

  void setToRandom(std::vector<CRGB*> leds, byte brightness){
    byte randomHue = random(0L, 255L);
    for(CRGB* led : leds){
      led->setHSV(randomHue, 255, brightness);
    }
  }

  void animationDone(){
     for (WordConfiguration* wConfig : _wConfigVec) // id for debug // sentenceID for automation
      {
        delete wConfig;
      }
  }

};


LEDController* myLedController = new LEDController();
AnimationController myAnimationController();
WordController* myWordController = new WordController();

int lastMinute = -1;
std::vector<std::vector<WordConfiguration*>> possibleSentences;

void setupWordControllerWithWords()
{
  //Serial.println("start adding Words");
  myWordController->addbasicWord(new Word(myLedController->getLeds(211, 212))); // Es
  //Serial.println("first word added");

  myWordController->addbasicWord(new Word(myLedController->getLeds(214, 216))); // ist
  myWordController->addbasicWord(new Word(myLedController->getLeds(91, 97))); // Minuten
  myWordController->addbasicWord(new Word(myLedController->getLeds(13, 15))); // Uhr

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
  myWordController->addminutesSecondDigitWord(new Word(myLedController->getLeds(148, 142))); // vierzig
  myWordController->addminutesSecondDigitWord(new Word(myLedController->getLeds(121, 128))); // fünfzig

  myWordController->addquaterWord(new Word(myLedController->getLeds(148, 142))); //viertel
  myWordController->addquaterWord(new Word(myLedController->getLeds(139, 136))); //halb

  myWordController->addandWord(new Word(myLedController->getLeds(163, 165))); // und minutes
  myWordController->addandWord(new Word(myLedController->getLeds(18, 16))); // und hours

  myWordController->addbeforeAfterWord(new Word(myLedController->getLeds(103, 105))); // vor
  myWordController->addbeforeAfterWord(new Word(myLedController->getLeds(99, 102))); // nach

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
  myWordController->addfillWord(new Word(myLedController->getLeds(98, 98))); // L
  myWordController->addfillWord(new Word(myLedController->getLeds(86, 86))); // I
  myWordController->addfillWord(new Word(myLedController->getLeds(81, 81))); // E
  myWordController->addfillWord(new Word(myLedController->getLeds(61, 62))); // EI
  myWordController->addfillWord(new Word(myLedController->getLeds(56, 56))); // X
  myWordController->addfillWord(new Word(myLedController->getLeds(46, 46))); // D
  myWordController->addfillWord(new Word(myLedController->getLeds(31, 32))); // MI
  myWordController->addfillWord(new Word(myLedController->getLeds(37, 39))); // ZUM
  myWordController->addfillWord(new Word(myLedController->getLeds(30, 27))); // AUCH
  myWordController->addfillWord(new Word(myLedController->getLeds(22, 19))); // BIND
  myWordController->addfillWord(new Word(myLedController->getLeds(1, 2))); // IN
  myWordController->addfillWord(new Word(myLedController->getLeds(10, 12))); // NUI

}



void setLedForTime(int minute, int hour, int seconds)
{

  int sentencesCount = 0;
  int lastSentenceID = -1;

  if (lastMinute != minute)
  {
    possibleSentences = myWordController->getWordsForTime(minute, hour);

    for (std::vector<WordConfiguration*> wConfigVec : possibleSentences)
    {
      sentencesCount++;
    }
  }
  int secondsForEachSentence = 60 / sentencesCount;
  int sentenceID = floor(seconds / secondsForEachSentence);
  int cnt = 0;

  if (lastSentenceID != sentenceID)
  {
    //trigger Animation with WconfigVector and decide wich Animation
    myAnimationController.startAnimation(possibleSentences.at(sentenceID), AnimationController::ANIMATION_ALL_ON_OTHER_OFF, 1000);
    printLEDDataSerial();
  }

}



void printLEDDataSerial(){

  for(int x = 0; x < 15; x++){

    int firstIndex = 225 - (x+1) * 15;
    int y;

    if(((x+1) % 2) == 1){ // L->R
      for(int z = 0; z <15 ;z++){

        y = myLedController->getLED(firstIndex+z)->r;
        //Serial.print(firstIndex+z);
        Serial.print(y==0?0:1);
        Serial.print(" ");
      }
    }else{ // R ->L
      for(int z = 0; z < 15 ;z++){

        y = myLedController->getLED(firstIndex+14-z)->r;
        Serial.print(y==0?0:1);
        // Serial.print(firstIndex+14-z);
        Serial.print(" ");
      }
    }
    Serial.println();
  }

  Serial.println();
  Serial.println();

}



void setup()
{


  //Serial.begin(115200);
  //Serial.println("Init");

  myLedController->setup(); // disable for debug // Add leds to FastLED Library
  //Serial.println("Setup Done");
  setupWordControllerWithWords(); // Vectoren mit Wörten füllen (mappen von led indexen)
  //Serial.println("Words created");
  myLedController->turnAllOn(CRGB::White); // show that every Led is working

  //Serial.println("send");

  //myLedController->output();

  Serial.begin(115200);



  // put your setup code here, to run once:
}

void loop()
{
  setLedForTime(0, 21, 1);
  myLedController->output();


  delay(2000);
}
# 1 "c:\\Users\\Sven Weinert\\OneDrive - Sennheiser electronic GmbH & Co. KG\\Projekte\\WordClock\\Arduino\\LEDController\\WordController.ino"
