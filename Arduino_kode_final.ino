//Inkluderer nødvendige biblioteker
#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 9
//Lager en instans av MFRC522 (RFID)
MFRC522 mfrc522(SS_PIN, RST_PIN);

//Lagrer siste tidspunkt et kort ble scannet, slik at man kan hindre at det registreres kort for hver loop som kjoeres.
//Bruker unsigned long siden det kan bli et tall som overgaar maks stoerrelse til int om det gaar lang tid.
unsigned long sisteScanning = 0;
//Hvor lang tid i millisekunder det skal ta foer det registreres en ny skanning av RFID-kort.
int scanneUtsettelse = 1000;


//Lagrer de tall for hvor de ulike knappene er koblet til arduinoen, const siden de aldri skal endres under kjoering.
const int opp = 6;
const int ned = 5;
const int tilbake = 4;
const int ok = 3;
const int slett = 2;

//Et array som holder tidspunkter for naar hver av de fem knappene ble trykket.
unsigned long sisteTrykk[] = {0, 0, 0, 0, 0};
//Utsettelse for aa hindre at man registrerer dobbeltrykk,
//med 250 vil det registreres fire trykk i sekunder om man holder knappen inne.
//Kom fram til 250 som et godt kompromiss etter aa ha testet flere ulike verdier testing.
int knappeUtsettelse = 250;

//Setup, kjoeres foer loop starter
void setup() {
  //Starter serial monitor
  Serial.begin(9600);

  //Initierer SPI-bus og MFRC522 (RFID)
  SPI.begin();
  mfrc522.PCD_Init();

  //Setter alle knappene til input
  pinMode(opp, INPUT);
  pinMode(ned, INPUT);
  pinMode(tilbake, INPUT);
  pinMode(ok, INPUT);
  pinMode(slett, INPUT);
}

void loop() {
  //Sjekker om hver knapp er trykket og skriver en streng med knappens navn til serial monitor.
  if (erTrykket(opp)) Serial.println("OPP");
  if (erTrykket(ned)) Serial.println("NED");
  if (erTrykket(tilbake)) Serial.println("TILBAKE");
  if (erTrykket(ok)) Serial.println("OK");
  if (erTrykket(slett)) Serial.println("SLETT");
  

  //Ser etter nye RFID-kort
  if ( ! mfrc522.PICC_IsNewCardPresent()) return;
  
  //Leser serienummeret paa RFID-kortet
  if ( ! mfrc522.PICC_ReadCardSerial()) return;

  //Oppretter en streng for kortets UID, detter er tilstrekkelig for aa skille de ulike kortene i vaart tilfelle.
  String rfidUID= "";
  byte letter; 
  //Setter det det hele sammen til en enkelt streng
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
    //Setter hver del av strengen sammen med resten
    rfidUID.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    rfidUID.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  //Setter UID-strengen til stor bokstaver, fordi det er slik den vanligvis skrives
  rfidUID.toUpperCase();

  //Dersom et kort er scannet printes informasjon til serial monitor
  if (erScannet()) {
    if (rfidUID.substring(1) == "EE 8D 31 B5") {
      Serial.println("kjoettdeig");
    }
    else if (rfidUID.substring(1) == "4E 8D 31 B5") {
      Serial.println("tacolefser");
    }
    else if (rfidUID.substring(1) == "9E 8D 31 B5") {
      Serial.println("salat");
    } 
    else if (rfidUID.substring(1) == "DE 6D 31 B5") {
      Serial.println("mais");
    } 
    else if (rfidUID.substring(1) == "2E 6E 31 B5") {
      Serial.println("paprika");
    } 
    else if (rfidUID.substring(1) == "EE 80 31 B5") {
      Serial.println("ost");
    } 
    else if (rfidUID.substring(1) == "9E 80 31 B5") {
      Serial.println("tomat");
    } 
    else if (rfidUID.substring(1) == "21 62 6F 3B") {
      Serial.println("exit");
    }
  }
}


//Sjekker om en knapp er trykket
boolean erTrykket(int knapp){
  //Om knappen holdes inne
  if (digitalRead(knapp)){
    //Om det er lenge nok siden siste trykk ([knapp-2] siden knappene gaar fra 2 til 6 og arrayet gaar fra 0 til 4)
    if (millis() - sisteTrykk[knapp-2] >= knappeUtsettelse){
      //Lagrer tidspunktet naa som sist knappen ble trykket
      sisteTrykk[knapp-2] = millis();
      //Returnerer true; knappen er trykket
      return true;
    }
  }
  //Om ikke true er returnert over returneres false; knappen er ikke trykket
  return false;
}

//Sjekker om et kort er scannet
boolean erScannet() {
  //Om det er lenge nok siden siste scanning
  if (millis() - sisteScanning >= scanneUtsettelse) {
    //Lagrer tidspunktet naa som sist et kort kle scannet
    sisteScanning = millis();
    //Returnerer ture; kort er scannet
    return true;
  }
  //Om ikke true er returnert over returneres false; kort er ikke scannet
  return false;
}