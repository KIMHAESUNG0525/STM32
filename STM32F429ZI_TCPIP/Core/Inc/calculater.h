#include "main.h"
#include "button.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>


extern uint8_t keypadScan();
extern uint8_t key_value;


typedef struct calculation{
	double data;
	struct calculation *next;
}Calcul;

typedef Calcul *calculator;
//calculator calc = NULL;




