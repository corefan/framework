#ifndef _OPENGLVIEW_H_
#define _OPENGLVIEW_H_

#include <FWViewBase.h>

#define FBO_COLOR	1
#define FBO_DEPTH	2
#define FBO_STENCIL	4

class OpenGLView : public FWViewBase {
 public:
  OpenGLView(int _id = 0) : FWViewBase(_id) { }

  void initialize(FWPlatform * _platform) override;

  void onResize(ResizeEvent & ev);
  
  int getLogicalWidth() const { return logical_width; }
  int getLogicalHeight() const { return logical_height; }
  int getActualWidth() const { return actual_width; }
  int getActualHeight() const { return actual_height; }

  void createFBO(int flags) { }

 protected:
  void checkGLError();

 private:
  int logical_width = 0, logical_height = 0, actual_width = 0, actual_height = 0;
};

#endif
