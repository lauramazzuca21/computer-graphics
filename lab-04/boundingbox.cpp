#include "boundingbox.h"
#ifdef WIN32
//#include "glut.h"
#include <GL\freeglut.h>
#else
#include <GL/glut.h>
#endif

// ====================================================================
// ====================================================================

void
BoundingBox::Paint () const
{
  // draw a wireframe box to represent the boundingbox

  glColor3f (0, 0, 0);
  glLineWidth (1);
  glDisable (GL_LIGHTING);
  glBegin (GL_LINES);

  glVertex3f (min.x (), min.y (), min.z ());
  glVertex3f (max.x (), min.y (), min.z ());
  glVertex3f (min.x (), min.y (), min.z ());
  glVertex3f (min.x (), max.y (), min.z ());
  glVertex3f (max.x (), max.y (), min.z ());
  glVertex3f (max.x (), min.y (), min.z ());
  glVertex3f (max.x (), max.y (), min.z ());
  glVertex3f (min.x (), max.y (), min.z ());

  glVertex3f (min.x (), min.y (), min.z ());
  glVertex3f (min.x (), min.y (), max.z ());
  glVertex3f (min.x (), max.y (), min.z ());
  glVertex3f (min.x (), max.y (), max.z ());
  glVertex3f (max.x (), min.y (), min.z ());
  glVertex3f (max.x (), min.y (), max.z ());
  glVertex3f (max.x (), max.y (), min.z ());
  glVertex3f (max.x (), max.y (), max.z ());

  glVertex3f (min.x (), min.y (), max.z ());
  glVertex3f (max.x (), min.y (), max.z ());
  glVertex3f (min.x (), min.y (), max.z ());
  glVertex3f (min.x (), max.y (), max.z ());
  glVertex3f (max.x (), max.y (), max.z ());
  glVertex3f (max.x (), min.y (), max.z ());
  glVertex3f (max.x (), max.y (), max.z ());
  glVertex3f (min.x (), max.y (), max.z ());

  glEnd ();
  glEnable (GL_LIGHTING);
}

// ====================================================================
// ====================================================================
