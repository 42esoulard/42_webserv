##################### SERVER INCLUDES #######################
SERV_INC		= webserv.h Server.hpp ClientRequest.hpp ServerResponse.hpp Cluster.hpp Cgi.hpp
SERV_INC_DIR 	= ./
SERV_INC 		:= $(addprefix ${INC_DIR}, ${INCLUDES})
#############################################################

##################### UTILS INCLUDES ########################
UTIL_INC		= Exception.hpp utils.hpp HashTable.hpp
UTIL_INC_DIR 	= ./utils/
UTIL_INC 		:= $(addprefix ${UTIL_INC_DIR}, ${UTIL_INC})
#############################################################

######################### SRC FILES #########################
SRCS			= main.cpp Server.cpp ClientRequest.cpp Cluster.cpp
SRCS_DIR		= ./
SRCS 			:= $(addprefix ${SRC_DIR}, ${SRCS})
#############################################################

######################### SRC UTILS #########################
SRCS_UTILS		= get_next_line.cpp get_next_line_utils.cpp ft_itoa.cpp split.cpp print_map.cpp conv_utils.cpp
SRCS_UTILS_DIR	= ./utils/
SRCS_UTILS 		:= $(addprefix ${SRCS_UTILS_DIR}, ${SRCS_UTILS})
#############################################################

################# GROUPING ALL SRCS/INCLUDES ################
SRCS 			+= ${SRCS_UTILS}
OBJS			= ${SRCS:.cpp=.o}
INCLUDES		= ${SERV_INC} ${UTIL_INC}
INC_DIRS 		= $(addprefix -I,${SERV_INC_DIR} ${UTIL_INC_DIR})
#############################################################

CPP			= clang++
CFLAGS		= -Wall -Wextra -Werror -std=c++98 -fsanitize=address -fno-omit-frame-pointer
NAME		= webserv
PKGS 		= ./pkgs.sh

.cpp.o:		${SRCS} ${INCLUDES}
			${CPP} ${CFLAGS} ${INC_DIRS} -c $^ -o ${<:.cpp=.o}

all:		${NAME}

get_pkg:
	${PKGS}

${NAME}:	get_pkg ${OBJS} ${INCLUDES}
			${CPP} ${CFLAGS} ${INC_DIRS} -o ${NAME} ${OBJS}

clean :
			rm -f ${OBJS}

fclean :	clean
			rm -f ${NAME}

re :		fclean all
