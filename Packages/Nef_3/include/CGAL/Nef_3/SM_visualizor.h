// Copyright (c) 1997-2002  Max-Planck-Institute Saarbruecken (Germany).
// All rights reserved.
//
// This file is part of CGAL (www.cgal.org); you may redistribute it under
// the terms of the Q Public License version 1.0.
// See the file LICENSE.QPL distributed with CGAL.
//
// Licensees holding a valid commercial license may use this file in
// accordance with the commercial license agreement provided with the software.
//
// This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//
// $Source$
// $Revision$ $Date$
// $Name$
//
// Author(s)     : Michael Seel    <seel@mpi-sb.mpg.de>
//                 Miguel Granados <granados@mpi-sb.mpg.de>
//                 Susan Hert      <hert@mpi-sb.mpg.de>
//                 Lutz Kettner    <kettner@mpi-sb.mpg.de>
#ifndef CGAL_SM_VISUALIZOR_H
#define CGAL_SM_VISUALIZOR_H

#include <CGAL/basic.h>
#include <CGAL/Nef_S2/Sphere_map.h>
#include <CGAL/Nef_S2/Sphere_geometry_OGL.h>
#include <CGAL/Nef_3/SNC_SM_decorator.h>
#include <CGAL/Nef_3/SNC_SM_triangulator.h>

#define LGREY CGAL::Color(170,170,200)
#define DGREY CGAL::Color(30,30,50)

CGAL_BEGIN_NAMESPACE

template <typename Refs_>
class SNC_SM_BooleColor 
{
  typedef typename Refs_::SVertex_const_handle   SVertex_const_handle;   
  typedef typename Refs_::SHalfedge_const_handle SHalfedge_const_handle;   
  typedef typename Refs_::SHalfloop_const_handle SHalfloop_const_handle;   
  typedef typename Refs_::SFace_const_handle     SFace_const_handle;   
  typedef typename Refs_::Mark Mark;   
public:
  Color color(SVertex_const_handle, Mark m) const
  { return ( m ? CGAL::BLACK : CGAL::WHITE ); }
  Color color(SHalfedge_const_handle, Mark m) const
  { return ( m ? CGAL::BLACK : CGAL::WHITE ); }
  Color color(SHalfloop_const_handle, Mark m) const
  { return ( m ? CGAL::BLACK : CGAL::WHITE ); }
  Color color(SFace_const_handle, Mark m) const
  { return ( m ? DGREY : LGREY ); }
};


template <typename SM_explorer>
class SM_visualizor 
{
/*{\Mdefinition An instance |\Mvar| of the data type |\Mname| is a
decorator to draw the structure of a sphere map into the surface of a
OpenGL sphere. It is generic with respect to the template concept.}*/

public:
  typedef typename SM_explorer::Refs_          Refs_;
  typedef CGAL::SNC_SM_BooleColor<Refs_>    Color_;
  typedef typename Refs_::Sphere_kernel        Sphere_kernel;
  typedef CGAL::Sphere_map<Sphere_kernel>      Sphere_map;
//  typedef SNC_SM_const_decorator<Refs_>        SM_const_decorator;
  typedef SNC_SM_triangulator<Refs_,Sphere_map,Sphere_kernel> 
                                               SNC_SM_triangulator;

#define USING(t) typedef typename Refs_::t t
  USING(Vertex_const_handle);
  USING(SVertex_const_handle);   
  USING(SHalfedge_const_handle); 
  USING(SFace_const_handle);     
  USING(SVertex_const_iterator);
  USING(SHalfedge_const_iterator);
  USING(SFace_const_iterator);
  USING(Mark);
#undef USING

  /*
    Here collide two handle worlds: Nef_S2::handles and
    Nef_3::handles: We have to separate symbols. In this class scope
    we stick to the Nef_3 world. All Nef_S2 handles obtain a SM_ prefix.
  */

  // types from the local sphere map type that stores triangulation:
  typedef typename Sphere_map::Vertex_const_iterator   
    SM_Vertex_const_iterator;
  typedef typename Sphere_map::Halfedge_const_iterator 
    SM_Halfedge_const_iterator;
  typedef typename Sphere_map::Halfloop_const_iterator 
    SM_Halfloop_const_iterator;
  typedef typename Sphere_map::Face_const_iterator     
    SM_Face_const_iterator;
  

  typedef typename Sphere_kernel::Sphere_point    Sphere_point;
  typedef typename Sphere_kernel::Sphere_segment  Sphere_segment;
  typedef typename Sphere_kernel::Sphere_circle   Sphere_circle;
  typedef typename Sphere_kernel::Sphere_triangle Sphere_triangle;
  typedef Color_                                  Color_objects;

protected:
  Sphere_map              MT_;
  SM_explorer             E_;
  SNC_SM_triangulator     T_;
  Color_objects           CO_;
  CGAL::OGL::Unit_sphere& S_;
public:

/*{\Mcreation 4}*/
SM_visualizor(const SM_explorer& E, CGAL::OGL::Unit_sphere& S)
  : MT_(), E_(E), T_(E,MT_), S_(S)
{ 
  T_.triangulate(); 
}


/*{\Moperations 2 1}*/

/* |draw_map| draws all object of the sphere map referenced by E_:
   1) edges, loops, and vertices are taken from E_
   2) faces are drawn via the calculated triangulation in MT_  */

void draw_map() const
/*{\Mop draw the whole plane map.}*/
{
  // draw sphere segments underlying edges of E_:
  SHalfedge_const_iterator e;
  CGAL_nef3_forall_sedges(e,E_) {
    if ( E_.source(e) == E_.target(e) ) {
      S_.push_back(E_.circle(e), CO_.color(e,E_.mark(e))); 
    } else {
      S_.push_back(Sphere_segment(E_.point(E_.source(e)),
				  E_.point(E_.target(e)),
				  E_.circle(e)),CO_.color(e,E_.mark(e)));
    }
  }
  // draw sphere circles underlying loops of E_:

  if ( E_.has_loop() )
    S_.push_back(
      Sphere_circle(E_.circle(E_.shalfloop())),
      CO_.color(E_.shalfloop(),E_.mark(E_.shalfloop())));

  // draw points underlying vertices of E_:
  SVertex_const_iterator v;
  CGAL_nef3_forall_svertices(v,E_)
    S_.push_back(E_.point(v),CO_.color(v,E_.mark(v)));


  SM_Halfedge_const_iterator h;
  Unique_hash_map<SM_Halfedge_const_iterator,bool> Done(false);
  CGAL_nef3_forall_halfedges(h,T_) {
    if ( Done[h] ) continue;
    SM_Halfedge_const_iterator hn(T_.next(h)),hnn(T_.next(hn));
    TRACEV(T_.incident_triangle(h));
    CGAL_nef3_assertion(
      T_.incident_mark(h) == T_.incident_mark(hn) &&
      T_.incident_mark(hn) == T_.incident_mark(hnn));
    Mark m = T_.incident_mark(h);
    Sphere_triangle t = T_.incident_triangle(h);
    S_.push_back(t, (m ? DGREY : LGREY) );
    Done[h]=Done[hn]=Done[hnn]=true;
  }

  CGAL_forall_edges(h,T_) // for all sphere edges
    S_.push_back_triangle_edge(Sphere_segment(T_.point(T_.source(h)),
					      T_.point(T_.target(h)),
					      T_.circle(h)));
}

/* |draw_triangulation| draws all object of the underlying triangulation:
   triangles, edges, loops, and vertices taken from MT_ */

void draw_triangulation() const
{ 
  // draw sphere segments underlying edges of triangulation:
  SHalfedge_const_iterator ed;
  SVertex_const_iterator vd;
  SM_Halfedge_const_iterator e;
  CGAL_nef3_forall_edges(e,T_) {
    S_.push_back(Sphere_segment(T_.point(T_.source(e)),T_.point(T_.target(e)),
				T_.circle(e)),CO_.color(ed,T_.mark(e)));
  }

  // draw points underlying vertices of triangulation:
  SM_Vertex_const_iterator v;
  CGAL_nef3_forall_vertices(v,T_)
    S_.push_back(T_.point(v),CO_.color(vd,T_.mark(v)));

  Unique_hash_map<SM_Halfedge_const_iterator,bool> Done(false);
  CGAL_nef3_forall_halfedges(e,T_) {
    if ( Done[e] ) continue;
    SM_Halfedge_const_iterator en(T_.next(e)),enn(T_.next(en));
    CGAL_nef3_assertion(T_.incident_mark(e)==T_.incident_mark(en)&&
			T_.incident_mark(en)==T_.incident_mark(enn));
    Mark m = T_.incident_mark(e);
    Sphere_triangle t = T_.incident_triangle(e);
    S_.push_back(t, (m ? DGREY : LGREY) );
    Done[e]=Done[en]=Done[enn]=true;
  }

}

}; // end of SM_Visualizor 



template <class Refs_>
void show_sphere_map_of(typename Refs_::Vertex_const_handle v)
{ CGAL::OGL::add_sphere();
  SM_visualizor<Refs_> V(v,CGAL::OGL::spheres_.back());
  V.draw_map();
  CGAL::OGL::start_viewer();
}


CGAL_END_NAMESPACE
#endif // CGAL_SM_VISUALIZOR_H

