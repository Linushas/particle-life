#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>

#define SCREEN_HEIGHT 758
#define SCREEN_WIDTH SCREEN_HEIGHT*16/9

#define MAX_FORCE 0.1f
#define FRICTION 0.95f
#define INTERACTION_RADIUS 100.0f
#define REPEL_RADIUS 20.0f
#define FPS 60

enum Colors{WHITE, RED, GREEN, BLUE, YELLOW, PURPLE};

typedef struct vec2 {
    float x, y;
} Vec2;

typedef struct particle {
    Vec2 pos;
    Vec2 v;
    uint8_t color;
} Particle; 

typedef struct rule {
    uint8_t self;
    uint8_t target;
    float attraction;
    float repulsion;
} Rule;

void initParticle(Particle *p, uint8_t color, float x, float y) {
    p->color = color;
    p->pos.x = x;
    p->pos.y = y;
    p->v.x = 0;
    p->v.y = 0;
}

void setColor(SDL_Renderer *rend, Particle p) {
    switch(p.color) {
        case WHITE:
            SDL_SetRenderDrawColor(rend, 255, 255, 255, 255);
            break;
        case RED:
            SDL_SetRenderDrawColor(rend, 255, 0, 0, 255);
            break;
        case GREEN:
            SDL_SetRenderDrawColor(rend, 0, 255, 0, 255);
            break;
        case BLUE:
            SDL_SetRenderDrawColor(rend, 0, 0, 255, 255);
            break;
        case YELLOW:
            SDL_SetRenderDrawColor(rend, 255, 255, 0, 255);
            break;
        case PURPLE:
            SDL_SetRenderDrawColor(rend, 255, 0, 255, 255);
            break;
        default:
            SDL_SetRenderDrawColor(rend, 255, 255, 255, 255);
    }
}

float forcelaw(float distance, float attraction, float repulsion) {
    if (distance < REPEL_RADIUS) {
        return repulsion * (REPEL_RADIUS - distance) / REPEL_RADIUS;
    } else if (distance < INTERACTION_RADIUS) {
        return attraction * (1.0f - fabsf(2.0f * distance - INTERACTION_RADIUS - REPEL_RADIUS) / (INTERACTION_RADIUS - REPEL_RADIUS));
    }
    return 0;
}

float distance(Vec2 p1, Vec2 p2) {
    return sqrtf(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2));
}

int main(int argc, char *argv[]) {
    srand(time(NULL));

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        printf("error initializing SDL: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window *win = SDL_CreateWindow("Particle life simulation", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    SDL_Renderer *rend = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    int particle_count = 4096;
    Particle *particles = malloc(sizeof(Particle) * particle_count);
    for(int i = 0; i < particle_count; i++) {
        initParticle(&particles[i], rand()%6, 50 + rand()%(SCREEN_WIDTH - 100), 50 + rand()%(SCREEN_HEIGHT - 100));
    }

    // Rule rules[25] = {
    //     {WHITE, WHITE, 0.05f, -0.05f}, {WHITE, RED, 0.1f, -0.05f},
    //     {WHITE, GREEN, 0.08f, -0.05f}, {WHITE, BLUE, 0.06f, -0.05f},
    //     {WHITE, YELLOW, 0.07f, -0.05f},

    //     {RED, WHITE, 0.06f, -0.05f}, {RED, RED, 0.15f, -0.1f},
    //     {RED, GREEN, 0.12f, -0.05f}, {RED, BLUE, -0.1f, -0.05f},
    //     {RED, YELLOW, 0.09f, -0.05f},

    //     {GREEN, WHITE, 0.08f, -0.05f}, {GREEN, RED, 0.12f, -0.05f},
    //     {GREEN, GREEN, 0.15f, -0.1f}, {GREEN, BLUE, 0.1f, -0.05f},
    //     {GREEN, YELLOW, 0.11f, -0.05f},

    //     {BLUE, WHITE, 0.06f, -0.05f}, {BLUE, RED, -0.1f, -0.05f},
    //     {BLUE, GREEN, 0.1f, -0.05f}, {BLUE, BLUE, 0.15f, -0.1f},
    //     {BLUE, YELLOW, 0.07f, -0.05f},

    //     {YELLOW, WHITE, 0.07f, -0.05f}, {YELLOW, RED, 0.09f, -0.05f},
    //     {YELLOW, GREEN, 0.11f, -0.05f}, {YELLOW, BLUE, 0.07f, -0.05f},
    //     {YELLOW, YELLOW, 0.15f, -0.1f}
    // };

//     Rule rules[25] = {
//     {RED, RED, 0.2f, -0.1f}, {RED, BLUE, 0.1f, -0.05f},
//     {RED, GREEN, -0.1f, -0.05f}, {RED, YELLOW, 0.05f, -0.02f},
//     {BLUE, RED, 0.1f, -0.05f}, {BLUE, BLUE, 0.2f, -0.1f},
//     {BLUE, GREEN, 0.1f, -0.05f}, {BLUE, YELLOW, -0.1f, -0.05f},
//     {GREEN, RED, -0.1f, -0.05f}, {GREEN, BLUE, 0.1f, -0.05f},
//     {GREEN, GREEN, 0.2f, -0.1f}, {GREEN, YELLOW, 0.1f, -0.05f},
//     {YELLOW, RED, 0.05f, -0.02f}, {YELLOW, BLUE, -0.1f, -0.05f},
//     {YELLOW, GREEN, 0.1f, -0.05f}, {YELLOW, YELLOW, 0.2f, -0.1f},
//     {WHITE, RED, 0.05f, -0.02f}, {WHITE, BLUE, 0.05f, -0.02f},
//     {WHITE, GREEN, 0.05f, -0.02f}, {WHITE, YELLOW, 0.05f, -0.02f},
//     {WHITE, WHITE, 0.1f, -0.05f},
//     {RED, WHITE, 0.05f, -0.02f}, {BLUE, WHITE, 0.05f, -0.02f},
//     {GREEN, WHITE, 0.05f, -0.02f}, {YELLOW, WHITE, 0.05f, -0.02f}
// };

////COOOL///////////////////
// Rule rules[25] = {
//     {RED, RED, 0.1f, -0.05f}, {RED, BLUE, 0.15f, -0.01f},
//     {RED, GREEN, 0.0f, 0.0f}, {RED, YELLOW, 0.0f, 0.0f},
//     {RED, WHITE, -0.1f, -0.05f},
//     {BLUE, BLUE, 0.1f, -0.05f}, {BLUE, RED, -0.15f, -0.01f},
//     {BLUE, GREEN, 0.15f, -0.01f}, {BLUE, YELLOW, 0.0f, 0.0f},
//     {BLUE, WHITE, -0.1f, -0.05f},
//     {GREEN, GREEN, 0.1f, -0.05f}, {GREEN, RED, 0.0f, 0.0f},
//     {GREEN, BLUE, -0.15f, -0.01f}, {GREEN, YELLOW, 0.15f, -0.01f},
//     {GREEN, WHITE, -0.1f, -0.05f},
//     {YELLOW, YELLOW, 0.1f, -0.05f}, {YELLOW, RED, 0.0f, 0.0f},
//     {YELLOW, BLUE, 0.0f, 0.0f}, {YELLOW, GREEN, -0.15f, -0.01f},
//     {YELLOW, WHITE, -0.1f, -0.05f},
//     {WHITE, WHITE, 0.2f, -0.1f}, {WHITE, RED, 0.05f, -0.02f},
//     {WHITE, BLUE, 0.05f, -0.02f}, {WHITE, GREEN, 0.05f, -0.02f},
//     {WHITE, YELLOW, 0.05f, -0.02f}
// };


// Rule rules[25] = {
//     {RED, RED, 0.15f, -0.1f}, {RED, BLUE, 0.1f, -0.05f},
//     {RED, GREEN, -0.1f, -0.05f}, {RED, YELLOW, 0.05f, -0.02f},
//     {RED, WHITE, -0.05f, -0.02f},
//     {BLUE, BLUE, 0.15f, -0.1f}, {BLUE, RED, -0.1f, -0.05f},
//     {BLUE, GREEN, 0.1f, -0.05f}, {BLUE, YELLOW, -0.1f, -0.05f},
//     {BLUE, WHITE, 0.05f, -0.02f},
//     {GREEN, GREEN, 0.15f, -0.1f}, {GREEN, RED, 0.1f, -0.05f},
//     {GREEN, BLUE, -0.1f, -0.05f}, {GREEN, YELLOW, 0.1f, -0.05f},
//     {GREEN, WHITE, -0.05f, -0.02f},
//     {YELLOW, YELLOW, 0.15f, -0.1f}, {YELLOW, RED, -0.1f, -0.05f},
//     {YELLOW, BLUE, 0.1f, -0.05f}, {YELLOW, GREEN, -0.1f, -0.05f},
//     {YELLOW, WHITE, 0.05f, -0.02f},
//     {WHITE, WHITE, 0.1f, -0.05f}, {WHITE, RED, 0.05f, -0.02f},
//     {WHITE, BLUE, -0.05f, -0.02f}, {WHITE, GREEN, 0.05f, -0.02f},
//     {WHITE, YELLOW, -0.05f, -0.02f}
// };

Rule rules[36] = {
    {RED, RED, 0.12f, -0.06f}, {RED, BLUE, 0.18f, -0.01f},
    {RED, GREEN, 0.0f, 0.0f}, {RED, YELLOW, 0.0f, 0.0f},
    {RED, WHITE, -0.12f, -0.06f},
    {BLUE, BLUE, 0.12f, -0.06f}, {BLUE, RED, -0.18f, -0.01f},
    {BLUE, GREEN, 0.18f, -0.01f}, {BLUE, YELLOW, 0.0f, 0.0f},
    {BLUE, WHITE, -0.12f, -0.06f},
    {GREEN, GREEN, 0.12f, -0.06f}, {GREEN, RED, 0.0f, 0.0f},
    {GREEN, BLUE, -0.18f, -0.01f}, {GREEN, YELLOW, 0.18f, -0.01f},
    {GREEN, WHITE, -0.12f, -0.06f},
    {YELLOW, YELLOW, 0.12f, -0.06f}, {YELLOW, RED, 0.0f, 0.0f},
    {YELLOW, BLUE, 0.0f, 0.0f}, {YELLOW, GREEN, -0.18f, -0.01f},
    {YELLOW, WHITE, -0.12f, -0.06f},
    {WHITE, WHITE, 0.22f, -0.11f}, {WHITE, RED, 0.06f, -0.03f},
    {WHITE, BLUE, 0.06f, -0.03f}, {WHITE, GREEN, 0.06f, -0.03f},
    {WHITE, YELLOW, 0.06f, -0.03f}, {PURPLE, YELLOW, 0.0f, -0.04f}, 
    {PURPLE, WHITE, 0.0f, -0.04f}, {PURPLE, RED, 0.0f, -0.04f}, 
    {PURPLE, BLUE, 0.0f, -0.04f}, {PURPLE, GREEN, 0.0f, -0.04f}
};


    int running = 1;
    int paused = 0;
    Uint32 frameStart, frameTime;

    while(running) {
        frameStart = SDL_GetTicks();

        SDL_Event event;
        while(SDL_PollEvent(&event)) {
            switch(event.type) {
                case SDL_QUIT: 
                    running = 0; 
                    break;
                case SDL_KEYDOWN: 
                    if(event.key.keysym.sym == SDLK_o) {}
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    break;
            }
        }

        if (!paused) {
            for (int i = 0; i < particle_count; i++) {
                Vec2 totalForce = {0, 0};
                for (int j = 0; j < particle_count; j++) {
                    if (i == j) continue;
                    float dist = distance(particles[i].pos, particles[j].pos);
                    if (dist < INTERACTION_RADIUS) {
                        for (int k = 0; k < 30; k++) {
                            if (particles[i].color == rules[k].self && particles[j].color == rules[k].target) {
                                Vec2 direction = {particles[j].pos.x - particles[i].pos.x, particles[j].pos.y - particles[i].pos.y};
                                float force = forcelaw(dist, rules[k].attraction, rules[k].repulsion);
                                totalForce.x += force * direction.x / dist;
                                totalForce.y += force * direction.y / dist;
                            }
                        }
                    }
                }
                
                float forceMagnitude = sqrt(totalForce.x * totalForce.x + totalForce.y * totalForce.y);
                if (forceMagnitude > MAX_FORCE) {
                    totalForce.x = (totalForce.x / forceMagnitude) * MAX_FORCE;
                    totalForce.y = (totalForce.y / forceMagnitude) * MAX_FORCE;
                }
                
                particles[i].v.x += totalForce.x;
                particles[i].v.y += totalForce.y;
                
                particles[i].v.x *= FRICTION;
                particles[i].v.y *= FRICTION;
                
                particles[i].pos.x += particles[i].v.x;
                particles[i].pos.y += particles[i].v.y;
                
                particles[i].pos.x = fmodf(particles[i].pos.x + SCREEN_WIDTH, SCREEN_WIDTH);
                particles[i].pos.y = fmodf(particles[i].pos.y + SCREEN_HEIGHT, SCREEN_HEIGHT);
            }
        }

        SDL_SetRenderDrawColor(rend, 0, 0, 0, 255);
        SDL_RenderClear(rend);

        for(int i = 0; i < particle_count; i++) {
            setColor(rend, particles[i]);
            SDL_RenderDrawPoint(rend, (int)particles[i].pos.x, (int)particles[i].pos.y);
        }

        SDL_RenderPresent(rend);
        
        frameTime = SDL_GetTicks() - frameStart;
        if (frameTime < 1000/FPS) {
            SDL_Delay(1000/FPS - frameTime);
        }
    }

    free(particles);
    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(win);
    SDL_Quit();

    return 0;
}
