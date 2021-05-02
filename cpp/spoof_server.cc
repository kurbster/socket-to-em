#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <net/ethernet.h>

int main()
{
    int sock_r;
    if ((sock_r = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) < 0)
    {
        perror("creating socket");
        return -1;
    }

    unsigned char *buffer = (unsigned char *) malloc(65536); //to receive data
    memset(buffer,0,65536);
    struct sockaddr saddr;
    int saddr_len = sizeof (saddr);
     
    //Receive a network packet and copy in to buffer
    int buflen;
    if ((buflen = recvfrom(sock_r,buffer,65536,0,&saddr,(socklen_t *)&saddr_len)) < 0)
    {
        perror("receiving");
        return -1;
    }

    struct ethhdr *eth = (struct ethhdr *)(buffer);
    printf("\nEthernet Header\n");
    printf("\t|-Source Address : %.2X-%.2X-%.2X-%.2X-%.2X-%.2X\n",eth->h_source[0],eth->h_source[1],eth->h_source[2],eth->h_source[3],eth->h_source[4],eth->h_source[5]);
    printf("\t|-Destination Address : %.2X-%.2X-%.2X-%.2X-%.2X-%.2X\n",eth->h_dest[0],eth->h_dest[1],eth->h_dest[2],eth->h_dest[3],eth->h_dest[4],eth->h_dest[5]);
    printf("\t|-Protocol : %d\n",eth->h_proto);
    
    unsigned short iphdrlen;
    struct iphdr *ip = (struct iphdr*)(buffer + sizeof(struct ethhdr));
    struct sockaddr_in source, dest;
    memset(&source, 0, sizeof(source));
    source.sin_addr.s_addr = ip->saddr;
    memset(&dest, 0, sizeof(dest));
    dest.sin_addr.s_addr = ip->daddr;
     
    printf("IP Header\n");
    printf("\t|-Version : %d\n",(unsigned int)ip->version);
    printf("\t|-Internet Header Length : %d DWORDS or %d Bytes\n",(unsigned int)ip->ihl,((unsigned int)(ip->ihl))*4);
    printf("\t|-Identification : %d\n",ntohs(ip->id));
    printf("\t|-Time To Live : %d\n",(unsigned int)ip->ttl);
    printf("\t|-Protocol : %d\n",(unsigned int)ip->protocol);
    printf("\t|-Header Checksum : %d\n",ntohs(ip->check));
    printf("\t|-Source IP : %s\n", inet_ntoa(source.sin_addr));
    printf("\t|-Destination IP : %s\n",inet_ntoa(dest.sin_addr));

    /* getting actual size of IP header*/
    iphdrlen = ip->ihl*4;

    struct tcphdr *udp=(struct tcphdr*)(buffer + iphdrlen + sizeof(struct ethhdr));
    unsigned char *data = (buffer + iphdrlen + sizeof(struct ethhdr) + sizeof(udp));
    int remaining_data = buflen - (iphdrlen + sizeof(struct ethhdr) + sizeof(udp));
     
    for(int i=0;i<remaining_data;i++)
    {
        if(i!=0 && i%16==0)
            printf("\n");
        printf("%.2X ",data[i]);
    }
    printf("\n");
    return 0;
}
