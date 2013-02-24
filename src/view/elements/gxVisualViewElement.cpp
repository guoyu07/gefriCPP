#include "view/elements/gxVisualViewElement.h"
#include "view/gxLightweightSystem.h"
#include "core/gxLog.h"

gxVisualViewElement::gxVisualViewElement()
  : mBounds( 0, 0, 0, 0 )
{
}

gxVisualViewElement::gxVisualViewElement( const gxRect &aBounds )
{
    mBounds = aBounds;
}

gxVisualViewElement::~gxVisualViewElement()
{
}

void gxVisualViewElement::Paint( gxPainter &aPainter )
{
    if ( !IsVisible() )
        return;

    // Push the painter state as the next line might change it.
    aPainter.PushState();

    // Sets the painter transform flags to my own transform flags.
    // This is needed as we're soon to call NeedsPainting.
    aPainter.SetTransformFlags( mTransformFlags );

    // Only paint if need to (intersect with the bounds of painting area and
    // damaged areas).
    if ( aPainter.NeedsPainting( mBounds ) )
    {
        // Push current painter state so it can be restored later on.
        aPainter.PushState();

        // Sets the clip area of the painter to the bounds
        if ( IsClippingChildren() )
            aPainter.SetClipArea( mBounds );

        PaintSelf( aPainter );
        PaintChildren( aPainter );
        PaintBorder( aPainter );

        // Pop (will also restore) the painter state to before any painting was
        // done.
        aPainter.PopState();
    }
  
    aPainter.PopState();
}

void gxVisualViewElement::PaintChildren( gxPainter &aPainter )
{
    // Return if there are no children.
    if ( IsChildless() )
        return;

    // Push current painter state so it can be poped after translate
    aPainter.PushState();

    // Offset all paint operation by the top-left point of this elemet
    aPainter.SetTranslate( mBounds.GetPosition() );

    forEachChild ( aChild )
    {
        // Paint the child
        aChild->Paint( aPainter );
    }

    // Pop (will also restore) the painter state to before translate.
    aPainter.PopState();
}

void gxVisualViewElement::GetDescendantsBounds( gxRect &aBounds )
{
    // Union with my bounds.
    aBounds.Union( GetBounds() );
  
    // And then with those of all children (in case these are not clipped).
    if ( !IsClippingChildren() )
        gxViewElement::GetDescendantsBounds( aBounds );
}

gxRect gxVisualViewElement::GetBounds() const
{
    return mBounds;
}

void gxVisualViewElement::SetBounds( const gxRect &aNewBounds )
{
    // Check if either translate or resize happened
    bool iTranslate = aNewBounds.GetPosition() != mBounds.GetPosition();
    bool iResize    = aNewBounds.GetSize()     != mBounds.GetSize();
    bool iChanged   = iTranslate || iResize;

    // Erase the view element if either happened
    if ( iChanged )
        Erase();

    // Preform translation
    if ( iTranslate )
    {
        gxPoint iDelta = aNewBounds.GetPosition() - mBounds.GetPosition();
        Translate( iDelta );
    }

    // Preform resize
    if ( iResize )
    {
        mBounds.SetSize( aNewBounds.GetSize() );
    }

    // Repaint
    if ( iChanged )
    {
        Invalidate();
        Repaint();
    }
}

void gxVisualViewElement::Translate( gxPoint aDelta )
{
    mBounds.Translate( aDelta );
    
    // There will be additional handling here for absolute positioned figures,
    // which will also need to translate all of its children.
}