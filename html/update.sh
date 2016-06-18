#!/bin/sh

#Get the latest updates
cd /home/pi/remote
git pull

#Copy HTML
cp -r html /var/www/

#Copy LIRC configuration
cp -r lirc/* /etc/lirc/

echo "OK"