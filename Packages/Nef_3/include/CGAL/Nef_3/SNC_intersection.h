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
// Author(s)     : Michael Seel       <seel@mpi-sb.mpg.de>
//                 Peter Hachenberger <hachenberger@mpi-sb.mpg.de> 
#ifndef CGAL_SNC_INTERSECTION_H
#define CGAL_SNC_INTERSECTION_H

#include <CGAL/basic.h>
/*
#include <CGAL/functional.h> 
#include <CGAL/function_objects.h> 
#include <CGAL/Circulator_project.h>
#include <CGAL/Nef_3/bounded_side_3.h>
#include <CGAL/Nef_3/Pluecker_line_3.h>
#include <CGAL/Nef_3/SNC_decorator.h>
#include <CGAL/Nef_3/SNC_SM_overlayer.h>
#include <CGAL/Nef_3/SNC_SM_point_locator.h>
#include <CGAL/Nef_3/SNC_FM_decorator.h>
#ifdef SM_VISUALIZOR
#include <CGAL/Nef_3/SNC_SM_visualizor.h>
#endif // SM_VISUALIZOR
#include <map>
#include <list>
#undef _DEBUG
#define _DEBUG 37
#include <CGAL/Nef_3/debug.h>
*/

CGAL_BEGIN_NAMESPACE

template < class Node, class Object, class DClass>
struct Project_halfedge_point {
  typedef Node         argument_type;
  typedef Object       result_type;
  Object& operator()( Node& x) const   { 
    DClass D;
    return D.point(D.source(x));
    /* a Point_3& reference must be returned by D.point() */
  }
  const Object& operator()( const Node& x) const   { 
    DClass D;
    return D.point(D.source(x)); 
    /* a Point_3& reference must be returned by D.point() */
  }
};

template<typename SNC_structure_>
class SNC_intersection : public SNC_const_decorator<SNC_structure_> {

  typedef SNC_structure_                     SNC_structure;
  typedef SNC_intersection<SNC_structure>    Self;
  typedef SNC_const_decorator<SNC_structure> Base;
  typedef SNC_const_decorator<SNC_structure> SNC_const_decorator;

  typedef typename SNC_structure::SHalfedge               SHalfedge;
  typedef typename SNC_structure::Halfedge_handle         Halfedge_handle;
  typedef typename SNC_structure::Halffacet_const_handle  Halffacet_const_handle;
  typedef typename SNC_structure::SHalfedge_handle        SHalfedge_handle;
  typedef typename SNC_structure::SHalfloop_handle        SHalfloop_handle;
  typedef typename SNC_structure::SHalfedge_around_facet_circulator 
                                  SHalfedge_around_facet_circulator;
  typedef typename SNC_structure::Halffacet_cycle_const_iterator
                                  Halffacet_cycle_const_iterator;

  typedef typename SNC_structure::Point_3        Point_3;
  typedef typename SNC_structure::Vector_3       Vector_3;
  typedef typename SNC_structure::Segment_3      Segment_3;
  typedef typename SNC_structure::Line_3         Line_3;
  typedef typename SNC_structure::Ray_3          Ray_3;
  typedef typename SNC_structure::Plane_3        Plane_3;

 public:

  SNC_intersection(const SNC_structure& W) : Base(W) {}

  bool does_contain_internally(const Segment_3& s, const Point_3& p) const {
    if(!s.has_on(p))
      return false;
    Comparison_result r1 = compare_xyz(s.source(),p);
    Comparison_result r2 = compare_xyz(s.target(),p);
    return (r1 == opposite(r2));
  }

  bool does_contain_internally( const Halffacet_const_handle f, 
				const Point_3& p) const {
    if( !plane(f).has_on(p))
      return false;
    return (locate_point_in_halffacet( p, f) == CGAL::ON_BOUNDED_SIDE);
  }

#ifdef LINE3_LINE3_INTERSECTION
  
  bool does_intersect_internally( const Segment_3& s1, 
				  const Segment_3& s2, 
				  Point_3& p) const  {
    TRACEN("does intersect internally with  LINE3_LINE3_INTERSECTION");
    if ( s1.is_degenerate() || s2.is_degenerate())
      /* the segment is degenerate so there is not internal intersection */
      return false;
    if ( s1.has_on(s2.source()) || s1.has_on(s2.target()) ||
	 s2.has_on(s1.source()) || s2.has_on(s1.target()))
      /* the segments does intersect at one endpoint */
      return false;
    Object o = intersection(Line_3(ray), Line_3(s)); 
    if ( !assign(p, o))
      return false;
    return( does_contain_internally( s, p));
  }

#else // LINE3_LINE3_INTERSECTION

  bool does_intersect_internally( const Segment_3& s1, 
				  const Segment_3& s2, 
				  Point_3& p) const {
    if(s2.has_on(s1.target())) 
      return false;
    return (does_intersect_internally(Ray_3(s1.source(), s1.target()), s2, p)
      && s1.has_on(p));
    
  }


  bool does_intersect_internally( const Ray_3& s1, 
				  const Segment_3& s2, 
				  Point_3& p) const {
    TRACEN("does intersect internally without  LINE3_LINE3_INTERSECTION");
    if ( s1.is_degenerate() || s2.is_degenerate())
      // the segment is degenerate so there is not internal intersection 
      return false;
    if ( s1.has_on(s2.source()) || s1.has_on(s2.target()) ||
	 s2.has_on(s1.source()))
      // the segments does intersect at one endpoint 
      return false;
    if ( orientation( s1.source(), s1.point(1), s2.source(), s2.target()) 
	 != COPLANAR)
      // the segments doesn't define a plane
      return false;
    if ( collinear( s1.source(), s1.point(1), s2.source()) &&
	 collinear( s1.source(), s1.point(1), s2.target()) )
      // the segments are collinear 
      return false;
    Line_3 ls1(s1), ls2(s2);
    if ( ls1.direction() ==  ls2.direction() ||
	 ls1.direction() == -ls2.direction() )
      // the segments are parallel 
      return false;
    Vector_3 vs1(s1.direction()), vs2(s2.direction()), 
      vt(cross_product( vs1, vs2)), 
      ws1(cross_product( vt, vs1)), ws2(cross_product( vt, vs2));
    Plane_3 hs1( s1.source(), ws1);
    Object o = intersection(hs1, ls2);
    CGAL_nef3_assertion(assign( p, o));
    // since line(s1) and line(s2) are not parallel they intersects in only
    //   one point 
    assign( p ,o);
    return( s1.has_on(p) && s2.has_on(p));
  }
    
#endif // LINE3_LINE3_INTERSECTION

  bool does_intersect_internally( const Ray_3& seg,
				  const Halffacet_const_handle f,
				  Point_3& p) const { 
    TRACEN("-> Intersection face - ray");
    Plane_3 h( plane(f));
    TRACEN("-> facet plane " << h);
    TRACEN("-> a point on " << h.point());
    TRACEN("-> seg segment " << seg);
    CGAL_nef3_assertion( !h.is_degenerate());
    if( seg.is_degenerate())
      /* no possible internal intersection */
      return false;
    if( h.has_on( seg.source()))
      /* no possible internal intersection */
	return false;
    Object o = intersection( h, seg);
    if( !assign( p, o))
      return false;
    TRACEN( "-> intersection point " << p );
    TRACEN( "-> point in facet? "<<locate_point_in_halffacet(p, f));
    return does_contain_internally( f, p);
  }

  bool does_intersect_internally( const Segment_3& seg,
				  const Halffacet_const_handle f,
				  Point_3& p) const { 
    if(plane(f).has_on(seg.target())) return false;
    return (does_intersect_internally(Ray_3(seg.source(),seg.target()),f,p)
	    && seg.has_on(p));
  }

  Bounded_side locate_point_in_halffacet( const Point_3& p, 
					  const Halffacet_const_handle f) const {
    typedef Project_halfedge_point
      < SHalfedge, const Point_3, SNC_const_decorator> Project;
    typedef Circulator_project
      < SHalfedge_around_facet_circulator, Project, 
      const Point_3&, const Point_3*> Circulator;
    typedef Container_from_circulator<Circulator> Container;

    Plane_3 h(plane(f));
    CGAL_nef3_assertion(h.has_on(p));
    Halffacet_cycle_const_iterator fc = f->facet_cycles_begin();
    SHalfedge_handle se;
    Bounded_side outer_bound_pos;
    if ( assign(se,fc) ) {
      SHalfedge_around_facet_circulator hfc(se);
      Circulator c(hfc);
      Container ct(c);
      CGAL_nef3_assertion( !is_empty_range(ct.begin(), ct.end()));
      outer_bound_pos = bounded_side_3(ct.begin(), ct.end(), p, h);
    } 
    else 
      CGAL_nef3_assertion_msg(0, "is facet first cycle a SHalfloop?");
    if( outer_bound_pos != CGAL::ON_BOUNDED_SIDE )
      return outer_bound_pos;
    /* The point p is not in the relative interior of the outer face cycle
       so it is not necesary to know the possition of p with respect to the 
       inner face cycles */
    Halffacet_cycle_const_iterator fe = f->facet_cycles_end();
    ++fc;
    if( fc == fe )
      return outer_bound_pos;
    Bounded_side inner_bound_pos;
    CGAL_For_all(fc, fe) {
      SHalfloop_handle l;
      if ( assign(l,fc) ) { 
        if( point(vertex(sface(l))) == p )
	  inner_bound_pos = CGAL::ON_BOUNDARY;
	else
	  inner_bound_pos = CGAL::ON_UNBOUNDED_SIDE;
      } 
      else if ( assign(se,fc) ) {
	SHalfedge_around_facet_circulator hfc(se);
	Circulator c(hfc);
	Container ct(c);
	CGAL_nef3_assertion( !is_empty_range(ct.begin(), ct.end()));
        inner_bound_pos = bounded_side_3( ct.begin(), ct.end(), 
					  p, h.opposite());
      } 
      else 
	CGAL_nef3_assertion_msg(0, "Damn wrong handle.");
      if( inner_bound_pos != CGAL::ON_UNBOUNDED_SIDE )
	return opposite(inner_bound_pos);
      /* At this point the point p belongs to relative interior of the facet's
	 outer cycle, and its possition is completely known when it belongs
	 to the clousure of any inner cycle */
    }
    return CGAL::ON_BOUNDED_SIDE;
  }
}; // SNC_intersection

CGAL_END_NAMESPACE

#endif //CGAL_SNC_INTERSECTION_H
