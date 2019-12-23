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

#define  PKT_SIZE            sizeof(struct pkt)         // Size of a pkt struct
#define  MSG_BUFFER_SIZE     50                         // Max amount of messages to buffer in sender while the window is full
#define  WINDOW_SIZE         8                          // Max amount of packets to send before waiting for ACKs from receiver


void init(){}
void generate_next_arrival(){}
void insertevnt(struct event *p){}
void starttimer(int AorB, float increment){}
void stoptimer(int AorB){}
void tolayer3(int AorB, struct pkt p){}
void tolayer5(char datasent[20]){}

struct sender {
  int next_seqnum;
  int window_base_seqnum;
  struct pkt pkt_buffer[WINDOW_SIZE];
} A_sender;

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
  if (A_sender.next_seqnum > A_sender.window_base_seqnum + WINDOW_SIZE) {
    printf("\t\t A_OUTPUT Buffer full. Dropping message: %s\n", message.data);
    return;
  }

  struct pkt *pkt_ptr = &A_sneder.pkt_buffer[A_sender.next_seqnum % WINDOW_SIZE];

  pkt_ptr->seqnum = A_sender.next_seqnum;
  pkt_ptr->acknum = 0;
  pkt_ptr->checksum = checksum(A_sender.next_seqnum, 0, message.data);

  memmove(pkt_ptr->payload, message.data, 20);

  printf("\t\t--------------------\n");
  printf("\t\tA_OUTPUT begin\n");
  printf("\t\t--------------------\n");
  printf("\t\tA_OUTPUT seq: %d, ack: %d, checksum: %d, payload: %s\n", pkt_ptr->seqnum, pkt_ptr->acknum, pkt_ptr->checksum, pkt_ptr->payload);

  tolayer3(0, *pkt_ptr);

  if (A_sender.window_base_seqnum == A_sender.next_seqnum) {
    printf("\t\tA_OUTPUT starting timer.\n");
    starttimer(0, 15.0);
  }
  printf("\t\tA_OUTPUT (A_sender.next_seqnum mod WINDOW_SIZE): %d\n", (A_sender.next_seqnum % WINDOW_SIZE));
  printf("\t\tEND A_OUTPUT\n");
  printf("\t\t--------------------\n");

  // Incrementseq number
  A_sender.next_seqnum++;
}

void A_input(struct pkt packet)
{
  printf("\t\tA_INPUT seq: %d, ack: %d, checksum: %d, payload: %s\n", packet.seqnum, packet.acknum, packet.checksum, packet.payload);

  int csum = checksum(packet.seqnum, packet.acknum, packet.payload);
  if ( csum != packet.checkum ) {
    printf("\t\tPacket arriving at A is CORRUPT! Packet checksum %d differs from %d\n", packet.checksum, csum);
    return;
  }

  if ( packet.acknum < 0 ) {
    stoptimer(0);


    printf("\t\tA_input received NACK message. Retransmitting all packets from window_base_seqnum to next_seqnum. seq: %d, ack: %d, checksum: %d, payload: %s\n", packet.seqnum, packet.acknum, packet.checksum, packet.payload);

    printf("\t\t------------------------------\n");
    printf("\t\tNACK loop: Resending packets in buffer\n");
    printf("\t\t------------------------------\n");
    printf("\t\tA_INPUT A_sender.window_base_seqnum: %d\n", A_sender.window_base_seqnum);
    printf("\t\tA_INPUT A_sender.next_seqnum: %d\n", A_sender.next_seqnum);
    for (int i=A_sneder.window_base_seqnum; i < A_sender.next_seqnum; i++) {
      int j = i % WINDOW_SIZE;
      printf("\t\tA_INPUT seq: %d, ack: %d, checksum: %d, payload: %s\n", A_sender.pkt_buffer[j].seqnum, A_sender.pkt_buffer[j].acknum, A_sender.pkt_buffer[j].checksum, A_sender.pkt_buffer[j].payload);
      tolayer3(0, A_sender.pkt_buffer[j]);
    }
    printf("\t\tEND OF NACK LOOP\n");
    printf("\t\t------------------------------\n");

    starttimer(0, 15.0);

    return;
  }

  if ( packet.acknum > 0 ) {
    printf("\t\tA_input received ACK message. Stopping timer. seq: %d, ack: %d, checksum: %d, payload: %s\n", packet.seqnum, packet.acknum, packet.checksum, packet.payload);
  }
}
