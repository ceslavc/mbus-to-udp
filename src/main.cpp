# include <stdio.h>
# include <stdlib.h>
# include <stdint.h>
# include <termios.h>
# include <fcntl.h>
# include <string.h>
# include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>

struct termios saved_attributes;        // lagre oppsett til serieport
int uart;                             	// serieport handle
char *port,*msg;						// filnavn for serieport og
				                        // string for UDP melding     
int sockfd;         					// socket
struct addrinfo *nodered;				// adresse og port til nodered mottaker

//prosedyre som setter serieport i oprinnelig tilstand


void reset_input_mode(void)
{
tcsetattr(uart, TCSANOW, &saved_attributes);
close(uart);
}

/* serieport må åpnes med O_NONBLOCK for at den ikke henger */
/* deretter må flagget settes tilbake for å kunne lese data */
/* hvis O_NONBLOCK er på venter ikke read() på data og returnerer tom */
// Denne prosedyren åpner serieport, eller viser hvorfor det ikke fungerte

int open_serial_port (void)
{
uart = open(port,O_RDWR|O_NOCTTY);

if (uart==-1)
	{
	fprintf (stderr,"Error opening port:");
	return 1;
	}
printf("port er åpnet\n");
return 0;
}



// diverse inisialiseringer
// sette fart og andre ting rundt serieporten

void set_input_mode (void)
{
struct termios tattr;
struct addrinfo hints;
int status;

tcgetattr (uart,&saved_attributes);
atexit (reset_input_mode);

/* hent attributter for modifikasjon */
tcgetattr (uart,&tattr);

//cfmakeraw(&tattr);

/* slå av kanonisk og ekko */
tattr.c_lflag &= ~(ICANON|ECHO|ECHOE|ISIG);
//tattr.c_cflag &= ~CSIZE;
tattr.c_cflag |= CS8|CLOCAL|CREAD;
tattr.c_iflag = IGNPAR;
tattr.c_oflag = 0;
//tattr.c_lflag = 0;
//tattr.c_cflag |= PARODD;
//tattr.c_cflag &= ~PARODD;

/* hent en og en tegn uten timeout */
tattr.c_cc[VMIN] = 1;
tattr.c_cc[VTIME] = 0;

/* set porthastighet før(!) du aktiverer nye instillinger*/
cfsetspeed(&tattr,B2400);

/* aktiver nye instillinger */
tcflush(uart,TCIFLUSH);
tcsetattr (uart,TCSANOW,&tattr);

// gjør klar adresse og socket
sockfd=socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
if (sockfd == -1) 
    {
        printf("socket feil\n");
        _Exit(1);
    }
printf("Socket OK\n");

memset(&hints, 0, sizeof hints);    // make sure the struct is empty
hints.ai_family = AF_UNSPEC;        // don't care IPv4 or IPv6
hints.ai_socktype = SOCK_DGRAM;    // TCP stream sockets
status = getaddrinfo("192.168.11.22", "3500", &hints, &nodered);
if (status != 0) 
    {
        printf("getaddrinfo error:\n");
        _Exit(1);
    }
printf("getaddr OK\n");

return;
}


char get_next(void)
{
char c;
	read(uart, &c, 1);
	printf("%i ",c);
return(c);
}

void klokke(void)
{
	int i;
	for (i=1;i<14;i++) get_next();	
	return;
}

void gs1_number(void)
{
	int i;
	printf("\nGS1 number - ikke definert ennå:");
	for (i=1;i<3;i++) get_next();	
	printf("\n");
	return;
}

void meter_type(void)
{
	int i;
	printf("\nmeter type - ikke definert ennå");
	for (i=1;i<3;i++) get_next();	
	printf("\n");
	return;
}

void p14(void)
{
	int i;
	char c,cc;
	printf("\nPOWER 14: ");
	for (i=0;i<7;i++) read(uart, &c, 1);
	read(uart, &cc, 1);
	i=(c*256+cc);
	if (i>20000) return;
	sprintf(msg,"{\"data\":\"power\",\"value\":%i,\"scale\":0,\"topic\":\"AMS\"}",i);
	sendto(sockfd, msg, strlen(msg), 0, nodered->ai_addr, nodered->ai_addrlen);
	return;
}

void p23(void)
{
	int i;
	char c,cc;
	printf("\nPOWER 23: ");
	for (i=0;i<7;i++) read(uart, &c, 1);
	read(uart, &cc, 1);
	i=(c*256+cc);
	printf("%iW\n",i);	
	printf("\n");
	return;
}

void q12(void)
{
	int i;
	char c,cc;
	printf("\nREAKTANS 12: ");
	for (i=0;i<7;i++) read(uart, &c, 1);
	read(uart, &cc, 1);
	i=(c*256+cc);
	printf("%iW\n",i);	
	printf("\n");
	return;
}

void q34(void)
{
	int i;
	char c,cc;
	printf("\nREAKTANS 34: ");
	for (i=0;i<7;i++) read(uart, &c, 1);
	read(uart, &cc, 1);
	i=(c*256+cc);
	printf("%iW\n",i);	
	printf("\n");
	return;
}

void il1(void)
{
	int i;
	char c,cc;
	printf("\nStrøm fase 1: ");
	for (i=0;i<7;i++) read(uart, &c, 1);
	read(uart, &cc, 1);
	i=c*256+cc;
	if (i>6000) return;
	sprintf(msg,"{\"data\":\"amp1\",\"value\":%i,\"scale\":-2,\"topic\":\"AMS\"}",i);
	sendto(sockfd, msg, strlen(msg), 0, nodered->ai_addr, nodered->ai_addrlen);
	return;
}

void il2(void)
{
	int i;
	char c,cc;
	printf("\nStrøm fase 2: ");
	for (i=0;i<7;i++) read(uart, &c, 1);
	read(uart, &cc, 1);
	i=(c*256+cc);
	if (i>6000) return;
	sprintf(msg,"{\"data\":\"amp2\",\"value\":%i,\"scale\":-2,\"topic\":\"AMS\"}",i);
	sendto(sockfd, msg, strlen(msg), 0, nodered->ai_addr, nodered->ai_addrlen);
	return;
}

void il3(void)
{
	int i;
	char c,cc;
	printf("\nStrøm fase 3: ");
	for (i=0;i<7;i++) read(uart, &c, 1);
	read(uart, &cc, 1);
	i=(c*256+cc);
	if (i>6000) return;
	sprintf(msg,"{\"data\":\"amp3\",\"value\":%i,\"scale\":-2,\"topic\":\"AMS\"}",i);
	sendto(sockfd, msg, strlen(msg), 0, nodered->ai_addr, nodered->ai_addrlen);
	return;
}

void ul1(void)
{
	int i;
	char c;
	printf("\nSpenning fase 1: ");
	for (i=0;i<6;i++) read(uart, &c, 1);
	if (c>251) return;
	sprintf(msg,"{\"data\":\"vol1\",\"value\":%i,\"scale\":0,\"topic\":\"AMS\"}",c);
	sendto(sockfd, msg, strlen(msg), 0, nodered->ai_addr, nodered->ai_addrlen);
	return;
}

void ul2(void)
{
	int i;
	char c;
	printf("\nSpenning fase 2: ");
	for (i=0;i<6;i++) read(uart, &c, 1);
	if (c>251) return;
	sprintf(msg,"{\"data\":\"vol2\",\"value\":%i,\"scale\":0,\"topic\":\"AMS\"}",c);
	sendto(sockfd, msg, strlen(msg), 0, nodered->ai_addr, nodered->ai_addrlen);
	return;
}

void ul3(void)
{
	int i;
	char c;
	printf("\nSpenning fase 3: ");
	for (i=0;i<6;i++) read(uart, &c, 1);
	if (c>251) return;
	sprintf(msg,"{\"data\":\"vol3\",\"value\":%i,\"scale\":\"A\",\"topic\":\"AMS\"}",c);
	sendto(sockfd, msg, strlen(msg), 0, nodered->ai_addr, nodered->ai_addrlen);
	return;
}

int main()
{
char c;
int n;

port=(char*)malloc(16);
msg=(char*)malloc(64);
strcpy(port,"/dev/ttyAMA0");
// åpne serieport og sett den opp
if (open_serial_port()) exit(EXIT_FAILURE);
printf("set input\n");
set_input_mode();


while (1)
        {
	c=get_next();
	
	if (c==9) 
		{
		printf("_");
		c=get_next();
		if (c==6) 		// her kommer OBIS data
			{
			c=get_next();
			if (c==0) klokke();		//0 - den eneste med klokke
			if (c==1) 				//1 - alle andre
				{
					c=get_next();
					if (c==1)		// alle har neste tall 1, for kontroll
						{
							c=get_next();
							if (c==0) gs1_number();
							else if (c==96) meter_type();
							else if (c==1) p14();
							else if (c==2) p23();
							else if (c==3) q12();
							else if (c==4) q34();
							else if (c==31) il1();
							else if (c==51) il2();
							else if (c==71) il3();
							else if (c==32) ul1();
							else if (c==52) ul2();
							else if (c==72) ul3();
							else printf ("tall 3 udefinert\n");
						}	
					else printf("tall 2 udefinert\n");
				} 
			else printf("tall 1 udefinert\n");
			}
		}
}

    reset_input_mode();
    free(port);free(msg);
return 0;
}
