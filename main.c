#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include "uv-ntp-client.h"

struct timeval ntp_to_timeval(uint32_t second, uint32_t fraction);
void poll_cb(uv_ntp_t* ntp, ntp_packet_t* packet, int status);

int main(int argc, char** argv) {
  uv_ntp_t ntp;
  int r = uv_ntp_init(uv_default_loop(), &ntp);
  if (r < 0) {
    fprintf(stderr, "ERROR: %s\n", uv_strerror(r));
    return r;
  }

  // poll NTP server "time-c-b.nist.gov" every 5 seconds
  r = uv_ntp_start(&ntp, "time-c-b.nist.gov", poll_cb, 5 * 1000);
  if (r < 0) {
    fprintf(stderr, "ERROR: %s\n", uv_strerror(r));
    return r;
  }

  r = uv_run(uv_default_loop(), UV_RUN_DEFAULT);
  if (r != 0) {
    fprintf(stderr, "ERROR: %s\n", uv_strerror(r));
    return 1;
  }

  return 0;
}

struct timeval uv_ntp_to_timeval(uint32_t second, uint32_t fraction) {
  struct timeval tv;
  tv.tv_sec  = second - 2208988800LU;
  tv.tv_usec = (uint32_t)((double)fraction * 1.0e6 / (double)(1LL << 32));
  return tv;
}

int uv_ntp_ref_id_to_str(uint32_t ref_id, char* str, size_t str_len) {
  if (str_len < 5) return -1;
  memcpy(str, &ref_id, 4);
  str[4] = '\0';
  return 1;
}

size_t uv_ntp_time_to_str(uint32_t second, uint32_t fraction, char* str, size_t str_len) {
  struct timeval tv = uv_ntp_to_timeval(second, fraction);

  int nwrite1 = strftime(str, str_len, "%Y/%m/%d %X", localtime(&tv.tv_sec));
  if (nwrite1 <= 0) return -1;

  int nwrite2 = snprintf(str + nwrite1, str_len - nwrite1, ".%06ld", tv.tv_usec);
  if (nwrite2 <= 0) return -1;

  return nwrite1 + nwrite2;
}

void poll_cb(uv_ntp_t* ntp, ntp_packet_t* packet, int status) {
  if (status < 0) {
    fprintf(stderr, "ERROR: %s\n", uv_strerror(status));
    return;
  }

  char ref_id_str[5];
  char ref_buffer[32];
  char origin_buffer[32];
  char rx_buffer[32];
  char tx_buffer[32];

  if (uv_ntp_ref_id_to_str(packet->reference_id, ref_id_str, 5) < 0) {
    return;
  }
  if (uv_ntp_time_to_str(packet->reference_timestamp_second, packet->reference_timestamp_fraction, ref_buffer, 32) <= 0) {
    return;
  }
  if (uv_ntp_time_to_str(packet->origin_timestamp_second, packet->origin_timestamp_fraction, origin_buffer, 32) <= 0) {
    return;
  }
  if (uv_ntp_time_to_str(packet->receive_timestamp_second, packet->receive_timestamp_fraction, rx_buffer, 32) <= 0) {
    return;
  }
  if (uv_ntp_time_to_str(packet->transmit_timestamp_second, packet->transmit_timestamp_fraction, tx_buffer, 32) <= 0) {
    return;
  }

  printf("li:              %d\n", UV_NTP_LI(packet->li_vn_mode));
  printf("vn:              %d\n", UV_NTP_VN(packet->li_vn_mode));
  printf("mode:            %d\n", UV_NTP_MODE(packet->li_vn_mode));
  printf("stratum:         %d\n", packet->stratum);
  printf("poll:            %d\n", packet->poll);
  printf("precision:       %d\n", packet->precision);
  printf("root delay:      %d\n", packet->root_delay);
  printf("root dispersion: %d\n", packet->root_dispersion);
  printf("reference ID:    %s\n", ref_id_str);
  printf("Reference Time:  %s\n", ref_buffer);
  printf("Origin Time:     %s\n", origin_buffer);
  printf("RX Time:         %s\n", rx_buffer);
  printf("TX Time:         %s\n\n", tx_buffer);
}