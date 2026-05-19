#include "squid.h"
#include <pthread.h>
#include <stdlib.h>
#include <time.h>

#define NUM_BRIDGES 3
#define NUM_STEPS 10
#define MAX_PLAYERS 100

int true_map[NUM_BRIDGES][NUM_STEPS];
int public_map[NUM_BRIDGES][NUM_STEPS];

struct Player {
    int active;
    int bridge;
    int step;
    int status; 
} players[MAX_PLAYERS];

int total_players = 0;
int survivors = 0;
time_t start_time;
int max_time = 60;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void init_game() {
    static int initialized = 0;
    if (!initialized) {
        srand(time(NULL));
        for(int b=0; b<NUM_BRIDGES; b++) {
            for(int s=0; s<NUM_STEPS; s++) {
                true_map[b][s] = rand() % 2; 
                public_map[b][s] = 0;        
            }
        }
        start_time = time(NULL);
        initialized = 1;
    }
}

int * join_game_1_svc(void *argp, struct svc_req *rqstp) {
    static int player_id;
    pthread_mutex_lock(&lock);
    init_game();
    player_id = ++total_players;
    players[player_id].active = 1;
    players[player_id].bridge = -1;
    players[player_id].step = -1;
    players[player_id].status = 0;
    pthread_mutex_unlock(&lock);
    return &player_id;
}

GameState * get_state_1_svc(int *argp, struct svc_req *rqstp) {
    static GameState state;
    int pid = *argp;
    
    pthread_mutex_lock(&lock);
    int elapsed = time(NULL) - start_time;
    state.time_left = max_time - elapsed;
    
    if (state.time_left <= 0 && players[pid].status == 0) {
        players[pid].status = 4;
    }
    
    state.player_status = players[pid].status;
    state.survivors = survivors;
    state.current_bridge = players[pid].bridge;
    state.current_step = players[pid].step;
    
    int idx = 0;
    for(int b=0; b<NUM_BRIDGES; b++){
        for(int s=0; s<NUM_STEPS; s++){
            state.map[idx++] = public_map[b][s];
        }
    }
    pthread_mutex_unlock(&lock);
    return &state;
}

GameState * move_1_svc(MoveReq *req, struct svc_req *rqstp) {
    static GameState state;
    int pid = req->player_id;
    
    pthread_mutex_lock(&lock);
    int elapsed = time(NULL) - start_time;
    
    if (max_time - elapsed <= 0) {
        players[pid].status = 4;
        goto fill_state;
    }
    if (players[pid].status != 0 && players[pid].status != 3) goto fill_state;

    int bridge = players[pid].bridge;
    if (bridge == -1) {
        bridge = req->bridge_choice;
        players[pid].bridge = bridge;
    }

    int current_step = players[pid].step;
    int target_step = current_step + req->jump;

    if (target_step >= NUM_STEPS) {
        if (current_step >= 0) public_map[bridge][current_step] = 0; 
        players[pid].step = target_step;
        players[pid].status = 2; 
        survivors++;
    } else {
        int collision = 0;
        for(int i=1; i<=total_players; i++) {
            if (i != pid && players[i].active && players[i].status == 0) {
                if (players[i].bridge == bridge && players[i].step == target_step) {
                    collision = 1; break;
                }
            }
        }
        
        if (collision) {
            players[pid].status = 3; 
            goto fill_state;
        }

        players[pid].status = 0; 
        if (current_step >= 0) public_map[bridge][current_step] = 0; 
        
        if (true_map[bridge][target_step] == 1) { 
            public_map[bridge][target_step] = 1; 
            players[pid].status = 1; 
            players[pid].step = target_step;
        } else {
            public_map[bridge][target_step] = 2; 
            players[pid].step = target_step;
        }
    }

fill_state:
    state.time_left = max_time - elapsed;
    state.player_status = players[pid].status;
    state.survivors = survivors;
    state.current_bridge = players[pid].bridge;
    state.current_step = players[pid].step;
    int idx = 0;
    for(int b=0; b<NUM_BRIDGES; b++)
        for(int s=0; s<NUM_STEPS; s++)
            state.map[idx++] = public_map[b][s];
            
    pthread_mutex_unlock(&lock);
    return &state;
}