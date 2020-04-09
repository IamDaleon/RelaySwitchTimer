#include <Arduino.h>
#include <Wire.h> // must be included here so that Arduino library object file references work
#include <RtcDS1307.h>

RtcDS1307<TwoWire> Rtc(Wire);

const int Relay = 4;
const int delayPeriod = 50;

void setup ()
{
  Serial.begin(115200);

  Serial.print("Loadout Date Acquired: ");
  Serial.print(__DATE__);
  Serial.println(__TIME__);

  Rtc.Begin();
  RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
  printDateTime(compiled);
  Serial.println();

  if (!Rtc.IsDateTimeValid())
  {
    if (Rtc.LastError() != 0)
    { //https://www.arduino.cc/en/Reference/WireEndTransmission
      Serial.print("Onboard time has encounted communications error = ");
      Serial.println(Rtc.LastError());
    }
    else
    { //    First Boot / Device not ready. Battery is low or missing
      Serial.println("Onoard time lost confidence in the DateTime!");
      Rtc.SetDateTime(compiled);
    }
  }

  if (!Rtc.GetIsRunning())
  {
    Serial.println("Onboard time was not actively running, Initalizing now");
    Rtc.SetIsRunning(true);
  }

  RtcDateTime now = Rtc.GetDateTime();
  if (RtcDateTime(now) < compiled)
  {
    Serial.println("Onboard time experienced a delay! (Updating now)");
    Rtc.SetDateTime(compiled);
  }
  else if (now > compiled)
  {
    Serial.println("Onboard time is ahead System time. (Resequencing now)");
  }
  else if (now == compiled)
  {
    Serial.println("Onboard time clear to initialization ( All is Well!)");
  }

  // never assume the Rtc was last configured by you, so
  // just clear them to your needed state
  Rtc.SetSquareWavePin(DS1307SquareWaveOut_Low);

  // Switch/ Relay
  pinMode(Relay, OUTPUT);
}

void loop ()
{
  if (!Rtc.IsDateTimeValid())
  {
    if (Rtc.LastError() != 0)
    {
      Serial.print("Onboard time communications error = ");
      Serial.println(Rtc.LastError());
    }
    else
    {
      Serial.println("Onboard time lost confidence in the DateTime!"); // The battery is low or even missing / power was disconnected
    }
  }

  RtcDateTime now = Rtc.GetDateTime();
  printDateTime(now);
  Serial.println();

  delay(10000); // ten seconds

  const int printHour = now.Hour();

  if (printHour >= 18 || printHour <= 7)
  {
    digitalWrite(Relay, HIGH);
  } else if (printHour <= 7) {
    digitalWrite(Relay, LOW);
  }
}

#define countof(a) (sizeof(a) / sizeof(a[0]))

void printDateTime(const RtcDateTime& dt)
{
  char datestring[20];

  snprintf_P(datestring,
             countof(datestring),
             PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
             dt.Month(),
             dt.Day(),
             dt.Year(),
             dt.Hour(),
             dt.Minute(),
             dt.Second() );
  Serial.print(datestring);
}
void printDate(const RtcDateTime& dt)
{
  char datestring[3];

  snprintf_P(datestring,
             countof(datestring),
             PSTR("%02u"),
             dt.Day());
  Serial.print(datestring);
}
void printHour(const RtcDateTime& dt)
{
  char datestring[3];

  snprintf_P(datestring,
             countof(datestring),
             PSTR("%02u"),
             dt.Hour());
  Serial.print(datestring);
}