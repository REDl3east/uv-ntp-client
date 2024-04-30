#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include "uv-ntp-client.h"

void poll_cb(uv_ntp_t* ntp, ntp_packet_t* packet, int status);

int main(int argc, char** argv) {
  uv_ntp_t ntp;
  int r = uv_ntp_init(uv_default_loop(), &ntp);
  if (r < 0) {
    fprintf(stderr, "ERROR: %s\n", uv_strerror(r));
    return r;
  }

  // poll NTP server "time-c-b.nist.gov" every 60 seconds
  r = uv_ntp_start(&ntp, "time-c-b.nist.gov", poll_cb, 60 * 1000);
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
  char dst_buffer[32];

  if (uv_ntp_ref_id_to_str(packet->reference_id, ref_id_str, 5) <= 0 ||
      uv_ntp_time_to_str(packet->reference_timestamp_second, packet->reference_timestamp_fraction, ref_buffer, 32) <= 0 ||
      uv_ntp_time_to_str(ntp->origin_second, ntp->origin_fraction, origin_buffer, 32) <= 0 ||
      uv_ntp_time_to_str(packet->receive_timestamp_second, packet->receive_timestamp_fraction, rx_buffer, 32) <= 0 ||
      uv_ntp_time_to_str(packet->transmit_timestamp_second, packet->transmit_timestamp_fraction, tx_buffer, 32) <= 0 ||
      uv_ntp_time_to_str(ntp->destination_second, ntp->destination_fraction, dst_buffer, 32) <= 0) {
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
  printf("Reference Time:  (%010u.%010u) %s\n", packet->reference_timestamp_second, packet->reference_timestamp_fraction, ref_buffer);
  printf("Origin Time:     (%010u.%010u) %s\n", ntp->origin_second, ntp->origin_fraction, origin_buffer);
  printf("RX Time:         (%010u.%010u) %s\n", packet->receive_timestamp_second, packet->receive_timestamp_fraction, rx_buffer);
  printf("TX Time:         (%010u.%010u) %s\n", packet->transmit_timestamp_second, packet->transmit_timestamp_fraction, tx_buffer);
  printf("Dest Time:       (%010u.%010u) %s\n\n", ntp->destination_second, ntp->destination_fraction, dst_buffer);
}