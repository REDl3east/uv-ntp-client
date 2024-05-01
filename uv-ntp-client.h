#ifndef _UV_NTP_CLIENT_H
#define _UV_NTP_CLIENT_H

#include "uv.h"

#define NTP_ADDR_PORT 123

#define UV_NTP_LI(li_vn_mode)   (li_vn_mode >> 6)
#define UV_NTP_VN(li_vn_mode)   ((li_vn_mode >> 3) & 0b00000111)
#define UV_NTP_MODE(li_vn_mode) ((li_vn_mode)&0b00000111)

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

typedef struct uv_ntp_t uv_ntp_t;
typedef void (*ntp_poll_cb)(uv_ntp_t* ntp, ntp_packet_t* packet, int status);

typedef struct uv_ntp_t {
  int connected;
  int interval;

  // private
  struct sockaddr_in addr;
  uv_getaddrinfo_t dns_addr;
  uv_udp_t udp;
  uv_udp_send_t udp_send;
  ntp_packet_t udp_packet;
  uv_buf_t udp_buf;
  uv_timer_t timer;
  ntp_poll_cb poll_cb;
  uv_loop_t* loop;

  uint32_t origin_second;
  uint32_t origin_fraction;
  uint32_t destination_second;
  uint32_t destination_fraction;
} uv_ntp_t;

int uv_ntp_init(uv_loop_t* loop, uv_ntp_t* ntp);
int uv_ntp_start(uv_ntp_t* ntp, const char* name, ntp_poll_cb poll_cb, uint64_t interval);
int ntp_ip4_stop(uv_ntp_t* ntp);

uv_timeval64_t uv_ntp_to_timeval(uint32_t second, uint32_t fraction);
void uv_timeval_to_ntp(uv_timeval64_t* tv, uint32_t* second, uint32_t* fraction);
int uv_ntp_ref_id_to_str(uint32_t ref_id, char* str, size_t str_len);
size_t uv_ntp_time_to_str(uint32_t second, uint32_t fraction, char* str, size_t str_len);

#endif // _UV_NTP_CLIENT_H