# x = input()
# print("Content-Type: text/html\r\n\r\n")
# print("hello, it's python cgi")
# print(x)

# HTTP/1.1 200 OK
# Content-Type: text/html
#
# hello, it's python cgi


x = input()
print("Content-Type text/html\r\n\r\n")
print("hello, it's python cgi")
print(x)

# HTTP/1.1 500 Internal Server Error
# Content-Type: text/html
#
# hello, it's python cgi


# x = input()
# print("Status: -70\r\n")
# print("Content-Type: text/html\r\n\r\n")
# print("hello, it's python cgi")
# print(x)

# HTTP/1.1 500 Internal Server Error
# Content-Type: text/html
#
# hello, it's python cgi


# x = input()
# print("Status: 500\r\n")
# print("Content-Type: text/html\r\n\r\n")
# print("hello, it's python cgi")
# print(x)

# HTTP/1.1 500 Internal Server Error
# Content-Type: text/html
#
# hello, it's python cgi


# x = input()
# print("Status: 200\r\n")
# print("Content-Type: text/html\r\n")
# print("hello, it's python cgi")  # Missing \r\n\r\n here
# print(x)

# HTTP/1.1 500 Internal Server Error
# Content-Type: text/plain
#
# Malformed CGI output: Headers not properly terminated


# x = input()
# print("Status: 403 Bad Request\r\n")
# print("Content-Type: text/html\r\n\r\n")
# print("hello, it's python cgi")
# print(x)

# HTTP/1.1 403 Forbidden
# Content-Type: text/html
#
# hello, it's python cgi


# x = input()
# print("Status: 200 OK\r\n\r\n")
# print("hello, it's python cgi")
# print(x)

# HTTP/1.1 200 OK
# Content-Type: text/plain
#
# hello, it's python cgi


# x = input()
# print("Status: 204 No Content\r\n")
# print("Content-Type: text/html\r\n\r\n")

# HTTP/1.1 204 No Content
# Content-Type: text/html


# x = input()
# print("Status: 404 Not Found\r\n")
# print("Status: 403 Forbidden\r\n")  # This should be ignored
# print("Content-Type: text/html\r\n\r\n")
# print("hello, it's python cgi")
# print(x)

# HTTP/1.1 404 Not Found
# Content-Type: text/html
#
# hello, it's python cgi


# x = input()
# print("Status: 200 OK\r\n")
# print("Content-Type: text/html\r\n")
# print("<html>")  # Mixing headers and body
# print("Content-Length: 27\r\n")
# print("Body continues here...</html>")

# HTTP/1.1 500 Internal Server Error
# Content-Type: text/plain
#
# Malformed CGI output: Headers and body intermingled


# x = input()
# print("Status: 200 OK\r\n")
# print("Content-Type: text/html\r\n")
# print("RateLimit-Reset: 332\r\n")
# print("X-Custom-Header: CustomValue\r\n\r\n")
# print("hello, it's python cgi")
# print(x)

# HTTP/1.1 200 OK
# X-Custom-Header: CustomValue
# Content-Type: text/html
#
# hello, it's python cgi


# x = input()
# print("Status: 200 OK\r\n")
# print("Content-Type: text/html")  # Missing final \r\n
# print("hello, it's python cgi")
# print(x)

# HTTP/1.1 500 Internal Server Error
# Content-Type: text/plain
#
# Malformed CGI output: Missing final \r\n after headers


# x = input()

# HTTP/1.1 500 Internal Server Error
# Content-Type: text/plain
#
# CGI script produced no output.
