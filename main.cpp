#include <SDL.h>
#include <SDL2/SDL.h>
#include <iostream>
#include <cmath>


class Gfx {
public:
  // Contructor which initialize the parameters.
  Gfx(int height_, int width_) : height(height_), width(width_) {
    SDL_Init(SDL_INIT_EVERYTHING); // Initializing SDL as Video
    this->window = SDL_CreateWindow("SDL Window", SDL_WINDOWPOS_UNDEFINED,
                                    SDL_WINDOWPOS_UNDEFINED, width, height, 0);
    this->renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0); // setting draw color
    SDL_RenderClear(renderer);   // Clear the newly created window
    SDL_RenderPresent(renderer); // Reflects the changes done in the
                                 //  window.
  }

  // Destructor
  ~Gfx() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
  }

  int height;                    // Height of the window
  int width;                     // Width of the window
  SDL_Renderer *renderer = NULL; // Pointer for the renderer
  SDL_Window *window = NULL;     // Pointer for the window
};

struct GameRect {
  //int color = 0xFFFFFF;
  int red = 255;
  int green = 255;
  int blue = 255;
  int alpha = 255;
  

  float x;
  float y;
  float w;
  float h;

  SDL_Rect rect;
};

struct GameState {
  bool running;
  GameRect paddle;

  GameRect ball;

  float speed = 0.5f;
  // normalized vector
  float dx = 1.0/sqrt(2.0);
  float dy = 1.0/sqrt(2.0);

  GameState(bool startRunning): running(startRunning) {}
  GameState(): running(true) {}
  
  void init() {
    this->paddle.x = 250;
    this->paddle.y = 550;
    this->paddle.w = 200;
    this->paddle.h = 50;

    this->ball.x = 250;
    this->ball.y = 200;
    this->ball.w = 25;
    this->ball.h = 25;
  }
  ~GameState(){}
};

void handleKeyPress(SDL_Event& event, GameState* state) {
  std::cout << "Key " << (char)event.key.keysym.sym << " "
            << ((event.key.state == SDL_PRESSED) ? "pressed" : "released")
            << std::endl;


  switch(event.key.keysym.sym) {
    
    case SDLK_ESCAPE:
      state->running = false;
      break;
    
    case SDLK_LEFT:
      std::cout << "Left Clicked" << std::endl;
      state->paddle.x += (-10);
      break;
    
    case SDLK_RIGHT:
      std::cout << "Right Clicked" << std::endl;
      state->paddle.x += (10);
      break;

    default:
      break;
  }
  if(event.key.keysym.sym == SDLK_ESCAPE) {
    state->running = false;
  }
}


void DrawGameRect(Gfx* gfx, GameRect* rect) {
  SDL_SetRenderDrawColor(gfx->renderer, rect->red, rect->green, rect->blue, rect->alpha);
  SDL_Rect sdlRect;
  sdlRect.x = std::round(rect->x);
  sdlRect.y = std::round(rect->y);
  sdlRect.w = std::round(rect->w);
  sdlRect.h = std::round(rect->h);
  SDL_RenderFillRect(gfx->renderer, &sdlRect); 
}

int main(int argc, char *argv[]) {

  Gfx gfx(600, 800);

  // game state 
  GameState state;
  state.init();


  SDL_Event event; // Event variable

  bool running = true;
  // Below while loop checks if the window has terminated using close in the
  //  corner.

  SDL_SetRenderDrawColor(gfx.renderer, 0, 0, 0, 255);
  
  while (state.running) {
    
    SDL_SetRenderDrawColor(gfx.renderer, 0, 0, 0, 255);
    SDL_RenderClear(gfx.renderer);
    

    // user input handling
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_QUIT:
        std::cout << "Quitting..." << std::endl;
        state.running = false;
        break;
      case SDL_KEYDOWN:
      case SDL_KEYUP:
        handleKeyPress(event, &state);
        break;
      default:
        break;
      }
    }

    // drawing
    DrawGameRect(&gfx, &(state.paddle));
    DrawGameRect(&gfx, &(state.ball));
    
    // update
    state.ball.x = state.ball.x + (state.speed * state.dx);
    state.ball.y = state.ball.y + (state.speed * state.dy);
    
    if(state.ball.x + state.ball.w > gfx.width) state.dx = -1.0*state.dx;
    if(state.ball.x < 0) state.dx = -1.0*state.dx;
  
    if(state.ball.y + state.ball.h > gfx.height) state.dy = -1.0*state.dy;
    if(state.ball.y < 0 ) state.dy = -1.0*state.dy;



    // end of loop
    SDL_RenderPresent(gfx.renderer);
  }
}
