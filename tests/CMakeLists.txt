set( _tests
  itkMesh2CGALMesh
  itkImage2CGALImage
)

foreach( t ${_tests} )
  add_executable( ${t} ${t}.cxx )
  target_link_libraries( ${t} ${ITK_LIBRARIES} ${CGAL_LIBRARIES} )

  add_test( NAME ${t}
            COMMAND ${t} )
endforeach()
