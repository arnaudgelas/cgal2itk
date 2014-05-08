#include <itkImage.h>
#include "itkImage2CGALImage3.h"

int main( int argc, char* argv[] )
{
  const unsigned int Dimension = 3;
  typedef unsigned char PixelType;
  typedef itk::Image< PixelType, Dimension > ITKImageType;

  ITKImageType::IndexType start;
  start.Fill( 0 );

  ITKImageType::SizeType size;
  size.Fill( 100 );

  ITKImageType::RegionType region(start, size);

  ITKImageType::Pointer image = ITKImageType::New();
  image->SetRegions(region);
  image->Allocate();
  image->FillBuffer(0);

  // Make a white square
  for(unsigned int r = 40; r < 60; r++)
    {
    for(unsigned int c = 40; c < 60; c++)
      {
      for( unsigned int d = 40; d < 60; d++ )
        {
        ITKImageType::IndexType pixelIndex;
        pixelIndex[0] = r;
        pixelIndex[1] = c;
        pixelIndex[1] = d;
        image->SetPixel(pixelIndex, 255);
        }
      }
    }

  CGAL::Image_3 output;
  ConvertImage< ITKImageType, CGAL::Image_3 >::fromITKToCGAL( image.GetPointer(), output );

  return EXIT_SUCCESS;
}
