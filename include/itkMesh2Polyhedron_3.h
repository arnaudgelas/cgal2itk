#ifndef __itkMesh2CGALPolyhedron_3_h
#define __itkMesh2CGALPolyhedron_3_h

#include <CGAL/Polyhedron_incremental_builder_3.h>

template< class TITKMesh, class TCGALMesh >
class Convert
{
public:
  typedef TITKMesh                                            ITKMeshType;
  typedef typename ITKMeshType::Pointer                  ITKMeshPointer;
  typedef typename ITKMeshType::ConstPointer                  ITKMeshConstPointer;

  typedef typename ITKMeshType::PointType                     ITKPointType;
  typedef typename ITKMeshType::PointsContainerConstPointer   ITKPointsContainerConstPointer;
  typedef typename ITKMeshType::PointsContainerConstIterator  ITKPointsContainerConstIterator;

  typedef typename ITKMeshType::CellType                      ITKCellType;
  typedef typename ITKMeshType::CellsContainerConstPointer    ITKCellsContainerConstPointer;
  typedef typename ITKMeshType::CellsContainerConstIterator   ITKCellsContainerConstIterator;


  typedef TCGALMesh                         CGALMeshType;
  typedef typename CGALMeshType::HalfedgeDS CGALHalfEdgeType;
  typedef typename CGALHalfEdgeType::Vertex CGALVertexType;
  typedef typename CGALVertexType::Point  CGALPointType;

  static void fromITKtoCGAL( ITKMeshType* input, CGALMeshType& oMesh )
  {
    cgal_surface_mesh_builder builder( input );
    oMesh.delegate( builder );
  }

  static void fromCGALToITK( const CGALMeshType& input, ITKMeshType* oMesh )
  {
    
  }

protected:
  Convert();
  ~Convert();

private:

  class cgal_surface_mesh_builder : public CGAL::Modifier_base< CGALHalfEdgeType >
  {
  public:
    cgal_surface_mesh_builder( ITKMeshType* iMesh )
    {
       m_Mesh = iMesh;
    }

    void operator() ( CGALHalfEdgeType& hds )
    {
      CGAL::Polyhedron_incremental_builder_3< CGALHalfEdgeType > builder( hds, true );
      builder.begin_surface(  m_Mesh->GetNumberOfPoints(),
                              m_Mesh->GetNumberOfCells(),
                              0 );

      typename TITKMesh::PointsContainerConstPointer points = m_Mesh->GetPoints();
      typename TITKMesh::PointsContainerConstIterator pIt = points->Begin();
      typename TITKMesh::PointsContainerConstIterator pEnd = points->End();

      while( pIt != pEnd )
      {
        ITKPointType p = pIt->Value();
        builder.add_vertex( CGALPointType( p[0], p[1], p[2] ) );
        ++pIt;
      }

      ITKCellsContainerConstPointer cells = m_Mesh->GetCells();
      ITKCellsContainerConstIterator cIt = cells->Begin();
      ITKCellsContainerConstIterator cEnd = cells->End();

      while( cIt != cEnd )
      {
        builder.begin_facet();

        const ITKCellType* cell = cIt->Value();

        typedef typename ITKCellType::PointIdConstIterator PointIdIterator;
        PointIdIterator faceIt = cell->PointIdsBegin();
        PointIdIterator faceEnd = cell->PointIdsEnd();

        while( faceIt != faceEnd )
        {
          builder.add_vertex_to_facet( *faceIt );
          ++faceIt;
        }
        builder.end_facet();

        ++cIt;
      }
    builder.end_surface();
    }
  private:
    ITKMeshPointer m_Mesh;
  };
};
#endif  __itkMesh2CGALPolyhedron_3_h

