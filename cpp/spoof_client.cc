#include <iostream>
#include <cstdlib>
#include <cstring>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <errno.h>

class IP
{
    struct ip ip;

public:
    IP ()
    {
        ip.ip_v   = 0x4;
        ip.ip_tos = 0x0;
        ip.ip_sum = 0x0;
    }

    IP (char* src, char* dest)
    {
        ip.ip_v = 0x4;
        ip.ip_tos = 0x0;
        ip.ip_sum = 0x0;
        ip.ip_src.s_addr = inet_addr(src);
        ip.ip_dst.s_addr = inet_addr(dest);
    }

    void set_header_len (int hl)      { ip.ip_hl = hl; }
    void set_len (int len)            { ip.ip_len = htons(len); }
    void set_id (int id)              { ip.ip_id = htons(id); }
    void set_offset (int offset)      { ip.ip_off = offset; }
    void set_ttl (int ttl)            { ip.ip_ttl = ttl; }
    void set_sum (unsigned short sum) { ip.ip_sum = sum; }
    void set_proto (int p)            { ip.ip_p = p; }
    
    unsigned short get_len() { return ntohs(ip.ip_len); }
    struct ip* get_addr()    { return &ip; }
};

class ICMP
{
    struct icmp icmp;

public:
    ICMP()
    {
        icmp.icmp_id    = 1234;
        icmp.icmp_seq   = 0;
        icmp.icmp_cksum = 0x0;
    }

    void set_type (unsigned int type) { icmp.icmp_type = type; }
    void set_code (unsigned int code) { icmp.icmp_code = code; }
    void set_cksum (unsigned short sum) { icmp.icmp_cksum = sum; }

    struct icmp* get_addr() { return &icmp; }
    unsigned int get_type() { return icmp.icmp_type; }
};

/*
 * Function to calculate the checksum required for the
 * ip_sum and icmp_cksum fields. Checksum is the one's 
 * complement sum of all the 16 bit words in the header
 */
unsigned short checksum (unsigned short *addr, int len)
{
    int nleft = len, sum = 0;
    unsigned short *w = addr;
    unsigned short result = 0;

    while (nleft > 1)
    {
        sum += *w++;
        nleft -= 2;
    }

    if (nleft == 1)
    {
        *(unsigned char*) (&result) = *(unsigned char *)w;
        sum += result;
    }

    sum  = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    result = ~sum;
    return result;
}

int main(int argc, char **argv)
{
    unsigned char *packet;
    char *src, *dest;
    int sd;
    const int on = 1;
    struct sockaddr_in sockaddr;

    if (argc == 3)
    {
        src = argv[1];
        dest = argv[2];
    } 
    else
    {
        printf("Correct usage is sourceIP destIP\n");
        exit(1);
    }

    // IP header
    IP ip_pkt(src, dest);
    ip_pkt.set_header_len(0x5);
    ip_pkt.set_id(12830);
    ip_pkt.set_offset(0x0);
    ip_pkt.set_ttl(64);
    ip_pkt.set_len(60);
    ip_pkt.set_proto(IPPROTO_ICMP);
    ip_pkt.set_sum(checksum((u_short *)ip_pkt.get_addr(), sizeof(struct ip)));
    
    // ICMP header
    ICMP icmp_pkt;
    icmp_pkt.set_type(ICMP_ECHO);
    icmp_pkt.set_code(0);
    icmp_pkt.set_cksum(checksum((u_short *)icmp_pkt.get_addr(), 8));

    /*
     * Allocate memory for the packet and copy the IP header first.
     * Then the ICMP header after an offset of 20 bytes
     */
    packet = (unsigned char *) malloc(ip_pkt.get_len());
    memcpy(packet, ip_pkt.get_addr(), sizeof(struct ip));
    memcpy(packet + 20, icmp_pkt.get_addr(), 8);
    
    char message[32] = {0};
    strcpy(message, "Ravioli Ravioli");
    memcpy(packet + 28, &message, sizeof(message));

    /*
     * Create raw socket so the kernel doesn't interfere
     * with the headers of the custom packet.
     */
    if ((sd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) < 0)
    {
        std::cout << strerror(errno) << std::endl;
        printf("Error creating socket\n");
        exit(1);
    }
    if (setsockopt(sd, IPPROTO_IP, IP_HDRINCL, &on, sizeof(on)) < 0)
    {
        std::cout << strerror(errno) << std::endl;
        printf("Error setting options\n");
        exit(1);
    }

    memset(&sockaddr, 0, sizeof(sockaddr));
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_addr.s_addr = inet_addr(dest);

    // yeet the packet
    if (sendto(sd, packet, ip_pkt.get_len(), 0,
        (struct sockaddr *)&sockaddr, sizeof(sockaddr)) < 0)
    {
        std::cout << strerror(errno) << std::endl;
        printf("Packet couldn't be sent\n");
        exit(1);
    }
    printf("Packet sent\n");
    return 0;
}
