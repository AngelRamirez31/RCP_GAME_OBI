struct MoveReq {
    int player_id;
    int bridge_choice;
    int jump;
};

struct GameState {
    int player_status;
    int map[30];
    int time_left;
    int survivors;
    int current_bridge;
    int current_step;
};

program SQUID_PROG {
    version SQUID_VERS {
        int JOIN_GAME(void) = 1;
        GameState MOVE(MoveReq) = 2;
        GameState GET_STATE(int) = 3;
    } = 1;
} = 0x31230000;