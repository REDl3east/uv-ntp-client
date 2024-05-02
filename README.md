# UV-NTP-Client
A header only simple NTP client that uses the asynchronous library [libuv](https://github.com/libuv/libuv) as a base.

### Quick Start
```bash
mkdir build
cd build
cmake ..
make

# run example
./build/uv-ntp-client
```

### API
```c
int uv_ntp_init(uv_loop_t* loop, uv_ntp_t* ntp);
int uv_ntp_start(uv_ntp_t* ntp, const char* name, ntp_poll_cb poll_cb, uint64_t interval);
int ntp_ip4_stop(uv_ntp_t* ntp);
```
An example on how to use this API can be found [here](./example.c).

### Dependancies
libuv v1.48.0  
NOTE: cmake will download libuv v1.48.0 locally and build a static library that will link into the example executable.

### Links
[NTP Spec](https://datatracker.ietf.org/doc/html/rfc5905)  
[A Very Short Introduction to NTP Timestamps](https://tickelton.gitlab.io/articles/ntp-timestamps/)
[NTP Fundamentals](https://support.huawei.com/enterprise/en/doc/EDOC1100112347/e02e56b2/ntp-fundamentals)
[libuv](https://github.com/libuv/libuv)