# Asjade Internet Foorilahendus

### DISCLAIMER
```
Antud kood teeb küll töö ära, kuid siiski üldpildis on optimiseerimata & üsnagi kaugel puhtast koodist.
Lisaks saaks veel teha terve selle projekti kuidagi ajapõhiseks, hetkel on oluline, et mitmest arvutist
Compile'itakse kood samal ajal. Ka laine käitumist nupuvajutuse korral saaks paremaks korrigeerida.
```

## Selgitus
```
Antud kood loeb ja vahetab infot läbi Firebase andmebaasi. Andmebaasis on objekt 1007, mille sees on
omakorda objektid buttonPushed, jointRegime ja trafficLights. buttonPushed objekt peaks teoorias kuuluma
hoopis jointRegime objekti alla, kuid Arduinost osutus sinna alla kirjutamine natuke keerulisemaks, seega
tegime sellest eraldi objekti.

jointRegime alla kuuluvad:
* active: boolean - Näitab, kas ühine lainerežiim on aktiivne. Kui ei ole, siis saab igat foori eraldi kontrollida
* cycle: integer - Määrab, kui pikk on ühine lainepikkus. Arduino siseselt tehakse selle põhjal arvutused, 
  kui kaua mingi foori tuled põlevad
* direction: 0 või 1 - Määrab ühise laine suuna, 0 on vasakult paremale ja 1 on paremalt vasakule
* distance: integer - Määrab, kui pikk on vahemaa kahe foori vahel. Seda arvu võetakse ka Arduino sisestel
  arvutustel arvesse
* regime: 0 või 1 - Määrab, kas ühine laine on tavatsüklis või vilkuva kollase tsüklis. (0 tava, 1 vilkuv)

trafficLights objekti alla iga foori kohta üks objekt, millest igal on olemas järgmised omadused:
* active: boolean - Näitab, kas foor on aktiivne või välja lülitatud. Välja lülitatud foori korral ei põle 
  ükski LED
* cycle: integer - Kui ühine lainerežiim on välja lülitatud, määrab see number, kui pikk on foori tsükkel
* regime: 0 või 1 - Määrab, kas foor on tavatsüklis või vilkuva kollase tsüklis. (0 tava, 1 vilkuv)

Seega kokkuvõttes peaks olema fooride infot võimalik muuta nii veebilehelt kui ka läbi Firebase'i. Tehtud
muudatused peaksid foorides kajastuma vähese aja jooksul (NodeMCUdel võib kohati päringutega tsipa aega kuluda).
Tegime tuleviku mõttes ka mõned videod, et näidata kuidas lahendus hetkel töötab. Link on järgmises peatükis.

```

### Videod
```
https://drive.google.com/drive/folders/1-4XkM9C6l1IehAyPvFdQICfQpaHqBhPo?usp=sharing
```
