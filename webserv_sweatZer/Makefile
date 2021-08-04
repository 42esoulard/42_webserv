#############################################################
################################################################
#############################################################

##################### SERVER INCLUDES #######################
SERV_INC		= webserv.h Server.hpp Cluster.hpp
SERV_INC_DIR 	= ./server/
SERV_INC 		:= $(addprefix ${SERV_INC_DIR}, ${SERV_INC})
#############################################################

##################### CLIENT INCLUDES #######################
CLI_INC			= ClientRequest.hpp 
CLI_INC_DIR 	= ./client/
CLI_INC 		:= $(addprefix ${CLI_INC_DIR}, ${CLI_INC})
#############################################################

####################### RESP INCLUDES #######################
RESP_INC		= ServerResponse.hpp
RESP_INC_DIR 	= ./response/
RESP_INC 		:= $(addprefix ${RESP_INC_DIR}, ${RESP_INC})
#############################################################

####################### CGI INCLUDES ########################
CGI_INC			= Cgi.hpp
CGI_INC_DIR 	= ./cgi/
CGI_INC 		:= $(addprefix ${CGI_INC_DIR}, ${CGI_INC})
#############################################################

##################### UTILS INCLUDES ########################
UTIL_INC		= Exception.hpp utils.hpp HashTable.hpp
UTIL_INC_DIR 	= ./utils/
UTIL_INC 		:= $(addprefix ${UTIL_INC_DIR}, ${UTIL_INC})
#############################################################

#############################################################
################################################################
#############################################################

######################### INC FILES #########################
INCLUDES		= 	${SERV_INC} ${CLI_INC} ${RESP_INC} ${CGI_INC} \
					${UTIL_INC}
INC_DIRS 		= 	$(addprefix -I,${SERV_INC_DIR} ${CLI_INC_DIR} \
					${RESP_INC_DIR} ${CGI_INC_DIR} ${UTIL_INC_DIR})
#############################################################

######################### SRC FILES #########################
SERV_SRC		= main.cpp Cluster.cpp Server.cpp
SERV_SRC_DIR	= ./server/

CLI_SRC			= ClientRequest.cpp
CLI_SRC_DIR		= ./client/

RESP_SRC		= ServerResponse.cpp
RESP_SRC_DIR	= ./response/

CGI_SRC			= Cgi.cpp
CGI_SRC_DIR		= ./cgi/

UTIL_SRC		= 	get_next_line.cpp get_next_line_utils.cpp \
					ft_itoa.cpp split.cpp print_map.cpp conv_utils.cpp \
					utils.cpp encode.cpp
UTIL_SRC_DIR	= ./utils/

SRCS 			= 	$(addprefix ${SERV_SRC_DIR}, ${SERV_SRC}) \
					$(addprefix ${CLI_SRC_DIR}, ${CLI_SRC}) \
					$(addprefix ${RESP_SRC_DIR}, ${RESP_SRC}) \
					$(addprefix ${CGI_SRC_DIR}, ${CGI_SRC}) \
					$(addprefix ${UTIL_SRC_DIR}, ${UTIL_SRC}) 

OBJS			= ${SRCS:.cpp=.o}

#############################################################
################################################################
#############################################################

CPP			= clang++
CFLAGS		= -Wall -Wextra -Werror -std=c++98
NAME		= webserv
INIT 		= ./scripts/init.sh
CLEAR 		= ./scripts/clear.sh

.cpp.o:		${SRCS} ${INCLUDES}
			${CPP} ${CFLAGS} ${INC_DIRS} -c $^ -o ${<:.cpp=.o}

all:		${NAME}
			@echo "\033[1;36m		-----> ./webserv [file.conf] <-----\n\033[0m"

init:
			${INIT}

${NAME}:	init ${OBJS} ${INCLUDES}
			${CPP} ${CFLAGS} ${INC_DIRS} -o ${NAME} ${OBJS}
			@echo "\033[1;36m	-----> SweatZer webserv ready to rock your world! <-----\n\033[0m"

clear_files:
			${CLEAR}
			@echo "\033[1;36m		-----> Test files cleared! <-----\n\033[0m"

clean :		
			rm -f ${OBJS}
			@echo "\n\033[1;36m	       -----> Object files cleared! <-----\n\033[0m"

fclean :	clean clear_files
			rm -f ${NAME}
			@echo "\n\033[1;36m	-----> Webserv atomized! So long, Webserv! <-----\n\033[0m"

re :		fclean all
