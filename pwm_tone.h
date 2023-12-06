/* Includes ------------------------------------------------------------------*/
#include "mbed.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define Do3  131 //C octave3
#define Do3s 139 //C#
#define Re3  147 //D
#define Re3s 156//D#
#define Mi3  165 //E
#define Fa3  175 //F
#define Fa3s 185 //F#
#define So3  196 //G
#define So3s 208 //G#
#define La3  220 //A
#define La3s 233 //A#
#define Ti3  247 //B
#define Do4  262 //C octave4
#define Do4s 277 //C#
#define Re4  294 //D
#define Re4s 311//D#
#define Mi4  330 //E
#define Fa4  349 //F
#define Fa4s 370 //F#
#define So4  392 //G
#define So4s 415 //G#
#define La4  440 //A
#define La4s 466 //A#
#define Ti4  494 //B
#define Do5  523 //C octave5
#define Do5s 554 //C#
#define Re5  587 //D
#define Re5s 622//D#
#define Mi5  659 //E
#define Fa5  699 //F
#define Fa5s 740 //F#
#define So5  784 //G
#define So5s 831 //G#
#define La5  880 //A
#define La5s 932 //A#
#define Ti5  988 //B
#define Do6  1047 //C octave6
#define Do6s 1109 //C#
#define Re6  1175 //D
#define Re6s 1245 //D#
#define Mi6  1319 //E
#define Fa6  1397 //F
#define Fa6s 1480 //F#
#define So6  1568 //G
#define So6s 1661 //G#
#define La6  1760 //A
#define La6s 1865 //A#
#define Ti6  1976 //B

/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void Tune(PwmOut name, int period, int beat);
void Auto_tunes(PwmOut name, int period, int beat);
void Stop_tunes(PwmOut name);
        
/* Private functions ---------------------------------------------------------*/
