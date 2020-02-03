#include <assert.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "notorios.h"
#include "project.h"

/* === Modem UART Functions  */

#define MAX_STARTS 8
#define MAX_REQUEST 1024
#define FIXED_HEADER "Content-Length: "
#define CHUNKED_HEADER "Transfer-Encoder: chunked"

static char chars[257] = {0};
static uint8_t num_chars = 0;

static const char *line = chars;
static const char *match = NULL;
static const char *pattern = NULL;

static void clear_chars(void) {
  Modem_UART_ClearRxBuffer();
  memset((void*)chars, 0, 256);
  num_chars = 0;
}

static bool wait_for(const char *string, uint32_t timeout) {
  pattern = string, match = NULL;
  uint32_t end = time() + timeout;
  while (!match && time() < end);
  return match != NULL;
}

CY_ISR(Modem_ISR) {
  /* Store received char in chars */
  char rx_char = Modem_UART_GetChar();
  if (rx_char) chars[num_chars++] = rx_char;

  /* Check pattern at end of line */
  if (rx_char == '\r') {
    match = strstr(line, pattern);
    line = chars + num_chars;
  }
}

/* === Modem Power === */

static bool at_write(const char *format, uint32_t timeout, ...) {
  clear_chars();
  va_list argp;
  va_start(argp, timeout);
  vdprintf(MODEM_STREAM, format, argp);
  va_end(argp);
  return wait_for("OK", timeout);
}

static bool power_on(void) {
  // Initialize UART interrupts
  Modem_UART_Start();
  Modem_ISR_StartEx(Modem_ISR);
  Modem_ON_Write(1);
  Modem_PWR_Write(1);
  // Start boot sequence
  Modem_ON_Write(0);
  wait_ms(100);
  Modem_ON_Write(1);
  return wait_for("^SYSTART", 30);
}

static void power_off(void) {
  at_write("AT^SMSO\r", 5);
  Modem_PWR_Write(0);
  Modem_ON_Write(0);
  Modem_ISR_Stop();
  Modem_UART_Stop();
}

static void reset(void) {
  Modem_RST_Write(1);
  wait_ms(200);
  Modem_RST_Write(0);
  wait_ms(5000);
}

/* == Modem Interactions == */

static bool setup(const modem_t *config) {
  // Set APN to config->apn
  return (at_write("AT+CGDCONT=3,\"IPV4V6\",\"%s\"\r", 1, config->apn) &&
          at_write("AT+CFUN=1\r", 1) && // Set functionality to full
          at_write("AT+CMEE=2\r", 1));  // Set verbose error reports
}

static bool set_context(bool activate) {
  /*
  Activate/deactivate internet service using PDP context 3
  
  Example Conversation:
  [Board] AT^SICA?
  [Modem] ^SICA: 1,1
          ^SICA: 3,0
  
          OK
  */
  if (!at_write("AT^SICA?\r", 1)) return false;
  // Check if PDP context 3 state already matches 
  unsigned pdp_state = atoi(strstr(line, "^SICA: 3,") + 9);
  if (pdp_state == activate) return true;
  return at_write("AT^SICA=%u,3\r", 60, activate);
}

static uint8_t startup(const modem_t *config) {
  for (uint8_t i = 1; i < MAX_STARTS; ++i) {
    // Turn on modem, cycle power on failure
    if (!power_on()) { reset(); continue; }
    // Attempt to connect to network
    if (setup(config) && set_context(true)) return i;
  }
  power_off(); return MAX_STARTS;
}

static bool get_meid(char *meid) {
  /*
  Return cell module ICCID

  Example Conversation:
  [Board] AT+CCID?
  [Modem] +CCID: "A1000049AB9082",""

          OK
  */
  if (!at_write("AT+CCID?\r", 1)) return false;
  const char *start = strstr(chars, "+CCID: \"") + 8;
  const char *end = strstr(start, "\",\"");
  if (!start || !end || end - start > 20) return false;
  memcpy(meid, start, end - start);
  return true;
}

static bool socket_open(const char *host) {
  // Open TCP socket to specified address
  return (at_write("AT^SISS=0,srvType,\"Socket\"\r", 20) &&
    at_write("AT^SISS=0,conID,\"3\"\r", 20) &&
    at_write("AT^SISS=0,\"address\",\"socktcp://%s\"\r", 20, host) &&
    at_write("AT^SISO=0\r", 20) && wait_for("^SISW: 0, 1", 30));
}

static bool socket_close(void) {
  // Close active TCP socket
  return at_write("AT^SISC=0\r", 1);  
}

static char req_buf[MAX_REQUEST] = {0};

static void send_request(const char *request, uint32_t len) {
  // Write request string to socket in blocks
  uint8_t n_blocks = (len + 255) / 256;
  for (uint8_t i = 0; i < n_blocks; ++i) {
    const char *block = request + i * 256;
    uint32_t write_len = (i < n_blocks - 1 ? 256 : len % 256);
    at_write("AT^SISW=0,%u\r", 10, write_len);
    at_write("%.*s\032", 10, write_len, block);
  }
}

static bool read_status() {
  at_write("AT^SISR=0,224\r", 10);
  const char *resp = strstr(modem_buf, "HTTP/1.1");
  return resp[9] == '2';
}

static uint32_t read_response(char *buf, uint32_t max_len) {
  // Check response code in 200 range
  at_write("AT^SISR=0,224\r", 10);
  const char *resp = strstr(modem_buf, "HTTP/1.1");
  printf("%s\n", resp);
  if (resp[9] != '2') return 0;

  // Read response length, setup first chunk
  uint32_t len = atoi(strstr(resp, FIXED_HEADER));
  if (len > max_len) len = max_len;
  resp = strstr(resp, "\r\n\r\n") + 4;
  const char *end = strstr(resp, "\r\n");

  // Keep copying buffers to buf
  printf("msg len: %x, msg: %s\n", len, resp);
  uint32_t i = 0, clen = end - resp;
  for (; i < len; i += clen) {
    memcpy(buf + i, resp, clen);
    at_write("AT^SISR=0,224\r", 10);
    resp = strstr(modem_buf, "\r\n") + 2;
    end = strstr(resp, "\r\n");

    printf("MESSAGE:\n")
    for (uint32_t j = 0; j < max_len; ++j)
        printf("%02x", (uint8_t)(buf[j]));
    printf("\n---\n");
  }
  return len;
}

static char *write_post(const modem_t *config, uint32_t len) {
  char *ptr = (char*)memset(req_buf, 0, MAX_REQUEST);
  ptr += sprintf(ptr, "POST /%s HTTP/1\r\n", "");
  ptr += sprintf(ptr, "Host: %s\r\nConnection: Close\r\n", config->host);
  ptr += sprintf(ptr, "X-Api-Key: %s\r\n", config->auth);
  ptr += sprintf(ptr, "Content-Type: application/octet-stream\r\n");
  ptr += sprintf(ptr, "Accept: application/octet-stream\r\n");
  return ptr + sprintf(ptr, "Content-Length: %lu\r\n\r\n", len);
}

static void write_readings(const modem_t *config) {
  uint32_t len = num_msgs(config->port_in) * sizeof(reading_t);
  char *ptr = write_post(config, 32 + len);

  // Copy sensor readings to request body
  *(uint32_t*)ptr = 0xf100d000; ptr += 4;
  *(uint32_t*)ptr = config->hash; ptr += 4;
  memcpy(ptr, config->node_id, 24); ptr += 24;
  for (uint8_t i = 0; i < num_msgs(config->port_in); ++i) {
    if (ptr - req_buf + sizeof(reading_t) > MAX_REQUEST) break;
    memcpy(ptr, &(recv(config->port_in)->reading), sizeof(reading_t));
    ptr += sizeof(reading_t);
  }
  send_request(req_buf, ptr - req_buf);
  // check response OK, otherwise put msgs back in port
  if (!read_status()) printf("Response not 200\n");
}

static void read_updates(const modem_t *config) {
  char *ptr = write_post(config, 24);
  memcpy(ptr, config->node_id, 24);
  send_request(req_buf, ptr + 24 - req_buf);

  uint32_t len = read_response(req_buf, MAX_REQUEST);
  if (*(uint32_t*)req_buf != 0xf100d100) {
    printf("Invalid magic bytes\n");
    return;
  }
  msg_t msg_out = {0};
  for (uint32_t i = 4; i < len; i += sizeof(reading_t)) {
    memcpy(msg_out.reading, req_buf + i);
    send(config->port_out, &msg_out);
  }
  for (uint8_t i = 0; i < num_msgs(config->port_out); ++i) {
    reading_t *trig = &(recv(config->port_in)->reading);
    print("val: %f, time: %d, label: %s\n",
        trig->value, trig->time, &(trig->label));
  }
}

void run_modem(modem_t *config) {
  while (true) {
    if (num_msgs(config->port_in) == 0) yield();
    if (startup(config) != MAX_STARTS) {
      get_meid(config->node_id);
      socket_open(config->host);
      write_readings(config);
      read_updates(config);
      socket_close();
    }
    power_off();
    sleep(config->interval);
  }
}
