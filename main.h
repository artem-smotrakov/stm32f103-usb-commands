#ifndef __MAIN_H
#define __MAIN_H

#include <stm32f10x.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_usart.h>

#define CR '\r'
#define LF '\n'
#define CRLF "\r\n"

// commands
#define A1_ON  "a1 on"
#define A1_OFF "a1 off"

// messages
#define UNKNOWN_COMMAND "unknown command"

#endif
