#include "uv-ntp-client.h"

#include <stdlib.h>
#include <string.h>

static void ntp_getaddrinfo_cb(uv_getaddrinfo_t* req, int status, struct addrinfo* res);
static void ntp_timer_cb(uv_timer_t* handle);

int uv_ntp_init(uv_loop_t* loop, uv_ntp_t* ntp) {
  int r = uv_udp_init(loop, &ntp->udp);
  if (r < 0) return r;

  r = uv_timer_init(loop, &ntp->timer);
  if (r < 0) return r;

  ntp->connected = 0;
  ntp->poll_cb   = NULL;
  ntp->interval  = 0;
  ntp->loop      = loop;

  ntp->udp.data      = ntp;
  ntp->dns_addr.data = ntp;
  ntp->timer.data    = ntp;

  ntp->udp_buf.base = (char*)&ntp->udp_packet;
  ntp->udp_buf.len  = sizeof(ntp->udp_packet);

  ntp->origin_second        = 0;
  ntp->origin_fraction      = 0;
  ntp->destination_second   = 0;
  ntp->destination_fraction = 0;

  return r;
}

int uv_ntp_start(uv_ntp_t* ntp, const char* name, ntp_poll_cb poll_cb, uint64_t interval) {
  if (poll_cb == NULL) return -1;
  ntp->poll_cb  = poll_cb;
  ntp->interval = interval;

  int r;
  if (!ntp->connected) {
    // resolve IP if needed
    r = uv_getaddrinfo(ntp->loop, &ntp->dns_addr, ntp_getaddrinfo_cb, name, NULL, NULL);
    if (r < 0) return r;
  } else {
    r = uv_timer_start(&ntp->timer, ntp_timer_cb, 0, ntp->interval);
    if (r < 0) {
      ntp->connected = 0;
      return r;
    }
  }

  return r;
}

int ntp_ip4_stop(uv_ntp_t* ntp) {
  return uv_timer_stop(&ntp->timer);
}

static void ntp_alloc_cb(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf) {
  buf->base = (char*)malloc(suggested_size);
  buf->len  = suggested_size;
}

static void ntp_recv_start(uv_udp_t* handle, ssize_t nread, const uv_buf_t* buf, const struct sockaddr* addr, unsigned flags) {
  uv_ntp_t* ntp = (uv_ntp_t*)handle->data;

  if (nread < 0) {
    ntp->connected = 0;
    ntp->poll_cb(ntp, NULL, nread);
    return;
  }

  if (nread == sizeof(ntp_packet_t)) {
    ntp_packet_t* packet    = (ntp_packet_t*)buf[0].base;
    packet->precision       = ntohl(packet->precision);
    packet->root_delay      = ntohl(packet->root_delay);
    packet->root_dispersion = ntohl(packet->root_dispersion);
    // packet->reference_id                 = ntohl(packet->reference_id);
    packet->reference_timestamp_second   = ntohl(packet->reference_timestamp_second);
    packet->reference_timestamp_fraction = ntohl(packet->reference_timestamp_fraction);
    packet->origin_timestamp_second      = ntp->origin_second;
    packet->origin_timestamp_fraction    = ntp->origin_fraction;
    packet->receive_timestamp_second     = ntohl(packet->receive_timestamp_second);
    packet->receive_timestamp_fraction   = ntohl(packet->receive_timestamp_fraction);
    packet->transmit_timestamp_second    = ntohl(packet->transmit_timestamp_second);
    packet->transmit_timestamp_fraction  = ntohl(packet->transmit_timestamp_fraction);

    uv_timeval64_t tv;
    uv_gettimeofday(&tv);
    uv_timeval_to_ntp(&tv, &ntp->destination_second, &ntp->destination_fraction);

    ntp->poll_cb(ntp, (ntp_packet_t*)buf[0].base, 0);
  }
}

static int uv_ntp_poll(uv_ntp_t* ntp) {
  if (!ntp->connected) return -1;

  ntp_packet_t packet = {0};
  // VN = 3
  // Mode = Client: 3
  packet.li_vn_mode = 0b0011011;
  uv_buf_t buf      = {(char*)&packet, sizeof(packet)};

  uv_timeval64_t tv;
  uv_gettimeofday(&tv);
  uv_timeval_to_ntp(&tv, &ntp->origin_second, &ntp->origin_fraction);

  int r = uv_udp_send(&ntp->udp_send, &ntp->udp, &buf, 1, NULL, NULL);
  if (r < 0) {
    return r;
  }

  return r;
}

static void ntp_timer_cb(uv_timer_t* handle) {
  uv_ntp_t* ntp = (uv_ntp_t*)handle->data;
  int r         = uv_ntp_poll(ntp);
  if (r < 0) {
    ntp->connected = 0;
    ntp->poll_cb(ntp, NULL, r);
  }
}

static void ntp_getaddrinfo_cb(uv_getaddrinfo_t* req, int status, struct addrinfo* res) {
  if (status < 0) return;

  uv_ntp_t* ntp = (uv_ntp_t*)req->data;

  char addr_ip[17] = {0};
  uv_ip4_name((struct sockaddr_in*)req->addrinfo->ai_addr, addr_ip, 16);
  uv_ip4_addr(addr_ip, NTP_ADDR_PORT, &ntp->addr);
  uv_freeaddrinfo(ntp->dns_addr.addrinfo);

  int r = uv_udp_connect(&ntp->udp, (const struct sockaddr*)&ntp->addr);

  if (r < 0) {
    ntp->connected = 0;
    ntp->poll_cb(ntp, NULL, r);
    return;
  }

  r = uv_udp_recv_start(&ntp->udp, ntp_alloc_cb, ntp_recv_start);

  if (r < 0) {
    ntp->connected = 0;
    ntp->poll_cb(ntp, NULL, r);
    return;
  }

  ntp->connected = 1;

  r = uv_timer_start(&ntp->timer, ntp_timer_cb, 0, ntp->interval);
  if (r < 0) {
    ntp->connected = 0;
    ntp->poll_cb(ntp, NULL, r);
    return;
  }
}

uv_timeval64_t uv_ntp_to_timeval(uint32_t second, uint32_t fraction) {
  uv_timeval64_t tv;
  tv.tv_sec  = second - 2208988800LU;
  tv.tv_usec = (uint32_t)((double)fraction * 1.0e6 / (double)(1LL << 32));
  return tv;
}

void uv_timeval_to_ntp(uv_timeval64_t* tv, uint32_t* second, uint32_t* fraction) {
  *second   = tv->tv_sec + 2208988800;
  *fraction = (uint32_t)((double)(tv->tv_usec + 1) * (double)(1LL << 32) * 1.0e-6);
}

int uv_ntp_ref_id_to_str(uint32_t ref_id, char* str, size_t str_len) {
  if (str_len < 5) return -1;
  memcpy(str, &ref_id, 4);
  str[4] = '\0';
  return 1;
}

size_t uv_ntp_time_to_str(uint32_t second, uint32_t fraction, char* str, size_t str_len) {
  uv_timeval64_t tv = uv_ntp_to_timeval(second, fraction);

  int nwrite1 = strftime(str, str_len, "%Y/%m/%d %X", localtime(&tv.tv_sec));
  if (nwrite1 <= 0) return -1;

  int nwrite2 = snprintf(str + nwrite1, str_len - nwrite1, ".%06d", tv.tv_usec);
  if (nwrite2 <= 0) return -1;

  return nwrite1 + nwrite2;
}