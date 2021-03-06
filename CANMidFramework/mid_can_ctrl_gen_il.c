
/**
 *    @file     mid_can_ctl_gen_il.c
 *    @brief
 *
 */

#include <stdint.h>
#include <memory.h>

#include "mid_can_ctrl_gen_il.h"


CANOpResult UnpackMessageToSignal(
	uint8_t const * _msg_buff,
	uint8_t _msg_buff_size,
	uint8_t _start_bit,
	uint8_t _signal_len,
	uint8_t * _p_signal_value,
	uint8_t _signal_value_byte_size)
{
	uint32_t signal_value = 0;
	uint8_t const signal_byte_size = ((_signal_len - 1) / 8) + 1;               // signal size count in bytes
	uint8_t const signal_lsb_byte_pos = _start_bit / 8;                         // LSB-byte in message payload 
	uint8_t const signal_msb_byte_pos = (_start_bit+_signal_len) / 8;           // LSB-byte in message payload 
	uint8_t const lsb_bit_pos_in_byte = _start_bit % 8;                         // LSB position in LSB-byte, 0-7
	uint32_t sig_mask = (_signal_len == 32 ? 0xFFFFFFFF : ~(~0 << _signal_len));// set n bits to be 1
																		    	// another algorithm is : n == 32 ? 0xffffffff : (1 << n) - 1
																				// when n == 32, <<n is a undefined behavior on 32bit platform
	uint8_t signal_crossed_bytes = signal_msb_byte_pos - signal_lsb_byte_pos + 1;
	uint8_t const UINT32_SIZE = 4;

	if (_signal_value_byte_size > UINT32_SIZE) {
		return PARAMETER_ERROR;
	}

	if (signal_crossed_bytes <= UINT32_SIZE) {
		memcpy((uint8_t*)&signal_value, _msg_buff + signal_lsb_byte_pos, signal_crossed_bytes);
		signal_value >>= lsb_bit_pos_in_byte;
		signal_value &= sig_mask;
	}
	else if (signal_crossed_bytes == (UINT32_SIZE + 1)) {
		uint32_t temp_mask =  ~(~0 << (32- signal_lsb_byte_pos));    // set n bits to be 1
		uint8_t msb_byte = _msg_buff[signal_lsb_byte_pos + UINT32_SIZE];
		uint8_t * p_msb_byte = (uint8_t*)&signal_value + 4;
		memcpy((uint8_t*)&signal_value, _msg_buff + signal_lsb_byte_pos, UINT32_SIZE);
		signal_value >>= lsb_bit_pos_in_byte;
		signal_value &= temp_mask;
		msb_byte <<= lsb_bit_pos_in_byte;
		(*p_msb_byte) |= msb_byte;
		signal_value &= sig_mask;
	}
	memcpy(_p_signal_value, (uint8_t*)&signal_value, _signal_value_byte_size);
	
	return SUCCESSED;
}
