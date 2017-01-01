#!/bin/bash

REMOTE="`echo ${QUERY_STRING} | sed -r 's/remote=(.*)&command=(.*)/\1/'`"
CMD="`echo ${QUERY_STRING} | sed -r 's/remote=(.*)&command=(.*)/\2/'`"

irsend SEND_ONCE $REMOTE $CMD  1> /dev/null 2> /dev/null
if [ $? -ne 0 ]; then
    echo "Content-type: application/json"
    echo ""
    echo '{"ok":0}'
else
    echo "Content-type: application/json"
    echo ""
    echo '{"ok":1}'
fi

exit 0
