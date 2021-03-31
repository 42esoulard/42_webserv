# 42_webserv
42's webserv, team rturcey-esoulard! [04/2021] [Ubuntu18]

**GOAL**: write a HTTP server in C++ 98, compliant with rfc 7230 to 7235 (http 1.1)
When in doubt, compare program behaviour with nginx.

**LINKS**:
- [Git](https://github.com/42esoulard/42_webserv)
- [Subject](https://cdn.intra.42.fr/pdf/pdf/19749/en.subject.pdf)

**RESOURCES**:
- [HTTP wiki](https://en.wikipedia.org/wiki/Hypertext_Transfer_Protocol)
- [HTTP BASIC TUTORIAL](https://code.tutsplus.com/tutorials/a-beginners-guide-to-http-and-rest--net-16340)
- [HTTP CS COURSE](https://www.tutorialspoint.com/http/index.htm)
- [RFC7230](https://tools.ietf.org/html/rfc7230)
- [RFC7235](https://tools.ietf.org/html/rfc7235)

- [PYTHON HTTP SERVER TUTORIAL](https://www.afternerd.com/blog/python-http-server/)
- [STACK EXCHANGE ON SETTING UP HTTP SERVER](https://softwareengineering.stackexchange.com/questions/200821/how-to-write-a-http-server)
- [HTTP SERVER WALKTHROUGH](https://medium.com/from-the-scratch/http-server-what-do-you-need-to-know-to-build-a-simple-http-server-from-scratch-d1ef8945e4fa)

**BEFORE STARTING, DO TESTS WITH**:
- telnet
- nginx

**KEY NOTIONS**:
- *Hypertext Transfer Protocol (HTTP)* : an application protocol for distributed, collaborative, hypermedia information systems. Was developed to facilitate hypertext and the World Wide Web. Communication between client and server uses HTTP. HTTP also includes ways of receiving content from clients. This feature is used for submitting web forms, including uploading of files.
- *World Wide Web* : where hypertext documents include hyperlinks to other resources that the user can easily access.
- *Web server* : stores, processes and delivers web pages to clients
- *HTML* : Pages delivered are most frequently HTML documents, which may include images, style sheets and scripts in addition to the text content.
- *User agent* :  commonly a web browser or web crawler. Initiates communication by
making a request for a specific resource using HTTP and the server responds with the
content of that resource or an error message if unable to do so. The resource is typically a real file on the server’s secondary storage, but this is not necessarily the case and depends on how the web server is implemented.
- [*CGI*](https://en.wikipedia.org/wiki/Common_Gateway_Interface)

**PROGRAM ARGUMENTS**
- A config file in argument (or use a default path). Inspired from the "server" part of nginx
configuration file:
  - [NGINX basic config](https://docs.nginx.com/nginx/admin-guide/basic-functionality/managing-configuration-files/)
  - [NGINX full example](https://www.nginx.com/resources/wiki/start/topics/examples/full/)

**MINDMAPS**:
- [Main modules of our program](https://app.mindmup.com/map/new/1617193151929)
- of more precise modules (TBD)

**TEAM NAMING FORMATS CONVENTION (TBC)**:
- function_name()
- variableName()
- _privateVariable
- ClassName
- s_struct t_struct

**ALLOWED TOOLS**:
- **Functions**: 
    - *allocation*: malloc, free
    - *file/repo handling*: write, open, read, close, mkdir, rmdir
    - *process handling*: unlink, fork, wait, waitpid, wait3, wait, signal, kill, exit, execve, dup, dup2, pipe
    - *path handling*: getcwd, chdir, stat, lstat, fstat, lseek, opendir, readdir, closedir 
    - *error handling*: strerror, errno
    - *time stuff*: gettimeofday, strptime, strftime, usleep, 
    - *fd/socket handling*: select, socket, accept, listen, send, recv, bind, connect, inet_addr, setsockopt, getsockname, fcntl

**BEFORE HANDING IT IN, DO TESTS!!**:
- The included tester in project page
- Do a stress test
- Test with several programs (different languages are allowed)