#include "View/Layouts/Operations/gxAlign.h"

gxAlign::gxAlign( const Type                     aType,
                  const gxRect&                  aRect,
                        gxViewElement::Iterator& aLayoutees,
                  const gxConstraints&           aConstraints,
                  const bool                     onMajorAxis,
                  const bool                     aRelative )
{
    if ( aType == None )
        return;
    
    gxPix iPosition = 0;
    gxPix iContainerSize = aRect.GetSize( onMajorAxis );
    gxPix iContainerPosition = aRect.GetPosition( onMajorAxis );
    
    gxPix iOffset = aRelative ? 0 : iContainerPosition;
    
    
    for ( aLayoutees.First(); aLayoutees.Current(); aLayoutees.Next() )
    {
        gxPix iSize = aLayoutees.Current()->GetSize( onMajorAxis );
        
        switch ( aType )
        {
            case None:                                               break;
            case Start:  iPosition = 0;                              break;
            case Middle: iPosition = ( iContainerSize - iSize ) / 2; break;
            case End:    iPosition = ( iContainerSize - iSize );     break;
        }
        
        aLayoutees.Current()->SetPosition( iPosition + iOffset, onMajorAxis );
    }
}
