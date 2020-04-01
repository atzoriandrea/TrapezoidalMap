#ifndef TRAPEZOID_H
#define TRAPEZOID_H
#include <cg3/geometry/point2.h>
#include <cg3/geometry/segment2.h>

class Trapezoid
{
public:
    Trapezoid();
    inline bool operator==(const Trapezoid& other) const {return (this->getTop().p1()==other.getTop().p1())?true:false;}
    inline bool operator!=(const Trapezoid& other) const {return !(this->getTop().p1()==other.getTop().p1())?true:false;}
    inline bool operator<(const Trapezoid& other) const {return !(this->getTop().p1()<other.getTop().p1())?true:false;}
    Trapezoid(cg3::Segment2d top, cg3::Segment2d bottom, cg3::Point2d leftp, cg3::Point2d rightp);
    cg3::Segment2d getTop() const;
    void setTop(const cg3::Segment2d &value);

    cg3::Segment2d getBottom() const;
    void setBottom(const cg3::Segment2d &value);

    cg3::Point2d getLeftp() const;
    void setLeftp(const cg3::Point2d &value);

    cg3::Point2d getRightp() const;
    void setRightp(const cg3::Point2d &value);



private:
    cg3::Segment2d top;
    cg3::Segment2d bottom;
    cg3::Point2d leftp;
    cg3::Point2d rightp;

};

#endif // TRAPEZOID_H
