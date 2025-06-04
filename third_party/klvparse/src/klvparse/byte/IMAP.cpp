#include <klvparse/byte/IMAP.h>

__IMAPB_FORWARD(uint8, uint8_t, 1)
__IMAPB_FORWARD(int8, int8_t, 1)
__IMAPB_FORWARD(uint16, uint16_t, 2)
__IMAPB_FORWARD(int16, int16_t, 2)
__IMAPB_FORWARD(uint32, uint32_t, 4)
__IMAPB_FORWARD(int32, int32_t, 4)
__IMAPB_FORWARD(uint64, uint64_t, 8)
__IMAPB_FORWARD(int64, int64_t, 8)

__IMAPB_BACKWARD(float, float, 4)
__IMAPB_BACKWARD(double, double, 8)