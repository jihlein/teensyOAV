// **************************************************************************
// * teensyOAV - a port of David Thompsen's OpenAero code to the teensy 4
// *
// * John Ihlein  July 2021
// **************************************************************************

// **************************************************************************
// OpenAero code by David Thompson, included open-source code as per quoted references.
//
// **************************************************************************
// *             GNU GPL V3 notice
// **************************************************************************
// * Copyright (C) 2016 David Thompson
// * 
// * This program is free software: you can redistribute it and/or modify
// * it under the terms of the GNU General Public License as published by
// * the Free Software Foundation, either version 3 of the License, or
// * (at your option) any later version.
// * 
// * This program is distributed in the hope that it will be useful,
// * but WITHOUT ANY WARRANTY; without even the implied warranty of
// * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// * GNU General Public License for more details.
// * 
// * You should have received a copy of the GNU General Public License
// * along with this program. If not, see <http://www.gnu.org/licenses/>.
// * 
// * tl;dr - all derivative code MUST be released with the source code!
// *
// **************************************************************************

// ************************************************************
// * Text to print (non-menu)
// ************************************************************

// Status menu
const char StatusText0[]     PROGMEM = "Version: 1.5";  // <-- Change version number here !!!
const char StatusText1[]     PROGMEM = "Mode:";
const char StatusText3[]     PROGMEM = "Profile:";
const char StatusText4[]     PROGMEM = ".";
const char StatusText5[]     PROGMEM = "0";  
const char StatusText7[]     PROGMEM = "Battery:";
const char StatusText8[]     PROGMEM = "Pos:";
const char StatusText9[]     PROGMEM = "Jitter:";
//
const char MenuFrame0[]      PROGMEM = "A";            // Down marker
const char MenuFrame2[]      PROGMEM = ">";            // Right
const char MenuFrame3[]      PROGMEM = "<";            // Left
const char MenuFrame4[]      PROGMEM = "D";            // Cursor
const char MenuFrame1[]      PROGMEM = "E";            // Up
const char MenuFrame5[]      PROGMEM = "Menu";         // Menu
const char MenuFrame6[]      PROGMEM = "Back";         // Back
const char MenuFrame7[]      PROGMEM = "Def.";         // Default
const char MenuFrame8[]      PROGMEM = "Save";         // Save
const char MenuFrame9[]      PROGMEM = "Abort";        // Abort
// Main menu list text
const char MainMenuItem0[]   PROGMEM = "1. General";
const char MainMenuItem1[]   PROGMEM = "2. Receiver setup";
const char MainMenuItem9[]   PROGMEM = "3. Receiver inputs";
const char MainMenuItem7[]   PROGMEM = "4. Stick polarity";
const char MainMenuItem8[]   PROGMEM = "5. Sensor calibration";
const char MainMenuItem10[]  PROGMEM = "6. Level meter";
const char MainMenuItem2[]   PROGMEM = "7. Flight profile 1";
const char MainMenuItem3[]   PROGMEM = "8. Flight profile 2";
const char MainMenuItem30[]  PROGMEM = "9. Curves";
const char MainMenuItem25[]  PROGMEM = "10. Output offsets";
const char MainMenuItem11[]  PROGMEM = "11. OUT1 Mixer";
const char MainMenuItem12[]  PROGMEM = "12. OUT2 Mixer";
const char MainMenuItem13[]  PROGMEM = "13. OUT3 Mixer";
const char MainMenuItem14[]  PROGMEM = "14. OUT4 Mixer";
const char MainMenuItem15[]  PROGMEM = "15. OUT5 Mixer";
const char MainMenuItem16[]  PROGMEM = "16. OUT6 Mixer";
const char MainMenuItem17[]  PROGMEM = "17. OUT7 Mixer";
const char MainMenuItem18[]  PROGMEM = "18. OUT8 Mixer";
const char MainMenuItem4[]   PROGMEM = "19. OUT9 Mixer";
const char MainMenuItem5[]   PROGMEM = "20. OUT10 Mixer";
const char MainMenuItem20[]  PROGMEM = "21. Servo direction";
const char MainMenuItem22[]  PROGMEM = "22. Neg. Servo trvl. (%)";
const char MainMenuItem23[]  PROGMEM = "23. Pos. Servo trvl. (%)";
const char MainMenuItem32[]  PROGMEM = "24. Custom Ch. order";
const char MainMenuItem31[]  PROGMEM = "25. In/Out display";
const char MainMenuItem24[]  PROGMEM = "26. Error log";
// Sensors text
const char PText15[]         PROGMEM = "Gyro";
const char PText16[]         PROGMEM = "Roll";
const char PText17[]         PROGMEM = "Pitch";
const char PText18[]         PROGMEM = "Yaw";
const char PText19[]         PROGMEM = "Acc";
const char PText20[]         PROGMEM = "IMU";
const char SensorMenuItem1[] PROGMEM = "Cal."; 
const char SensorMenuItem2[] PROGMEM = "Inv."; 
// Profile
const char AutoMenuItem1[]   PROGMEM = "Roll P:";
const char StabMenuItem2[]   PROGMEM = "Roll I:"; 
const char StabMenuItem10[]  PROGMEM = "Roll I limit:"; 
const char StabMenuItem3[]   PROGMEM = "Roll I rate:";
const char AutoMenuItem20[]  PROGMEM = "Roll autolevel:"; 
const char AutoMenuItem200[] PROGMEM = "Yaw autolevel:";
const char AutoMenuItem7[]   PROGMEM = "Roll trim:";
const char AutoMenuItem4[]   PROGMEM = "Pitch P:";
const char StabMenuItem5[]   PROGMEM = "Pitch I:";
const char StabMenuItem11[]  PROGMEM = "Pitch I limit:";
const char StabMenuItem6[]   PROGMEM =  "Pitch I rate:"; 
const char AutoMenuItem21[]  PROGMEM = "Pitch autolevel:";
const char AutoMenuItem8[]   PROGMEM = "Pitch trim:";
const char StabMenuItem7[]   PROGMEM = "Yaw P:"; 
const char StabMenuItem8[]   PROGMEM = "Yaw I:";
const char StabMenuItem12[]  PROGMEM = "Yaw I limit:";
const char StabMenuItem9[]   PROGMEM = "Yaw I rate:";
const char StabMenuItem30[]  PROGMEM = "Yaw trim:";
const char StabMenuItem13[]  PROGMEM = "Z-axis P:";   
const char StabMenuItem131[] PROGMEM = "Z-axis I:";  
const char StabMenuItem132[] PROGMEM = "Z-axis I limit:";  
//
const char AutoMenuItem11[]  PROGMEM = "OFF";
const char AutoMenuItem15[]  PROGMEM = "ON"; 
// RC setup text
const char RCMenuItem1[]     PROGMEM = "RX type:"; 
const char RCMenuItem0[]     PROGMEM = "Ch ord:";   
const char RCMenuItem2[]     PROGMEM = "Profile chan:";
const char Transition[]      PROGMEM = "Transition:";
const char Transition_P1[]   PROGMEM = "Transition low:";
const char Transition_P1n[]  PROGMEM = "Transition mid:";
const char Transition_P2[]   PROGMEM = "Transition high:";
const char RCMenuItem30[]    PROGMEM = "Vibration display:";
const char TransitionOut[]   PROGMEM = "Outbound trans:";
const char TransitionIn[]    PROGMEM = "Inbound trans:";
const char RCMenuItem300[]   PROGMEM = "AccVert filter:";
// RX mode text
const char RXMode0[]         PROGMEM = "S-Bus";
const char RXMode1[]         PROGMEM = "Spektrum";
// Channel order
const char RCMenuItem6[]     PROGMEM = "JR/Spektrum";
const char RCMenuItem7[]     PROGMEM = "Futaba"; 
const char RCMenuItem8[]     PROGMEM = "Custom"; 
// General text
const char MixerMenuItem0[]  PROGMEM = "Orientation:";
const char GeneralText100[]  PROGMEM = "Tail sitter:"; 
const char AutoMenuItem2[]   PROGMEM = "Safety:";
const char GeneralText2[]    PROGMEM = "Disarm time:";
const char GeneralText6[]    PROGMEM = "Accel LPF:";
const char GeneralText16[]   PROGMEM = "Gyro LPF:";
const char GeneralText7[]    PROGMEM = "AL correct:";
const char BattMenuItem2[]   PROGMEM = "Low V alarm:";
const char GeneralText20[]   PROGMEM = "Preset:";
const char GeneralText21[]   PROGMEM = "Buzzer:";
// Advanced text
const char MixerMenuItem1[]  PROGMEM = "P1 orientn.:";
const char MixerMenuItem8[]  PROGMEM = "P1 refrnce.:";
const char AdvancedDummy0[]  PROGMEM = " ";
const char AdvancedDummy1[]  PROGMEM = " ";
//
const char P1text[]          PROGMEM = "P1";
const char P2text[]          PROGMEM = "P1.n";
const char P3text[]          PROGMEM = "P2";
const char P4text[]          PROGMEM = "P1-P1.n";
const char P5text[]          PROGMEM = "P1.n-P2";
// Mixer menu items
const char MixerItem1[]      PROGMEM = "Device:";
const char MixerItem20[]     PROGMEM = "P1 Offset:";
const char MixerItem36[]     PROGMEM = "P1.n % of trans:";
const char MixerItem35[]     PROGMEM = "P1.n Offset:";
const char MixerItem34[]     PROGMEM = "P2 Offset:";
const char MixerItem23[]     PROGMEM = "P1 Thr. volume:";
const char MixerItem33[]     PROGMEM = "P2 Thr. volume:";
const char Mixeritem50[]     PROGMEM = "Trans. curve:";
//
const char MixerItem51[]     PROGMEM = "P1 Ail. volume:";
const char MixerItem54[]     PROGMEM = "P2 Ail. volume:";
const char MixerItem52[]     PROGMEM = "P1 Ele. volume:";
const char MixerItem55[]     PROGMEM = "P2 Ele. volume:";
const char MixerItem53[]     PROGMEM = "P1 Rud. volume:";
const char MixerItem56[]     PROGMEM = "P2 Rud. volume:";
//
const char MixerItem4[]      PROGMEM = "P1 Roll gyro:";
const char MixerItem24[]     PROGMEM = "P2 Roll gyro:"; 
const char MixerItem5[]      PROGMEM = "P1 Pitch gyro:";
const char MixerItem25[]     PROGMEM = "P2 Pitch gyro:";
const char MixerItem6[]      PROGMEM = "P1 Yaw gyro:";
const char MixerItem26[]     PROGMEM = "P2 Yaw gyro:";
const char MixerItem7[]      PROGMEM = "P1 Roll AL:";
const char MixerItem27[]     PROGMEM = "P2 Roll AL:";
const char MixerItem700[]    PROGMEM = "P1 Yaw AL:";
const char MixerItem3[]      PROGMEM = "P1 Pitch AL:";
const char MixerItem28[]     PROGMEM = "P2 Pitch AL:";
const char MixerItem42[]     PROGMEM = "P1 Alt. damp:";
const char MixerItem43[]     PROGMEM = "P2 Alt. damp:";
//
const char MixerItem0[]      PROGMEM = "P1 Source A:";
const char MixerItem2[]      PROGMEM = "P1 Volume:";
const char MixerItem29[]     PROGMEM = "P2 Source A:";
const char MixerItem30[]     PROGMEM = "P2 Volume:";
const char MixerItem21[]     PROGMEM = "P1 Source B:";
const char MixerItem31[]     PROGMEM = "P2 Source B:";
// Generic sensors for universal mixer
const char MixerItem70[]     PROGMEM = "GyroRoll"; 
const char MixerItem71[]     PROGMEM = "GyroPitch";
const char MixerItem72[]     PROGMEM = "GyroYaw";
const char MixerItem73[]     PROGMEM = "AccRoll";
const char MixerItem730[]    PROGMEM = "AccYaw";
const char MixerItem74[]     PROGMEM = "AccPitch";
// Alternate text for universal accs
const char MixerItemP73[]    PROGMEM = "Acc X";
const char MixerItemP74[]    PROGMEM = "Acc Y";
const char MixerItemP730[]   PROGMEM = "Acc Z";
const char MixerItem80[]     PROGMEM = "AL Roll";
const char MixerItem81[]     PROGMEM = "AL Pitch";
const char MixerItem800[]    PROGMEM = "AL Yaw";
const char MixerItem420[]    PROGMEM = "Alt. damp";
//
const char MixerItem40[]     PROGMEM = "A.Servo";
const char MixerItem41[]     PROGMEM = "D.Servo";
const char MixerItem49[]     PROGMEM = "Motor";
const char MixerItem60[]     PROGMEM = "Linear";
const char MixerItem61[]     PROGMEM = "Sine";
const char MixerItem62[]     PROGMEM = "SqrtSine";
// RC channel text
const char ChannelRef0[]     PROGMEM = "1";
const char ChannelRef1[]     PROGMEM = "2"; 
const char ChannelRef2[]     PROGMEM = "3"; 
const char ChannelRef3[]     PROGMEM = "4"; 
const char ChannelRef4[]     PROGMEM = "5"; 
const char ChannelRef5[]     PROGMEM = "6"; 
const char ChannelRef6[]     PROGMEM = "7"; 
const char ChannelRef7[]     PROGMEM = "8"; 
const char ChannelRef8[]     PROGMEM = "None";
const char ChannelRef10[]    PROGMEM = "Thr";
const char ChannelRef12[]    PROGMEM = "Ele"; 
// Error text
const char ErrorText3[]      PROGMEM = "NO SIGNAL";
const char ErrorText5[]      PROGMEM = "Error";
const char ErrorText6[]      PROGMEM = "Lost";
const char ErrorText7[]      PROGMEM = "Model";
const char Status3[]         PROGMEM = "BATTERY LOW";
const char Disarmed[]        PROGMEM = "DISARMED";
const char ErrorText8[]      PROGMEM = "Brownout";
const char ErrorText9[]      PROGMEM = "Occurred";
const char ErrorText11[]     PROGMEM = "THROTTLE HIGH";
// Init
const char PText0[]          PROGMEM = "OpenAero2";
const char PText1[]          PROGMEM = "RESET"; 
const char PText2[]          PROGMEM = "HOLD STEADY";
const char PText3[]          PROGMEM = "ESC CALIBRATION";
const char PText4[]          PROGMEM = "CAL FAILED";
const char PText5[]          PROGMEM = "UPDATING";
const char PText6[]          PROGMEM = "SETTINGS";
// Wizard screen
const char WizardText0[]     PROGMEM = "No RX signal";
const char WizardText1[]     PROGMEM = "Hold as shown";
const char WizardText2[]     PROGMEM = "Done";
// Idle text
const char Status0[]         PROGMEM = "Press";
const char Status2[]         PROGMEM = "for status";
const char Status4[]         PROGMEM = "(ARMED)";
const char Status5[]         PROGMEM = "(DISARMED)";
// Outputs
const char MOUT1[]           PROGMEM = "OUT1";
const char MOUT2[]           PROGMEM = "OUT2";  
const char MOUT3[]           PROGMEM = "OUT3";  
const char MOUT4[]           PROGMEM = "OUT4";  
const char MOUT5[]           PROGMEM = "OUT5";  
const char MOUT6[]           PROGMEM = "OUT6";  
const char MOUT7[]           PROGMEM = "OUT7";  
const char MOUT8[]           PROGMEM = "OUT8";
const char MOUT9[]           PROGMEM = "OUT9";
const char MOUT10[]          PROGMEM = "OUT10";  
// Orientation
const char MixerMenuItem2[]  PROGMEM = "Forward";
const char MixerMenuItem3[]  PROGMEM = "Vertical";
const char MixerMenuItem4[]  PROGMEM = "Inverted";
const char MixerMenuItem5[]  PROGMEM = "Aft";
const char MixerMenuItem6[]  PROGMEM = "Sideways";
const char MixerMenuItem7[]  PROGMEM = "PitchUp";
// 295 (24) Orientations
const char Orientation1[]    PROGMEM = "Top-Rear";
const char Orientation2[]    PROGMEM = "Top-Left";
const char Orientation3[]    PROGMEM = "Top-Front";
const char Orientation4[]    PROGMEM = "Top-Right";
const char Orientation5[]    PROGMEM = "Back-Down";
const char Orientation6[]    PROGMEM = "Back-Left";
const char Orientation7[]    PROGMEM = "Back-Up";
const char Orientation8[]    PROGMEM = "Back-Right";
const char Orientation9[]    PROGMEM = "Bott.-Back";
const char Orientation10[]   PROGMEM = "Bott.-Right";
const char Orientation11[]   PROGMEM = "Bott.-Front";
const char Orientation12[]   PROGMEM = "Bott.-Left";
const char Orientation13[]   PROGMEM = "Front-Down";
const char Orientation14[]   PROGMEM = "Front-Right";
const char Orientation15[]   PROGMEM = "Front-Up";
const char Orientation16[]   PROGMEM = "Front-Left";
const char Orientation17[]   PROGMEM = "Left-Down";
const char Orientation18[]   PROGMEM = "Left-Front";
const char Orientation19[]   PROGMEM = "Left-Up";
const char Orientation20[]   PROGMEM = "Left-Back";
const char Orientation21[]   PROGMEM = "Right-Down";
const char Orientation22[]   PROGMEM = "Right-Back";
const char Orientation23[]   PROGMEM = "Right-Up";
const char Orientation24[]   PROGMEM = "Right-Front";
// Misc
const char MixerItem11[]     PROGMEM = "Normal";
const char MixerItem12[]     PROGMEM = "Reversed";
const char MixerItem15[]     PROGMEM = "Scaled";
const char MixerItem16[]     PROGMEM = "RevScaled";
// PWM output modes
const char GeneralText5[]    PROGMEM  = "Sync RC";
//
const char Safety1[]         PROGMEM = "Armed";
const char Safety2[]         PROGMEM = "Armable";
const char Random1[]         PROGMEM = "High";
const char Random10[]        PROGMEM = "high";
const char Random11[]        PROGMEM = "low";
const char Random12[]        PROGMEM = "signal";
// Software LPFs
const char SWLPF1[]          PROGMEM = "67Hz";
const char SWLPF2[]          PROGMEM = "38Hz";
const char SWLPF3[]          PROGMEM = "27Hz";
const char SWLPF4[]          PROGMEM = "17Hz";
//
const char GeneralText10[]   PROGMEM = "MPU LPF:";
const char MPU6050LPF1[]     PROGMEM = "5Hz";
const char MPU6050LPF2[]     PROGMEM = "10Hz";
const char MPU6050LPF3[]     PROGMEM = "21Hz";
const char MPU6050LPF4[]     PROGMEM = "44Hz";
const char MPU6050LPF5[]     PROGMEM = "94Hz";
const char MPU6050LPF6[]     PROGMEM = "184Hz";
const char MPU6050LPF7[]     PROGMEM = "260Hz";
//
const char Dummy0[]          PROGMEM = "";
//
const char VBAT32[]          PROGMEM = "3.2V";
const char VBAT33[]          PROGMEM = "3.3V";
const char VBAT34[]          PROGMEM = "3.4V";
const char VBAT35[]          PROGMEM = "3.5V";
const char VBAT36[]          PROGMEM = "3.6V";
const char VBAT37[]          PROGMEM = "3.7V";
const char VBAT38[]          PROGMEM = "3.8V";
const char VBAT39[]          PROGMEM = "3.9V";
// Preset names
const char PRESET_0[]        PROGMEM = "Blank";
const char PRESET_1[]        PROGMEM = "Quad X";
const char PRESET_2[]        PROGMEM = "Quad P";
const char PRESET_3[]        PROGMEM = "Tricopter";
const char PRESET_4[]        PROGMEM = "Options";
//
const char WARNING_0[]       PROGMEM = " WARNING - Erases Setup";
const char WARNING_1[]       PROGMEM = "REMOVE PROPS";
//
const char ERROR_0[]         PROGMEM = "Reboot";
const char ERROR_1[]         PROGMEM = "Manual disarm";
const char ERROR_2[]         PROGMEM = "No signal";
const char ERROR_3[]         PROGMEM = "Disarm timer";
const char ERROR_4[]         PROGMEM = "BOD reset";
//
const char ERROR_MSG_0[]     PROGMEM = "Clear";
//
const char Misc_Msg01[]      PROGMEM = "Alt.";
// Reference
const char Reference_0[]     PROGMEM = "No";
const char Reference_1[]     PROGMEM = "Earth ref.";
const char Reference_2[]     PROGMEM = "Vert. AP ref.";
// Curves
const char Curve_0[]         PROGMEM = "P1 Throttle Curve";
const char Curve_1[]         PROGMEM = "P2 Throttle Curve";
const char Curve_2[]         PROGMEM = "P1 Collective Curve";
const char Curve_3[]         PROGMEM = "P2 Collective Curve";
const char Curve_4[]         PROGMEM = "Generic Curve C";
const char Curve_5[]         PROGMEM = "Generic Curve D";
// Misc
const char Misc_1[]          PROGMEM = "CRC:";
// Short channel names
const char Chan0[]           PROGMEM = "THR";
const char Chan1[]           PROGMEM = "AIL";
const char Chan2[]           PROGMEM = "ELE";
const char Chan3[]           PROGMEM = "RUD";
const char Chan4[]           PROGMEM = "GEAR";
const char Chan5[]           PROGMEM = "AUX1";
const char Chan6[]           PROGMEM = "AUX2";
const char Chan7[]           PROGMEM = "AUX3";
// New universal mixer items
const char Uni1[]            PROGMEM = "Collective";
const char Uni3[]            PROGMEM = "Curve C";
const char Uni4[]            PROGMEM = "Curve D";
// IO
const char I1[]              PROGMEM = "1:";
const char I2[]              PROGMEM = "2:";
const char I3[]              PROGMEM = "3:";
const char I4[]              PROGMEM = "4:";
const char I5[]              PROGMEM = "5:";
const char I6[]              PROGMEM = "6:";
const char I7[]              PROGMEM = "7:";
const char I8[]              PROGMEM = "8:";
const char I9[]              PROGMEM = "Pn:";
const char I10[]             PROGMEM = "Out:";
const char I11[]             PROGMEM = "9:";
const char I12[]             PROGMEM = "10:";
//
const char O0[]              PROGMEM = "Thr:";
const char O1[]              PROGMEM = "Ail:";
const char O2[]              PROGMEM = "Ele:";
const char O3[]              PROGMEM = "Rud:";
const char O4[]              PROGMEM = "Gr:";
const char O5[]              PROGMEM = "Ax1:";
const char O6[]              PROGMEM = "Ax2:";
const char O7[]              PROGMEM = "Ax3:";
// Custom channel inputs
const char Ch1[]             PROGMEM = "TX Thr Channel:";
const char Ch2[]             PROGMEM = "TX Ail Channel:";
const char Ch3[]             PROGMEM = "TX Ele Channel:";
const char Ch4[]             PROGMEM = "TX Rud Channel:";
const char Ch5[]             PROGMEM = "TX Gear Channel:";
const char Ch6[]             PROGMEM = "TX Aux1 Channel:";
const char Ch7[]             PROGMEM = "TX Aux2 Channel:";
const char Ch8[]             PROGMEM = "TX Aux3 Channel:";

const char* textMenu[] = 
  {
    AutoMenuItem11, 
    VBAT32,
    VBAT33,
    VBAT34,
    VBAT35,
    VBAT36,
    VBAT37,
    VBAT38,
    VBAT39,                  // 0 to 8 Vbat cell voltages
    //
    MenuFrame0,
    MenuFrame1,
    MenuFrame2,
    MenuFrame3,
    MenuFrame4,
    MenuFrame5,
    MenuFrame6,
    MenuFrame7,
    MenuFrame8,              // 9 to 17 Menu frame text
    //
    Disarmed,
    ErrorText5,
    ErrorText6,
    ErrorText7,              // 18 to 21, Disarmed, Error, lost, model
    //
    Transition,
    StatusText8,
    StatusText9,
    SensorMenuItem2,         // 22 to 25 Misc    
    //
    PText15,
    PText16,
    PText17,
    PText18,
    PText19,
    PText20,                 // 26 to 31 Sensors
    //
    ChannelRef1,
    ChannelRef2,
    ChannelRef0,
    ChannelRef3,
    ChannelRef4,             // 32 to 36 RC inputs
    //
    MPU6050LPF1,
    MPU6050LPF2,
    MPU6050LPF3,
    MPU6050LPF4,
    MPU6050LPF5,
    MPU6050LPF6,
    MPU6050LPF7,
    ChannelRef8,             // 37 to 44  MPU6050 LPF
    //
    ErrorText11,             // 45 THROTTLE 
    //
    MixerItem40,
    MixerItem41,             // 46 to 47 Device types
    // 
    P1text,
    P2text,
    P3text,
    P4text,
    P5text,                  // 48 to 52 P1, P1.n, P2, P1 to P1.n, P1.n to P2 
    
    Dummy0,
    Dummy0,                  // 53, 54 Spare  
    //
    Random1,                 // 55 High
    //
    MixerItem60,
    MixerItem61,
    MixerItem62,             // 56 to 58 Linear, Sine, Sqrt Sine
    
    //
    PText3,                  // 59 ESC Calibrate
    //
    SensorMenuItem1,         // 60 calibrate
    //
    PText4,                  // 61 Failed
    //
    ErrorText8,
    ErrorText9,              // 62, 63, "Brownout", "Occurred"
    //
    Dummy0,
    Dummy0,
    Dummy0,
    Dummy0,                  // 64 to 67 Spare
    //
    AutoMenuItem11,
    AutoMenuItem15,
    MixerItem15,
    MixerItem12,
    MixerItem16,             // 68 to 71 off/on/scale/rev/revscale 
    //
    Misc_1,                  // 73 CRC:
    //
    Dummy0,                  // 74 Spare
    //
    ErrorText3,              // 75 No SIGNAL 
    //
    Dummy0,                  // 76 Spare
    //
    MainMenuItem0,
    MainMenuItem1,
    MainMenuItem9,
    MainMenuItem7,
    MainMenuItem8,
    MainMenuItem10,
    MainMenuItem2,
    MainMenuItem3,
    MainMenuItem30,
    MainMenuItem25,
    MainMenuItem11,
    MainMenuItem12,
    MainMenuItem13,
    MainMenuItem14,
    MainMenuItem15,
    MainMenuItem16,
    MainMenuItem17,
    MainMenuItem18,
    MainMenuItem4,
    MainMenuItem5,
    MainMenuItem20,
    MainMenuItem22,
    MainMenuItem23,
    MainMenuItem32,
    MainMenuItem31,
    MainMenuItem24,          // 77 to 102 Main menu
    //
    Dummy0,
    Dummy0,                  // 103 to 104 Spare
    //
    ChannelRef0,
    ChannelRef1,
    ChannelRef2,
    ChannelRef3,
    ChannelRef4,
    ChannelRef5,
    ChannelRef6,
    ChannelRef7,
    ChannelRef8,
    ChannelRef10,
    ChannelRef12,            // 105 to 115 Ch. names
    //
    Dummy0,
    Dummy0,
    Dummy0,                  // 116 to 118 Spare
    //
    GeneralText5,
    Random1,                 // 119 to 120 Sync RC, High
    //
    Status0,
    Status2,                 // 121 to 122 Press any button 
    //
    Dummy0,                  // 123 Spare
    //
    MixerMenuItem2,
    MixerMenuItem3,
    MixerMenuItem4,
    MixerMenuItem5,
    MixerMenuItem6,
    MixerMenuItem7,          // 124 to 129 H/V/UD/Aft/Sideways/PitchUp
    //                                
    RCMenuItem6,
    RCMenuItem7,
    RCMenuItem8,             // 130 to 132 JR/Futaba/CUSTOM
    //
    Dummy0,                  // 133 Spare
    //
    Status3,                 // 134 Battery
    //
    WizardText0,
    WizardText1,
    WizardText2,             // 135 to 137
    //
    Status4,
    Status5,                 // 138 to 139
    //
    Dummy0,                  // 140 Spare
    //
    MixerItem11,
    MixerItem12,             // 141 to 142 Norm/Rev
    //
    PText16,
    PText17,
    PText18,                 // 143 to 145 X/Y/Z
    //
    RCMenuItem1,
    RCMenuItem0,
    RCMenuItem2,
    TransitionOut,
    TransitionIn,
    Transition_P1, 
    Transition_P1n,
    Transition_P2,
    RCMenuItem30,
    RCMenuItem300,           // 146 to 155 RC menu
    //
    Dummy0, 
    Dummy0,                  // 156 to 157 Spare
    //
    MixerMenuItem0,
    GeneralText100,
    AutoMenuItem2,
    GeneralText2,
    BattMenuItem2,
    GeneralText10,
    GeneralText6,
    GeneralText16,
    GeneralText7,
    GeneralText20,
    GeneralText21,           // 158 to 168 General
    //
    Dummy0,
    Dummy0,
    Dummy0,                  // 169 to 171 Spare
    //
    AutoMenuItem1,
    StabMenuItem2,
    StabMenuItem10,
    StabMenuItem3,
    AutoMenuItem20,
    AutoMenuItem7,
    AutoMenuItem4,
    StabMenuItem5,
    StabMenuItem11,
    StabMenuItem6,
    AutoMenuItem21,
    AutoMenuItem8,
    StabMenuItem7,
    StabMenuItem8,
    StabMenuItem12,
    StabMenuItem9,
    StabMenuItem30,
    StabMenuItem13,
    StabMenuItem131,
    StabMenuItem132,         // 172 - 191 FLight Menu (EARTH)
    //
    MixerItem1,
    MixerItem23,
    MixerItem33,
    Mixeritem50,
    MixerItem51,
    MixerItem54,
    MixerItem52,
    MixerItem55,
    MixerItem53,
    MixerItem56,
    MixerItem4,
    MixerItem24,
    MixerItem5,
    MixerItem25,
    MixerItem6,
    MixerItem26,
    MixerItem7,
    MixerItem27,
    MixerItem3,
    MixerItem28,
    MixerItem42,
    MixerItem43,
    MixerItem0,
    MixerItem2,
    MixerItem29,
    MixerItem30,
    MixerItem21,
    MixerItem2,
    MixerItem31,
    MixerItem30,             // 192 to 221 Mixer Items (EARTH)
    //
    Dummy0,
    Dummy0,
    Dummy0,
    Dummy0,                  // 222 to 225 Spare
    //
    MixerItem40,
    MixerItem41,
    MixerItem49,             // 226 to 228 Output types 
    //
    Misc_Msg01,              // 229 AccVert
    //
    MOUT1,
    MOUT2,
    MOUT3,
    MOUT4,
    MOUT5,
    MOUT6,
    MOUT7,
    MOUT8,
    MOUT9,
    MOUT10,                  // 230 to 239 Sources OUT1 - OUT10,
    //
    Safety2,
    Safety1,
    Chan4,
    Chan5,
    Chan6,
    Chan7,                   // 240 to 245 Safety
    //
    Dummy0,
    Dummy0,
    Dummy0,
    Dummy0,
    Dummy0,
    Dummy0,
    Dummy0,                  // 246 to 252 Spare
    //
    ChannelRef8,             // 253
    //
    MixerItem40,
    MixerItem41,
    MixerItem49,             // 254 to 256 Device types
    //	
    Dummy0,
    Dummy0,                  // 257 to 258 Spare
    //
    PText5,
    PText6,                  // 259 to 260 Updating settings
    //
    PText0,                  // 261 Logo
    //
    PText1,                  // 262 Reset
    //
    PText2,                  // 263 Hold steady
    //
    StatusText0,
    StatusText1,
    RCMenuItem1,
    StatusText3,
    StatusText4,
    StatusText5,             // 264 to 269 Status menu 
    //
    Random10,
    Random11,
    Random12,                 // 270 - 272
    //  
    PRESET_1,
    PRESET_2,
    PRESET_3,
    PRESET_0,
    PRESET_4,                // 273 to 277 Mixer presets
    //
    Dummy0,
    Dummy0,                  // 278 to 279 Spare
    //
    MenuFrame9,              // 280 Abort
    //
    WARNING_0,
    WARNING_1,               // 281 to 282 Warnings
    //
    Dummy0,                  // 283 Spare
    //
    ERROR_0,
    ERROR_1,
    ERROR_2,
    ERROR_3,
    ERROR_4,                 // 284 to 288 Error messages
    //
    StatusText7,             // 289 Battery:
    //
    Dummy0,                  // 290 Spare 
    //
    ERROR_MSG_0,             // 291 Log menu
    //
    Dummy0,
    Dummy0,
    Dummy0,                  // 292 to 294 Spare
    //
    Orientation1,
    Orientation2,
    Orientation3,
    Orientation4,
    Orientation5,
    Orientation6,
    Orientation7,
    Orientation8,
    Orientation9,
    Orientation10,
    Orientation11,
    Orientation12,
    Orientation13,
    Orientation14,
    Orientation15,
    Orientation16,
    Orientation17,
    Orientation18,
    Orientation19,
    Orientation20,
    Orientation21,
    Orientation22,
    Orientation23,
    Orientation24,           // 295 - 318 Orientations
    //
    Dummy0,                  // 319 Spare 
    //
    Reference_0,
    Reference_1,
    Reference_2,             // 320 to 322 Orientation reference (No, Earth, Vert AP)
    //
    Dummy0,                  // 323 Spare
    //
    MixerMenuItem1,
    MixerMenuItem8,
    AdvancedDummy0,
    AdvancedDummy1,          // 324 to 327 Advanced menu
    //  
    AutoMenuItem1,
    StabMenuItem2,
    StabMenuItem10,
    StabMenuItem3,
    AutoMenuItem200,
    AutoMenuItem7,
    AutoMenuItem4,
    StabMenuItem5,
    StabMenuItem11,
    StabMenuItem6,
    AutoMenuItem21,
    AutoMenuItem8,
    StabMenuItem7,
    StabMenuItem8,
    StabMenuItem12,
    StabMenuItem9,
    StabMenuItem30,
    StabMenuItem13,
    StabMenuItem131,
    StabMenuItem132,         // 328 to 347 Flight menu (Model)
    //
    MixerItem1,
    MixerItem23,
    MixerItem33,
    Mixeritem50,
    MixerItem51,
    MixerItem54,
    MixerItem52,
    MixerItem55,
    MixerItem53,
    MixerItem56,
    MixerItem4,
    MixerItem24,
    MixerItem5,
    MixerItem25,
    MixerItem6,
    MixerItem26,
    MixerItem700,
    MixerItem27,
    MixerItem3,
    MixerItem28,
    MixerItem42,
    MixerItem43,
    MixerItem0,
    MixerItem2,
    MixerItem29,
    MixerItem30,
    MixerItem21,
    MixerItem2,
    MixerItem31,
    MixerItem30,             // 348 to 377 Mixer Menu
    //
    Dummy0,
    Dummy0,
    Dummy0,
    Dummy0,
    Dummy0,
    Dummy0,                  // 378 to 383 Spare
    //
    Ch1,
    Ch2,
    Ch3,
    Ch4,
    Ch5,
    Ch6,
    Ch7,
    Ch8,                     // 384 to 391 TX channel numbers  
    //  
    Dummy0,
    Dummy0,
    Dummy0,
    Dummy0,                  // 392 to 395 Spare
    //
    RXMode0,
    RXMode1,                 // 396 to 397 RX mode
    //
    Dummy0,
    Dummy0,
    Dummy0,
    Dummy0,
    Dummy0,                  // 398 to 402 Spare
    //
    Curve_0,
    Curve_1,
    Curve_2,
    Curve_3,
    Curve_4,
    Curve_5,                 // 403 to 408 Curves
    //
    Dummy0,                  // 409 Spare
    //
    Chan0,
    Chan1,
    Chan2,
    Chan3,
    Chan4,
    Chan5,
    Chan6,
    Chan7,
    ChannelRef8,             // 410 to 418 Short channel names
    //
    Dummy0,
    Dummy0,                  // 419 to 420 Spare
    //
    Chan0,
    Uni3,
    Uni4,
    Uni1,
    Chan0,
    Chan1,
    Chan2,
    Chan3,
    Chan4,
    Chan5,
    Chan6,
    Chan7,
    MixerItem70,
    MixerItem71,
    MixerItem72,
    MixerItemP73,
    MixerItemP74,
    MixerItem80,
    MixerItem81,
    MixerItem420,
    ChannelRef8,             // 421 to 441 Universal Sources (EARTH)
    //
    Chan0,
    Uni3,
    Uni4,
    Uni1,
    Chan0,
    Chan1,
    Chan2,
    Chan3,
    Chan4,
    Chan5,
    Chan6,
    Chan7,
    MixerItem70,
    MixerItem71,
    MixerItem72,
    MixerItemP730,
    MixerItemP74,
    MixerItem800,
    MixerItem81,
    MixerItem420,
    ChannelRef8,             // 442 - 462 Universal Sources (MODEL)
    //
    Dummy0,
    Dummy0,
    Dummy0,
    Dummy0,
    Dummy0,                  // 463 to 467 Spare
    //
    I1,
    I2,
    I3,
    I4,
    I5,
    I6,
    I7,
    I8,
    I11,
    I12,
    I9,
    I10,                     // 468 to 479 Channel Numbers
    //
    O0,
    O1,
    O2,
    O3,
    O4,
    O5,
    O6,
    O7,                      // 480 to 487 Channel Text
  }; 
  
//************************************************************
// GLCD text subroutines
//************************************************************

// Print Menuitem from Program memory at a particular location
void lcdDisplayText(uint16_t menuItem, uint16_t x, uint16_t y)
{
  u8g2.setCursor(x, y);
  u8g2.print((char*)((textMenu[menuItem])));
}

// Pop up the Idle screen
void idleScreen(void)
{
  u8g2.clearBuffer();
  
  // Change Status screen depending on arm mode
  lcdDisplayText(121, 51,  3);     // "Press"
  lcdDisplayText(122, 42, 23);     // "for status"  

  // Display most important error
  if (generalError & (1 << LVA_ALARM))           // Low voltage
  {
    lcdDisplayText(134, 28, 43);  // BATTERY LOW
  }
  
  else if (generalError & (1 << NO_SIGNAL))      // No signal
  {
    u8g2.setFont(u8g2_font_helvR14_tr);
    lcdDisplayText(75, 12, 43);  // NO SIGNAL
    u8g2.setFont(u8g2_font_helvR08_tr);
  }
  
  else if (generalError & (1 << THROTTLE_HIGH))  // Throttle high
  {
    lcdDisplayText(45, 24, 43);  // THROTTLE HIGH
  }
  
  else if (generalError & (1 << DISARMED))       // Disarmed
  {
    u8g2.setFont(u8g2_font_helvR14_tr);
    lcdDisplayText(139, 5, 43);   // (DISARMED)
    u8g2.setFont(u8g2_font_helvR08_tr);
  }
  
  else
  {
    u8g2.setFont(u8g2_font_helvR14_tr);
    lcdDisplayText(138, 26, 43);  // "(ARMED)"
    u8g2.setFont(u8g2_font_helvR08_tr);
  }

  u8g2.sendBuffer();
}
