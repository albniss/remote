#!/usr/bin/python

import cgi
import requests

form=cgi.FieldStorage()

url='http://192.168.200.250/YamahaRemoteControl/ctrl'
payload=form.getvalue('payload')

requests.post(url,payload)

print 'Content-type: application/json'
print ''
print '{"ok":1}'
