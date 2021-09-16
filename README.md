# 42_webserv
42's webserv, team rturcey-esoulard! [05/2021-08/2021] [Ubuntu18]

**GOAL**: write a HTTP server in C++ 98 (http 1.1)
When in doubt, compare program behaviour with nginx.

**LINKS**:
- [Git](https://github.com/42esoulard/42_webserv)
- [Subject](https://cdn.intra.42.fr/pdf/pdf/19749/en.subject.pdf)

**RESOURCES**: *difficulty rated 1 to 3, 1 being pretty understandable, 3 is as obscure as the deepest darkest pits of hell. 2 means you're not gonna have the best time but it needs to be done, kinda like a very awkward family dinner*
- **HTTP: BASICS**
  - [HTTP wiki {1}](https://en.wikipedia.org/wiki/Hypertext_Transfer_Protocol): details HTTP's evolution, and main technical characteristics.
  - [HTTP BASIC TUTORIAL {1}](https://code.tutsplus.com/tutorials/a-beginners-guide-to-http-and-rest--net-16340): great intro to HTTP for us n00bs. Details requests, headers, methods (GET, PUT/POST, DELETE), response codes, and best practice. Also gives an example of a short server in PHP.
  - [HTTP AND CGI BASIC TUTORIAL {1}](https://www.garshol.priv.no/download/text/http-tut.html): a pretty thorough beginner guide. Also contains all of the above plus details on CGI, caching and cookies, and a FAQ.
  - [HTTP methods and thei corresponding headers {1}](https://www.tutorialspoint.com/http/http_methods.htm)
  - **TUTORIALS: SETTING UP A HTTP SERVER**
    - [HTTP SERVER WALKTHROUGH {1}](https://medium.com/from-the-scratch/http-server-what-do-you-need-to-know-to-build-a-simple-http-server-from-scratch-d1ef8945e4fa) : step-by-step tutorial on how to build a http server from scratch in C!!! (see [/TUTO_MEDIUM](https://github.com/42esoulard/42_webserv/tree/main/TUTO_MEDIUM))
    - [STACK EXCHANGE ON SETTING UP HTTP SERVER](https://softwareengineering.stackexchange.com/questions/200821/how-to-write-a-http-server) : just to boost our ego, someone says they did this as a 3rd
    year engineering student. Also, they do a pretty good summing up of what the exercise implies.
    - [PYTHON HTTP SERVER TUTORIAL {1}](https://www.afternerd.com/blog/python-http-server/) : simply explains what is a webserver (+ basic python implementation).
- **HTTP: MORE ADVANCED RESOURCES**
  - [HTTP CS COURSE {2}](https://www.tutorialspoint.com/http/index.htm): much more in depth and condensed guide, seems to contain most info we should need. Will be great for reference.
  - **RFC7230 through 7235: HTTP/1.1 GUIDELINES**
    - [RFC7230: *Message Syntax and Routing* {3}](https://tools.ietf.org/html/rfc7230)
    - [RFC7231: *Semantics and Content* {3}](https://tools.ietf.org/html/rfc7231)
    - [RFC7232: *Conditional Requests* {3}](https://tools.ietf.org/html/rfc7232)
    - [RFC7233: *Range Requests* {3}](https://tools.ietf.org/html/rfc7233)
    - [RFC7234: *Caching* {3}](https://tools.ietf.org/html/rfc7234)
    - [RFC7235: *Authentication* {3}](https://tools.ietf.org/html/rfc7235)
    - [RFCs explained by our new friend Stephane](https://www.bortzmeyer.org/7230.html)

- **OTHER USEFUL RESOURCES**
  - ["Computer Networking: A Top-Down Approach 7th Edition" - 856p](https://www.ucg.ac.me/skladiste/blog_44233/objava_64433/fajlovi/Computer%20Networking%20_%20A%20Top%20Down%20Approach,%207th,%20converted.pdf): a huge book on networks.

**BEFORE STARTING, DO TESTS WITH THESE TO SEE WHAT OUR SERVER SHOULD DO**:
- [telnet](https://www.howtoforge.com/how-to-install-and-use-telnet-on-ubuntu-1804/): /!\ telnet is an unencrypted and therefore insecure protocol.
- [nginx](http://nginx.org/en/docs/beginners_guide.html#proxy): nginx is said to be pretty secure but
steps can be taken in its configuration to secure it further.

**KEY NOTIONS**:
- *Hypertext Transfer Protocol (HTTP)* : an application protocol for distributed, collaborative, hypermedia information systems. Was developed to facilitate hypertext and the World Wide Web. Communication between client and server uses HTTP. HTTP also includes ways of receiving content from clients. This feature is used for submitting web forms, including uploading of files.
- *HTTP session* : An HTTP session is a sequence of network request–response transactions. An HTTP client initiates a request by establishing a Transmission Control Protocol (TCP) connection to a particular port on a server (typically port 80, occasionally port 8080; see List of TCP and UDP port numbers). An HTTP server listening on that port waits for a client's request message. Upon receiving the request, the server sends back a status line, such as "HTTP/1.1 200 OK", and a message of its own. The body of this message is typically the requested resource, although an error message or other information may also be returned. With HTTP/1.1, a connection could be reused for more than one request. The body can contain data in any format that server and client both know how to handle: plain text, pictures, HTML, XML..
- [*HTTP header*](https://en.wikipedia.org/wiki/Hypertext_Transfer_Protocol#Message_format) : the format of the messages exchanged between client and server. See [HTTP_header_walkthrough.txt](https://github.com/42esoulard/42_webserv/blob/main/HTTP_header_walkthrough.txt)
- *World Wide Web* : where hypertext documents include hyperlinks to other resources that the user can easily access.
- *Web server* : stores, processes and delivers web pages to clients. It is a regular network application that is listening on a specific port (by default, 80, for https 443). The HTTP request must be addressed to THAT port.
- *HTML* : Pages delivered are most frequently HTML documents, which may include images, style sheets and scripts in addition to the text content.
- *User agent* :  commonly a web browser or web crawler. Initiates communication by
making a request for a specific resource using HTTP and the server responds with the
content of that resource or an error message if unable to do so. The resource is typically a real file on the server’s secondary storage, but this is not necessarily the case and depends on how the web server is implemented.
- *cURL*: very popular HTTP client library. Includes both a standalone command line program, and a library that can be used by various programming languages.
- [*CGI: Common Gateway Interface*](https://en.wikipedia.org/wiki/Common_Gateway_Interface): interface specification that enables web servers to execute an external program, typically to process user requests. A typical use case occurs when a Web user submits a Web form on a web page that uses CGI. The form's data is sent to the Web server within an HTTP request with a URL denoting a CGI script. The Web server then launches the CGI script in a new computer process, passing the form data to it. The output of the CGI script, usually in the form of HTML, is returned by the script to the Web server, and the server relays it back to the browser as its response to the browser's request. [For more info on CGI](https://help.superhosting.bg/en/cgi-common-gateway-interface-fastcgi.html)
- *Socket* : mechanism that most popular operating systems provide to give programs access to the network. It allows messages to be sent and received between applications (unrelated processes) on different networked machines.
- [*What's the deal with select()*](https://www.gnu.org/software/libc/manual/html_node/Server-Example.html)


**PROGRAM ARGUMENTS**
- A config file in argument (or use a default path). From this we will initialize all our cool variables (port, server name, etc). Inspired from the "server" part of nginx configuration file (TEST ALL OF THIS IN THE VM, needs sudo rights):
  - [NGINX basic config](https://docs.nginx.com/nginx/admin-guide/basic-functionality/managing-configuration-files/)
  - [NGINX full example](https://www.nginx.com/resources/wiki/start/topics/examples/full/)
  - [NGINX basic tutorial](https://phoenixnap.com/kb/nginx-start-stop-restart)
  - [NGINX beginner's guide](https://nginx.org/en/docs/beginners_guide.html)
  - [NGINX configuration Bible](https://docs.nginx.com/nginx/admin-guide/web-server/)

**MINDMAPS**:
- [Main modules of our program](https://app.mindmup.com/map/_free/2021/04/8cefb3c0987f11eb9c427911d58b5ce6)(careful, each time we save it triggers a new link that we need to update here, otherwise the update will be lost!)
- of more precise modules (TBD)

**TEAM NAMING FORMATS CONVENTION (TBC)**:
- function_name()
- variable_name()
- _private_variable
- ClassName
- s_struct t_struct

**ALLOWED TOOLS**:
- Everything in C and C++ 98 but no external library.


**RESOURCES ON SPECIFIC ISSUES**
- [Why we cannot use getline() to read requests](https://stackoverflow.com/questions/1589168/getline-over-a-socket)

**BEFORE HANDING IT IN, DO TESTS!!**:
- The included tester in project page
- Compare returns with nginx
- [Do a stress test](https://github.com/tsenart/vegeta)
- Test with several programs (different languages are allowed)
- [acoudert tester](https://github.com/acoudert/webserv_tester)


**[GIT BRANCHING](https://alexgirard.com/git-book/basic/branches-et-merges/)**

