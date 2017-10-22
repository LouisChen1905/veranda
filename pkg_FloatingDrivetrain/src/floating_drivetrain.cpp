#include "floating_drivetrain.h"
#include "std_msgs/Float64.h"
#include <QDebug>

Floating_Drivetrain::Floating_Drivetrain(QObject *parent) : DriveTrain_If(parent)
{
    connect(this, &Floating_Drivetrain::_incomingMessageSi, this, &Floating_Drivetrain::_incomingMessageSl);

    qRegisterMetaType<std_msgs::Float64MultiArray>("std_msgs::Float64MultiArray");
}

QVector<QString> Floating_Drivetrain::getChannelDescriptions()
{
    return QVector<QString>{"Absolute world velocities : Float64MultiArray{ x, y }"};
}

QVector<QString> Floating_Drivetrain::getChannelList()
{
    return QVector<QString>{_velocityChannel};
}

void Floating_Drivetrain::setChannelList(const QVector<QString>& channels)
{
    if(channels.size())
        _velocityChannel = channels[0];

    if(_connected)
    {
        disconnectFromROS();
        connectToROS();
    }
}

void Floating_Drivetrain::actualVelocity(double xDot, double yDot, double thetaDot)
{

}

void Floating_Drivetrain::connectToROS()
{
    if(_connected)
        disconnectFromROS();

    _listenChannel = _rosNode.subscribe(_velocityChannel.toStdString(), 10, &Floating_Drivetrain::_incomingMessageSi, this);
    _connected = true;
}

void Floating_Drivetrain::disconnectFromROS()
{
    _listenChannel.shutdown();
    _connected = false;
}

QVector<b2Shape*> Floating_Drivetrain::getModel()
{

}

void Floating_Drivetrain::_incomingMessageSl(std_msgs::Float64MultiArray data)
{
    if(sizeof(data.data) >= 2 * sizeof(std_msgs::Float64))
    {
        qDebug() << "Setting target velocity to" << data.data[0] << data.data[1];
        targetVelocity(data.data[0], data.data[1], 0);
    }
}