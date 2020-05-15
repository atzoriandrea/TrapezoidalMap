#include "trapezoid.h"
#include "dag_node.h"

Trapezoid::~Trapezoid(){}

Trapezoid::Trapezoid(cg3::Segment2d top, cg3::Segment2d bottom, cg3::Point2d leftp, cg3::Point2d rightp, DagNodeArea*& n):node(n){
    this->top = top;
    this->bottom = bottom;
    this->leftp = leftp;
    this->rightp = rightp;
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

bool Trapezoid::leftUpNull() const
{
    return leftUp==nullptr;
}

Trapezoid& Trapezoid::getLeftUp() const
{
    return *leftUp;
}

void Trapezoid::setLeftUp(Trapezoid &value)
{
    leftUp = &value;
}

void Trapezoid::setLeftUpNull()
{
    leftUp=nullptr;
}

bool Trapezoid::leftDownNull() const
{
    return leftDown==nullptr;
}

Trapezoid& Trapezoid::getLeftDown() const
{
    return *leftDown;
}

void Trapezoid::setLeftDown(Trapezoid& value)
{
    leftDown = &value;
}

void Trapezoid::setLeftDownNull()
{
    leftDown=nullptr;
}

bool Trapezoid::rightUpNull() const
{
    return rightUp==nullptr;
}

Trapezoid &Trapezoid::getRightUp() const
{
    return *rightUp;
}

void Trapezoid::setRightUp(Trapezoid &value)
{
    rightUp = &value;
}

void Trapezoid::setRightUpNull()
{
    rightUp=nullptr;
}

bool Trapezoid::rightDownNull() const
{
    return rightDown==nullptr;
}

Trapezoid &Trapezoid::getRightDown() const
{
    return *rightDown;
}

void Trapezoid::setRightDown(Trapezoid &value)
{
    rightDown = &value;
}

void Trapezoid::setRightDownNull()
{
    rightDown=nullptr;
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

Trapezoid& Trapezoid::setItr(const std::list<Trapezoid>::iterator &  value )
{
    itr = value;
    return *this;
}




