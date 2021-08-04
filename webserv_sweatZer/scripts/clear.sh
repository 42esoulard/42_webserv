#!/bin/bash

MAGENTA="\033[1;35m"
NC="\033[0;0m"
BOLD=$(tput bold)
NB=$(tput sgr0)

echo -e "\n${MAGENTA}${BOLD}Deleting all test generated files...${NB}${NC}"
rm www/YoupiBanane/uploads/*
rm www/sweatzer/uploads/*
rm tests/acoudert_tester/pythonTest/putStuff/*
rm scripts/host
chmod 655 www/sweatzer/noperm
chmod 655 tests/acoudert_tester/pythonTest/noperm.html
echo -e "${MAGENTA}${BOLD}Good boi! Now won't u look at that webserv, all spick and span and bright as a new penny!${NB}${NC}\n"