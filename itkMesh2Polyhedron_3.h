#ifndef __itkMesh2CGALPolyhedron_3_h
#define __itkMesh2CGALPolyhedron_3_h

template< class TITKMesh, class TCGALMesh >
class Convert
{
public:
  typedef TITKMesh                                            ITKMeshType;
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
  typedef typename CGALHalfEdgeType::Point  CGALPointType;

  static void fromITKtoCGAL( const ITKMeshType* input, CGALMeshType& oMesh ) const
  {
    cgal_surface_mesh_builder builder( input );
    oMesh.delegate( builder );
  }

  static void fromCGALToITK( const CGALMeshType& input, ITKMeshType* oMesh ) const
  {
    
  }

protected:
  Convert();
  ~Convert();

private:

  class cgal_surface_mesh_builder : public CGAL::Modifier_base<  >
  {
  public:
    ty
    cgal_surface_mesh_builder( const ITKMeshType* iMesh ) : m_Mesh( iMesh ) {}

    void operator() ( CGALHalfEdgeType& hds )
    {
      CGAL::Polyhedron_incremental_builder_3< CGALHalfEdgeType > builder( hds, true );
      builder.begin_surface(  m_Mesh->GetNumberOfPoints(),
                              m_Mesh->GetNumberOfFaces(),
                              2 * m_Mesh->GetNumberOfEdges() );

      typename TITKMesh::PointsContainerConstPointer points = m_Mesh->GetPoints();
      typename TITKMesh::PointsContainerConstIterator pIt = points->Begin();
      typename TITKMesh::PointsContainerConstIterator pEnd = points->End();

      while( pIt != pEnd )
      {
        ITKPointType p = pIt->Value();
        builder.add_vertex( Point( p[0], p[1], p[2] ) );
        ++pIt;
      }

      ITKCellsContainerConstPointer cells = m_Mesh->GetCells();
      ITKCellsContainerConstIterator cIt = cells->Begin();
      ITKCellsContainerConstIterator cEnd = cells->End();

      while( cIt != cEnd )
      {
        builder.begin_facet();

        const ITKCellType* cell = cIt->Value();

        typedef typename ITKCellType::PointIdIterator PointIdIterator;
        PointIdIterator faceIt = cell->PointIdsBegin();
        PointIdIterator faceEnd = cell->PointIdsEnd();

        while( faceIt != faceEnd )
        {
          builder.add_vertex_to_facet( *faceIt );
          ++faceIt;
        }
        builder._facet();

        ++cIt;
      }
    builder.end_surface();
    }
  };
  private:
    ITKMeshConstPointer m_Mesh;
};
#endif  __itkMesh2CGALPolyhedron_3_h

