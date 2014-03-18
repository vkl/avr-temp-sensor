#ifndef _onewire_h_
#define _onewire_h_

unsigned char onewire_reset(void);

void onewire_bit_write(unsigned char b);

unsigned char onewire_bit_read(void);

void onewire_byte_write(unsigned char b);

unsigned char onewire_byte_read(void);

unsigned char onewire_read(unsigned char bits);

#endif
