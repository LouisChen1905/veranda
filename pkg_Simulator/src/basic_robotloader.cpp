#include "basic_robotloader.h"

#include <QDebug>
#include <Box2D/Box2D.h>
#include <cmath>
#include <ctime>

BasicRobotLoader::BasicRobotLoader(const QMap<QString, DriveTrain_Plugin_If*>& drivePlugs, const QMap<QString, Sensor_Plugin_If*>& sensorPlugs)
    : _drivetrains(drivePlugs), _sensors(sensorPlugs)
{
    srand(time(nullptr));
}

Robot *BasicRobotLoader::loadRobotFile(QString file)
{
    if(_drivetrains.size())
    {
        b2CircleShape* circle = new b2CircleShape;
        circle->m_p.Set(0, 0);
        circle->m_radius = 0.5;

        b2EdgeShape* line = new b2EdgeShape;
        line->Set(b2Vec2(0, 0), b2Vec2(0.5, 0));

        //qDebug() << "Construct drivetrain";
        DriveTrain_If* dt;
        if(_drivetrains.contains("org.sdsmt.2dSim.drivetrain.differential"))
            dt = _drivetrains["org.sdsmt.2dSim.drivetrain.differential"]->createDrivetrain();
        else
            dt = _drivetrains.first()->createDrivetrain();

        QVector<Sensor_If*> sensors;
        if(_sensors.contains("org.sdsmt.2dSim.sensor.touchring"))
        {
            sensors.push_back(_sensors["org.sdsmt.2dSim.sensor.touchring"]->createSensor());
        }

        //qDebug() << "Construct robot";
        return new Robot({circle, line}, dt, sensors);
    }
    return nullptr;
}
