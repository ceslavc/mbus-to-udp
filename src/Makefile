#This file is part of mbus-to-udp.
#
#   mbus-to-udp is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    mbus-to-udp is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with mbus-to-udp.  If not, see <http://www.gnu.org/licenses/>.

CC = gcc
FILES = mbustoudp.cpp
build: $(FILES)
	gcc mbustoudp.cpp -o mbustoudp
	echo "You must edit ip_address and port in the mbustoudp.service file before installing"

clean:
	echo "Stopping and disabling service"
	systemctl stop mbustoudp.service
	systemctl disable mbustoudp.service
	echo "Removing files"
	rm mbustoudp
	rm /usr/bin/mbustoudp
	rm /lib/systemd/system/mbustoudp.service

install:
	echo "Copying files"
	cp mbustoudp /usr/bin/
	cp mbustoudp.service /lib/systemd/system/
	
	systemctl start mbustoudp.service
	systemctl enable mbustoudp.service
