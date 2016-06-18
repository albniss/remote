#!/bin/sh

#Get the latest updates
cd /home/pi/remote
git pull 2>&1

#Copy HTML
cp -r html /var/www/ 2>&1

#Copy LIRC configuration
cp -r lirc/* /etc/lirc/ 2>&1

echo "OK"