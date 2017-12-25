#ifndef NEW_WORLD_OBJECT_H
#define NEW_WORLD_OBJECT_H

#include <QObject>
#include <QVector>

#include <Box2D/Box2D.h>

#include "sdsmt_simulator/world_object_component_if.h"

class newWorldObject_If : public QObject
{
    Q_OBJECT

public:
    newWorldObject_If(QObject* parent=nullptr) : QObject(parent){}

    //Virtual clone
    virtual newWorldObject_If* clone(QObject* newParent=nullptr) = 0;

    //Access to components for physics and graphics
    virtual QVector<WorldObjectComponent_If*> getComponents() = 0;

    //Interfaces for UI to display properties
    virtual QMap<QString, PropertyView>& getAllProperties() = 0;
    virtual QString propertyGroupName() = 0;

    //Interface for world view to draw
    virtual QVector<Model*> getModels(){ return {}; }

    //Physics engine assigns bodies
    virtual void setStaticBodies(QVector<b2Body*>&){}
    virtual QVector<b2JointDef*> setDynamicBodies(QVector<b2Body*>&){ return {}; }
    virtual void clearStaticBodies(){}
    virtual void clearDynamicBodies(){}

public slots:
    //Interface to connect/disconnect to external communications
    virtual void connectChannels(){}
    virtual void disconnectChannels(){}

    //Interface to update on world ticks
    virtual void worldTicked(const b2World*, const double&){}
};

class newWorldObjectProperties_If : public QObject
{
    Q_OBJECT
    newWorldObject_If* _obj;

public:
    newWorldObjectProperties_If(newWorldObject_If* obj, QObject* parent=nullptr) : QObject(parent), _obj(obj){}

    virtual QVector<WorldObjectComponent_If*> getComponents(){return _obj->getComponents();}

    //Interfaces for UI to display properties
    virtual QMap<QString, PropertyView>& getAllProperties(){return _obj->getAllProperties();}
    virtual QString propertyGroupName(){return _obj->propertyGroupName();}

    //Interface for world view to draw
    virtual QVector<Model*> getModels(){return _obj->getModels();}

    //Interface to connect/disconnect to external communications
    virtual void connectChannels(){_obj->connectChannels();}
    virtual void disconnectChannels(){_obj->disconnectChannels();}
};

class newWorldObjectPhysics_If : public QObject
{
    Q_OBJECT

    newWorldObject_If* _obj;

public:
    newWorldObjectPhysics_If(newWorldObject_If* obj, QObject* parent=nullptr) : QObject(parent), _obj(obj){}

    virtual QVector<WorldObjectComponent_If*> getComponents(){return _obj->getComponents();}

    virtual void clearDynamicBodies(){_obj->clearDynamicBodies();}
    virtual void clearStaticBodies(){_obj->clearStaticBodies();}

public slots:
    //Interface to update on world ticks
    virtual void worldTicked(const b2World* w, const double t){_obj->worldTicked(w, t);}
};

#endif // NEW_WORLD_OBJECT_H
