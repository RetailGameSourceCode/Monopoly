#ifndef MESH_H
#define MESH_H

struct Box;
class view;

// .......... The abstract base mesh class (3D point-based geometry and texture ).
// .......... It is the source for a variety of application or system specific mesh formats and
// .......... rendering techniques. A bare minimum of functions must be implemented by the derived
// .......... class so that any class dealing with it knows its type and what its bounds are in 3D space.
// .......... The bounds are represented by an axis aligned box. The basic implementation of
// .......... 'render' simply draws the object's bounding box.
class mesh
{
  public:
	  virtual int 	getType() const = 0; // must be overridden
	  virtual void	getBox(Box& bounds) const = 0; // must be overridden
	  virtual int		render(view *in); // not necessary to override. returns non-zero on success
};


#endif