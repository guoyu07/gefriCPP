#ifndef gxRootViewElement_h
#define gxRootViewElement_h

#include "view/elements/structural/gxStructural.h"

/**
 * @brief The root view element in any {@link gxViewElement view element}
 * hierarchy, which links all children to the lightweight system. 
 * 
 * The root view element is not really something that is visiable, but it
 * provides a way for all view elements to communicate with the lightweight
 * system, for example, by keeping a reference to it.
 * Created by the {@link gxLightweightSystem lightweight system}
 */
class gxRootViewElement: public gxStructural
{
public:
    /**
     * @brief The root view elemetn constructor
     * @param aLightweightSystem The {@link gxLightweightSystem lightweight
     * system} associated with the root view element.
     */
    gxRootViewElement( gxLightweightSystem *aLightweightSystem );

    /**
     * @brief Returns the {@link gxLightweightSystem lightweight system}
     * associated with this view element (and all its children).
     */
    virtual gxLightweightSystem* GetLightweightSystem() const;

    /**
     * @brief Overrides the base class implementation to return itself.
     */
    virtual const gxRootViewElement* GetRootViewElement() const;

    /**
     * @brief Returns the bounds of this view element by getting the bounds of the
     * lightweight control, but with the origin set to (0,0).
     */
    virtual gxRect GetBounds() const;

    // Validate is promoted to public as the lightweight system needs to call it.
    virtual void Validate() { gxStructural::Validate(); }
protected:
    virtual void TransformToAbsolute( gxRect       &aRect,
                                      gxTransFlags &aTransFlags);
    virtual void Invalidate();
private:
    /// The {@link gxLightweightSystem lightweight system} associated with this
    /// root view element.
    gxLightweightSystem *mLightweightSystem;
};

#endif // gxRootViewElement_h