#ifndef _NTP_PACKET_H_
#define _NTP_PACKET_H_

#include <stdint.h>

typedef struct ntp_packet_t {
  union {
    uint32_t packet[12];
    struct {
      uint8_t li_vn_mode;
      uint8_t stratum;
      uint8_t poll;
      uint8_t precision;
      uint32_t root_delay;
      uint32_t root_dispersion;
      uint32_t reference_id;
      uint32_t reference_timestamp_second;
      uint32_t reference_timestamp_fraction;
      uint32_t origin_timestamp_second;
      uint32_t origin_timestamp_fraction;
      uint32_t receive_timestamp_second;
      uint32_t receive_timestamp_fraction;
      uint32_t transmit_timestamp_second;
      uint32_t transmit_timestamp_fraction;
    };
  };

} ntp_packet_t;

#endif // _NTP_PACKET_H_