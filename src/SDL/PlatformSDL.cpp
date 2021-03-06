#include <FWPlatform.h>
#include <EventLoop.h>
#include <CurlClient.h>
#include <Logger.h>
#include <ContextCairo.h>
#include <SDLSoundCanvas.h>
#include <Message.h>
#include <FWApplication.h>

#include <TouchEvent.h>
#include <DrawEvent.h>
#include <SysEvent.h>
#include <UpdateEvent.h>
#include <ResizeEvent.h>

#include <SDL/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include <cstdio>
#include <cstdlib>
#include <sys/time.h>

using namespace std;

class PlatformSDL : public FWPlatform {
public:
  PlatformSDL() : FWPlatform(1.0f,
			     // "#version 300 es",
			     "#version 100",
			     true) { }

  double getTime() const override {
    struct timeval tv;
    struct timezone tz;
    int r = gettimeofday(&tv, &tz);
    double t = 0;
    if (r == 0) {
      t = (double)tv.tv_sec + tv.tv_usec / 1000000.0;
    }
    return t;
  }
    
  string getLocalFilename(const char * fn, FileType type) {
    string s = "assets/";
    return s + fn;
  }
  
  std::shared_ptr<HTTPClientFactory> createHTTPClientFactory() const {
    return std::make_shared<CurlClientFactory>();
  }
  
  int showActionSheet(const FWRect&, const FWActionSheet&) {
    return 0;
  }
  
  std::shared_ptr<canvas::ContextFactory> createContextFactory() const {
    return std::shared_ptr<canvas::ContextFactory>(new canvas::CairoContextFactory);
  }

  void sendMessage(const Message & message) override {
    cerr << "sendMessage(" << int(message.getType()) << ")\n";
    FWPlatform::sendMessage(message);
    switch (message.getType()) {
    case Message::SET_CAPTION:
      SDL_WM_SetCaption(message.getTextValue().c_str(),
			message.getTextValue().c_str());
      break;
    case Message::SHOW_MESSAGE_DIALOG:
#if 0
      SDL_ShowSimpleMessageBox( SDL_MESSAGEBOX_INFORMATION,
				title.c_str(),
				message.c_str(),
				NULL);
#endif
      break;
    default:
      break;
    }
  }

  std::shared_ptr<SoundCanvas> createSoundCanvas() const override {
    return std::make_shared<SDLSoundCanvas>();
  }
  
  std::shared_ptr<Logger> createLogger() const override {
    return std::make_shared<BasicLogger>();
  }

  // std::shared_ptr<EventLoop> createEventLoop() override;
  
  std::string showTextEntryDialog(const std::string & message) {
    return "";
  }

  std::string getBundleFilename(const char * filename) {
    string s = "assets/";
    return s + filename;
  }

  void storeValue(const std::string & key, const std::string & value) {
    
  }

  std::string loadValue(const std::string & key) {
    return "";
  }

  void swapBuffers() {
    SDL_GL_SwapBuffers( );
  }

  void run() {
    SDL_Event event;

    cerr << "starting runloop\n";
    
    while ( true ) {
      /* Grab all the events off the queue. */
      while( SDL_PollEvent( &event ) ) {
	switch( event.type ) {
	case SDL_KEYDOWN:
	  /* Handle key presses. */
	  // handle_key_down( &event.key.keysym );
	  break;
	case SDL_MOUSEBUTTONDOWN:
	  {
	    TouchEvent ev(TouchEvent::ACTION_DOWN, mouse_x, mouse_y, getTime(), 0);
	    postEvent(getActiveViewId(), ev);
	    button_pressed = true;
	  }
	  break;
	case SDL_MOUSEBUTTONUP:
	  {
	    TouchEvent ev(TouchEvent::ACTION_UP, mouse_x, mouse_y, getTime(), 0);
	    postEvent(getActiveViewId(), ev);
	    button_pressed = false;
	  }
	  break;
	case SDL_MOUSEMOTION:
	  if (button_pressed) {
	    TouchEvent ev(TouchEvent::ACTION_MOVE, event.motion.x, event.motion.y, getTime(), 0);
	    postEvent(getActiveViewId(), ev);
	  }
	  mouse_x = event.motion.x;
	  mouse_y = event.motion.y;
	  break;
	case SDL_VIDEORESIZE:
	  {
	    int w = event.resize.w, h = event.resize.h;
	    cerr << "resized (" << w << " " << h << ")\n";
	    setDisplayWidth(w);
	    setDisplayHeight(h);
	    ResizeEvent ev(getTime(), w, h, w, h);
	    postEvent(getActiveViewId(), ev);
	  }
	  break;
	case SDL_QUIT:
	  return;
	}
      }

      getApplication().loadEvents();

      UpdateEvent ev0(getTime());
      postEvent(getActiveViewId(), ev0);

      if (isRedrawNeeded()) {
	DrawEvent ev(getTime());
	postEvent(getActiveViewId(), ev);
	swapBuffers();
	clearRedrawNeeded();
      }
    }
  }
  
private:
  bool button_pressed = false;
  int mouse_x = 0, mouse_y = 0;
};

#if 0
static void handle_key_down(SDL_keysym* keysym) {

    /* 
     * We're only interested if 'Esc' has
     * been presssed.
     *
     * EXERCISE: 
     * Handle the arrow keys and have that change the
     * viewing position/angle.
     */
    switch( keysym->sym ) {
    case SDLK_ESCAPE:
        quit_tutorial( 0 );
        break;
    case SDLK_SPACE:
        should_rotate = !should_rotate;
        break;
    default:
        break;
    }

}
#endif

extern FWApplication * applicationMain();

int main(int argc, char *argv[]) {
  if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_AUDIO ) < 0 ) {
    /* Failed, exit. */
    fprintf( stderr, "Video initialization failed: %s\n",
             SDL_GetError( ) );
    SDL_Quit( );
    exit(1);
  }

  const SDL_VideoInfo * info = SDL_GetVideoInfo( );

  if (!info) {
    /* This should probably never happen. */
    fprintf( stderr, "Video query failed: %s\n", SDL_GetError() );
    SDL_Quit();
    exit(1);
  }

  int width = 800, height = 600;
  int bpp = info->vfmt->BitsPerPixel;

  SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 5 );
  SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 5 );
  SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 5 );
  SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 16 );
  SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
  
  int flags = SDL_OPENGL; // | SDL_FULLSCREEN;

  if (SDL_SetVideoMode( width, height, bpp, flags ) == 0) {
    fprintf( stderr, "Video mode set failed: %s\n",
             SDL_GetError( ) );
    SDL_Quit();
    exit(1);
  }

  FWApplication * application = applicationMain();

  cerr << "starting, app = " << application << "\n";

  PlatformSDL platform;  
  platform.setApplication(application);
  platform.getApplication().initialize(&platform);
  platform.getApplication().onCmdLine(argc, argv);

  platform.setDisplayWidth(width);
  platform.setDisplayHeight(height);
  platform.getApplication().initializeContent();	   
    
  platform.run();
  
#if 0
  auto eventloop = platform.createEventLoop();
  eventloop->run();
#endif
  
  SysEvent ev(platform.getTime(), SysEvent::SHUTDOWN);
  ev.dispatch(platform.getApplication());
  SDL_Quit();
  return 0;
}
