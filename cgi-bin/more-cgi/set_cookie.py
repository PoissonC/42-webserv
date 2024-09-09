#! /usr/bin/python3

import os
from http import cookies
import cgi, cgitb

form = cgi.FieldStorage()

key = form.getvalue('key')
value = form.getvalue('value')

if key is None or value is None:
    print("HTTP/1.1 400 Bad Request")
    print("Content-Type: text/plain\r\n")
    print("Missing 'key' or 'value' parameter.")
else:
    cookie = cookies.SimpleCookie()
    cookie[key] = value
    print("HTTP/1.1 204 OK")
    print("Content-Type: text/plain")
    print(cookie.output())
    print("\r\n")
