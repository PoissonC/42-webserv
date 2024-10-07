### Introduction
Developed a custom HTTP/1.1 server in C++ from scratch, gaining hands-on experience with networking, socket programming, and I/O multiplexing using poll. Implemented key HTTP features, including client request handling, CGI script execution, chunked responses, and robust error management. This project enhanced my understanding of web technologies and low-level network communication.

### Difficulties
From the most resources on the Internet, including the same projects that 42 students did previously. They don't implement the "Non-Blocking" well. The essence of this project is to make every I/O operation non-blocking. So not just the socket connections should be set as non-blocking mode and be monitored by the poll API (or equivalent), CGI reading/writing, file reading/saving should also be set to non-blocking mode. To implement this, this whole project would be much more difficult and hard to handle many details according to the error conditions.

(If the project calls the 'fstream' or similar reading utils in STL is considered as a violation of the project rule.)
