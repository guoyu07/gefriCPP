#include "view/gxLightweightSystem.h"
#include "gxLightweightControl.h"
#include "view/gxDcPainter.h"

gxLightweightSystem::gxLightweightSystem(gxLightweightControl *aControl)
  : mContents(NULL), mScrollManager(NULL)
{
  // Sets this a the lightweight system of the control so delegation can start.
  aControl->SetLightweightSystem(this);

  // Keep a reference to the control so we can call methods like Refresh.
  mControl = aControl;

  // Create the root view element
  SetRootViewElement(new gxRootViewElement);
}

gxLightweightSystem::~gxLightweightSystem()
{
  // Unset the lightweight system on the control, so event like paint won't be
  // delegated to an object that has been destroyed
  mControl->UnsetLightweightSystem();
  mControl = NULL;
}

void gxLightweightSystem::SetContents(gxViewElement *aViewElement)
{
  // Remove any view element already set.
  if (mContents != NULL)
    mRootViewElement->RemoveChild(mContents);

  // Add the new view element as a child to the root view element.
  mContents = aViewElement;
  mRootViewElement->AddChild(mContents);
}

void gxLightweightSystem::SetRootViewElement(gxRootViewElement *aRootViewElement)
{
  aRootViewElement->SetLightweightSystem(this);
  mRootViewElement = aRootViewElement;
}

void gxLightweightSystem::SetScrollManager(gxScrollManager *aScrollManager)
{
  // Remove notification from previous scroll manager, (if such exist).
  if (mScrollManager)
    mScrollManager->mObservers.Remove( gxCALLBACK(gxLightweightSystem, OnScrollRangeChanged) );
    
  mScrollManager = aScrollManager;
  mScrollManager->mObservers.Add( gxCALLBACK(gxLightweightSystem, OnScrollRangeChanged) );
}

void gxLightweightSystem::OnScrollRangeChanged(const gxNotification *aNotification)
{
  const gxScrollRangeChangedNotification* iNotification = dynamic_cast<const gxScrollRangeChangedNotification*> (aNotification);
  if ( iNotification )
  {
    mControl->SetScrollbar(wxHORIZONTAL, iNotification->mScroll.X, iNotification->mVisible.X, iNotification->mRange.X);
    mControl->SetScrollbar(wxVERTICAL, iNotification->mScroll.Y, iNotification->mVisible.Y, iNotification->mRange.Y);
  }
}

void gxLightweightSystem::Paint(gxPaintDC *aDc, gxRects const &aDamagedRects)
{
  // Create a painter
  gxDcPainter iPainter(aDc, aDamagedRects);

  // Ask the root view element to paint itself using the painter.
  mRootViewElement->Paint(iPainter);
}

void gxLightweightSystem::AddDirtyRegion(gxRect &aRect)
{
  // This will invalidate the rect and fire a paint event (the current event
  // execution will continue).
  mControl->RefreshRect(aRect);
}

gxRect gxLightweightSystem::GetControlBounds() const
{
  return mControl->GetBounds();
}

gxSize gxLightweightSystem::GetTextSize(std::string aText)
{
  gxSize iSize = mControl->GetTextSize(aText);
  return gxSize(iSize.X, iSize.Y);
}

void gxLightweightSystem::OnScroll (const bool isVertical, const int aPosition)
{
  if (mScrollManager)
    mScrollManager->SetScroll(isVertical, aPosition);
}