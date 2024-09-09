#! /usr/bin/python3

import os
from http import cookies 
import cgi, cgitb

form = cgi.FieldStorage()

key = form.getvalue('key')
cookie = cookies.SimpleCookie()
if 'HTTP_COOKIE' in os.environ:
    cookie.load(os.environ['HTTP_COOKIE'])
if key in cookie:
    print("HTTP/1.1 200 OK")
    print("Content-type: text/html\r\n")
    print("Cookie Value of", key, "is :", cookie[key].value)
else:
    print("HTTP/1.1 200 OK")
    print("Content-type: text/html\r\n")
    print("Cookie does not exist")
