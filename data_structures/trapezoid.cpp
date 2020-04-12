#include "trapezoid.h"

Trapezoid::Trapezoid()
{

}

Trapezoid::Trapezoid(cg3::Segment2d top, cg3::Segment2d bottom, cg3::Point2d leftp, cg3::Point2d rightp, DagNodeArea* n){
    this->top = top;
    this->bottom = bottom;
    this->leftp = leftp;
    this->rightp = rightp;
    this->node = n;
    this->leftUp=nullptr;
    this->rightUp=nullptr;
    this->leftDown=nullptr;
    this->rightDown=nullptr;
}

cg3::Segment2d Trapezoid::getTop() const
{
    return top;
}

void Trapezoid::setTop(const cg3::Segment2d &value)
{
    top = value;
}

cg3::Segment2d Trapezoid::getBottom() const
{
    return bottom;
}

void Trapezoid::setBottom(const cg3::Segment2d &value)
{
    bottom = value;
}

cg3::Point2d Trapezoid::getLeftp() const
{
    return leftp;
}

void Trapezoid::setLeftp(const cg3::Point2d &value)
{
    leftp = value;
}

cg3::Point2d Trapezoid::getRightp() const
{
    return rightp;
}

void Trapezoid::setRightp(const cg3::Point2d &value)
{
    rightp = value;
}

Trapezoid& Trapezoid::getLeftUp() const
{
    return *leftUp;
}

void Trapezoid::setLeftUp(Trapezoid &value)
{
    leftUp = &value;
}

Trapezoid& Trapezoid::getLeftDown() const
{
    return *leftDown;
}

void Trapezoid::setLeftDown(Trapezoid& value)
{
    leftDown = &value;
}

Trapezoid &Trapezoid::getRightUp() const
{
    return *rightUp;
}

void Trapezoid::setRightUp(Trapezoid &value)
{
    rightUp = &value;
}

Trapezoid &Trapezoid::getRightDown() const
{
    return *rightDown;
}

void Trapezoid::setRightDown(Trapezoid &value)
{
    rightDown = &value;
}

void Trapezoid::setNeighbors(Trapezoid &leftUp, Trapezoid &leftDown,Trapezoid &rightUp,Trapezoid &rightDown)
{
    this->setLeftUp(leftUp);
    this->setLeftDown(leftDown);
    this->setRightUp(rightUp);
    this->setRightDown(rightDown);
}

DagNodeArea* Trapezoid::getNode() const
{
    return node;
}

DagNodeArea *&Trapezoid::getNodeRef()
{
    DagNodeArea*& ref = node;
    return ref;
}

void Trapezoid::setNode(DagNodeArea *value)
{
    node = value;
}

std::list<Trapezoid>::iterator Trapezoid::getItr() const
{
    return itr;
}

void Trapezoid::setItr(const std::list<Trapezoid>::iterator &value)
{
    itr = value;
}



//void Trapezoid::setNode(DagNodeArea &value)
//{
//    node = value;
//}





