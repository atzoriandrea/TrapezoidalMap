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
    Trapezoid& operator=(Trapezoid& other){
        *this = other;
        //this->defaultNode=other.defaultNode;
        //this->setNode(this->defaultNode);
        return *this;
    }
    const Trapezoid& operator=(const Trapezoid& other) const{
        *this = other;
        return *this;
    }
    Trapezoid& operator=(Trapezoid *other){
        *this = other;
        return *this;
    }
//    Trapezoid& operator=(const Trapezoid* other){
//        *this = other;
//        return *this;
//    }
//    inline bool operator==(const Trapezoid& other) const {return (this->getTop().p1()==other.getTop().p1() && this->getBottom().p2()==other.getBottom().p2())?true:false;}
    inline bool operator==(const Trapezoid& other) const {return (this==&other)?true:false;}
    inline bool operator!=(const Trapezoid& other) const {return !(this==&other)?true:false;}
    //inline bool operator!=(const Trapezoid& other) const {return !(this->getTop().p1()==other.getTop().p1()&& this->getBottom().p2()==other.getBottom().p2())?true:false;}
    inline bool operator<(const Trapezoid& other) const {return !(this->getTop().p1()<other.getTop().p1())?true:false;}
    Trapezoid(cg3::Segment2d top, cg3::Segment2d bottom, cg3::Point2d leftp, cg3::Point2d rightp,DagNodeArea*& n);
    cg3::Segment2d getTop() const;
    void setTop(const cg3::Segment2d &value);

    cg3::Segment2d getBottom() const;
    void setBottom(const cg3::Segment2d &value);

    cg3::Point2d getLeftp() const;
    void setLeftp(const cg3::Point2d &value);

    cg3::Point2d getRightp() const;
    void setRightp(const cg3::Point2d &value);

    bool leftUpNull() const;
    Trapezoid &getLeftUp() const;
    void setLeftUp(Trapezoid &value);

    bool leftDownNull() const;
    Trapezoid &getLeftDown() const;
    void setLeftDown(Trapezoid& value);

    bool rightUpNull() const;
    Trapezoid &getRightUp() const;
    void setRightUp(Trapezoid &value);

    bool rightDownNull() const;
    Trapezoid &getRightDown() const;
    void setRightDown(Trapezoid &value);

    void setNeighbors(Trapezoid &leftUp, Trapezoid &leftDown,Trapezoid &rightUp,Trapezoid &rightDown);

    DagNodeArea* getNode() const;
    DagNodeArea*& getNodeRef();

    //void setNode(DagNodeArea &value);

    void setNode(DagNodeArea *value);

    std::list<Trapezoid>::iterator getItr() const;
    Trapezoid& setItr(const std::list<Trapezoid>::iterator &value);



private:
    std::list<Trapezoid>::iterator itr;
    DagNodeArea*& node;
    cg3::Segment2d top;
    cg3::Segment2d bottom;
    cg3::Point2d leftp;
    cg3::Point2d rightp;
    //DagNodeArea*& node;
    Trapezoid* leftUp;
    Trapezoid* leftDown;
    Trapezoid* rightUp;
    Trapezoid* rightDown;

};

#endif // TRAPEZOID_H
