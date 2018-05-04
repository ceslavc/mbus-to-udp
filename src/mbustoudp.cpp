/* 
MBUS to UDP converter v0.9
(c) 2018 Ceslav Czyz 

This file is part of mbus-to-udp.

    mbus-to-udp is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    mbus-to-udp is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with mbus-to-udp.  If not, see <http://www.gnu.org/licenses/>.

*/
# include <stdio.h>
# include <stdlib.h>
# include <stdint.h>
# include <termios.h>
# include <fcntl.h>
# include <string.h>
# include <unistd.h>
# include <netdb.h>
# include <sys/socket.h>
# include <netinet/in.h>

struct termios gammel_uart;        		// structure to save the state of the serial port
int uart;                             	// serial port handle
char *port, *msg;						// filename of the serial port, string for the UDP message
int sockfd;         					// IP socket for UDP message
struct addrinfo *nodered;				// stucture to address the data receiver

// Print help text
void printhelp(void)
{
printf("Converter from MBUS to UDP\n");
printf("Usage:\n\tmbustoudp <parameter> <ip_address> <port>\n");
printf("\tMust be run as root due to serial port access rights\n");
printf("\tMake sure the firewall is open on receiver\n");
printf("Parameters:\n\t--help\tshow this text\n\t--mon\trun in monitor mode - show what is coming on serial port\n");
printf("\t--test\tsend a test JSON object\n");
return;
}


// reset the serial port to its original state

void reset_serial_port(void)
{
tcsetattr(uart, TCSANOW, &gammel_uart);
free(port);
free(msg);
close(uart);
}

// initialize and open serial port

int open_serial_port (void)
{
struct termios tattr;

// set the name of the serial port
port=(char*)malloc(16);
strcpy(port,"/dev/ttyAMA0");

// open serial port
uart = open(port,O_RDWR|O_NOCTTY);
if (uart==-1)
	{
	printf("Error opening port\n");
	exit(0);
	}

// save serial port state for recovery at exit
tcgetattr (uart,&gammel_uart);
atexit (reset_serial_port);

// copy serial port state for modificatiom
tcgetattr (uart,&tattr);

// modify serial port parameters
tattr.c_lflag &= ~(ICANON|ECHO|ECHOE|ISIG);
tattr.c_cflag |= CS8|CLOCAL|CREAD;
tattr.c_iflag = IGNPAR;
tattr.c_oflag = 0;
tattr.c_cc[VMIN] = 1;
tattr.c_cc[VTIME] = 0;
cfsetspeed(&tattr,B2400);

// clean the buffer 
tcflush(uart,TCIFLUSH);

// set new state
tcsetattr (uart,TCSANOW,&tattr);

return 0;
}

// initialize socket for UDP sending

int set_socket (char* ip, char* port)
{
struct addrinfo hints;
int status;

sockfd=socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
if (sockfd == -1) 
    {
        printf("Error initializing socket\n");
        exit(1);
    }

memset(&hints, 0, sizeof hints);    
hints.ai_family = AF_UNSPEC;        
hints.ai_socktype = SOCK_DGRAM;
status = getaddrinfo(ip, port, &hints, &nodered);
if (status != 0) 
    {
        printf("Cannot open socket, is the IP address and port valid?\n");
        exit(1);
    }
return 0;
}

void sendjson(int ign, float scale, const char *data_id)
{
	int val;
	char c, cc;
	int i;
	for(i=0;i<ign;i++) read(uart,&c,1);
	read(uart,&cc,1);							//higher byte
	read(uart,&c,1);							//lower byte
	val=(cc*256+c)*scale;						//multiply and scale
	sprintf(msg,"{\"data\":\"%s\",\"value\":%d,\"topic\":\"AMS\"}",data_id,val);
	sendto(sockfd, msg, strlen(msg), 0, nodered->ai_addr, nodered->ai_addrlen);	
	return;
}


void transceive(void)
{
char c,c1,c2,c3,c4;
int n;
msg=(char*)malloc(64);

while (1) {
	read(uart, &c, 1);
	
	if (c==9) {
		read(uart, &c, 1);
		if (c==6) {					// 9,6 - OBIS code is following
			read(uart, &c1, 1);
			if (c1==1) {				//1 - all other except clock, but who cares about clock
				read(uart, &c2, 1);
					if (c2==1) {		// All codes have 1 here, just checking
						read(uart, &c3, 1);
						if (c3==1) {
							read(uart, &c4,1);						//ACTIVE power FROM grid, check next byte
							if (c4==7) sendjson(5,1,"power");		//power from grid
							else if (c4==8) sendjson(5,10,"pcons"); //cummulative consumed power
						}
						else if (c3==2) {
							read(uart, &c4,1);						//ACTIVE power TO grid, check next byte
							if (c4==7) sendjson(5,1,"powerto");		//power to grid
							else if (c4==8) sendjson(5,10,"pdelv"); 	//cummulative delivered power
						}
						else if (c3==3)  {
							read(uart, &c4,1);						//REACTIVE power FROM grid, check next byte
							if (c4==7) sendjson(5,1,"rpower");		//power from grid
							else if (c4==8) sendjson(5,10,"rpcons"); //cummulative consumed reactive power
						}
						else if (c3==4)  {
							read(uart, &c4,1);						//REACTIVE power TO grid, check next byte
							if (c4==7) sendjson(5,1,"rpowerto");		//power to grid
							else if (c4==8) sendjson(5,10,"rpdelv"); //cummulative delivered reactive power
						}
						else if (c3==31) sendjson(6,0.01,"amp1");	//current phase 1, divided by 100;
						else if (c3==51) sendjson(6,0.01,"amp2");	//current phase 2
						else if (c3==71) sendjson(6,0.01,"amp3");	//current phase 3
						else if (c3==32) sendjson(4,1,"vol1");		//voltage phase 1, no scaling
						else if (c3==52) sendjson(4,1,"vol2");		//voltage phase 2
						else if (c3==72) sendjson(4,1,"vol3");		//voltage phase 3
						}
					}	
				} 
			}
	}


return;
}

int main(int argc, char* argv[])
{
char c;

// Must be run as root due serial port ownership


// no parameters
if (argc==1) printf("No parameters, type 'mbustoudp --help' for help\n");

// one parameter - may be help or monitor
else if (argc==2) {
    if (!strcmp(argv[1],"--help")) printhelp();
    else if (!strcmp(argv[1],"--mon")) {
		if (getuid()) {printf("You must be root.\n"); return 0; } 
		open_serial_port();
		printf("Running in monitor mode, press ctrl-c to quit.\n");
		while (1) {
			read(uart,&c,1);
			printf("%c ",c);
		}
	}
	else if (!strcmp(argv[1],"--test")) {
		printf("Please provide ip address and port.\n");
		}
	else printf("Unknown parameter\n");
}

// two parameters, lets hope it is ip address and port
else if (argc==3) {
	if (getuid()) {printf("You must be root.\n"); return 0; } 
	set_socket(argv[1],argv[2]);
	open_serial_port();
	transceive();
}

else if ((argc==4) && (!strcmp(argv[1],"--test"))) {
	set_socket(argv[2],argv[3]);
	msg=(char*)malloc(64);
	sprintf(msg,"{\"data\":\"power\",\"value\":1342,\"scale\":0,\"topic\":\"AMS\"}");
	sendto(sockfd, msg, strlen(msg), 0, nodered->ai_addr, nodered->ai_addrlen);
	printf("Sendt single JSON object to address/port. Power node should show 1342W.\n");
	exit(0);
}

else printf("Unknown parameter\n");

return 0;
}
