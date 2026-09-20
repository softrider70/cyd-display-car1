# Copilot-Instruktionen – cyd-display-car1

**Projekt:** Auto-Dashboard auf einem **CYD** (Cheap Yellow Display,
ESP32-2432S028R: ILI9341 320x240 quer + Touch) mit LVGL-Oberflaeche.

## Bedeutung fuer andere Projekte

Dieses Projekt ist die **Referenz fuer die CYD-Hardware**. Die hier
funktionierende Belegung und der rohe SPI-Treiber wurden fuer
`../autofrontcam/cyd` als Vorlage uebernommen. Wer die CYD-Pins braucht,
schaut in `include/config.h` dieses Projekts.

## Wichtig fuer die Zusammenarbeit

- **Sprache ist Deutsch** - Antworten, Kommentare und Doku.
- Kurze Saetze, Fachwoerter erklaeren, keine Vermutungen: pruefen statt raten.
- Nach jeder Aenderung **bauen und flashen**, dann die Ausgabe im Log pruefen.
- Bauen und flashen: ESP-IDF (6.1) aktivieren, PATH aufraeumen, dann im
  Projektordner `idf.py -p COMx flash`.

Doku im Projekt: `PROJECT.md` (Spezifikation), `README.md` (Uebersicht),
`include/config.h` (Pins/Parameter).
