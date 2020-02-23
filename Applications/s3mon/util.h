#ifndef _UTIL_H_
#define _UTIL_H_

#include <stdint.h>

uint16_t crc16(uint8_t *ptr, int count);
uint8_t	dec_to_str(uint16_t dec, uint8_t *str, uint8_t pad_len);
void 	print_title(char *title);
uint32_t str_to_dec(uint8_t type, char *str);
uint32_t get_number_stdin(char *msg, uint8_t type, uint8_t digi);
uint8_t UserKeyInValue(uint8_t type, uint8_t len, uint32_t *value);
#endif	/*_UTIL_H_*/
