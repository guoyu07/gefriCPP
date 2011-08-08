#include "view/elements/gxViewElement.h"
#include "view/gxLightweightSystem.h"
#include "core/gxAssert.h"
#include "core/gxLog.h"

gxViewElement::gxViewElement()
: mFlags(0)
{
}

gxViewElement::~gxViewElement()
{
}

const gxRootViewElement* gxViewElement::GetRootViewElement() const
{
  if (GetParent() != NULL)
  {
    return GetParent()->GetRootViewElement();
  } else {
    return NULL;
  }
}

gxLightweightSystem* gxViewElement::GetLightweightSystem() const
{
  // Get root view element and return if no such found.
  const gxRootViewElement *iRoot = GetRootViewElement();
  gxASSERT(iRoot == NULL, "gxViewElement: could not find root element");

  // Get the lightweight system and return if no such found.
  gxLightweightSystem *iLws = iRoot->GetLightweightSystem();
  gxASSERT(iLws == NULL, "gxViewElement: Could not find the lightweight system");

  return iLws;
}

void gxViewElement::TransformToAbsolute(gxRect &aRect, gxTransformFlags &aTransFlags)
{ 
  gxASSERT(GetParent() == NULL, "gxViewElement::TransformToAbsolute called, but no parent");

  GetParent()->Transform(aRect, aTransFlags);
  GetParent()->TransformToAbsolute(aRect, aTransFlags);
}

void gxViewElement::Transform(gxRect &aRect, gxTransformFlags &aTransFlags)
{
  if ( aTransFlags.IsSet(gxTransformFlags::Translate) )
    aRect.Offset(GetBounds().GetPosition());
}

void gxViewElement::Erase()
{
  // Repaint really does what we need - takes the element's bounds and adds
  // dirty region to queue repaint.
  Repaint();
}

void gxViewElement::Repaint()
{
  // No point repainting the figure if it is invalid.
  if (!IsValid())
    return;
    
  gxRect iBounds = GetBounds();
  Repaint(iBounds);
}

void gxViewElement::Repaint(gxRect &aBounds)
{
  // No point repainting the figure if it is invalid.
  if (!IsValid())
    return;

  // Translate the bounds to absolute coordinates.
  TransformToAbsolute(aBounds, mTransformFlags);

  // instruct the lightweight system to mark the bounds of this view element
  // as ones need repainting
  GetLightweightSystem()->AddDirtyRegion(aBounds);
}

void gxViewElement::GetDescendantsBounds(gxRect &aBounds)
{
  for (EACHCHILD)
  {
    gxRect iChildBounds;
    CHILD->GetDescendantsBounds(iChildBounds);

    // When getting the children bounds we want all transformations to be done
    // but scroll.
    gxTransformFlags iFlags(gxTransformFlags::All & ~gxTransformFlags::Scroll);

    Transform(iChildBounds, iFlags);

    aBounds.Union(iChildBounds);
  }
}

void gxViewElement::Invalidate()
{
  mFlags.Unset(gxViewElement::Valid);
}

void gxViewElement::Revalidate()
{
  Invalidate();

  // View elements parent might be null before all elements are inserted to the
  // hierarchy tree (when they are still created and added to their parents).
  if (GetParent() != NULL)
  {
    // Revalidate further up the hierarchy tree.
    GetParent()->Revalidate();
  }
}

void gxViewElement::Validate()
{
  if (IsValid())
    return;

  bool iNeedsRepainting = false;

  // Validate myself
  iNeedsRepainting = ValidateSelf();
  
  // Set myself as Valid
  mFlags.Set(gxViewElement::Valid);

  // Ask all children to validate themselves.
  for (EACHCHILD)
  {
    CHILD->Validate();
  }

  if (iNeedsRepainting)
    Repaint();
}

bool gxViewElement::IsValid()
{
  return mFlags.IsSet(gxViewElement::Valid);
}

void gxViewElement::OnAddChild(gxViewElement *aChild)
{
  // Newly created children are invalid. So once a child is added try to
  // revalidate it. If the child is not inserted into a composition which
  // has root view element at the very top of the hierarchy tree, the view
  // element will remain invalid and therefore will not be repainted as
  // Repaint will return.
  aChild->Revalidate();
  aChild->Repaint();
}

void gxViewElement::OnBeforeChildRemoval(gxViewElement *aChild)
{
  aChild->Erase();
}

void gxViewElement::OnAfterChildRemoval()
{
  Revalidate();
}