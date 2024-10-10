#ifndef GAMEJAM2024_CORE_H
#define GAMEJAM2024_CORE_H

    /***************************************************************
                         Public Core Functions
    ***************************************************************/

    // Use this to standardize player colors
    #define PLAYERCOLOR_1  0xFF0000
    #define PLAYERCOLOR_2  0x00FF00
    #define PLAYERCOLOR_3  0x0000FF
    #define PLAYERCOLOR_4  0xFFFF00

    // Player number definition
    typedef enum {
        PLAYER_1,
        PLAYER_2,
        PLAYER_3,
        PLAYER_4,
    } PlyNum;

    // AI difficulty definition
    typedef enum {
        DIFF_EASY,
        DIFF_MEDIUM,
        DIFF_HARD,
    } AiDiff;

    /*==============================
        core_get_playercount
        Get the number of human players
        @return The number of players
    ==============================*/
    uint32_t core_get_playercount();

    /*==============================
        core_get_playercontroller
        Get the controller port of this player.
        Because player 1's controller might not be plugged 
        into port number 1.
        @param  The player we want
        @return The controller port
    ==============================*/
    joypad_port_t core_get_playercontroller(PlyNum ply);

    /*==============================
        core_get_aidifficulty
        Gets the current AI difficulty
        @return The AI difficulty
    ==============================*/
    AiDiff core_get_aidifficulty();

    /*==============================
        core_get_subtick
        Gets the current subtick. Use this to help smooth
        movements in your draw loop
        @return The current subtick
    ==============================*/
    double core_get_subtick();

    /*==============================
        core_set_winner
        Set the winner of the minigame. You can call this
        multiple times to set multiple winners.
        @param  The winning player
    ==============================*/
    void core_set_winner(PlyNum ply);

    
    /***************************************************************
                        Internal Core Functions
                  Do not use anything below this line
    ***************************************************************/

    #define TICKRATE   30
    #define DELTATIME  (1.0f/(double)TICKRATE)

    #define MAXPLAYERS  4

    void core_set_playercount(uint32_t playercount);
    void core_set_subtick(double subtick);
    void core_reset_winners();

#endif