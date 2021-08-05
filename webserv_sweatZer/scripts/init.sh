#!/bin/bash

RED="\033[0;31m"
MAGENTA="\033[1;35m"
GREEN="\033[0;32m"
NC="\033[0;0m"
BOLD=$(tput bold)
NB=$(tput sgr0)
CYAN="\033[1;36m"
options=("y" "n")

echo -e "\n${CYAN}    -----> Initialising the infamous SweatZer Webserv... <-----\n${NC}"

if ! [ -x "$(command -v php-cgi)" ];
then
    while true; do
        read -p $'\033[01;35mDo you wish to install php-cgi?: y/n \033[0m' yn
        case $yn in
            [Yy]* ) if [[ "$OSTYPE" == "linux-gnu"* ]] 
                    then
                        echo -e "Installing php-cgi..." &&
                        sudo apt-get install php-cgi &&
                        echo -e "php-cgi installed!"
                    else
                        echo -e "${RED}Please run this webserv in ubuntu to be able to install php-cgi and do some CGI magic!${NC}"
                    fi; 
                    break;;
            [Nn]* ) echo -e "Skipping php-cgi install - Ain't nobody got time for dat!"
                    break;;
            * ) echo -e "${RED}Please press y or n.${NC}";;
        esac
    done
else
    echo -e "\n\n${MAGENTA}${BOLD}Great, php-cgi already installed!\n${NC}"
fi

if ! [ -x "$(command -v siege)" ];
then
    while true; do
        read -p $'\033[01;35mDo you wish to install siege for testing purposes?: y/n \033[0m' yn
        case $yn in
            [Yy]* ) if [[ "$OSTYPE" == "linux-gnu"* ]] 
                    then
                        echo -e "${MAGENTA}${BOLD}Installing siege...${NC}" &&
                        sudo apt-get install -y siege &&
                        echo -e "${MAGENTA}${BOLD}siege installed!\n${NC}"
                        echo -e "${MAGENTA}${BOLD}Please uncomment the following line from /etc/siege/siegerc: \nlogfile = \$(HOME)/var/log/siege.log\n${NC}"
                        read -p $'\033[01;35m[...Press enter once you\'re done...]\033[0m'
                    else
                        echo -e "${RED}Please run this webserv in ubuntu to be able to install siege and do some stress tests on our webserv and ourselves!${NC}"
                    fi; 
                    break;;
            [Nn]* ) echo -e "Skipping siege install - Ain't nobody got time for dat!"
                    break;;
            * ) echo -e "${RED}Please press y or n.${NC}";;
        esac
    done
else
    echo -e "\n\n${MAGENTA}${BOLD}Great, siege already installed!\n${NC}"
fi

if [ ! -f ./scripts/host ]; then
    echo -e "\n\n${MAGENTA}${BOLD}Please use your mighty root power to copy and paste the following lines to /etc/hosts:\n${NC}"

    echo -e "####################################"
    echo -e "##### <webserv sweatzer hosts> #####\n"

    echo -e "127.0.0.1   sweatzer"
    echo -e "127.0.0.1   sweatzer.com"
    echo -e "127.0.0.1   www.sweatzer.com"

    echo -e "127.0.0.1   youpi"
    echo -e "127.0.0.1   youpi.com"
    echo -e "127.0.0.1   www.youpi.com\n"

    echo -e "127.0.0.1   acoudert.com"
    echo -e "127.0.0.1   www.acoudert.com\n"

    echo -e "##### </webserv sweatzer hosts> #####${NC}"
    echo -e "#####################################\n"

    read -p $'\033[01;35m[...Press enter once you\'re done...]\033[0m'
    echo -e "I hope you rocked that copy pasting to the moon\n"

    chmod 000 www/sweatzer/noperm
    chmod 000 tests/acoudert_tester/pythonTest/noperm.html

    touch ./scripts/host
    
fi