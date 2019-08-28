/**
 * An Arduino Midi Drum Sequencer / Step sequencer
 * 
 * A Midi controller with 48 2-pin toggle switches and 48 diodes to build a keyboard matrix,
 * a pot to control the tempo and a 2-pin toggle switch to start and stop the output.
 * 
 * The matrix is a 6 x 8 grid, giving control of 6 drums (notes) over 8 beats.
 * 
 * Tested using Arduino Nano.
 * 
 * The circuit:
 * - Switch matrix row pins 2,3,4,5,6,7
 * - Switch matrix columns with diodes on 8, 9, 10, 11, 12, 14 (A0), 15 (A1), 16 (A2)
 * - Potentiometer with center pin on A6, one pin to ground and the other to +5V
 * - 2 position switch on A5
 * - LED with anode on 13 and cathode to ground via a 220 ohm resistor
 * - Female Midi jack with midi pin 5 to Arduino pin 1 through a 220 ohm resistor, midi pin
 *   2 to Arduino ground, midi pin 4 to Arduino +5V through a 220 ohm resistor
 * - Power using a 9v wall wart on with + to Vin and - to ground
 */
 
const byte rows[] = {2,3,4,5,6,7}; 
const byte rowCount = sizeof(rows) / sizeof(rows[0]);

const byte cols[] = {8,9,10,11,12,14,15,16};
const int colCount = sizeof(cols) / sizeof(cols[0]);

const byte tempoPin = A6;
const byte onOffPin = A5;
const byte ledPin = 13;
const byte midiMap[] = {0x24, 0x26, 0x27, 0x2a, 0x2e, 0x38}; // The midi note in hex each row corresponds to

void setup() {
  Serial.begin(31250);

  for (byte x = 0; x < rowCount; x++) {
    pinMode(rows[x], INPUT);
  }

  for (byte x = 0; x < colCount; x++) {
    pinMode(cols[x], INPUT_PULLUP);
  }
  pinMode(tempoPin, INPUT);
  pinMode(onOffPin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);
}

void loop() {
  // Iterate the columns
  for (byte colIndex = 0; colIndex < colCount; colIndex++) {
    if (digitalRead(onOffPin)) {
      colIndex = 0;
      continue;
    }

    digitalWrite(ledPin, HIGH);

    int delayTime = map(analogRead(tempoPin), 0, 1023, 900, 50);

    byte curCol = cols[colIndex];

    // Enable the column
    pinMode(curCol, OUTPUT);
    digitalWrite(curCol, LOW);

    // Iterates the rows
    for (byte rowIndex = 0; rowIndex < rowCount; rowIndex++) {
      byte curRow = rows[rowIndex];
      pinMode(curRow, INPUT_PULLUP);
      if (!digitalRead(curRow)) {
        noteOn(midiMap[rowIndex]);
      }
      pinMode(curRow, INPUT);
    }

    // disable the column
    pinMode(curCol, INPUT_PULLUP);

    // Divide the delay time so the LED can flash
    delay(delayTime * 0.2);
    digitalWrite(ledPin, LOW);
    delay(delayTime * 0.8);
    // Turn all notes off
    for (int x = 0; x < rowCount; x++) {
      noteOff(midiMap[x]);
    }
  }
}

void noteOn(int note) {
  Serial.write(0x99);
  Serial.write(note);
  Serial.write(0x45);
}

void noteOff(int note) {
  Serial.write(0x8A);
  Serial.write(note);
  Serial.write(0x45);
}
