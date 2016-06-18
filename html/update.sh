#!/bin/sh

#Get the latest updates
if [ ! -d "/tmp/remote" ]; then
  git clone https://github.com/albniss/remote.git /tmp/remote 2>&1
fi

cd /tmp/remote
git pull 2>&1

#Copy HTML
cp -r html /var/www/ 2>&1

#Copy LIRC configuration
cp -r lirc/* /etc/lirc/ 2>&1

echo "OK"