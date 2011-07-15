#include "view/elements/gxScroller.h"
#include "core/gxCallback.h"
#include "core/gxAssert.h"
#include "core/gxLog.h"

gxScroller::gxScroller()
  : mScrollX(0), mScrollY(0), mScrollManager(NULL)
{
}

gxScroller::gxScroller(gxScrollManager *aScrollManager)
  : mScrollX(0), mScrollY(0), mScrollManager(NULL)
{
  SetScrollManager(aScrollManager);
}

gxScroller::~gxScroller()
{
    // Remove the callback from the previous scroll manager (if any).
  if (mScrollManager)
    mScrollManager->mObservers.Remove( gxCALLBACK(gxScroller, OnScrollManagerUpdate) );
}

void gxScroller::SetScrollManager(gxScrollManager *aScrollManager)
{
  // Remove the callback from the previous scroll manager (if any).
  if (mScrollManager)
    mScrollManager->mObservers.Remove( gxCALLBACK(gxScroller, OnScrollManagerUpdate) );

  mScrollManager = aScrollManager;
  aScrollManager->mObservers.Add( gxCALLBACK(gxScroller, OnScrollManagerUpdate) );
}

void gxScroller::SetScroll(float aScrollX, float aScrollY)
{
  if (mScrollX != aScrollX || mScrollY != aScrollY)
  {
    Erase();
    mScrollX = aScrollX;
    mScrollY = aScrollY;
    Repaint();
  }
}

void gxScroller::OnScrollManagerUpdate(const gxNotification *aNotification)
{
  SetScroll(mScrollManager->GetScrollX(), mScrollManager->GetScrollY());
}

void gxScroller::Paint(gxPainter &aPainter)
{
  // Push current painter state so it can be restored after setting the
  // scroll values
  aPainter.PushState();

  aPainter.SetTranslate(-mScrollX, -mScrollY);

  PaintChildren(aPainter);

  // Pop (will also restore) the painter state to before SetTranslate().
  aPainter.PopState();
}

void gxScroller::TransformChild(gxBounds &aBounds)
{
  // Structural areas don't need scrolling
  if (aBounds.Structural)
    return;  

  if (mScrollX != 0 || mScrollY != 0)
  {
    aBounds.Offset(-mScrollX, -mScrollY);
  }
}

// TODO: Replace this with Bounds.IgnoreScroll() or something.
// Where the bounds create in ReadjustScrollbars has IgnoreScroll set
// So transform won't scroll it, and so we won't need to have this method.
void gxScroller::GetChildrenBounds(gxBounds &aBounds)
{
  for (EACHCHILD)
  {
    gxBounds childBounds;
    CHILD->GetChildrenBounds(childBounds);
    aBounds.Union(childBounds);
  }
}

void gxScroller::ReadjustScrollbars()
{
  gxBounds bounds;
  GetChildrenBounds(bounds);
  
  gxSize mySize = GetBounds().GetSize();

  if (mScrollManager)
    mScrollManager->AdjustScrollbars(mySize.x, bounds.x + bounds.width, mySize.y, bounds.y + bounds.height);
}