#!/bin/bash

RED="\033[0;31m"
MAGENTA="\033[1;35m"
GREEN="\033[0;32m"
NC="\033[0;0m"
BOLD=$(tput bold)
NB=$(tput sgr0)

options=("y" "n")

echo -e "\n${BOLD}Initialising the infamous SweatZer Webserv...\n"

while true; do
    read -p $'\033[01;35mDo you wish to install php-cgi (only available on Ubuntu w/ sudo rights)?: y/n \033[0m' yn
    case $yn in
        [Yy]* ) if [[ "$OSTYPE" == "linux-gnu"* ]] 
                then
                    echo -e "Installing php-cgi..." &&
                    sudo apt-get install php-cgi &&
                    echo -e "php-cgi installed!"
                else
                    echo -e "${RED}Please run this webserv in administrator ubuntu to be able to install php-cgi and do some CGI magic!${NC}"
                fi; 
                break;;
        [Nn]* ) echo -e "Skipping php-cgi install - Ain't nobody got time for dat!"
                break;;
        * ) echo -e "${RED}Please press y or n.${NC}";;
    esac
done

echo -e "\n\n${MAGENTA}${BOLD}Please use your mighty root power to copy and paste the following lines to your /etc/hosts file:\n${NC}"

echo -e "####################################"
echo -e "##### <webserv sweatzer hosts> #####\n"

echo -e "127.0.0.1   sweatzer"
echo -e "127.0.0.1   sweatzer.com"
echo -e "127.0.0.1   www.sweatzer.com"
echo -e "127.0.0.1   sweatzersupasecret.com"
echo -e "127.0.0.1   www.sweatzersupasecret.com\n"

echo -e "127.0.0.1   youpi"
echo -e "127.0.0.1   youpi.com"
echo -e "127.0.0.1   www.youpi.com\n"

echo -e "127.0.0.1   acoudert.com"
echo -e "127.0.0.1   www.acoudert.com\n"

echo -e "##### </webserv sweatzer hosts> #####${NC}"
echo -e "#####################################\n"

read -p $'\033[01;35m[...Please press enter once you\'re done...]\033[0m'
echo -e "I hope you rocked that copy pasting to the moon\n"


