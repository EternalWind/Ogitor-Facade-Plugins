#include "FaTriggerArea.h"
#include "OBBoxRenderable.h"

#include <stdio.h>

using namespace std;

TiXmlElement* TriggerAreaEditor::exportDotScene(TiXmlElement *pParent)
{
    Ogre::String shape;
    PropertyOptionsVector *option = TriggerAreaEditorFactory::GetAreaShapes();
    
    for(auto iter = option->begin() ; iter != option->end() ; ++iter)
    {
        if(*Ogre::any_cast<int>(&(iter->mValue)) == mShape->get())
        {
            shape = iter->mKey;
        }
    }

    // player object 
    TiXmlElement *pPlayer = pParent->InsertEndChild(TiXmlElement("trigger_area"))->ToElement();
    pPlayer->SetAttribute("name", mName->get().c_str());
    pPlayer->SetAttribute("id", Ogre::StringConverter::toString(mObjectID->get()).c_str());
    pPlayer->SetAttribute("shape", shape.c_str());
    pPlayer->SetAttribute("enabled", Ogre::StringConverter::toString(mIsEnabled->get()).c_str());

    // position
    TiXmlElement *pPosition = pPlayer->InsertEndChild(TiXmlElement("position"))->ToElement();
    pPosition->SetAttribute("x", Ogre::StringConverter::toString(mPosition->get().x).c_str());
    pPosition->SetAttribute("y", Ogre::StringConverter::toString(mPosition->get().y).c_str());
    pPosition->SetAttribute("z", Ogre::StringConverter::toString(mPosition->get().z).c_str());

    // scale
    TiXmlElement *pScale = pPlayer->InsertEndChild(TiXmlElement("scale"))->ToElement();
    pScale->SetAttribute("x", Ogre::StringConverter::toString(mScale->get().x).c_str());
    pScale->SetAttribute("y", Ogre::StringConverter::toString(mScale->get().y).c_str());
    pScale->SetAttribute("z", Ogre::StringConverter::toString(mScale->get().z).c_str());

    return pPlayer;
}


Ogre::AxisAlignedBox TriggerAreaEditor::getAABB()
{
    if(mEntity)
    {
        return mEntity->getBoundingBox();
    } 
    else 
        return Ogre::AxisAlignedBox::BOX_NULL;
}

void TriggerAreaEditor::setDerivedPosition(Ogre::Vector3 val)
{
    if(getParent())
    {
        Ogre::Quaternion qParent = getParent()->getDerivedOrientation().Inverse();
        Ogre::Vector3 vParent = getParent()->getDerivedPosition();
        Ogre::Vector3 newPos = (val - vParent);
        val = qParent * newPos;
    }
    mPosition->set(val);
}

Ogre::Vector3 TriggerAreaEditor::getDerivedPosition()
{
    if(mHandle)
        return mHandle->_getDerivedPosition();
    else
        return getParent()->getDerivedPosition() + (getParent()->getDerivedOrientation() * mPosition->get());
}

void TriggerAreaEditor::setSelectedImpl(bool bSelected)
{
    CBaseEditor::setSelectedImpl(bSelected);

    showBoundingBox(bSelected);
}

bool TriggerAreaEditor::setHighlightedImpl(bool highlight )
{
    showBoundingBox(highlight);

    return true;
}

void TriggerAreaEditor::showBoundingBox(bool bShow)
{
    if(!mBoxParentNode)
        createBoundingBox();

    if(mBBoxNode)
    {
        int matpos = 0;
        if(mHighlighted->get())
        {
            ++matpos;
            if(mSelected->get())
                ++matpos;
        }
        
        mOBBoxRenderable->setMaterial(mOBBMaterials[matpos]);
        mBBoxNode->setVisible(( bShow || mSelected->get() || mHighlighted->get() ));
    }
}

void TriggerAreaEditor::createProperties(OgitorsPropertyValueMap &params)
{
    PROPERTY_PTR(mIsEnabled, "enabled", bool, false, 0, SETTER(bool, TriggerAreaEditor, _setEnabled));
    PROPERTY_PTR(mShape, "shape", int, (int)TriggerAreaEditor::BOX, 0, SETTER(int, TriggerAreaEditor, _setShape));
    PROPERTY_PTR(mPosition, "position", Ogre::Vector3, Ogre::Vector3::ZERO, 0, SETTER(Ogre::Vector3, TriggerAreaEditor, _setPosition));
    PROPERTY_PTR(mScale, "scale", Ogre::Vector3, Ogre::Vector3(10.0f, 10.0f, 10.0f), 0, SETTER(Ogre::Vector3, TriggerAreaEditor, _setScale));
    mProperties.initValueMap(params);
}

bool TriggerAreaEditor::_setPosition(OgitorsPropertyBase* property, const Ogre::Vector3& position)
{
    if(mHandle)
    {
      mHandle->setPosition(position);
    }

    _updatePaging();

    return true;
}

bool TriggerAreaEditor::_setScale(OgitorsPropertyBase* property, const Ogre::Vector3& scale)
{
    if(mHandle)
    {
        mHandle->setScale(scale);
    }

    //_updatePaging();

    return true;
}

bool TriggerAreaEditor::_setShape(OgitorsPropertyBase* property, const int& shape)
{
    if(mHandle)
    {
        auto manager = mOgitorsRoot->GetSceneManager();

        if(mEntity)
        {
            mHandle->detachObject(mEntity);
            manager->destroyEntity(mName->get());
        }

        mEntity = manager->createEntity(mName->get(), Ogre::String("TriggerArea") + Ogre::StringConverter::toString(shape)
            + Ogre::String(".mesh"));

        mHandle->attachObject(mEntity);

        mHandle->setPosition(mPosition->get());
        mHandle->setScale(mScale->get());
    }

    return true;
}

bool TriggerAreaEditor::_setEnabled(OgitorsPropertyBase* property, const bool& is_enabled)
{
    if(this->getParent()->getNode())
        return true;
    else
        return false;
}

void TriggerAreaEditor::setParentImpl(CBaseEditor *oldparent, CBaseEditor *newparent)
{
    if(!mHandle)
        return;

    if(oldparent) 
        oldparent->getNode()->removeChild(mHandle);
    
    if(newparent) 
        newparent->getNode()->addChild(mHandle);
}

bool TriggerAreaEditor::load(bool async)
{
    cout << mName->get() << endl;
    if(mLoaded->get())
        return true;
    if(!getParent()->load())
        return false;

    //Create Physics Object
    mEntity = mOgitorsRoot->GetSceneManager()->createEntity(mName->get(), Ogre::String("TriggerArea") + Ogre::StringConverter::toString(mShape->get())
        + Ogre::String(".mesh"));

    mHandle = getParent()->getNode()->createChildSceneNode(mName->get());
    mHandle->attachObject(mEntity);

    mHandle->setPosition(mPosition->get());
    mHandle->setScale(mScale->get());

    mLoaded->set(true);

    return true;
}

bool TriggerAreaEditor::unLoad()
{
    if(!mLoaded->get())
        return true;

    destroyBoundingBox();

    if(mHandle)
    {
        Ogre::Node *parent = mHandle->getParent();
        parent->removeChild(mHandle);
        mOgitorsRoot->GetSceneManager()->destroySceneNode(mHandle);
        mOgitorsRoot->GetSceneManager()->destroyEntity(mEntity);
        mHandle = 0;
        mEntity = 0;
    }
    
    mLoaded->set(false);
    return true;
}

TriggerAreaEditor::TriggerAreaEditor(CBaseEditorFactory *factory)
    : CBaseEditor(factory)
{
    mHandle = 0;
    mPosition = 0;
    mIsEnabled = 0;
    mScale = 0;
    mShape = 0;
    mUsesGizmos = true;
}

TriggerAreaEditor::~TriggerAreaEditor()
{
}

//////////////////////////////////////////////////////////////////////////////////////////////////
TriggerAreaEditorFactory::TriggerAreaEditorFactory(OgitorsView *view) : CBaseEditorFactory(view)
{
    mTypeName = "Trigger Area Object";
    mEditorType = ETYPE_NODE;
    mAddToObjectList = true;
    mRequirePlacement = true;
    mIcon = "Icons/TriggerArea.svg";//----------------
    mCapabilities = CAN_MOVE|CAN_DELETE|CAN_CLONE|CAN_FOCUS|CAN_DRAG|CAN_ACCEPTCOPY|CAN_SCALE;

    mAreaShapes.clear();
    mAreaShapes.push_back(PropertyOption("BOX", Ogre::Any((int)TriggerAreaEditor::BOX)));
    mAreaShapes.push_back(PropertyOption("CYLINDER", Ogre::Any((int)TriggerAreaEditor::CYLINDER)));
    mAreaShapes.push_back(PropertyOption("SPHERE", Ogre::Any((int)TriggerAreaEditor::SPHERE)));

    AddPropertyDefinition("enabled", "Enabled", "If this component is enabled.", PROP_BOOL);
    AddPropertyDefinition("position", "Position", "The position of the object.", PROP_VECTOR3);
    AddPropertyDefinition("scale", "Scale", "The scale of the trigger area.", PROP_VECTOR3);
    AddPropertyDefinition("shape", "Shape", "The shape of the trigger area.", PROP_INT)->setOptions(&mAreaShapes);
}

CBaseEditorFactory *TriggerAreaEditorFactory::duplicate(OgitorsView *view)
{
    CBaseEditorFactory *ret = OGRE_NEW TriggerAreaEditorFactory(view);
    ret->mTypeID = mTypeID;

    return ret;
}

CBaseEditor *TriggerAreaEditorFactory::CreateObject(CBaseEditor **parent, OgitorsPropertyValueMap &params)
{
    TriggerAreaEditor *object = OGRE_NEW TriggerAreaEditor(this);
    OgitorsPropertyValueMap::iterator ni;

    if ((ni = params.find("init")) != params.end())
    {
        OgitorsPropertyValue value = EMPTY_PROPERTY_VALUE;
        value.val = Ogre::Any(CreateUniqueID("TriggerArea"));
        params["name"] = value;
        params.erase(ni);
    }

    object->createProperties(params);
    object->mParentEditor->init(*parent);
    object->registerForPostSceneUpdates();

    mInstanceCount++;
    return object;
}

Ogre::String TriggerAreaEditorFactory::GetPlaceHolderName()
{
    return "TriggerArea.mesh";
}

PropertyOptionsVector* TriggerAreaEditorFactory::GetAreaShapes()
{
    return &mAreaShapes;
}

PropertyOptionsVector TriggerAreaEditorFactory::mAreaShapes;

/////////////////////////////////////////////////////////////////////////
TriggerAreaEditorFactory *playerFac = 0;

bool dllStartPlugin(void *identifier, Ogre::String& name)
{
    name = "Trigger Area Object";
    playerFac = OGRE_NEW TriggerAreaEditorFactory();
    OgitorsRoot::getSingletonPtr()->RegisterEditorFactory(identifier, playerFac);
    return true;
}

bool dllStopPlugin(void)
{
    OGRE_DELETE playerFac;
    playerFac = 0;
    return true;
}
