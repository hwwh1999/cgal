#ifndef CGAL_NO_DEPRECATED_CODE

#define CGAL_NO_DEPRECATION_WARNINGS

#include <CGAL/Simple_cartesian.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/Polyhedron_items_with_id_3.h>
#include <CGAL/IO/Polyhedron_iostream.h>
// HalfedgeGraph adapters for Polyhedron_3
#include <CGAL/boost/graph/graph_traits_Polyhedron_3.h>
#include <CGAL/boost/graph/properties_Polyhedron_3.h>

#include <CGAL/Deform_mesh.h>

#include <fstream>


typedef CGAL::Simple_cartesian<double>                                   Kernel;
typedef CGAL::Polyhedron_3<Kernel, CGAL::Polyhedron_items_with_id_3> Polyhedron;

typedef boost::graph_traits<Polyhedron>::vertex_descriptor    vertex_descriptor;
typedef boost::graph_traits<Polyhedron>::vertex_iterator        vertex_iterator;

typedef CGAL::Deform_mesh<Polyhedron> Deform_mesh;

int main()
{
  Polyhedron mesh;
  std::ifstream input("data/cactus.off");

  if ( !input || !(input >> mesh) || mesh.empty() ) {
    std::cerr<< "Cannot open  data/cactus.off" << std::endl;
    return 1;
  }

  // Init the indices of the halfedges and the vertices.
  set_halfedgeds_items_id(mesh);

  // Create a deformation object
  Deform_mesh deform_mesh(mesh);

  // Definition of the region of interest (use the whole mesh)
  vertex_iterator vb,ve;
  boost::tie(vb, ve) = vertices(mesh);
  deform_mesh.insert_roi_vertices(vb, ve);

  // Select two control vertices ...
  vertex_descriptor control_1 = *CGAL::cpp11::next(vb, 1);
  vertex_descriptor control_2 = *CGAL::cpp11::next(vb, 2);

  // ... and insert them
  deform_mesh.insert_control_vertex(control_1);
  deform_mesh.insert_control_vertex(control_2);

  // The definition of the ROI and the control vertices is done, call preprocess
  bool is_matrix_factorization_OK = deform_mesh.preprocess();
  if(!is_matrix_factorization_OK){
    std::cerr << "Error in preprocessing, check documentation of preprocess()" << std::endl;
    return 1;
  }

  // Use set_target_position() to set the constained position
  // of control_1. control_2 remains at the last assigned positions
  Deform_mesh::Point constrained_pos_1(-0.35, 0.40, 0.60);
  deform_mesh.set_target_position(control_1, constrained_pos_1);

  // Deform the mesh, the positions of vertices of 'mesh' are updated
  deform_mesh.deform();
  // The function deform() can be called several times if the convergence has not been reached yet
  deform_mesh.deform();

  // Set the constained position of control_2
  Deform_mesh::Point constrained_pos_2(0.55, -0.30, 0.70);
  deform_mesh.set_target_position(control_2, constrained_pos_2);

  // Call the function deform() with one-time parameters:
  // iterate 10 times and do not use energy based termination criterion
  deform_mesh.deform(10, 0.0);


  // Add another control vertex which requires another call to preprocess
  vertex_descriptor control_3 = *CGAL::cpp11::next(vb, 3);
  deform_mesh.insert_control_vertex(control_3);

  // The prepocessing step is again needed
  if(!deform_mesh.preprocess()){
    std::cerr << "Error in preprocessing, check documentation of preprocess()" << std::endl;
    return 1;
  }

  // Deform the mesh
  Deform_mesh::Point constrained_pos_3(0.55, 0.30, -0.70);
  deform_mesh.set_target_position(control_3, constrained_pos_3);

  deform_mesh.deform(15, 0.0);
}
#else
int main(){}
#endif
