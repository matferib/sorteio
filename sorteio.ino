#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(/*i2c address=*/0x27, /*columns=*/20, /*rows=*/4);

// constants won't change. They're used here to set pin numbers:
constexpr int drawButtonPin = 10;
constexpr int buttonTwoPin = 12;
constexpr int backButtonPin = 8;
constexpr int relayOnePin = 2;
constexpr int relayTwoPin = 3;
constexpr int screenGround = 18;
constexpr int screenPower = 19;
constexpr bool debug = true;
constexpr unsigned long kPauseButtontoleranceMs = 300;
constexpr unsigned long kBlinkIntervalMs = 300;
constexpr unsigned long kDelayMs = 30000;

// variables will change:
unsigned long counter_one_ms = 0;
unsigned long counter_two_ms = 0;
unsigned long counter_pause_ms = 0;
unsigned long left_pause_ms = 0;

bool ReadButton(const int buttonPin) {
  const int buttonState = digitalRead(buttonPin);
  // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
  if (buttonState == HIGH) {
    return true;
  } else {
    return false;
  }
}

void ControlRelay(const int relayPin, const unsigned long now_ms, unsigned long& counter_ms) {
  const unsigned long future_ms = counter_ms + kDelayMs;
  if (debug && (now_ms % 1000) == 0) { 
    Serial.print(" f:");
    Serial.println(future_ms);
  }
  if (counter_ms > 0 && future_ms >= now_ms) {
    digitalWrite(relayPin, LOW);
  } else {
    counter_ms = 0;
    digitalWrite(relayPin, HIGH);
  }
}

bool PlayerButton(const int buttonPin, const unsigned long now_ms, unsigned long& counter_ms) {
  bool button_state = false;
  if (ReadButton(buttonPin)) {
    button_state = true;
    if (counter_ms == 0) { counter_ms = now_ms; }
  }
  return button_state;
}


// Blink the lights that are active.
void BlinkLights(unsigned long now_ms) {
  int state = ((((now_ms - counter_pause_ms) / kBlinkIntervalMs) % 2) == 0) ? HIGH : LOW;
  if (counter_one_ms > 0) {
    digitalWrite(relayOnePin, state);
  }
  if (counter_two_ms > 0) {
    digitalWrite(relayTwoPin, state);
  }
}

const char* players[] = {
  "abnet    ",  // 1
  "mribeiro ",
  "willrosa ",
  "willazul ",
  "willmarro",  // 5
  "maromfive",
  "subaru!  ",
  "schvaisteingengsdfsdfsd",
  "9        ",
  "10       ",  // 10
  "11       ",
  "12       ",
  "13       ",
  "14       ",
  "15       ",  // 15
  "16       ",
  "17       ",
  "18       ",
  "19       ",
  "20       ",  // 20
  "21       ",
  "22       ",
  "23       ",
  "24       ",
  "25       ",  // 25
  "26       ",
  "27       ",
  "28       ",
  "29       ",
  "30       ",  // 30
  "31       ",
  "32       ",
  "33       ",
  "34       ",
  "35       ",  // 35
  "36       ",
  "37       ",
  "38       ",
  "39       ",
  "40       ",  // 40
  "41       ",
  "42       ",
  "43       ",
  "44       ",
  "45       ",  // 45
  "46       ",
  "47       ",
  "48       ",
  "49       ",
  "50       ",  // 50
  "51       ",
  "52       ",
  "53       ",
  "54       ",
  "55       ",  // 55
  "56       ",
  "57       ",
  "58       ",
  "59       ",
  "60       ",  // 60
  "61       ",
  "62       ",
  "63       ",
  "64       ",
  "65       ",  // 65
  "66       ",
  "67       ",
  "68",
  "69",
  "70",  // 70
  "71",
  "72",
  "73",
  "74",
  "75",  // 75
  "76",
  "77",
  "78",
  "79",
  "80",  // 80
  "81",
  "82",
  "83",
  "84",
  "85",  // 85
  "86",
  "87",
  "88",
  "89",
  "90",  // 90
  "91",
  "92",
  "93",
  "94",
  "95",  // 95
  "96",
  "97",
  "98",
  "99",
  "100",  // 100
  "101",
  "102",
  "103",
  "104",
  "105",  // 105
  "106",
  "107",
  "108",
  "109",
  "110",  // 110
  "111",
  "112",
  "113",
  "114",
  "115",  // 115
  "116",
  "117",
  "118",
  "119",
  "120",  // 120
  "121",
  "122",
  "123",
  "124",
  "125",  // 125
  "126",
  "127",
  "128",
  "129",
  "130",  // 130
  "131",
  "132",
  "133",
  "134",
  "135",  // 135
};

constexpr int kNumPlayers = sizeof(players) / sizeof(char*);

struct Node {
  struct Node* before = nullptr;
  struct Node* after = nullptr;
  int index = 0;

  void RemoveMe() {
    before->after = after;
    after->before = before;
    after = nullptr;
    before = nullptr;
  }

  void AddBefore(Node* node) {
    this->after = node;
    this->before = node->before;
    this->before->after = this;
    this->after->before = this;
  }
};

void PrintName(const char* name) {
  int len = strlen(name);
  for (int i = 0; i < 9; ++i) {
    lcd.print(i < len ? name[i] : ' ');
  }
}

void PrintNameById(int id) {
  PrintName(players[id]);
}

struct List {
  Node head;
  Node tail;
  char id = '\0';
  void Initialize(char id) {
    head.after = &tail;
    tail.before = &head;
    this->id = id;
  }

  void Print() {
    int row = 0;
    int column = 0;
    lcd.setCursor(column, row++);
    lcd.print("Group ");
    lcd.print(this->id);
    lcd.print("             ");
    for (auto* n = head.after; n != &tail; n = n->after) {
      lcd.setCursor(column, row);
      if (column == 0) { column = 10; }
      else { ++row; column = 0; } 
      PrintName(players[n->index]);
    }
    for (int r = row; r < 4; ++r ) {
      lcd.setCursor(column, r);
      if (column == 0) { lcd.print("                    "); }
      else { lcd.print("          "); column = 0; }
    }
  }
};

int current_players = kNumPlayers;
int current_group = -1;
int display_group = 0;
constexpr int kNumGroups = 32;
constexpr int kPlayersPerGroup = kNumPlayers / kNumGroups;
constexpr int kNumGroupsWithOneMorePlayer = kNumPlayers % kNumGroups;

List player_list;
List groups[kNumGroups];

int kCilds = 43;

void PrintDebugPage() {
  int row = 0;
  int column = 0;
  lcd.clear();
  lcd.setCursor(column, row++);
  lcd.print("BavepToyama Debug XI");
  lcd.setCursor(0, row++);
  lcd.print("Seed: "); lcd.print(kCilds); lcd.print(", miss: ");
  int missing_players = 0;
  for (const auto* n = player_list.head.after; n != &player_list.tail; n = n->after) {
    ++missing_players;
  }
  lcd.print(missing_players);
  for (const auto* n = player_list.head.after; n != &player_list.tail; n = n->after) {
    if (row < 4) {
      lcd.setCursor(column, row++);
      PrintNameById(n->index);  
    }
  }
}

void setup() {
  // initialize the LED pin as an output:
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(relayOnePin, OUTPUT);
  pinMode(relayTwoPin, OUTPUT);
  pinMode(screenGround, OUTPUT);
  pinMode(screenPower, OUTPUT);
  // initialize the pushbutton pin as an input:
  pinMode(backButtonPin, INPUT);
  pinMode(buttonTwoPin, INPUT);
  pinMode(backButtonPin, INPUT);
  if (debug) {
    Serial.begin(9600);
  }
  digitalWrite(screenGround, LOW);
  digitalWrite(screenPower, HIGH);
  digitalWrite(relayOnePin, HIGH);
  digitalWrite(relayTwoPin, HIGH);

  lcd.init();

  player_list.Initialize(' ');
  for (int i = 0; i < kNumPlayers; ++i) {
    auto* n = new Node;
    n->index = i;
    n->AddBefore(&player_list.tail);
  }

  int gid = 0;
  for (auto& group : groups) {
    group.Initialize(gid < kNumGroups - 1 ?'A' + gid++ : '!');
  }
  randomSeed(kCilds);
  PrintDebugPage();
}

bool GroupDrawAndUpdateDisplay() {
    if (current_group >= kNumGroups - 1) {
      // Final verification.
      return player_list.head.after = &player_list.tail;
    }
    ++current_group;
    int num_players = kPlayersPerGroup; 
    if (current_group >= (kNumGroups - kNumGroupsWithOneMorePlayer)) {
      ++num_players;
    }
    for (int i = 0; i < num_players && current_players > 0; ++i) {
      int pos = random(0, current_players);
      auto* current = player_list.head.after;
      for (int j = 0; j < pos && current->after != nullptr; ++j) {
        current = current->after;
      }
      current->RemoveMe();
      current->AddBefore(&groups[current_group].tail);
      --current_players;  
    }
    display_group = current_group;
    if (current_group == kNumGroups - 1) {
      digitalWrite(relayOnePin, LOW);
    }
    return true;
}

void loop() {
  const unsigned long now_ms = millis();
  lcd.setBacklight(HIGH);

  bool pushed = false;
  if (ReadButton(drawButtonPin)) {
    pushed = true;    
    if (!GroupDrawAndUpdateDisplay()) {
      digitalWrite(relayTwoPin, LOW);
    }
  } else if (ReadButton(buttonTwoPin)) {
    pushed = true;
    display_group = (display_group + 1) % (kNumGroups + 1);
  } else if (ReadButton(backButtonPin)) {
    pushed = true;
    display_group = (display_group == 0) ? kNumGroups : display_group - 1;    
  }

  if (pushed) {
    if (display_group >= kNumGroups) {
      PrintDebugPage();
    } else {
      groups[display_group].Print();
    }
    delay(250);
  }
}
