#include <stdio.h>      // Defines three variable types, several macros, and various functions for performing input and output.

#include "project.h"

uint8 App_LED();

bool App_LED_enabled;
char app_led_inbox[100];
void App_LED_Update(char * message);
