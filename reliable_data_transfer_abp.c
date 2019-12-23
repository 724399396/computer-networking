#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BIDIRECTIONAL 0

struct msg {
  char data[20];
};

struct pkt {
  int seqnum;
  int acknum;
  int checksum;
  char payload[20];
};

struct event {
  float evtime;
  int evtype;
  int eventity;
  struct pkt *pktptr;
  struct event *prev;
  struct event *next;
};

void init(){}
void generate_next_arrival(){}
void insertevnt(struct event *p){}
void starttimer(int AorB, float increment){}
void stoptimer(int AorB){}
void tolayer3(int AorB, struct pkt p){}
void tolayer5(char datasent[20]){}

enum sending_state {
                    READY,
                    WATING_FOR_ACK
};

struct sender {
  int seqnum;
  int acknum;
  enum sending_state sending_state;
  struct pkt last_packet;
} A_sender, B_sender;

int checksum(int seqnum, int acknum, char payload[20])
{
  int sum = 0;

  sum += (seqnum + acknum);

  for (int i=0; i < 20; i++) {
    sum += (int) payload[i];
  }
  return sum;
}

int main()
{
  return 0;
}

void A_output(struct msg message)
{
  if (A_sender.sending_state == WATING_FOR_ACK) {
    printf("\t\tA_sender.sending_state is WAITING_FOR_ACK. Dropping new packet to A_output until current packet is sent.\n");
    return;
  }

  struct pkt A_out;

  A_out.seqnum = A_sender.seqnum;
  A_out.acknum = A_sender.acknum;
  A_out.checksum = checksum(A_sender.seqnum, A_sender.acknum, message.data);

  size_t dest_size = sizeof(message.data);
  strncpy(A_out.payload, message.data, dest_size);

  printf("\t\tA_OUTPUT seq: %d, ack: %d, checksum: %d, payload: %s\n", A_out.seqnum, A_out.acknum, A_out.checksum, A_out.payload);

  tolayer3(0, A_out);

  A_sender.seqnum = 1 - A_sender.seqnum;

  starttimer(0, 20.0);

  A_sender.sending_state = WATING_FOR_ACK;

  A_sender.last_packet = A_out;
}

void A_input(struct pkt packet)
{
  printf("\t\tA_INPUT seq: %d, ack: %d, checksum: %d, payload: %s\n", packet.seqnum, packet.acknum, packet.checksum, packet.payload);

  int csum = checksum(packet.seqnum, packet.acknum, packet.payload);
  if ( csum != packet.checksum ) {
    printf("\t\tPACKET arriving at A is CORRUPT! Packet checksum %d differs from %d\n", packet.checksum, csum);
    return;
  }

  if ( csum == packet.checksum && packet.acknum == (A_sender.last_packet.seqnum - 1)) {
    stoptimer(0);

    printf("\t\tA_input received NACK message. Retransmitting last packet. seq: %d, ack: %d, checksum: %d, payload: %s\n", A_sender.last_packet.seqnum, A_sender.last_packet.acknum, A_sender.last_packet.checksum, A_sender.last_packet.payload);
    tolayer3(0, A_sender.last_packet);

    printf("\t\tA_OUTPUT seq: %d, ack: %d, checksum: %d, payload: %s\n", A_sender.last_packet.seqnum, A_sender.last_packet.acknum, A_sender.last_packet.checksum, A_sender.last_packet.payload);

    starttimer(0, 20.0);

    return;
  }

  if ( csum == packet.checksum && packet.acknum == A_sender.last_packet.seqnum ) {
    stoptimer(0);

    printf("\t\tA_input received ACK message. Stopping timer and setting A_sender.sending_state to READY. seq: %d, ack: %d, checksum: %d, payload: %s\n", packet.seqnum, packet.acknum, packet.checksum, packet.payload);

    A_sender.sending_state = READY;

    return;
  }
}

void A_timeinterrupt()
{
  printf("\t\tA_timerinterrupt has gone off. Resending last packet. seq: %d, ack: %d, checksum: %d, payload: %s\n", A_sender.last_packet.seqnum, A_sender.last_packet.acknum, A_sender.last_packet.checksum, A_sender.last_packet.payload);

  tolayer3(0, A_sender.last_packet);

  printf("\t\tA_OUTPUT seq: %d, ack: %d, checksum: %d, payload: %s\n", A_sender.last_packet.seqnum, A_sender.last_packet.acknum, A_sender.last_packet.checksum, A_sender.last_packet.payload);

  starttimer(0, 20.0);
}

void A_init()
{
  A_sender.sending_state = READY;
  A_sender.seqnum = 0;
  A_sender.acknum = 0;
}
