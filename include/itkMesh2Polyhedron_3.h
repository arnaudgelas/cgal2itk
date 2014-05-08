#ifndef __itkMesh2CGALPolyhedron_3_h
#define __itkMesh2CGALPolyhedron_3_h

#include <CGAL/Polyhedron_incremental_builder_3.h>

#include <itkTriangleCell.h>
#include <itkPolygonCell.h>
#include <itkQuadrilateralCell.h>

template< class TITKMesh, class TCGALMesh >
class Convert
{
public:
  typedef TITKMesh                                            ITKMeshType;
  typedef typename ITKMeshType::Pointer                       ITKMeshPointer;
  typedef typename ITKMeshType::ConstPointer                  ITKMeshConstPointer;

  typedef typename ITKMeshType::PointIdentifier               ITKPointIdentifier;
  typedef typename ITKMeshType::PointType                     ITKPointType;
  typedef typename ITKMeshType::PointsContainerConstPointer   ITKPointsContainerConstPointer;
  typedef typename ITKMeshType::PointsContainerConstIterator  ITKPointsContainerConstIterator;

  typedef typename ITKMeshType::CellIdentifier                ITKCellIdentifier;
  typedef typename ITKMeshType::CellType                      ITKCellType;
  typedef typename ITKMeshType::CellAutoPointer               ITKCellAutoPointer;
  typedef typename ITKMeshType::CellsContainerConstPointer    ITKCellsContainerConstPointer;
  typedef typename ITKMeshType::CellsContainerConstIterator   ITKCellsContainerConstIterator;

  typedef itk::TriangleCell< ITKCellType >                    ITKTriangleCellType;
  typedef itk::PolygonCell< ITKCellType >                     ITKPolygonCellType;
  typedef itk::QuadrilateralCell< ITKCellType >               ITKQuadrilateralCellType;


  typedef TCGALMesh                         CGALMeshType;
  typedef typename CGALMeshType::HalfedgeDS CGALHalfEdgeType;
  typedef typename CGALHalfEdgeType::Vertex CGALVertexType;
  typedef typename CGALVertexType::Point    CGALPointType;

  static void fromITKtoCGAL( ITKMeshType* input, CGALMeshType& oMesh )
  {
    cgal_surface_mesh_builder builder( input );
    oMesh.delegate( builder );
  }

  static void fromCGALToITK( const CGALMeshType& input, ITKMeshType* oMesh )
  {
    typedef typename CGALMeshType::Vertex_const_iterator CGALMeshVertexConstIterator;

    CGALMeshVertexConstIterator vIt = input.vertices_begin();
    const CGALMeshVertexConstIterator vEnd = input.vertices_end();

    typename ITKMeshType::PointsContainerPointer points = oMesh->GetPoints();
    points->Reserve( static_cast< ITKPointIdentifier >( input.size_of_vertices() ) );

    std::map< typename CGALMeshType::Vertex_const_handle, ITKPointIdentifier > vertexMap;

    ITKPointIdentifier i = 0;
    while( vIt != vEnd )
      {
      ITKPointType p;
      p[0] = vIt->point().x();
      p[1] = vIt->point().y();
      p[2] = vIt->point().z();

      points->SetElement( i, p );

      vertexMap[ vIt ] = i;

      ++i;
      ++vIt;
      }

    oMesh->SetPoints( points );

    typename ITKMeshType::CellsContainerPointer cells = oMesh->GetCells();
    cells->Reserve( static_cast< ITKCellIdentifier >( input.size_of_facets() ) );

    typedef typename CGALMeshType::Facet_const_iterator CGALMeshFacesConstIterator;
    CGALMeshFacesConstIterator fIt  = input.facets_begin();
    const CGALMeshFacesConstIterator fEnd = input.facets_end();

    typedef enum { TRIANGLE_CELL = 3, QUADRILATERAL_CELL, POLYGON_CELL } CellGeometryType;

    ITKCellIdentifier k = 0;
    while( fIt != fEnd )
      {
      typename CGALMeshType::Halfedge_around_facet_const_circulator circulator = fIt->facet_begin();

      CellGeometryType type = static_cast< CellGeometryType >( fIt->facet_degree() );

      ITKCellAutoPointer cell;

      switch( type )
      {
        case TRIANGLE_CELL:
          {
          ITKTriangleCellType* triangle = new ITKTriangleCellType;
          unsigned int j = 0;
          do
            {
            typename CGALMeshType::Vertex_const_handle v = circulator->vertex();
            triangle->SetPointId( j++, vertexMap[ v ] );
            } while( circulator != fIt->facet_begin() );

          cell.TakeOwnership( triangle );
          oMesh->SetCell( k++, cell );
          break;
          }

        case QUADRILATERAL_CELL:
          {
          ITKQuadrilateralCellType* quad = new ITKQuadrilateralCellType;
          unsigned int j = 0;
          do
            {
            typename CGALMeshType::Vertex_const_handle v = circulator->vertex();
            quad->SetPointId( j++, vertexMap[ v ] );
            } while( circulator != fIt->facet_begin() );

          cell.TakeOwnership( quad );
          oMesh->SetCell( k++, cell );
          break;
          }

        default:
        case POLYGON_CELL:
          {
          ITKPolygonCellType* polygon = new ITKPolygonCellType;
          unsigned int j = 0;
          do
            {
            typename CGALMeshType::Vertex_const_handle v = circulator->vertex();
            polygon->SetPointId( j++, vertexMap[ v ] );
            } while( circulator != fIt->facet_begin() );

          cell.TakeOwnership( polygon );
          oMesh->SetCell( k++, cell );
          break;
          }
        }

      ++fIt;
      }
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
#endif  // __itkMesh2CGALPolyhedron_3_h
