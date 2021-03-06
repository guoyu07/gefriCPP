#ifndef gxScroller_h
#define gxScroller_h

#include "View/Elements/Structural/gxStructural.h"
#include "core/observer/gxObserver.h"
#include "View/Managers/gxScrollManager.h"
#include "View/gxTransformations.h"

/**
 * @brief A view element that scrolls all its children.
 * 
 * Scrollers are inserted into the hierarchy of of view elements to facilitate
 * scrolling.
 */
class gxScroller: public gxStructural
{
    gxDeclareObserver( gxScroller )
public:
    gxScroller();
    gxScroller( gxScrollManager *aScrollManager );
    ~gxScroller();

    /**
     * @brief Sets the {@link gxScrollManager scroll manager} for this scroller.
     * @param aScrollManager The new {@link gxScrollManager scroll manager}.
     */
    void SetScrollManager( gxScrollManager *aScrollManager );
  
    /**
     * @brief Sets the scroll.
     * @param aScrollX The X axis scroll
     * @param aScaleY The Y axis scroll
     */
    void SetScroll( gxPoint const &aScrollPosition );

    /**
     * @brief A handler method for scroll position events from the
     * {@link gxScrollManager scroll manager}.
     * 
     * @param aScroll The new scroll.
     */
    void OnScrollChanged( const gxScroll *aScroll );

    /**
     * @brief Paints the view element by translating the painter then calling
     * {@link gxViewElement::PaintChildren() PaintChildren()}.
     * 
     * @param aPainter The {@link gxPainter painter} to be used for drawing.
     */
    void Paint( gxPainter &aPainter );
    
    
    virtual void GetDescendantsBounds( gxRect &aBounds );
protected:
    virtual void Transform( gxRect &aRect );

    void DoValidate();

    /**
     * @brief This method is called when the scroller needs to readjust the
     * scroll bars associated with it.
     */
    void ReadjustScrollbars();

    gxScrollManager *mScrollManager;
private:
    gxPoint mScrollPosition;
};

#endif // gxScroller_h