#include "view/gxPainter.h"
#include <wx/log.h>
#include <math.h>
#include "core/gxLog.h"

gxPainter::gxPainter()
: mNeedsTranslating(false),
  mNeedsScaling(false),
  mNeedsScrolling(false),
  mTransformEnabledFlags(gxTransformFlags::All)
{
}

void gxPainter::UpdateTransformationsNeeded()
{
  mNeedsTranslating = ( (mTrans.Translate.X != 0) || (mTrans.Translate.Y != 0) ) &&
                      mTransformEnabledFlags.IsSet(gxTransformFlags::Translate);

  mNeedsScrolling = ( (mTrans.Scroll.X != 0) || (mTrans.Scroll.Y != 0) ) &&
                    mTransformEnabledFlags.IsSet(gxTransformFlags::Scroll);

  mNeedsScaling = ( (mTrans.Scale.X != 1) || (mTrans.Scale.Y != 1) ) &&
                  mTransformEnabledFlags.IsSet(gxTransformFlags::Scale);
}

void gxPainter::SetTranslate(int dx, int dy)
{
  if (mNeedsScaling)
  {
    // Take into account any scaling that is in force.
    // Say the value given is (40,40), with a scale set to 2 the resultant
    // position will be (80,80). Makes sense innit?
    mTrans.Translate.X += (int)floor( dx * mTrans.Scale.X);
    mTrans.Translate.Y += (int)floor( dy * mTrans.Scale.Y);
  } else {
    mTrans.Translate.X += dx;
    mTrans.Translate.Y += dy;
  }

  UpdateTransformationsNeeded();
}

void gxPainter::SetScroll(int sx, int sy)
{
  if (mNeedsScaling)
  {
    // Take into account any scaling that is in force.
    // Say the value given is (40,40), with a scale set to 2 the resultant
    // position will be (80,80). Makes sense innit?
    mTrans.Scroll.X += (int)floor( sx * mTrans.Scale.X);
    mTrans.Scroll.Y += (int)floor( sy * mTrans.Scale.Y);
  } else {
    mTrans.Scroll.X += sx;
    mTrans.Scroll.Y += sy;
  }

  UpdateTransformationsNeeded();
}

void gxPainter::SetScale(float sx, float sy)
{
  mTrans.Scale.X *= sx;
  mTrans.Scale.Y *= sy;

  UpdateTransformationsNeeded();
}

void gxPainter::PushState()
{
  //gxRect clipRect = GetDcClipRect();

  gxPainterState *s = new gxPainterState();

  s->transformations = mTrans;
  s->transformEnabledFlags = mTransformEnabledFlags;
  s->clipArea = GetClipRect();

  mStateStack.push(s);
}

void gxPainter::PopState()
{
  // Get the top state from the stack
  gxPainterState *s = mStateStack.top();
  
  // Restore it
  RestoreState(s);

  // Remove it from stack;
  mStateStack.pop();

  delete s;
  s = NULL;
}

void gxPainter::RestoreState()
{
  // Get the top state from the stack
  gxPainterState *s = mStateStack.top();
  // Restore it
  RestoreState(s);
}

void gxPainter::RestoreState(gxPainterState *aState)
{
  mTrans = aState->transformations;
  mTransformEnabledFlags = aState->transformEnabledFlags;
  UpdateTransformationsNeeded();

  SetAbsoluteClipArea(aState->clipArea);
}

void gxPainter::SetClipArea(gxRect const &aRect)
{
  // Transforms the rect, taking into account trnaslation, scale, etc.
  gxRect transformedRect(aRect);
  Transform(transformedRect);
  
  IntersectClipArea(transformedRect);
}

void gxPainter::SetTransformFlags(gxTransformFlags aFlags)
{
  mTransformEnabledFlags = aFlags;
  UpdateTransformationsNeeded();
}

void gxPainter::Transform(gxRect &aRect)
{
  if (mNeedsScaling)
    aRect.Scale(mTrans.Scale.X, mTrans.Scale.Y);

  if (mNeedsTranslating)
    aRect.Offset(mTrans.Translate.X, mTrans.Translate.Y); 

  if (mNeedsScrolling)
    aRect.Offset(-mTrans.Scroll.X, -mTrans.Scroll.Y); 
}

void gxPainter::Transform(gxPoint &aPoint)
{
  if (mNeedsScaling)
  {
    aPoint.x = (int)(aPoint.x * mTrans.Scale.X);
    aPoint.y = (int)(aPoint.y * mTrans.Scale.Y);
  }

  if (mNeedsTranslating)
  {
    aPoint.x += mTrans.Translate.X;
    aPoint.y += mTrans.Translate.Y;
  }

  if (mNeedsScrolling)
  {
    aPoint.x -= mTrans.Scroll.X;
    aPoint.y -= mTrans.Scroll.Y;
  }
}