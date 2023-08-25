#include <stdlib.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1351.h>
#include <SPI.h>
#include <EEPROM.h>

// Screen dimensions
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 128  // Change this to 96 for 1.27" OLED.

// You can use any (4 or) 5 pins
#define SCLK_PIN 13 //YELLOW
#define MOSI_PIN 11 //BLUE
#define DC_PIN 7 //RED
#define CS_PIN 10 //ORANGE
#define RST_PIN 8 //WHITE

// Color definitions
#define BLACK 0x0000
#define BLUE 0x001F
#define RED 0xF800
#define GREEN 0x07E0
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define YELLOW 0xFFE0
#define WHITE 0xFFFF

// Option 2: must use the hardware SPI pins
// (for UNO thats sclk = 13 and sid = 11) and pin 10 must be
// an output. This is much faster - also required if you want
// to use the microSD card (see the image drawing example)
Adafruit_SSD1351 display = Adafruit_SSD1351(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, CS_PIN, DC_PIN, RST_PIN);

int score = 0;
int adcValue;
uint16_t playerPosition;
unsigned long previousMillis = 0;

// Alien Settings
#define ALIEN_HEIGHT 8  
#define NUM_ALIEN_COLUMNS 6
#define NUM_ALIEN_ROWS 3
#define X_START_OFFSET 6
#define SPACE_BETWEEN_ALIEN_COLUMNS 5
#define LARGEST_ALIEN_WIDTH 11
#define SPACE_BETWEEN_ROWS 16
#define INVADERS_DROP_BY 4            // pixel amount that invaders move down by
#define INVADERS_SPEED 42             // speed of movement, lower=faster.
#define INVADER_HEIGHT 8
#define EXPLOSION_GFX_TIME 7  // How long an ExplosionGfx remains on screen before dissapearing
#define INVADERS_Y_START 25//MOTHERSHIP_HEIGHT-1
#define AMOUNT_TO_DROP_BY_PER_LEVEL 4   //NEW How much farther down aliens start per new level
#define LEVEL_TO_RESET_TO_START_HEIGHT 4  // EVERY MULTIPLE OF THIS LEVEL THE ALIEN y START POSITION WILL RESET TO TOP
#define ALIEN_X_MOVE_AMOUNT 15                       //number of pixels moved at start of wave
#define CHANCEOFBOMBDROPPING 20                     // Higher the number the rarer the bomb drop, 
#define BOMB_HEIGHT 4
#define BOMB_WIDTH 2
#define MAXBOMBS 3   // Max number of bombs allowed to drop at a time
// These two lines are for when bombs collide with the bases
#define CHANCE_OF_BOMB_DAMAGE_TO_LEFT_OR_RIGHT 3    // higher more chance
#define CHANCE_OF_BOMB_PENETRATING_DOWN 3           // higher more chance

// Player settingsc
#define TANKGFX_WIDTH 13
#define TANKGFX_HEIGHT 8
#define PLAYER_X_MOVE_AMOUNT 2
#define LIVES 1                 //NEW
#define PLAYER_EXPLOSION_TIME 10  // How long an ExplosionGfx remains on screen before dissapearing
#define PLAYER_Y_START 120
#define PLAYER_X_START 0
#define BASE_WIDTH 16               
#define BASE_WIDTH_IN_BYTES 2
#define BASE_HEIGHT 8
#define BASE_Y 110
#define NUM_BASES 2
#define BASE_MARGINS 10   

#define MISSILE_HEIGHT 4
#define MISSILE_WIDTH 1
#define MISSILE_SPEED 8

// Status of a game object constants
#define ACTIVE 0
#define EXPLODING 1
#define DESTROYED 2
 
// background dah dah dah sound setting
#define NOTELENGTH 1                      // larger means play note longer

// #define FIRE_BUT 4

// graphics
// aliens

const unsigned char InvaderTopGfx [] PROGMEM = {
  B00011000,
  B00111100,
  B01111110,
  B11011011,
  B11111111,
  B00100100,
  B01011010,
  B10100101
};
 
const unsigned char InvaderTopGfx2 [] PROGMEM = {
  B00011000,
  B00111100,
  B01111110,
  B11011011,
  B11111111,
  B01011010,
  B10000001,
  B01000010
};
 
const unsigned char PROGMEM InvaderMiddleGfx []=
{
  B00100000,B10000000,
  B00010001,B00000000,
  B00111111,B10000000,
  B01101110,B11000000,
  B11111111,B11100000,
  B10111111,B10100000,
  B10100000,B10100000,
  B00011011,B00000000
};
 
const unsigned char PROGMEM InvaderMiddleGfx2 [] = {
  B00100000,B10000000,
  B00010001,B00000000,
  B10111111,B10100000,
  B10101110,B10100000,
  B11111111,B11100000,
  B00111111,B10000000,
  B00100000,B10000000,
  B01000000,B01000000
};
 
const unsigned char PROGMEM InvaderBottomGfx [] = {
  B00001111,B00000000,
  B01111111,B11100000,
  B11111111,B11110000,
  B11100110,B01110000,
  B11111111,B11110000,
  B00111001,B11000000,
  B01100110,B01100000,
  B00110000,B11000000
};
 
const unsigned char PROGMEM InvaderBottomGfx2 [] = {
  B00001111,B00000000,
  B01111111,B11100000,
  B11111111,B11110000,
  B11100110,B01110000,
  B11111111,B11110000,
  B00111001,B11000000,
  B01000110,B00100000,
  B10000000,B00010000
};

static const unsigned char PROGMEM ExplosionGfx [] = {
  B00001000,B10000000,
  B01000101,B00010000,
  B00100000,B00100000,
  B00010000,B01000000,
  B11000000,B00011000,
  B00010000,B01000000,
  B00100101,B00100000,
  B01001000,B10010000
};
 
// Player grafix
const unsigned char PROGMEM TankGfx [] = {
  B00000010,B00000000,
  B00000111,B00000000,
  B00000111,B00000000,
  B01111111,B11110000,
  B11111111,B11111000,
  B11111111,B11111000,
  B11111111,B11111000,
  B11111111,B11111000,
};

static const unsigned char PROGMEM MissileGfx [] = {
  B10000000,
  B10000000,
  B10000000,
  B10000000
  
};

static const unsigned char PROGMEM AlienBombGfx [] = {
  B10000000,
  B01000000,
  B10000000,
  B01000000
  
};

static const unsigned char PROGMEM BaseGfx [] = {
  B00011111,B11111000,
  B01111111,B11111110,
  B11111111,B11111111,
  B11111111,B11111111,
  B11111111,B11111111,
  B11111000,B00011111,
  B11100000,B00000111,
  B11100000,B00000111
};


// Game structures
 
struct GameObjectStruct  {
  // base object which most other objects will include
    signed int X;
    signed int Y;  
    unsigned char Status;  //0 active, 1 exploding, 2 destroyed
};
 

struct BaseStruct {
    GameObjectStruct Ord;
    unsigned char Gfx[16];
};


struct AlienStruct  {
  GameObjectStruct Ord;
  unsigned char ExplosionGfxCounter; // how long we want the ExplosionGfx to last
};
 
struct PlayerStruct  {
    GameObjectStruct Ord;
    unsigned int Score;
    unsigned char Lives;
    unsigned char Level;
    unsigned char AliensDestroyed;    // count of how many killed so far
    unsigned char AlienSpeed;         // higher the number slower they go, calculated when ever alien destroyed
    unsigned char ExplosionGfxCounter; // how long we want the ExplosionGfx to last
};


// general global variables
// Adafruit_SSD1351 display(1);

 
//alien global vars
//The array of aliens across the screen
AlienStruct  Alien[NUM_ALIEN_COLUMNS][NUM_ALIEN_ROWS];
GameObjectStruct AlienBomb[MAXBOMBS];
BaseStruct Base[NUM_BASES];

static const int TOTAL_ALIENS=NUM_ALIEN_COLUMNS*NUM_ALIEN_ROWS;  //NEW

 
// widths of aliens
// as aliens are the same type per row we do not need to store their graphic width per alien in the structure above
// that would take a byte per alien rather than just three entries here, 1 per row, saving significnt memory
byte AlienWidth[]={8,11,12};  // top, middle ,bottom widths

 
char AlienXMoveAmount=2;  
signed char InvadersMoveCounter;            // counts down, when 0 move invaders, set according to how many aliens on screen
bool AnimationFrame=false;   // two frames of animation, if true show one if false show the other

// Player global variables
PlayerStruct Player;
GameObjectStruct Missile;
 
// game variables
unsigned int HiScore;
bool GameInPlay=false;

// Sound settings and variables

// music (dah dah dah sound) control, these are the "pitch" of the four basic notes
const unsigned char Music[] =  {
  160,100,80,62
};

unsigned char MusicIndex;                     // index pointer to next note to play
unsigned MusicCounter;                        // how long note plays for countdown timer, is set to
                                              // NOTELENGTH define above

bool PlayerExplosionNoiseCompleted=false;     // flag to indicate when noise has completed
bool ShootCompleted=true;                     // stops music when this is false, so we can here shoot sound

void setup() {
  // put your setup code here, to run once:
  display.begin();

  Serial.begin(9600);

  uint16_t time = millis();

  display.fillScreen(BLACK);
  time = millis() - time;

  //put player at the center;
  playerPosition = 64;

  //SPACEINVADERS SETUP WELCOME SCREEN
  // srand(time(NULL));
  display.setCursor(10, 50);
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.print("Welcome soldier!");
  delay(3000);
  display.fillScreen(BLACK);
  display.setCursor(10, 50);
  display.print("Brave the COMMAND PROMPT INVADERS and come back a hero.");
  delay(5500);
  display.fillScreen(BLACK);
  display.setCursor(10, 50);
  display.print("Your operating system is depending upon you.");
  delay(5500);
  display.fillScreen(BLACK);
  display.setCursor(10, 50);
  display.print("Good luck.");
  delay(3000);
  display.fillScreen(BLACK);

  InitAliens(0); 
  InitPlayer();

  display.setTextSize(1);
  display.setTextColor(WHITE);

  EEPROM.get(0,HiScore);
  if(HiScore==65535)  // new unit never written to
  { 
    HiScore=0;
    EEPROM.put(0,HiScore);
  }

}

void loop() {
  if(GameInPlay)
  {
    Physics();
    UpdateDisplay();
  }
  else  
    AttractScreen();
  
}

void drawPlayer() {
  display.fillRect(0, 120, 10, 128, BLACK);
  
  // player
  if(Player.Ord.Status==ACTIVE)
    display.drawBitmap(Player.Ord.X, Player.Ord.Y,  TankGfx, TANKGFX_WIDTH, TANKGFX_HEIGHT,GREEN);
  else {    
    if(Player.Ord.Status==EXPLODING)  {
      for(int i=0;i<TANKGFX_WIDTH;i+=2)  {
        display.drawBitmap(Player.Ord.X+i, Player.Ord.Y,  ExplosionGfx, random(4)+2, 8,YELLOW);
      }
      Player.ExplosionGfxCounter--;
      if(Player.ExplosionGfxCounter==0)  {
        Player.Ord.Status=DESTROYED;
        LoseLife();
      }
    }
  }
}

void default_HUD() {
  display.setCursor(5, 5);
  display.setTextColor(WHITE);
  display.setTextSize(0.7);
  display.print("SCORE:");
  display.print(score);
  display.drawFastHLine(0, 15, 128, GREEN);
}

void AttractScreen()
{
  display.fillScreen(BLACK);   
  CentreText("Play",0); 
  CentreText("Space Invaders",12); 
  CentreText("High Score     ",36);   
  display.setCursor(80,36);
  display.print(HiScore);
  delay(1000);

  GameInPlay=true;
  NewGame();
}

void Physics()  {  
  if(Player.Ord.Status==ACTIVE) {
    AlienControl(); 
    PlayerControl(); 
    MissileControl();
    CheckCollisions();
  }
}

unsigned char GetScoreForAlien(int RowNumber)
{
   // returns value for killing and alien at the row indicated
   switch (RowNumber)
   {
     case 0:
      return 30;
     case 1:
      return 20;
     case 2:
      return 10;
   }
}
 
void PlayerControl()  {
  // user input checks
  adcValue = analogRead(A0);
  Player.Ord.X = map(adcValue, 0, 1023, 0, 126);

  if(Missile.Status!=ACTIVE)
  {
    Missile.X=Player.Ord.X+(TANKGFX_WIDTH/2);
    Missile.Y=PLAYER_Y_START;
    Missile.Status=ACTIVE;
  }
}

void MissileControl()
{ 
  if(Missile.Status==ACTIVE)  
  {
    Missile.Y-=MISSILE_SPEED;
    if(Missile.Y+MISSILE_HEIGHT<0)       // If off top of screen destroy so can be used again
      Missile.Status=DESTROYED;
  }
}


void AlienControl()
{
  if((InvadersMoveCounter--)<0)
  {
    bool Dropped=false;
    if((RightMostPos()+AlienXMoveAmount>=SCREEN_WIDTH) |(LeftMostPos()+AlienXMoveAmount<0)) // at edge of screen
    {
      AlienXMoveAmount=-AlienXMoveAmount;               // reverse direction
      Dropped=true;                                     // and indicate we are dropping
    }

    for(int Across=0;Across<NUM_ALIEN_COLUMNS;Across++) 
    {
      for(int Down=0;Down<3;Down++)
      {
        if(Alien[Across][Down].Ord.Status==ACTIVE)
        {
          if(Dropped==false)
            Alien[Across][Down].Ord.X+=AlienXMoveAmount;
          else
            Alien[Across][Down].Ord.Y+=INVADERS_DROP_BY;
        }
      }
    }  
    InvadersMoveCounter=Player.AlienSpeed;
    AnimationFrame=!AnimationFrame;  ///swap to other frame
  }
  // should the alien drop a bomb
  if(random(CHANCEOFBOMBDROPPING)==1)  
    DropBomb();
  MoveBombs();
}
 
 
void MoveBombs(){
  for(int i=0;i<MAXBOMBS;i++){
    if(AlienBomb[i].Status==ACTIVE)
      AlienBomb[i].Y+=2;
  }
}


void DropBomb()  {
  // if there is a free bomb slot then drop a bomb else nothing happens
  bool Free=false;
  unsigned char ActiveCols[NUM_ALIEN_COLUMNS];  
  unsigned char BombIdx=0;
  // find a free bomb slot
  while((Free==false)&(BombIdx<MAXBOMBS)){
    if(AlienBomb[BombIdx].Status==DESTROYED)
      Free=true;
    else
      BombIdx++;
  }
  if(Free)  {
    unsigned char Columns=0;
    // now pick and alien at random to drop the bomb
    // we first pick a column, obviously some columns may not exist, so we count number of remaining cols 
    // first, this adds time but then also picking columns randomly that don't exist may take time also
    unsigned char ActiveColCount=0;
    signed char Row;
    unsigned char ChosenColumn;
    
    while(Columns<NUM_ALIEN_COLUMNS){
      Row=2;
      while(Row>=0)  {
        if(Alien[Columns][Row].Ord.Status==ACTIVE)
        {
          ActiveCols[ActiveColCount]=Columns;
          ActiveColCount++;
          break;
        }
        Row--;
      }
      Columns++;
    }
    // we have ActiveCols array filled with the column numbers of the active cols and we have how many
    // in ActiveColCount, now choose a column at random
    ChosenColumn=random(ActiveColCount);  // random number between 0 and the amount of columns
    // we now find the first available alien in this column to drop the bomb from
    Row=2;
    while(Row>=0)  {
      if(Alien[ActiveCols[ChosenColumn]][Row].Ord.Status==ACTIVE)  {
        // Set the bomb from this alien
        AlienBomb[BombIdx].Status=ACTIVE;
        AlienBomb[BombIdx].X=Alien[ActiveCols[ChosenColumn]][Row].Ord.X+int(AlienWidth[Row]/2);
        // above sets bomb to drop around invaders centre, here we add a litrle randomness around this pos
        AlienBomb[BombIdx].X=(AlienBomb[BombIdx].X-2)+random(0,4);
        AlienBomb[BombIdx].Y=Alien[ActiveCols[ChosenColumn]][Row].Ord.Y+4;        
        break;
      }
      Row--;
    }
  }
}



void BombCollisions()
{
  //check bombs collisions
  for(int i=0;i<MAXBOMBS;i++)
  {
    if(AlienBomb[i].Status==ACTIVE)
    {
      if(AlienBomb[i].Y>128)             // gone off bottom of screen
        AlienBomb[i].Status=DESTROYED;
      else  
      {
        // HAS IT HIT PLAYERS missile
          if(Collision(AlienBomb[i],BOMB_WIDTH,BOMB_HEIGHT,Missile,MISSILE_WIDTH,MISSILE_HEIGHT))
          {
              // destroy missile and bomb
              AlienBomb[i].Status=EXPLODING;
              Missile.Status=DESTROYED;
          }
          else
          {      
             // has it hit players ship          
            if(Collision(AlienBomb[i],BOMB_WIDTH,BOMB_HEIGHT,Player.Ord,TANKGFX_WIDTH,TANKGFX_HEIGHT))
            {
               PlayerHit();
               AlienBomb[i].Status=DESTROYED;
            }
            else  
              BombAndBasesCollision(&AlienBomb[i]);
          }
       }
    }
  }  
}

void BombAndBasesCollision(GameObjectStruct *Bomb)  {
  // check and handle any bomb and base collision
  for(int i=0;i<NUM_BASES;i++)
  {
      if(Collision(*Bomb,BOMB_WIDTH,BOMB_HEIGHT,Base[i].Ord,BASE_WIDTH,BASE_HEIGHT)) 
      {
        // Now get the position of the bomb within the structure so we can destroy it bit by bit
        // we first get the relative position from the left hand side of the base
        // then we divide this by 2 (X>>1)  , this gives us a position with 2bit resolution
        unsigned char X=Bomb->X-Base[i].Ord.X;
        X=X>>1;
        
        // Because the bomb is 2 pixels wide it's X pos could have been less than the bases XPos, if this is the case the above substaction
        // on an unsigned char (byte) will cause a large number to occur (255), we check for this and if this large number has
        // resulted we just set X to 0 for the start of the bases structure
        if(X>7)  X=0; 

        // We now look at wether the bomb is hitting a part of the structure below it.
        // the bomb may be past the top of the base (even on first collision detection) as it moves in Y amounts greater than 1 pixel to give a higher speed
        // so we start from top of the base and destroy any structure of the base that comes before or equal to the
        // bombs Y pos, if we go past the bomb Y without finding any bit of base then stop looking and bomb is allowed to progress further down
        signed char Bomb_Y=(Bomb->Y+BOMB_HEIGHT)-Base[i].Ord.Y;
        unsigned char Base_Y=0;

        while((Base_Y<=Bomb_Y)&(Base_Y<BASE_HEIGHT)&(Bomb->Status==ACTIVE))
        {
          unsigned char Idx=(Base_Y*BASE_WIDTH_IN_BYTES)+(X>>2);  // this gets the index for the byte in question from the gfx array
          unsigned char TheByte=Base[i].Gfx[Idx];   // we now have the byte to inspect, but need to only look at the 2 bits where the bomb is colliding
          // now isolate the 2 bits in question, we have the correct byte and we only have need the bottom 2 bits of X now to denote which bits
          // we need to destroy in the byte itself
          unsigned char BitIdx=X & 3;       // this preseves the bottom 2 bits and removes the rest
          // A value of X of 0 (zero) would mean we want to look at the first 2 bits of the byte
          // A value of X of 1 (zero) would mean we want to look at the second 2 bits of the byte
          // A value of X of 2 (zero) would mean we want to look at the third 2 bits of the byte
          // A value of X of 3 (zero) would mean we want to look at the fourth 2 bits of the byte
          
          // So we need an AND mask to isolate these bits depending on the value of X
          // Here it is and we initially set it to the first 2 bits
          unsigned char Mask=B11000000;
          // now we need to move those 2 "11"'s to the right depending on the value of X as noted above
          // the next line may look odd but all we're doing is multiplying X by 2 initially, so the values above would become,0,2,4,6 rather than 0,1,2,3
          // Then we move the bits in the mask by this new amount, check it above,a value of X of 2 would shift the bits 4 places, whichis correct!
          Mask=Mask>>(BitIdx<<1);
          
          //now we peform a logical and to remove all bits (pixels) from around these two bits
          TheByte=TheByte & Mask;
          
          // and if there are any set pixels (bits) then they must be destroyed, else the bomb is allowed to continue
          if(TheByte>0) 
          { 
            // There are some pixels to destroy
            //We need to remove the pixels(bits) where there were "11"'s in the mask and leave those intact where there were 0's
            //easiest way, flip all 0's in the mask to ones and all 1's to 0's, the tilde "~" means invert (swap), reffered to as logical NOT
            Mask=~Mask; 
            // we can then "AND" the byte with the Mask to remove the bits but leave the rest intact
            Base[i].Gfx[Idx]=Base[i].Gfx[Idx] & Mask;

            // now do some collateral damage to surrounding bricks, try left hand side first
            if(X>0){          // not at far left, if we were there would be nothing to destroy to this left
              if(random(CHANCE_OF_BOMB_DAMAGE_TO_LEFT_OR_RIGHT))   // if true blow some bricks to the left hand side
              {
                if(X!=4)       // we not at start of second byte
                {                  
                  Mask=(Mask<<1)|1;
                  Base[i].Gfx[Idx]=Base[i].Gfx[Idx] & Mask;
                }
                else          // we are at very start of second byte, so wipe out adjacent pixel first byte
                {                  
                  Base[i].Gfx[Idx-1]=Base[i].Gfx[Idx-1] & B11111110;
                }
              }
            } 
            
            // now do some collateral damage right hand side first
            if(X<7){          // not at far right, if we were there would be nothing to destroy to this right
              if(random(CHANCE_OF_BOMB_DAMAGE_TO_LEFT_OR_RIGHT))   // if true blow some bricks to the left hand side
              {
                if(X!=3)       // not at last pixel of left of first byte
                {                  
                  Mask=(Mask>>1)|128;
                  Base[i].Gfx[Idx]=Base[i].Gfx[Idx] & Mask;
                }
                else          // we are at last pixel of first byte so destroy pixil in adjacent byte
                {                  
                  Base[i].Gfx[Idx+1]=Base[i].Gfx[Idx+1] & B01111111;
                }
              }
            }
            if(random(CHANCE_OF_BOMB_PENETRATING_DOWN)==false)   // if false BOMB EXPLODES else carries on destroying more
              Bomb->Status=EXPLODING;
          }
          else
            Base_Y++;
        }
      }
  }
}

void MissileAndBasesCollisions()  {
  // check and handle any player missile and base collision
  for(int i=0;i<NUM_BASES;i++)
  { 
      if(Collision(Missile,MISSILE_WIDTH,MISSILE_HEIGHT,Base[i].Ord,BASE_WIDTH,BASE_HEIGHT)) 
      {
        // Now get the position of the bomb within the structure so we can destroy it bit by bit
        // we first get the relative position from the left hand side of the base
        // then we divide this by 2 (X>>1)  , this gives us a position with 2bit resolution
        unsigned char X=Missile.X-Base[i].Ord.X;
        X=X>>1;
        
        // Because the bomb is 2 pixels wide it's X pos could have been less than the bases XPos, if this is the case the above substaction
        // on an unsigned char (byte) will cause a large number to occur (255), we check for this and if this large number has
        // resulted we just set X to 0 for the start of the bases structure
        if(X>7)  X=0; 

        // We now look at wether the bomb is hitting a part of the structure above it.
        // the bomb may be past the top of the base (even on first collision detection) as it moves in Y amounts greater than 1 pixel to give a higher speed
        // so we start from top of the base and destroy any structure of the base that comes before or equal to the
        // bombs Y pos, if we go past the bomb Y without finding any bit of base then stop looking and bomb is allowed to progress further down
        signed char Missile_Y=Missile.Y-Base[i].Ord.Y;
        signed char Base_Y=BASE_HEIGHT-1;
        while((Base_Y>=Missile_Y)&(Base_Y>=0)&(Missile.Status==ACTIVE))
        {
         // if(Base_Y<0) {Serial.println("oop"); delay(999999);}
          unsigned char Idx=(Base_Y*BASE_WIDTH_IN_BYTES)+(X>>2);  // this gets the index for the byte in question from the gfx array
          unsigned char TheByte=Base[i].Gfx[Idx];   // we now have the byte to inspect, but need to only look at the 2 bits where the bomb is colliding
          
          // now isolate the 2 bits in question, we have the correct byte and we only have need the bottom 2 bits of X now to denote which bits
          // we need to destroy in the byte itself
          unsigned char BitIdx=X & 3;       // this preseves the bottom 2 bits and removes the rest
          // A value of X of 0 (zero) would mean we want to look at the first 2 bits of the byte
          // A value of X of 1 (zero) would mean we want to look at the second 2 bits of the byte
          // A value of X of 2 (zero) would mean we want to look at the third 2 bits of the byte
          // A value of X of 3 (zero) would mean we want to look at the fourth 2 bits of the byte
          
          // So we need an AND mask to isolate these bits depending on the value of X
          // Here it is and we initially set it to the first 2 bits
          unsigned char Mask=B11000000;
          
          // now we need to move those 2 "11"'s to the right depending on the value of X as noted above
          // the next line may look odd but all we're doing is multiplying X by 2 initially, so the values above would become,0,2,4,6 rather than 0,1,2,3
          // Then we move the bits in the mask by this new amount, check it above,a value of X of 2 would shift the bits 4 places, whichis correct!
          Mask=Mask>>(BitIdx<<1);
          
          //now we peform a logical AND to remove all bits (pixels) from around these two bits
          TheByte=TheByte & Mask;
          
          // and if there are any set pixels (bits) then they must be destroyed, else the bomb is allowed to continue
          if(TheByte>0) 
          { 
            // There are some pixels to destroy
            //We need to remove the pixels(bits) where there were "11"'s in the mask and leave those intact where there were 0's
            //easiest way, flip all 0's in the mask to ones and all 1's to 0's, the tilde "~" means invert (swap), reffered to as logical NOT
            Mask=~Mask; 
            // we can then "AND" the byte with the Mask to remove the bits but leave the rest intact
            Base[i].Gfx[Idx]=Base[i].Gfx[Idx] & Mask;

            // now do some collateral damage to surrounding bricks, try left hand side first
            if(X>0){          // not at far left, if we were there would be nothing to destroy to this left
              if(random(CHANCE_OF_BOMB_DAMAGE_TO_LEFT_OR_RIGHT))   // if true blow some bricks to the left hand side
              {
                if(X!=4)       // we not at start of second byte
                {                  
                  Mask=(Mask<<1)|1;
                  Base[i].Gfx[Idx]=Base[i].Gfx[Idx] & Mask;
                }
                else          // we are at very start of second byte, so wipe out adjacent pixel first byte
                {                  
                  Base[i].Gfx[Idx-1]=Base[i].Gfx[Idx-1] & B11111110;
                }
              }
            } 
            
            // now do some collateral damage right hand side first
            if(X<7){          // not at far right, if we were there would be nothing to destroy to this right
              if(random(CHANCE_OF_BOMB_DAMAGE_TO_LEFT_OR_RIGHT))   // if true blow some bricks to the left hand side
              {
                if(X!=3)       // not at last pixel of left of first byte
                {                  
                  Mask=(Mask>>1)|128;
                  Base[i].Gfx[Idx]=Base[i].Gfx[Idx] & Mask;
                }
                else          // we are at last pixel of first byte so destroy pixil in adjacent byte
                {                  
                  Base[i].Gfx[Idx+1]=Base[i].Gfx[Idx+1] & B01111111;
                }
              }
            }
            if(random(CHANCE_OF_BOMB_PENETRATING_DOWN)==false)   // if false BOMB EXPLODES else carries on destroying more
              Missile.Status=EXPLODING;
          }
          else
            Base_Y--;
        }
      }
  }
}

void PlayerHit()  {
  Player.Ord.Status=EXPLODING;
  Player.ExplosionGfxCounter=PLAYER_EXPLOSION_TIME;
  Missile.Status=DESTROYED;
}

void CheckCollisions()
{
  MissileAndAlienCollisions();
  MissileAndBasesCollisions();
  BombCollisions(); 
  AlienAndBaseCollisions();
}

char GetAlienBaseCollisionMask(int AlienX, int AlienWidth,int BaseX)
{
  signed int DamageWidth;
  unsigned char LeftMask,RightMask,FullMask;
  

  // this routine uses a 1 to mean remove bit and 0 to preserve, this is kind of opposite of what we would 
  // normally think of, but it's done this way as when we perform bit shifting to show which bits are preserved
  // it will shift in 0's in (as that's just was the C shift operater ">>" and "<<" does
  // at the end we will flip all the bits 0 becomes 1, 1 becomes 0 etc. so that the mask then works correctly
  // with the calling code
  
  LeftMask=B11111111;     // initially set to remove all
  RightMask=B11111111;    // unless change in code below                
  // if Alien X more than base x then some start bits are unaffected
  if(AlienX>BaseX)  
  {                    
    // we shift the bits above to the right by the amount unaffected, thus putting 0's into the bits
    // not to delete
    DamageWidth=AlienX-BaseX;
    LeftMask>>=DamageWidth;          
  }
  
  // now work out how much of remaining byte is affected
  
  // if right hand side of alien is less than BaseX right hand side then some preserved at the right hand end
  if(AlienX+AlienWidth<BaseX+(BASE_WIDTH/2))
  {
    DamageWidth=(BaseX+(BASE_WIDTH/2))-(AlienX+AlienWidth);
    RightMask<<=DamageWidth;      
    
  }
  // we now have two masks, one showing which bits to preserve on left of the byte, the other the right hand side, 
  // we need to combine them to one mask, the code in the brackets does this combining

  // at the moment a 0 means keep, 1 destroy, but this is actually makes it difficult to implement later on, so we flip
  // the bits to be a more logical 1= keep bit and 0 remove bit (pixel) and then return the mask
  // the tilde (~) flips the bits that resulted from combining the two masks
  
  return ~(LeftMask & RightMask);
}

void DestroyBase(GameObjectStruct *Alien,BaseStruct *Base,char Mask,int BaseByteOffset)
{
  signed char Y;
  // go down "removing" bits to the depth that the alien is down into the base
  Y=(Alien->Y+ALIEN_HEIGHT)-Base->Ord.Y;
  if(Y>BASE_HEIGHT-1) Y=BASE_HEIGHT-1;
  for(;Y>=0;Y--){
    Base->Gfx[(Y*2)+BaseByteOffset]=Base->Gfx[(Y*2)+BaseByteOffset] & Mask;
  }
}

void AlienAndBaseCollisions()  
{
  unsigned char Mask;
  // checks if aliens are in collision with the tank
  // start at bottom row as they are most likely to be in contact or not and if not then none further up are either
  for(int row=2;row>=0;row--)
  {
    for(int column=0;column<NUM_ALIEN_COLUMNS;column++)
    {
      if(Alien[column][row].Ord.Status==ACTIVE)
      {
        // now scan for a collision with each base in turn
        for(int BaseIdx=0;BaseIdx<NUM_BASES;BaseIdx++)
        {          
          if(Collision(Alien[column][row].Ord,AlienWidth[row],ALIEN_HEIGHT,Base[BaseIdx].Ord,BASE_WIDTH,BASE_HEIGHT))
          {
            // WE HAVE A COLLSISION, REMOVE BITS OF BUILDING
            // process left half (first byte) of base first
            Mask=GetAlienBaseCollisionMask(Alien[column][row].Ord.X,AlienWidth[row],Base[BaseIdx].Ord.X);
            DestroyBase(&Alien[column][row].Ord,&Base[BaseIdx],Mask,0);
            
            // do right half, second byte of base
            Mask=GetAlienBaseCollisionMask(Alien[column][row].Ord.X,AlienWidth[row],Base[BaseIdx].Ord.X+(BASE_WIDTH/2));
            DestroyBase(&Alien[column][row].Ord,&Base[BaseIdx],Mask,1);            
          }
        }
      }
    }
  }
}

void MissileAndAlienCollisions()
{
   for(int across=0;across<NUM_ALIEN_COLUMNS;across++)
  {
    for(int down=0;down<NUM_ALIEN_ROWS;down++)
    {
      if(Alien[across][down].Ord.Status==ACTIVE)
      {
        if(Missile.Status==ACTIVE)
        {
          if(Collision(Missile,MISSILE_WIDTH,MISSILE_HEIGHT,Alien[across][down].Ord,AlienWidth[down],INVADER_HEIGHT))
          {
              // missile hit
              Alien[across][down].Ord.Status=EXPLODING;
              Missile.Status=DESTROYED;
              Player.Score+=GetScoreForAlien(down);
              Player.AliensDestroyed++;
              // calc new speed of aliens, note (float) must be before TOTAL_ALIENS to force float calc else
              // you will get an incorrect result
              Player.AlienSpeed=((1-(Player.AliensDestroyed/(float)TOTAL_ALIENS))*INVADERS_SPEED);              
              // for very last alien make to  fast!
              if(Player.AliensDestroyed==TOTAL_ALIENS-2)
                if(AlienXMoveAmount>0)
                  AlienXMoveAmount=ALIEN_X_MOVE_AMOUNT*2;
                else
                  AlienXMoveAmount=-(ALIEN_X_MOVE_AMOUNT*2);
              // for very last alien make to super fast!
              if(Player.AliensDestroyed==TOTAL_ALIENS-1)
                if(AlienXMoveAmount>0)
                  AlienXMoveAmount=ALIEN_X_MOVE_AMOUNT*4;
                else
                  AlienXMoveAmount=-(ALIEN_X_MOVE_AMOUNT*4);
                  
              if(Player.AliensDestroyed==TOTAL_ALIENS)
                NextLevel(&Player);            
          }
        }
          if(Alien[across][down].Ord.Status==ACTIVE)      // double check still active afer above code
        {
          // check if this alien is in contact with TankGfx
          if(Collision(Player.Ord,TANKGFX_WIDTH,TANKGFX_HEIGHT,Alien[across][down].Ord,AlienWidth[down],ALIEN_HEIGHT))
             PlayerHit();  
          else 
          {
            // check if alien is below bottom of screen
             if(Alien[across][down].Ord.Y+8>SCREEN_HEIGHT)
                 PlayerHit();                   
          }
        }
      }
    }
  }
}

bool Collision(GameObjectStruct Obj1,unsigned char Width1,unsigned char Height1,GameObjectStruct Obj2,unsigned char Width2,unsigned char Height2)
{
  return ((Obj1.X+Width1>Obj2.X)&(Obj1.X<Obj2.X+Width2)&(Obj1.Y+Height1>Obj2.Y)&(Obj1.Y<Obj2.Y+Height2));
}

int RightMostPos()  {
  //returns x pos of right most alien
  int Across=NUM_ALIEN_COLUMNS-1;
  int Down;
  int Largest=0;
  int RightPos;
  while(Across>=0){
    Down=0;
    while(Down<NUM_ALIEN_ROWS){
      if(Alien[Across][Down].Ord.Status==ACTIVE)
      {
        // different aliens have different widths, add to x pos to get rightpos
        RightPos= Alien[Across][Down].Ord.X+AlienWidth[Down];
        if(RightPos>Largest)
          Largest=RightPos;
      }
      Down++;
    }
    if(Largest>0)  // we have found largest for this coloum
      return Largest;
    Across--;
  }
  return 0;  // should never get this far
}
 
int LeftMostPos()  {
  //returns x pos of left most alien
  int Across=0;
  int Down;
  int Smallest=SCREEN_WIDTH*2;
  while(Across<NUM_ALIEN_COLUMNS){
    Down=0;
    while(Down<3){
      if(Alien[Across][Down].Ord.Status==ACTIVE)
        if(Alien[Across][Down].Ord.X<Smallest)
          Smallest=Alien[Across][Down].Ord.X;
      Down++;
    }
    if(Smallest<SCREEN_WIDTH*2)  // we have found smalest for this coloum
      return Smallest;
    Across++;
  }
  return 0;  // should nevr get this far
}
 
void UpdateDisplay()  
{
  int i;
  
  display.fillScreen(BLACK); 
  display.setCursor(0,0);
  display.print(Player.Score);

  //BOMBS
  // draw bombs next as aliens have priority of overlapping them
  for(i=0;i<MAXBOMBS;i++)  {
      if(AlienBomb[i].Status==ACTIVE)
        display.drawBitmap(AlienBomb[i].X, AlienBomb[i].Y,  AlienBombGfx, 2, 4, RED);
      else  {// must be destroyed
        if(AlienBomb[i].Status==EXPLODING)
          display.drawBitmap(AlienBomb[i].X-4, AlienBomb[i].Y,  ExplosionGfx, 4, 8, YELLOW);
        // Ensure on next draw that ExplosionGfx dissapears
        AlienBomb[i].Status=DESTROYED;
      }
  }
  
  //Invaders
  
  // unsigned long currentMillis = millis();
    
  for(int across=0;across<NUM_ALIEN_COLUMNS;across++)
  {
    for(int down=0;down<NUM_ALIEN_ROWS;down++)
    {
      if(Alien[across][down].Ord.Status==ACTIVE){
        switch(down)  {
          case 0: 
            if(AnimationFrame)
              display.drawBitmap(Alien[across][down].Ord.X, Alien[across][down].Ord.Y,  InvaderTopGfx, AlienWidth[down],INVADER_HEIGHT,RED);
            else
              display.drawBitmap(Alien[across][down].Ord.X, Alien[across][down].Ord.Y,  InvaderTopGfx2, AlienWidth[down],INVADER_HEIGHT,RED);
            break;
          case 1: 
            if(AnimationFrame)
              display.drawBitmap(Alien[across][down].Ord.X, Alien[across][down].Ord.Y,  InvaderMiddleGfx, AlienWidth[down],INVADER_HEIGHT,RED);
            else
              display.drawBitmap(Alien[across][down].Ord.X, Alien[across][down].Ord.Y,  InvaderMiddleGfx2, AlienWidth[down],INVADER_HEIGHT,RED);
            break;
          default: 
            if(AnimationFrame)
              display.drawBitmap(Alien[across][down].Ord.X, Alien[across][down].Ord.Y,  InvaderBottomGfx, AlienWidth[down],INVADER_HEIGHT,RED);
            else
              display.drawBitmap(Alien[across][down].Ord.X, Alien[across][down].Ord.Y,  InvaderBottomGfx2, AlienWidth[down],INVADER_HEIGHT,RED);
        }
      }
      else  {
        if(Alien[across][down].Ord.Status==EXPLODING){
          Alien[across][down].ExplosionGfxCounter--;
          if(Alien[across][down].ExplosionGfxCounter>0)  {
            display.drawBitmap(Alien[across][down].Ord.X, Alien[across][down].Ord.Y,  ExplosionGfx, 13, 8,YELLOW);
          }
          else
            Alien[across][down].Ord.Status=DESTROYED;
        }
      }
    }
  }  
  
  //player
  drawPlayer();

  //missile  
  if(Missile.Status==ACTIVE)
  {
    display.drawBitmap(Missile.X, Missile.Y,  MissileGfx, MISSILE_WIDTH, MISSILE_HEIGHT,MAGENTA);
    display.drawBitmap(Missile.X, Missile.Y-2,  MissileGfx, MISSILE_WIDTH, MISSILE_HEIGHT,BLACK);
  }

  // plot bases
  
  for(i=0;i<NUM_BASES;i++)  {    
    if(Base[i].Ord.Status==ACTIVE)
      display.drawBitmap(Base[i].Ord.X, Base[i].Ord.Y,  Base[i].Gfx, BASE_WIDTH, BASE_HEIGHT,BLUE,true);
  }
}

void LoseLife()  {
  Player.Lives--;
  if(Player.Lives>0)  {
    DisplayPlayerAndLives(&Player);
    // clear alien missiles
    for(int i=0;i<MAXBOMBS;i++)  {
      AlienBomb[i].Status=DESTROYED;
      AlienBomb[i].Y=0;
    }
    // ENABLE PLAYER
    Player.Ord.Status=ACTIVE;
    Player.Ord.X=0;
  }
  else  {
    GameOver();
  }
}


void GameOver()  {  
  GameInPlay=false;
  display.fillScreen(BLACK);   
  CentreText("Game Over", 12);   
  CentreText("Score ", 24);   
  display.print(Player.Score);
  if(Player.Score>HiScore)
  {
    CentreText("NEW HIGH SCORE!!!",36);
    CentreText("**CONGRATULATIONS**",48);    
  }
  if(Player.Score>HiScore){    
    HiScore=Player.Score;
    EEPROM.put(0,HiScore);
  }
  delay(2500);  
}

void DisplayPlayerAndLives(PlayerStruct *Player)  {
  display.fillScreen(BLACK); 
  CentreText("Score ",12);   
  display.print(Player->Score);
  delay(2000);
  Player->Ord.X=PLAYER_X_START;
}


void CentreText(const char *Text,unsigned char Y)  {
  // centres text on screen
  display.setCursor(int((float)(SCREEN_WIDTH)/2-((strlen(Text)*6)/2)),Y);
  display.print(Text);
}


void InitPlayer()  {
  Player.Ord.Y=PLAYER_Y_START;
  Player.Ord.X=PLAYER_X_START;
  Player.Ord.Status=ACTIVE;
  Player.Lives=LIVES;
  Player.Level=0;
  Missile.Status=DESTROYED;
  Player.Score=0;
}


void NextLevel(PlayerStruct *Player)  {
  // reset any dropping bombs
  int YStart;
  for(int i=0;i<MAXBOMBS;i++)
    AlienBomb[i].Status=DESTROYED;
  AnimationFrame=false;
  Player->Level++;
  YStart=((Player->Level-1) % LEVEL_TO_RESET_TO_START_HEIGHT)*AMOUNT_TO_DROP_BY_PER_LEVEL;
  InitAliens(YStart);  
  AlienXMoveAmount=ALIEN_X_MOVE_AMOUNT;
  Player->AlienSpeed=INVADERS_SPEED;
  Player->AliensDestroyed=0;
  Missile.Status=DESTROYED;
  randomSeed(100);     
  InitBases();
  DisplayPlayerAndLives(Player);  
  MusicIndex=0;
  MusicCounter=NOTELENGTH;
}


void InitBases()  {
  // Bases need to be re-built!
  uint8_t TheByte;
  int Spacing=(SCREEN_WIDTH-(NUM_BASES*BASE_WIDTH))/NUM_BASES;
  for(int i=0;i<NUM_BASES;i++)  
  {    
    for(int DataIdx=0;DataIdx<BASE_HEIGHT*BASE_WIDTH_IN_BYTES;DataIdx++)  
    {
        TheByte   = pgm_read_byte(BaseGfx + DataIdx);
        Base[i].Gfx[DataIdx]=TheByte;
    }
    Base[i].Ord.X=(i*Spacing)+(i*BASE_WIDTH)+(Spacing/2);
    Base[i].Ord.Y=BASE_Y;
    Base[i].Ord.Status=ACTIVE;
  }
}


void NewGame(){
  InitPlayer();
  NextLevel(&Player);
}

void InitAliens(int YStart)  { 
  for(int across=0;across<NUM_ALIEN_COLUMNS;across++)  {
    for(int down=0;down<3;down++)  {
      // we add down to centralise the aliens, just happens to be the right value we need per row!
      // we need to adjust a little as row zero should be 2, row 1 should be 1 and bottom row 0
      Alien[across][down].Ord.X=X_START_OFFSET+(across*(LARGEST_ALIEN_WIDTH+SPACE_BETWEEN_ALIEN_COLUMNS))-(AlienWidth[down]/2);  
      Alien[across][down].Ord.Y=YStart+(down*SPACE_BETWEEN_ROWS);
      Alien[across][down].Ord.Status=ACTIVE;
      Alien[across][down].ExplosionGfxCounter=EXPLOSION_GFX_TIME;
    }
  }
}