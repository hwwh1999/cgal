// Copyright (c) 1999  Utrecht University (The Netherlands),
// ETH Zurich (Switzerland), Freie Universitaet Berlin (Germany),
// INRIA Sophia-Antipolis (France), Martin-Luther-University Halle-Wittenberg
// (Germany), Max-Planck-Institute Saarbruecken (Germany), RISC Linz (Austria),
// and Tel-Aviv University (Israel).  All rights reserved.
//
// This file is part of CGAL (www.cgal.org); you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License as
// published by the Free Software Foundation; version 2.1 of the License.
// See the file LICENSE.LGPL distributed with CGAL.
//
// This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//
// $Source$
// $Revision$ $Date$
// $Name$
//
// Author(s)     : Stefan Schirra
 

#ifndef CGAL__TEST_CLS_VECTOR_2_H
#define CGAL__TEST_CLS_VECTOR_2_H

template <class R>
bool
_test_cls_vector_2(const R& )
{
 std::cout << "Testing class Vector_2" ;

 typedef typename  R::RT    RT;
 typedef typename  R::FT    FT;

 typename R::Vector_2       iv;
 CGAL::Vector_2<R>  v1;
 CGAL::Vector_2<R>  v2(iv);
 CGAL::Vector_2<R>  v0(CGAL::NULL_VECTOR);

 RT  n1( 12 );
 RT  n2( -4 );
 RT  n4(  2 );

 CGAL::Vector_2<R>  v3(n1, n2 );       // ( 12, -4)
 CGAL::Vector_2<R>  v4(n1, n2, n4);    // (  6, -2)
 CGAL::Vector_2<R>  v5(n1, n2, n4);    // (  6, -2)
 CGAL::Vector_2<R>  v6( v5 );
                   v1 = v4;
 CGAL::Vector_2<R>  v7(-n1, -n2, -n4); // (  6, -2)

 std::cout << '.';

 assert( v5 == v7 );
 assert( v4 == v5 );
 assert( v5 == v6 );
 assert( v4 == v6 );
 assert( v1 == v6 );
 assert( v0 == CGAL::NULL_VECTOR);

 assert( v3 != v4 );
 assert( v0 != v1 );
 assert( v1 != CGAL::NULL_VECTOR);

 assert( v3.hx() == n1 );   // don't replace v3
 assert( v3.hy() == n2 );

 assert( FT( v5.hx()) / FT(v5.hw()) == FT( n1) / FT( n4) );
 assert( FT( v5.hy()) / FT(v5.hw()) == FT( n2) / FT( n4) );

 assert( v5.x() == FT( n1) / FT( n4) );
 assert( v5.y() == FT( n2) / FT( n4) );

 std::cout << '.';

 assert( v3.homogeneous(0) == v3.hx() );  // don't replace v3
 assert( v3.homogeneous(1) == v3.hy() );
 assert( v3.homogeneous(2) == v3.hw() );
 assert( v6.cartesian(0) == v6.x() );
 assert( v6.cartesian(1) == v6.y() );

 std::cout << '.';

 assert( v0.dimension() == 2 );
 assert( v4.homogeneous( v4.dimension() ) == v4.hw() );

 CGAL::Point_2<R> p0(4,2,1);
 CGAL::Point_2<R> p1(2,4,1);
 CGAL::Segment_2<R> s1(p0, p1);
 CGAL::Ray_2<R> r1(p0, p1);
 CGAL::Line_2<R> l1(p0, p1);
 CGAL::Vector_2<R> v8(p0, p1);
 CGAL::Vector_2<R> v9(s1);
 CGAL::Vector_2<R> v10(r1);
 CGAL::Vector_2<R> v11(l1);

 assert( v8 == (p1-p0) );
 assert( v8 == v9 );
 assert( v10.direction() == v8.direction() );
 assert( v11.direction() == v8.direction() );

 std::cout << "done" << std::endl;
 return true;
}
#endif // CGAL__TEST_CLS_VECTOR_2_H
