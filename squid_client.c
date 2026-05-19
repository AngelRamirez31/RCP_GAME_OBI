#include "squid.h"
#include <stdio.h>
#include <stdlib.h>

void print_map(GameState *state) {
    printf("\n============== MAPA DEL PUENTE ==============\n");
    printf("Sobrevivientes: %d | Tiempo Restante: %d s\n\n", state->survivors, state->time_left);
    
    for(int b=0; b<3; b++) {
        printf("Puente %d: Inicio -> ", b);
        for(int s=0; s<10; s++) {
            int val = state->map[b*10 + s];
            if (b == state->current_bridge && s == state->current_step) {
                printf("[TU] ");
            } else if (val == 0) {
                printf("[?] "); 
            } else if (val == 1) {
                printf("[X] "); 
            } else if (val == 2) {
                printf("[P] "); 
            }
        }
        printf("-> Fin\n");
    }
    printf("=============================================\n");
}

int main(int argc, char *argv[]) {
    CLIENT *cl;
    if (argc != 2) {
        printf("Uso: %s <localhost>\n", argv[0]);
        exit(1);
    }
    
    cl = clnt_create(argv[1], SQUID_PROG, SQUID_VERS, "tcp");
    if (cl == NULL) {
        clnt_pcreateerror(argv[1]);
        exit(1);
    }
    
    int *player_id = join_game_1(NULL, cl);
    if (player_id == NULL) {
        clnt_perror(cl, "Error al conectarse");
        exit(1);
    }
    printf("Conectado! Eres el Jugador: %d\n", *player_id);
    
    int my_id = *player_id;
    int bridge = -1;
    
    while(1) {
        GameState *state = get_state_1(&my_id, cl);
        
        if (state->player_status == 1) {
            printf("\nCRACK! Pisaste un cristal debil. HAS MUERTO.\n");
            break;
        } else if (state->player_status == 2) {
            printf("\nGANASTE! Llegaste al otro lado con vida.\n");
            break;
        } else if (state->player_status == 4) {
            printf("\nTIEMPO AGOTADO! Todos mueren.\n");
            break;
        }
        
        print_map(state);
        
        if (state->player_status == 3) {
            printf(">> Hay un jugador enfrente, espera.\n");
        }
        
        MoveReq req;
        req.player_id = my_id;
        
        if (bridge == -1) {
            printf("Elige puente (0, 1 o 2): ");
            scanf("%d", &bridge);
            req.bridge_choice = bridge;
        } else {
            req.bridge_choice = bridge;
        }
        
        printf("Cuanto avanzas? (1 paso o 2 pasos): ");
        scanf("%d", &req.jump);
        
        move_1(&req, cl);
    }
    
    clnt_destroy(cl);
    return 0;
}