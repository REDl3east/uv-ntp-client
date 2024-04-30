#ifndef _UV_NTP_CLIENT_H
#define _UV_NTP_CLIENT_H

#include <stdlib.h>

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
  struct sockaddr_in addr;
  uv_getaddrinfo_t dns_addr;
  uv_udp_t udp;
  uv_udp_send_t udp_send;
  ntp_packet_t udp_packet;
  uv_buf_t udp_buf;
  uv_timer_t timer;

  ntp_poll_cb poll_cb;
  int interval;
  uv_loop_t* loop;
} uv_ntp_t;

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
    packet->origin_timestamp_second      = ntohl(packet->origin_timestamp_second);
    packet->origin_timestamp_fraction    = ntohl(packet->origin_timestamp_fraction);
    packet->receive_timestamp_second     = ntohl(packet->receive_timestamp_second);
    packet->receive_timestamp_fraction   = ntohl(packet->receive_timestamp_fraction);
    packet->transmit_timestamp_second    = ntohl(packet->transmit_timestamp_second);
    packet->transmit_timestamp_fraction  = ntohl(packet->transmit_timestamp_fraction);

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

void ntp_getaddrinfo_cb(uv_getaddrinfo_t* req, int status, struct addrinfo* res) {
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

#endif // _UV_NTP_CLIENT_H