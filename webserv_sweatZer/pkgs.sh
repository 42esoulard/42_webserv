#!/bin/bash

if [[ "$OSTYPE" == "linux-gnu"* ]] 
then
    echo "Installing php-cgi..." &&
    sudo apt-get install php-cgi &&
    echo "php-cgi installed!"
else
    echo "Please run this webserv in administrator ubuntu to be able to install php-cgi and do some CGI magic!"
fi