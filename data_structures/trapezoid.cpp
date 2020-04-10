#include "trapezoid.h"

//Trapezoid::Trapezoid()
//{

//}

Trapezoid::Trapezoid(cg3::Segment2d top, cg3::Segment2d bottom, cg3::Point2d leftp, cg3::Point2d rightp, DagNodeArea* n):defaultNode(nullptr), node(defaultNode){
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

DagNodeArea* Trapezoid::getNode()
{
    return node;
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

void merge(Trapezoid &tLeft, Trapezoid &tRight, std::vector<std::list<Trapezoid>::iterator>& garbageCollector)
{
    DagNodeArea * tLeftLeaf = tLeft.getNode();
    tRight.setLeftp(tLeft.getLeftp());
    tRight.setLeftUp(tLeft.getLeftUp());
    tRight.setLeftDown(tLeft.getLeftDown());
    tRight.setTop(cg3::Segment2d(tLeft.getTop().p1(), tRight.getTop().p2()));
    tRight.setBottom(cg3::Segment2d(tLeft.getBottom().p1(), tRight.getBottom().p2()));
    if(tLeft.getLeftUp().getRightUp()==tLeft)
        tLeft.getLeftUp().setRightUp(tRight);

    if(tLeft.getLeftUp().getRightDown()==tLeft)
        tLeft.getLeftUp().setRightDown(tRight);

    if(tLeft.getLeftDown().getRightUp()==tLeft)
        tLeft.getLeftDown().setRightUp(tRight);

    if(tLeft.getLeftDown().getRightDown()==tLeft)
        tLeft.getLeftDown().setRightDown(tRight);

    tLeftLeaf->setTrap(tRight);
    garbageCollector.push_back(tLeft.getItr());
    //TrapezoidalMap::removeTrapezoid(tLeft);
}

//void Trapezoid::setNode(DagNodeArea &value)
//{
//    node = value;
//}





