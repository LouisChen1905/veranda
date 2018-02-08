#include "sdsmt_simulator/world_object.h"

#include <QSet>

WorldObject::WorldObject(QVector<WorldObjectComponent_If *> components, QObject *parent) : QObject(parent), _components(components)
{
    QMap<QString, int> groupcounts;

    //Quick tally of if any components use the same group name
    for(WorldObjectComponent_If* c : _components)
    {
        groupcounts[c->getPropertyGroup()]++;
    }

    QSet<QString> multiples;
    for(auto iter = groupcounts.begin(); iter != groupcounts.end(); iter++)
        if(iter.value() > 1) multiples.insert(iter.key());

    //Initialize all aggregates
    for(WorldObjectComponent_If* c : _components)
    {
       /*************
        * Parenting
        * Auto-delete components when world object dies
        *************/
        c->setParent(this);

       /*************
        * Properties
        *************/
        //Get properties of child
        QMap<QString, PropertyView> props = c->getProperties();

        //If more than 1 child uses group, append a number
        //to the group
        QString group = c->getPropertyGroup();

        if(multiples.contains(group))
            group += QString::number(groupcounts[group]--);

        //Add all properties to property map
        for(auto iter = props.begin(); iter != props.end(); iter++)
        {
            _properties[group + "/" + iter.key()] = iter.value();
        }

       /*************
        * Models
        *************/
        _models += c->getModels();

       /*************
        * Channels
        *************/
        if(c->usesChannels()) _useChannels = true;

       /*************
        * Mass Aggregation
        *************/
        connect(c, &WorldObjectComponent_If::massChanged, this, &WorldObject::componentMassChanged);
    }

    debugModel = new Model();
    //_models += debugModel;
}

WorldObject* WorldObject::clone(QObject *newParent)
{
    QVector<WorldObjectComponent_If*> childClones;
    for(WorldObjectComponent_If* c : _components)
        childClones.push_back(c->clone());

    WorldObject* copy = new WorldObject(childClones, newParent);
    copy->_objName.set(_objName.get());
    copy->_locX.set(_locX.get());
    copy->_locY.set(_locY.get());
    copy->_locTheta.set(_locTheta.get());

    return copy;
}

void WorldObject::clearBodies(b2World *world)
{
    QVector<WorldObjectComponent_If*> components = getComponents();

    disconnect(this, &WorldObject::massChanged, 0, 0);
    _totalMass = 0;
    _componentMasses.clear();

    if(anchorBody)
    {
        for(WorldObjectComponent_If* c : components)
            c->clearBodies(world);

        world->DestroyBody(anchorBody);
        anchorBody = nullptr;
    }
}

void WorldObject::generateBodies(b2World* world, object_id oId)
{
    clearBodies(world);

    qDebug() << "Creating object at " << _locX.get().toDouble() << ", " << _locY.get().toDouble() << " : " << _locTheta.get().toDouble();

    QVector<WorldObjectComponent_If*> components = getComponents();

    b2BodyDef anchorDef;
    anchorDef.type = b2_dynamicBody;
    anchorDef.position.Set(_locX.get().toDouble(), _locY.get().toDouble());
    anchorDef.angle = _locTheta.get().toDouble() * DEG2RAD;
    anchorBody = world->CreateBody(&anchorDef);

    b2CircleShape* circ = new b2CircleShape;
    circ->m_p = b2Vec2(0, 0);
    circ->m_radius = 1;

    b2FixtureDef fix;
    fix.shape = circ;
    fix.density = 1;
    fix.isSensor = true;
    anchorBody->CreateFixture(&fix);

    debugModel->addShapes(QVector<b2Shape*>{circ});

    for(int i = 0; i < components.size(); i++)
    {
        components[i]->generateBodies(world, oId, anchorBody);
    }

    _totalMass += anchorBody->GetMass();

    for(WorldObjectComponent_If* c : components)
    {
        connect(this, &WorldObject::massChanged, c, &WorldObjectComponent_If::setObjectMass);
        c->setObjectMass(_totalMass);
    }
}

void WorldObject::connectChannels()
{
    if(_useChannels)
    {
        for(WorldObjectComponent_If* c : _components)
            c->connectChannels();
    }
}

void WorldObject::disconnectChannels()
{
    if(_useChannels)
    {
        for(WorldObjectComponent_If* c : _components)
            c->disconnectChannels();
    }
}

void WorldObject::worldTicked(const b2World* w, const double t)
{
    debugModel->setTransform(anchorBody->GetPosition().x, anchorBody->GetPosition().y, anchorBody->GetAngle()*RAD2DEG);

    _locX.set(anchorBody->GetPosition().x);
    _locY.set(anchorBody->GetPosition().y);
    _locTheta.set(anchorBody->GetAngle()*RAD2DEG);

    for(WorldObjectComponent_If* c : _components)
        c->worldTicked(w, t);
}

void WorldObject::setROSNode(std::shared_ptr<rclcpp::Node> node)
{
    for(WorldObjectComponent_If* c : _components)
        c->setROSNode(node);
}

void WorldObject::componentMassChanged(WorldObjectComponent_If *component, double mass)
{
    auto curr = _componentMasses.find(component);
    if(curr == _componentMasses.end())
    {
        curr = _componentMasses.insert(component, 0);
    }

    _totalMass -= curr.value();
    curr.value() = mass;
    _totalMass += curr.value();

    massChanged(_totalMass);
}
