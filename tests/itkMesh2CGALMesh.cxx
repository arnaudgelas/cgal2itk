#include <itkRegularSphereMeshSource.h>
#include <itkMesh.h>
#include <itkQuadEdgeMesh.h>

#include <CGAL/Cartesian.h>
#include <CGAL/Polyhedron_3.h>

#include "itkMesh2Polyhedron_3.h"

template< class TInputMesh, class TOutputMesh >
int CompareMesh( TInputMesh* input, const TOutputMesh& oMesh )
{
  typename TInputMesh::PointsContainerConstPointer points = input->GetPoints();
  typename TInputMesh::PointsContainerConstIterator pIt   = points->Begin();
  typename TInputMesh::PointsContainerConstIterator pEnd  = points->End();

  typename TOutputMesh::Vertex_const_iterator vIt   = oMesh.vertices_begin();
  typename TOutputMesh::Vertex_const_iterator vEnd  = oMesh.vertices_end();

  const double distanceThreshold = 1e-9;

  std::map< typename TOutputMesh::Vertex_const_handle, typename TInputMesh::PointIdentifier > mapping;

  while( ( pIt != pEnd ) && ( vIt != vEnd ) )
  {
    typename TInputMesh::PointType p = pIt->Value();

    typename TInputMesh::PointType q;
    q[0] = vIt->point().x();
    q[1] = vIt->point().y();
    q[2] = vIt->point().z();

    mapping[ vIt ] = pIt->Index();

    if( p.EuclideanDistanceTo( q ) > distanceThreshold )
    {
      std::cerr << "ITK point " << p << " != CGAL point " << q << std::endl;
      return EXIT_FAILURE;
    }
    ++vIt;
    ++pIt;
  }

  typename TInputMesh::CellsContainerConstPointer cells = input->GetCells();
  typename TInputMesh::CellsContainerConstIterator cIt  = cells->Begin();
  typename TInputMesh::CellsContainerConstIterator cEnd = cells->End();

  typename TOutputMesh::Face_const_iterator fIt   = oMesh.facets_begin();
  typename TOutputMesh::Face_const_iterator fEnd  = oMesh.facets_end();

  while( ( cIt != cEnd ) && ( fIt != fEnd ) )
  {
    typename TOutputMesh::Halfedge_around_facet_const_circulator circulator = fIt->facet_begin();

    std::set< typename TInputMesh::PointIdentifier > list;

    do
    {
      typename TOutputMesh::Vertex_const_handle v = circulator->vertex();
      list.insert( mapping[ v ] );
    } while( circulator != fIt->facet_begin() );

    typedef typename TInputMesh::CellType ITKCellType;
    typedef typename ITKCellType::PointIdConstIterator PointIdIterator;

    typename TInputMesh::CellType* cell = cIt->Value();

    PointIdIterator faceIt  = cell->PointIdsBegin();
    PointIdIterator faceEnd = cell->PointIdsEnd();

    while( faceIt != faceEnd )
    {
      list.erase( *faceIt );
      ++faceEnd;
    }

    if( !list.empty() )
    {
      std::cerr << "faces are different" << std::endl;
      return EXIT_FAILURE;
    }
    ++fIt;
    ++cIt;
  }

  return EXIT_SUCCESS;

}

template< class TITKInputMesh, class TCGALOutputMesh >
int LoadAndTestSphere()
{
  typedef itk::RegularSphereMeshSource< TITKInputMesh > SourceType;
  typename SourceType::Pointer source = SourceType::New();

  TCGALOutputMesh oMesh;
  Convert< TITKInputMesh, TCGALOutputMesh >::fromITKtoCGAL( source->GetOutput(), oMesh );

  if( CompareMesh( source->GetOutput(), oMesh ) == EXIT_FAILURE )
    {
    std::cerr << "failure in conversion from itk to cgal" << std::endl;
    return EXIT_FAILURE;
    }

  typename TITKInputMesh::Pointer itkMesh = TITKInputMesh::New();

  Convert< TITKInputMesh, TCGALOutputMesh >::fromCGALToITK( oMesh, itkMesh.GetPointer() );

  if( CompareMesh( itkMesh.GetPointer(), oMesh ) == EXIT_FAILURE )
    {
    std::cerr << "failure in conversion from cgal to itk" << std::endl;
    return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}

int main()
{
  const unsigned int Dimension = 3;
  typedef itk::Mesh< double, Dimension > ITKMesh3d;
  typedef itk::Mesh< float, Dimension > ITKMesh3f;
  typedef itk::QuadEdgeMesh< double, Dimension > ITKQEMesh3d;

  typedef CGAL::Cartesian< double > Cartesiand;
  typedef CGAL::Polyhedron_3< Cartesiand > CGALMeshd;

  std::cout << "ITKMesh3d <-> CGALMeshd" << std::endl;
  if( LoadAndTestSphere< ITKMesh3d, CGALMeshd >() == EXIT_FAILURE )
  {
    return EXIT_FAILURE;
  }
  std::cout << "Success" << std::endl << std::endl;

  std::cout << "ITKMesh3f <-> CGALMeshd" << std::endl;
  if( LoadAndTestSphere< ITKMesh3f, CGALMeshd >() == EXIT_FAILURE )
  {
    return EXIT_FAILURE;
  }
  std::cout << "Success" << std::endl << std::endl;

  std::cout << "ITKQEMesh3d <-> CGALMeshd" << std::endl;
  if( LoadAndTestSphere< ITKQEMesh3d, CGALMeshd >() == EXIT_FAILURE )
  {
    return EXIT_FAILURE;
  }
  std::cout << "Success" << std::endl << std::endl;

  return EXIT_SUCCESS;
}
