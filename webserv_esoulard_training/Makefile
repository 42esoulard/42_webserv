##################### INCLUDES FILES ########################
INCLUDES	= webserv.h Exception.hpp
INC_DIR		= ./
INCLUDES 	:= $(addprefix ${INC_DIR}, ${INCLUDES})
#############################################################


######################### SRC FILES #########################
SRCS		= server.cpp
SRC_DIR		= ./
SRCS 		:= $(addprefix ${SRC_DIR}, ${SRCS})

# INC_TEST 	= tests.hpp
# INC_TDIR	= tests/
# INC_TEST	:= $(addprefix ${INC_TDIR}, ${INC_TEST})
#############################################################

################# GROUPING ALL SRCS/INCLUDES ################
# SRCS 			+= ${SRC_TEST}
OBJS			= ${SRCS:.cpp=.o}
# INCLUDES		= ${INC_CONT} ${INC_UTILS} ${INC_TEST}
# INC_DIRS 		= $(addprefix -I,${INC_UDIR} ${INC_TDIR} ${INC_CDIR})
#############################################################

CPP			= clang++
CFLAGS		= -Wall -Wextra -Werror -std=c++98 #-fsanitize=address -fno-omit-frame-pointer 
NAME		= webserv


.cpp.o:		${SRCS} ${INCLUDES}
			${CPP} ${CFLAGS} ${INC_DIRS} -c $^ -o ${<:.cpp=.o}

all:		${NAME}

${NAME}:	${OBJS} ${INCLUDES}
			${CPP} ${CFLAGS} ${INC_DIRS} -o ${NAME} ${OBJS}

clean :
			rm -f ${OBJS}

fclean :	clean
			rm -f ${NAME} 

re :		fclean all