#include <mc1322x.h>
#include <board.h>
#include <stdio.h>

#include "tests.h"
#include "config.h"

static uint16_t
chksum(uint16_t sum, const uint8_t *data, uint16_t len)
{
  uint16_t t;
  const uint8_t *dataptr;
  const uint8_t *last_byte;

  dataptr = data;
  last_byte = data + len - 1;
  
  while(dataptr < last_byte) {   /* At least two more bytes */
    t = (dataptr[0] << 8) + dataptr[1];
    sum += t;
    if(sum < t) {
      sum++;      /* carry */
    }
    dataptr += 2;

  }
  
  if(dataptr == last_byte) {
    t = (dataptr[0] << 8) + 0;
    sum += t;
    if(sum < t) {
      sum++;      /* carry */
    }
  }

  /* Return sum in host byte order. */
  return sum;
}

typedef union uip_ip6addr_t {
  uint8_t  u8[16];			/* Initializer, must come first. */
  uint16_t u16[8];
} uip_ip6addr_t;

typedef uip_ip6addr_t uip_ipaddr_t;

struct uip_ip_hdr {
  /* IPV6 header */
  uint8_t vtc;
  uint8_t tcflow;
  uint16_t flow;
  uint8_t len[2];
  uint8_t proto, ttl;
  uip_ip6addr_t srcipaddr, destipaddr;
};

#define UIP_BUFSIZE 1300

typedef union {
  uint32_t u32[(UIP_BUFSIZE + 3) / 4];
  uint8_t u8[UIP_BUFSIZE];
} uip_buf_t;

uip_buf_t uip_aligned_buf;
#define uip_buf (uip_aligned_buf.u8)

#define UIP_IPH_LEN    40
#define UIP_LLH_LEN     14
#define UIP_PROTO_UDP   17
#define UIP_IP_BUF          ((struct uip_ip_hdr *)&uip_buf[UIP_LLH_LEN])
#define uip_ext_len 0

/**
 * Convert 16-bit quantity from host byte order to network byte order.
 *
 * This macro is primarily used for converting constants from host
 * byte order to network byte order. For converting variables to
 * network byte order, use the uip_htons() function instead.
 *
 * \hideinitializer
 */
#ifndef UIP_HTONS
#   if UIP_BYTE_ORDER == UIP_BIG_ENDIAN
#      define UIP_HTONS(n) (n)
#      define UIP_HTONL(n) (n)
#   else /* UIP_BYTE_ORDER == UIP_BIG_ENDIAN */
#      define UIP_HTONS(n) (uint16_t)((((uint16_t) (n)) << 8) | (((uint16_t) (n)) >> 8))
#      define UIP_HTONL(n) (((uint32_t)UIP_HTONS(n) << 16) | UIP_HTONS((uint32_t)(n) >> 16))
#   endif /* UIP_BYTE_ORDER == UIP_BIG_ENDIAN */
#else
#error "UIP_HTONS already defined!"
#endif /* UIP_HTONS */

/**
 * Convert a 16-bit quantity from host byte order to network byte order.
 *
 * This function is primarily used for converting variables from host
 * byte order to network byte order. For converting constants to
 * network byte order, use the UIP_HTONS() macro instead.
 */
#ifndef uip_htons
uint16_t uip_htons(uint16_t val);
#endif /* uip_htons */
#ifndef uip_ntohs
#define uip_ntohs uip_htons
#endif

#ifndef uip_htonl
uint32_t uip_htonl(uint32_t val);
#endif /* uip_htonl */
#ifndef uip_ntohl
#define uip_ntohl uip_htonl
#endif

uint16_t
uip_htons(uint16_t val)
{
  return UIP_HTONS(val);
}

void fill_packet(volatile packet_t *p, uint8_t *payload, uint8_t paylen) {
	static volatile uint8_t count=0;
	volatile uint8_t i;
	uint16_t sum;

	p->length = 71 + paylen;
	p->offset = 0;

	/* FCF 0xcc61 */
	p->data[0] = 0x61;
	p->data[1] = 0xcc;

	/* seq number */
	p->data[2] = count++;

	/* PAN id contiki = 0xabcd*/
	p->data[3] = 0xcd;
	p->data[4] = 0xab;

	/* dest mac addr: should be addr of the border-router radio */
	p->data[5] =  0x8c;
	p->data[6] =  0x7e;
	p->data[7] =  0x7e;
	p->data[8] =  0x8c;
	p->data[9] =  0x2a;
	p->data[10] = 0x0c;
	p->data[11] = 0x05;
	p->data[12] = 0x00;

	/* source addr (our mac addr) */
	p->data[13] = 0xd7;
	p->data[14] = 0xd1;
	p->data[15] = 0xbd;
	p->data[16] = 0x12;
	p->data[17] = 0x4d;
	p->data[18] = 0x3c;
	p->data[19] = 0x47;
	p->data[20] = 0xec;

	/* UDP */
	p->data[21] = 0x7e;
	p->data[22] = 0x00;

	/* source IP addr */
	p->data[23] = 0x20;
	p->data[24] = 0x02;
	p->data[25] = 0xc6;
	p->data[26] = 0x3d;
	p->data[27] = 0xee;
	p->data[28] = 0xef;
	p->data[29] = 0x00;
	p->data[30] = 0x10;
	p->data[31] = 0xee;
	p->data[32] = 0x47;
	p->data[33] = 0x3c;
	p->data[34] = 0x4d;
	p->data[35] = 0x12;
	p->data[36] = 0xbd;
	p->data[37] = 0xd1;
	p->data[38] = 0xd7;

	/*  dest IP addr */
	p->data[39] = 0x20;
	p->data[40] = 0x01;
	p->data[41] = 0x48;
	p->data[42] = 0x01;
	p->data[43] = 0x78;
	p->data[44] = 0x17;
	p->data[45] = 0x00;
	p->data[46] = 0x72;
	p->data[47] = 0xd8;
	p->data[48] = 0x44;
	p->data[49] = 0x1b;
	p->data[50] = 0x33;
	p->data[51] = 0xff;
	p->data[52] = 0x10;
	p->data[53] = 0x4d;
	p->data[54] = 0xca;
	
	/* dest inline */
	p->data[55] = 0xf0;

	/* source port */
	p->data[56] = 0x16;
	p->data[57] = 0x33;

	/* dest port */
	p->data[58] = 0x16;
	p->data[59] = 0x33;


	sum = 0;
	sum += 17 + paylen + 17; /* udplen (paylen + 17) + upd proto 17 */

//	printf("sum 1: %04x\n", sum);
	gpio_set(KBI3);
	/* sum src and dest addr */
	sum = chksum(sum, (uint8_t *) &p->data[23], 32);
	  
//	printf("sum 2: %04x\n", sum);

	/* UDP checksum */
	/* 60 */
	/* 61 */
	/* stuffed later */

	/* coap non */
	p->data[62] = 0x51;

	/* coap post*/
	p->data[63] = 0x02;

	/* TID */
	p->data[64] = count; 	
	p->data[65] = count;

	/* path and len*/
	p->data[66] = 0x94;

	/* path: /sink */
	p->data[67] = 0x73;
	p->data[68] = 0x69;
	p->data[69] = 0x6e;
	p->data[70] = 0x6b;


	/* sum ports */
	sum = chksum(sum, (const uint8_t *) &p->data[56], 4);
	/* sum another len */
	{
	  uint8_t len[4];
	  len[0] = 0;
	  len[1] = paylen + 17;
	  len[2] = 0;
	  len[3] = 0;
	  sum = chksum(sum, (const uint8_t *) len, 4);
	}

	/* fill payload here */
	/* (up to 31 bytes) */
	/* p->data[71] = first byte of data */

	for(i=0; i < paylen; i++) {
	  p->data[71+i] = payload[i];
	}

	/* sum data */
	sum = chksum(sum, (const uint8_t *) &p->data[62], 9 + paylen);

//	printf("sum: %04x\n", sum);

	sum = ~((sum == 0) ? 0xffff : uip_htons(sum));

//	printf("real sum: %04x\n", sum);

	/* UDP checksum */
	p->data[60] = (uint8_t)(sum >> 8);
	p->data[61] = (uint8_t)(sum & 0x00ff);

}

