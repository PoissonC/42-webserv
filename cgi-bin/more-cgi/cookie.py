#! /usr/bin/python3

from http import cookies
import os 

def show_cookie(dictionary):
    for key, morsel in dictionary.items():
        print(key, ':', morsel.value, '<br>')

cookie = cookies.BaseCookie()
print("Content-type: text/html\r\n")
if 'HTTP_COOKIE' in os.environ:
    cookie.load(os.environ['HTTP_COOKIE'])
    show_cookie(cookie)
else:
    print("No cookies")
