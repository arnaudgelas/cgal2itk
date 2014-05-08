#ifndef ITKIMAGE2CGALIMAGE3_H
#define ITKIMAGE2CGALIMAGE3_H

#include <CGAL/Image_3.h>
#include <itkNumericTraits.h>

namespace
{
template< class TPixel >
struct ITKPixelToCGALImageIOTypeMapper
{};

template<>
struct ITKPixelToCGALImageIOTypeMapper< bool >
{
  static const WORD_KIND    word_kind = WK_UNKNOWN;
  static const SIGN         sign = SGN_UNKNOWN;
  static const unsigned int wdim = 0;
};

template<>
struct ITKPixelToCGALImageIOTypeMapper< char >
{
  static const WORD_KIND    word_kind = WK_FIXED;
  static const SIGN         sign = SGN_SIGNED;
  static const unsigned int wdim = 1;
};

template<>
struct ITKPixelToCGALImageIOTypeMapper< unsigned char >
{
  static const WORD_KIND    word_kind = WK_FIXED;
  static const SIGN         sign = SGN_UNSIGNED;
  static const unsigned int wdim = 1;
};


template<>
struct ITKPixelToCGALImageIOTypeMapper< short >
{
  static const WORD_KIND    word_kind = WK_FIXED;
  static const SIGN         sign = SGN_SIGNED;
  static const unsigned int wdim = 2;
};

template<>
struct ITKPixelToCGALImageIOTypeMapper< unsigned short >
{
  static const WORD_KIND    word_kind = WK_FIXED;
  static const SIGN         sign = SGN_UNSIGNED;
  static const unsigned int wdim = 2;
};

template<>
struct ITKPixelToCGALImageIOTypeMapper< int >
{
  static const WORD_KIND    word_kind = WK_FIXED;
  static const SIGN         sign = SGN_SIGNED;
  static const unsigned int wdim = 4;
};

template<>
struct ITKPixelToCGALImageIOTypeMapper< unsigned int >
{
  static const WORD_KIND    word_kind = WK_FIXED;
  static const SIGN         sign = SGN_UNSIGNED;
  static const unsigned int wdim = 4;
};

template<>
struct ITKPixelToCGALImageIOTypeMapper< long >
{
  static const WORD_KIND    word_kind = WK_FIXED;
  static const SIGN         sign = SGN_SIGNED;
  static const unsigned int wdim = 8;
};

template<>
struct ITKPixelToCGALImageIOTypeMapper< unsigned long >
{
  static const WORD_KIND    word_kind = WK_FIXED;
  static const SIGN         sign = SGN_UNSIGNED;
  static const unsigned int wdim = 8;
};

template<>
struct ITKPixelToCGALImageIOTypeMapper< float >
{
  static const WORD_KIND    word_kind = WK_FIXED;
  static const SIGN         sign = SGN_SIGNED;
  static const unsigned int wdim = 4;
};

template<>
struct ITKPixelToCGALImageIOTypeMapper< double >
{
  static const WORD_KIND    word_kind = WK_FIXED;
  static const SIGN         sign = SGN_SIGNED;
  static const unsigned int wdim = 8;
};

}


template< class TITKImage, class CGALImage = CGAL::Image_3 >
class ConvertImage
{
public:
  static bool fromITKToCGAL( TITKImage* input, CGALImage& output )
  {
    if( !input )
      {
      return false;
      }
    ::_image* image = ::_initImage();

    typedef typename TITKImage::RegionType   RegionType;
    typedef typename TITKImage::SizeType     SizeType;
    typedef typename TITKImage::SpacingType  SpacingType;

    RegionType region = input->GetLargestPossibleRegion();
    SizeType size = region.GetSize();
    SpacingType spacing = input->GetSpacing();

    image->vectMode = VM_SCALAR;
    image->xdim = size[0];
    image->ydim = size[1];
    image->zdim = size[2];

    image->vdim = itk::NumericTraits< typename TITKImage::PixelType >::GetLength();

    image->vx = spacing[0];
    image->vy = spacing[1];
    image->vz = spacing[2];

    image->endianness = ::_getEndianness();

    ITKPixelToCGALImageIOTypeMapper< typename TITKImage::PixelType > mapper;
    image->wdim = mapper.wdim;
    image->wordKind = mapper.word_kind;
    image->sign = mapper.sign;

    image->data = ::ImageIO_alloc(size[0]*size[1]*size[2]*image->wdim);
    typename TITKImage::PixelType* buffer = input->GetBufferPointer();
    std::copy( buffer, buffer + size[0]*size[1]*size[2], (typename TITKImage::PixelType*) image->data );

    output = CGALImage( image );

    return true;
  }
};

#endif // ITKIMAGE2CGALIMAGE3_H
