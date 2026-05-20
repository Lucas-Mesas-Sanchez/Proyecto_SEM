/**
 * @file wifi.h
 *
**/

#ifndef WIFI_H
#define WIFI_H

#include <stdbool.h>

/* Prepares the wifi subsystem and connects to the AP */
void wifi_init (void);
/* Returns true if the we are connected to the AP */
bool wifi_is_connected(void);

#endif
/*** End of file ***/