#ifndef GAME_CONSTS_HEADER
#define GAME_CONSTS_HEADER

#define MAX_USER_NAME_LENGTH 31 // including \0
#define MAX_NUM_OF_PLAYERS 4
#define MAX_USER_NAME_MESSAGE_LENGTH (10+MAX_USER_NAME_LENGTH) //username=<username>\n
#define MAX_WELCOME_MESSAGE_LENGTH (MAX_USER_NAME_LENGTH+20) // <username> you game piece is <game piece>\n
#define MAX_PLAYER_JOINED_MESSAGE_LENGTH (35+MAX_USER_NAME_LENGTH) // New player joined the game: <username> <game piece>\n
#define MAX_PLAYERS_LIST_MESSAGE_LENGTH (MAX_NUM_OF_PLAYERS*(MAX_USER_NAME_LENGTH + 4) + 3) // <user1>-<game_piece>, <user2>-<game_piece>, <user3>-<game_piece>, <user4>-<game_piece>.\n
#define MAX_USER_NOT_EXIST_MESSAGE_LENGTH (MAX_USER_NAME_LENGTH+35) // User <username> doesn't exist in the game.\n
#define MAX_PLAYER_TURN_MESSAGE_LENGTH (MAX_USER_NAME_LENGTH+30) // It is now <username>'s turn to play.\n

#define MAX_MESSAGE_LENGTH 80
#define MAX_PRIVATE_MESSAGE_LENGTH (MAX_USER_NAME_LENGTH+MAX_MESSAGE_LENGTH+30) // Private message from <username>: <message>
#define MAX_BROADCAST_MESSAGE_LENGTH (MAX_USER_NAME_LENGTH+MAX_MESSAGE_LENGTH+20) // Broadcast from <username>: <message>

#define MAX_COMMAND_LENGTH 200 // longer than the max command(players message response) just to be on the safe side..

#define MAX_SIZE_OF_USERNAME_MESSAGE 41
#define MAX_USER_MESSAGE_LENGTH 80

#define COMMAND_MESSAGE "message"
#define COMMAND_PLAY "play"
#define COMMAND_PLAYERS "players"
#define COMMAND_BROADCAST "broadcast"

#define CONNETION_REFUSED_MSG "Connection to server refused. Exiting."

#define COMMAND_MESSAGE_MIN_ARGS 3
#define COMMAND_BROADCAST_MIN_ARGS 2

#endif