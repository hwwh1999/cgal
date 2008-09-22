#ifndef CGAL_LINE_3_BBOX_3_DO_INTERSECT_H
#define CGAL_LINE_3_BBOX_3_DO_INTERSECT_H

#include <CGAL/Line_3.h>
#include <CGAL/Bbox_3.h>

// inspired from http://cag.csail.mit.edu/~amy/papers/box-jgt.pdf

CGAL_BEGIN_NAMESPACE

namespace CGALi {

  template <class K>
  bool do_intersect(const typename K::Line_3& line, 
    const CGAL::Bbox_3& bbox,
    const K& kernel)
  {
    typedef typename K::FT FT;
    typedef typename K::Point_3 Point;
    typedef typename K::Vector_3 Vector;

    const Point source = line.point(0);

    Point parameters[2];
    parameters[0] = Point(bbox.xmin(), bbox.ymin(), bbox.zmin());
    parameters[1] = Point(bbox.xmax(), bbox.ymax(), bbox.zmax());

    const Vector direction = line.to_vector();
    // CAREFUL, when 1.0/0.0, this works only with doubles, not with filtered kernels
    const Vector inv_direction(1.0/direction.x(),
      1.0/direction.y(),
      1.0/direction.z()); 
    const int sign_x = inv_direction.x() < (FT)0.0;
    const int sign_y = inv_direction.y() < (FT)0.0;
    const int sign_z = inv_direction.z() < (FT)0.0;

    FT tmin = (parameters[sign_x].x() - source.x()) * inv_direction.x();
    FT tmax = (parameters[1-sign_x].x() - source.x()) * inv_direction.x();

    const FT tymin = (parameters[sign_y].y() - source.y()) * inv_direction.y();
    const FT tymax = (parameters[1-sign_y].y() - source.y()) * inv_direction.y();

    if(tmin > tymax || tymin > tmax) 
      return false;

    if(tymin > tmin)
      tmin = tymin;

    if(tymax < tmax)
      tmax = tymax;

    FT tzmin = (parameters[sign_z].z() - source.z()) * inv_direction.z();
    FT tzmax = (parameters[1-sign_z].z() - source.z()) * inv_direction.z();

    if(tmin > tzmax || tzmin > tmax) 
      return false;

    return true;
  }

} // namespace CGALi

template <class K>
bool do_intersect(const CGAL::Line_3<K>& line, 
		  const CGAL::Bbox_3& bbox)
{
  return CGALi::do_intersect(line, bbox, K());
//   return typename K::Do_intersect_3()(line, bbox);
}

template <class K>
bool do_intersect(const CGAL::Bbox_3& bbox, 
		  const CGAL::Line_3<K>& line)
{
  return CGALi::do_intersect(line, bbox, K());
//   return typename K::Do_intersect_3()(line, bbox);
}

CGAL_END_NAMESPACE

#endif  // CGAL_LINE_3_BBOX_3_DO_INTERSECT_H


