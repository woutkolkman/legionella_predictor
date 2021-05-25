/*when DEBUG is not defined, normal operations happen on the STM32. USART2 will be disabled.
 *USART2 is used to print data on screen to debug things.
 *when DEBUG is defined, USART2 is enabled. Normal operations continue, but some debug data
 *will be printed. This does use some more energy, since USART2 is enables continuously.
*/

//#define DEBUG 0
