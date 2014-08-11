#include "SFApp.h"
#include "SFAsset.h"

int SFApp::getint()
{
  return i;
}


SFApp::SFApp() : fire(0), is_running(true) {

  surface = SDL_GetVideoSurface();
  app_box = make_shared<SFBoundingBox>(Vector2(surface->w/2, surface->h/2), surface->w/2, surface->h/2);
  player  = make_shared<SFAsset>(SFASSET_PLAYER);
  auto player_pos = Point2(surface->w/2, 88.0f);
  player->SetPosition(player_pos);

  const int number_of_aliens = 10;
  for(int i=0; i<number_of_aliens; i++) {
    // place an alien at width/number_of_aliens * i
    auto alien = make_shared<SFAsset>(SFASSET_ALIEN);
    auto pos   = Point2((surface->w/number_of_aliens) * i + 30, 400.0f);
    alien->SetPosition(pos);
    aliens.push_back(alien);
  }

  auto coin = make_shared<SFAsset>(SFASSET_COIN);
  auto pos  = Point2((surface->w/4), 100);
  coin->SetPosition(pos);
  coins.push_back(coin);
  
  background = IMG_Load("assets/background.png");
  
  textcolor = {255,255,255};
  
  font = TTF_OpenFont("src/Starjedi.ttf" , 28);
  if(font == NULL)
  {
    std::cout << "ttf fail\n";
    return;
  }  
}

SFApp::~SFApp() {
}

/**
 * Handle all events that come from SDL.
 * These are timer or keyboard events.
 */
void SFApp::OnEvent(SFEvent& event) {
  SFEVENT the_event = event.GetCode();
    
  switch (the_event) {
  case SFEVENT_QUIT:
    is_running = false;
    break;
  case SFEVENT_UPDATE:
    OnUpdateWorld();
    OnRender();
    break;
  case SFEVENT_PLAYER_LEFT:
    player->GoWest();
    break;
  case SFEVENT_PLAYER_RIGHT:
    player->GoEast();
    break;
  case SFEVENT_FIRE:
    fire ++;
    std::stringstream sstm;
    sstm << "Fire " << fire;
    //SDL_WM_SetCaption(sstm.str().c_str(),  sstm.str().c_str());
    FireProjectile();
    break;
  }
}

int SFApp::OnExecute() {
  // Execute the app
  SDL_Event event;
  while (SDL_WaitEvent(&event) && is_running) {
    // if this is an update event, then handle it in SFApp,
    // otherwise punt it to the SFEventDispacher.
    SFEvent sfevent((const SDL_Event) event);
    OnEvent(sfevent);
  }
}

void SFApp::OnUpdateWorld() {
  // Update projectile positions
  for(auto p: projectiles) {
    p->GoNorth();
  }

  for(auto c: coins) {
    c->GoSouth();
  }

  // Update enemy positions
  
  for(auto a : aliens) {
  }

  // Detect collisions
  for(auto p : projectiles) {
    for(auto a : aliens) {
      if(p->CollidesWith(a)) {
        p->HandleCollision();
        a->HandleCollision();
	i += 10;
      }
      
    }
  }

  // remove dead aliens (the long way)
  list<shared_ptr<SFAsset>> tmp;
  for(auto a : aliens) {
    if(a->IsAlive()) {
      tmp.push_back(a);
    }
  }
  aliens.clear();
  aliens = list<shared_ptr<SFAsset>>(tmp);
  if(aliens.empty())
  {
    gameOver();
  }
  sprintf(text , "player score: %d" , i  );
}

void SFApp::OnRender() {
  // clear the surface
  
  SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 8, 54, 129) );
  SDL_BlitSurface(background , NULL , surface , NULL);
  score = TTF_RenderText_Solid(font , text , textcolor );
  SDL_BlitSurface(score , NULL , surface , NULL);

  // draw the player
  player->OnRender(surface);

  for(auto p: projectiles) {
    if(p->IsAlive()) {p->OnRender(surface);}
  }

  for(auto a: aliens) {
    if(a->IsAlive()) {a->OnRender(surface);}
  }

  for(auto c: coins) {
    c->OnRender(surface);
  }
  
  // Switch the off-screen buffer to be on-screen
  SDL_Flip(surface);
}

void SFApp::FireProjectile() {
  auto pb = make_shared<SFAsset>(SFASSET_PROJECTILE);
  auto v  = player->GetPosition();
  pb->SetPosition(v);
  projectiles.push_back(pb);
}

void SFApp::gameOver()
{
  char gameOverText[20];
  sprintf(gameOverText , "game over");
  SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0, 0, 0) );
  SDL_Surface* text = TTF_RenderText_Solid(font , gameOverText , textcolor);
  SDL_BlitSurface(text , NULL , surface , NULL);
  SDL_Flip(surface);
  SDL_Delay(3000);
  TTF_CloseFont(font);
  TTF_Quit();
  is_running = false;
}

