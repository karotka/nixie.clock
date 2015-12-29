#ifndef USART_H
#define USART_H

void USART_init();
void USART_sendChar(char c);
void USART_sendString(char *str);
void USART_sendInt(const int val);
char USART_receiveByte();

#endif
