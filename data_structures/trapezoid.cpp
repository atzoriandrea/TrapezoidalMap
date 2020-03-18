#include "trapezoid.h"

Trapezoid::Trapezoid()
{

}

Trapezoid::Trapezoid(cg3::Segment2d top, cg3::Segment2d bottom, cg3::Point2d leftp, cg3::Point2d rightp)
{
    this->top = top;
    this->bottom = bottom;
    this->leftp = leftp;
    this->rightp = rightp;
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


