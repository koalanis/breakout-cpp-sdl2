#include <SDL.h>
#include <SDL2/SDL.h>
#include <iostream>
#include <cmath>
#include <vector>
#include <memory>

class Gfx {
public:
  // Contructor which initialize the parameters.
  Gfx(int width_, int height_) : height(height_), width(width_) {
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
  int height;
  int width;

  bool running;
  GameRect paddle;
  GameRect ball;

  float speed = 0.5f;
  // normalized vector
  float dx = 1.0/sqrt(2.0);
  float dy = 1.0/sqrt(2.0);

  GameState(bool startRunning): running(startRunning) {}
  GameState(): running(true) {}
  
  void init(int width, int height) {
    this->paddle.x = 250;
    this->paddle.y = 575;
    this->paddle.w = 200;
    this->paddle.h = 15;

    this->ball.x = 250;
    this->ball.y = 200;
    this->ball.w = 25;
    this->ball.h = 25;
    
    this->height = height;
    this->width = width;

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

bool rectIntersection(GameRect& rect1, GameRect& rect2) {
  bool oneIntersectTwoX = (rect1.x <= rect2.x && rect2.x <= (rect1.x + rect1.w)) || (rect2.x <= rect1.x && rect1.x <= (rect2.x + rect2.w));
  bool oneIntersectTwoY = (rect1.y <= rect2.y && rect2.y <= (rect1.y + rect1.h)) || (rect2.y <= rect1.y && rect1.y <= (rect2.y + rect2.h));
  return oneIntersectTwoX && oneIntersectTwoY;
}

enum RectCollisionSide {TOP, BOTTOM, LEFT, RIGHT, NONE};

/**
 * Returns the side that Rect1 is touching on Rect2
**/
RectCollisionSide getCollisionSide(GameRect& rect1, GameRect& rect2) {

  if(rectIntersection(rect1, rect2)) {
    if((abs((rect1.x + rect1.w) - rect2.x)) <= 1) return RectCollisionSide::LEFT;
    if(abs((rect1.x) - (rect2.x + rect2.w)) <= 1) return RectCollisionSide::RIGHT;
    if(abs((rect1.y + rect1.h) - rect2.y) <= 1) return RectCollisionSide::TOP;
    if(abs((rect1.y) - (rect2.y + rect2.h)) <= 1) return RectCollisionSide::BOTTOM;
  }

  return RectCollisionSide::NONE;
}

void UpdateGameState(GameState& state) {
    // ball movement
    state.ball.x = state.ball.x + (state.speed * state.dx);
    state.ball.y = state.ball.y + (state.speed * state.dy);
    
    // ball window collision
    if(state.ball.x + state.ball.w > state.width) state.dx = -1.0*state.dx;
    if(state.ball.x < 0) state.dx = -1.0*state.dx;
    if(state.ball.y + state.ball.h > state.height) state.dy = -1.0*state.dy;
    if(state.ball.y < 0 ) state.dy = -1.0*state.dy; 

    GameRect gameBounds;
    gameBounds.x = 0;
    gameBounds.y = 0;
    gameBounds.w = state.width;
    gameBounds.h = state.height;

    if(rectIntersection(state.paddle, state.ball)) {
//      std::cout << "ball hit paddle" << std::endl;
      RectCollisionSide side = getCollisionSide(state.ball, state.paddle);
//       std::cout << side << std::endl;
      if(side != RectCollisionSide::NONE) {
        switch(side) {
          case RectCollisionSide::TOP:
            std::cout << "TOP" << std::endl;
            state.dy = -1.0 * state.dy;
            break;
          case RectCollisionSide::LEFT:
            std::cout << "LEFT" << std::endl;
            state.dx = -1.0 * state.dx;
            break;
          case RectCollisionSide::RIGHT:
            std::cout << "RIGHt" << std::endl;
            state.dx = -1.0 * state.dx;
            break;
          case RectCollisionSide::BOTTOM:
            std::cout << "BOTTOM" << std::endl;
            state.dy = -1.0 * state.dy;
            break;
          default:
            break;
           
        }
      }
    }
}

int main(int argc, char *argv[]) {
  int width = 800;
  int height = 600;
  Gfx gfx(width, height);

  // game state 
  GameState state;
  state.init(width, height);

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
    UpdateGameState(state);

    // end of loop
    SDL_RenderPresent(gfx.renderer);
  }
}
