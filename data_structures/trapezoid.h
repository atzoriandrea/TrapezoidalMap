#ifndef TRAPEZOID_H
#define TRAPEZOID_H
#include <cg3/geometry/point2.h>
#include <cg3/geometry/segment2.h>
#include "dag_node.h"
class DagNodeArea;
class Trapezoid
{
public:
    Trapezoid();
    Trapezoid& operator=(const Trapezoid& other){
        *this = other;
        return *this;
    }
//    Trapezoid& operator=(const Trapezoid* other){
//        *this = other;
//        return *this;
//    }
    inline bool operator==(const Trapezoid& other) const {return (this->getTop().p1()==other.getTop().p1() && this->getBottom().p2()==other.getBottom().p2())?true:false;}
    inline bool operator!=(const Trapezoid& other) const {return !(this->getTop().p1()==other.getTop().p1()&& this->getBottom().p2()==other.getBottom().p2())?true:false;}
    inline bool operator<(const Trapezoid& other) const {return !(this->getTop().p1()<other.getTop().p1())?true:false;}
    Trapezoid(cg3::Segment2d top, cg3::Segment2d bottom, cg3::Point2d leftp, cg3::Point2d rightp,DagNodeArea* n);
    cg3::Segment2d getTop() const;
    void setTop(const cg3::Segment2d &value);

    cg3::Segment2d getBottom() const;
    void setBottom(const cg3::Segment2d &value);

    cg3::Point2d getLeftp() const;
    void setLeftp(const cg3::Point2d &value);

    cg3::Point2d getRightp() const;
    void setRightp(const cg3::Point2d &value);


    Trapezoid &getLeftUp() const;
    void setLeftUp(Trapezoid &value);

    Trapezoid &getLeftDown() const;
    void setLeftDown(Trapezoid& value);

    Trapezoid &getRightUp() const;
    void setRightUp(Trapezoid &value);

    Trapezoid &getRightDown() const;
    void setRightDown(Trapezoid &value);

    void setNeighbors(Trapezoid &leftUp, Trapezoid &leftDown,Trapezoid &rightUp,Trapezoid &rightDown);

    const DagNodeArea* getNode();

//    void setNode(const DagNodeArea &value);

    void setNode(const DagNodeArea *value);

private:
    cg3::Segment2d top;
    cg3::Segment2d bottom;
    cg3::Point2d leftp;
    cg3::Point2d rightp;
    const DagNodeArea* node;
    Trapezoid* leftUp;
    Trapezoid* leftDown;
    Trapezoid* rightUp;
    Trapezoid* rightDown;

};

#endif // TRAPEZOID_H
