#ifndef DAG_NODE_H
#define DAG_NODE_H
#include <cg3/geometry/point2.h>
#include <cg3/geometry/segment2.h>
#include "trapezoid.h"
#include "algorithms/algorithms.h"
class Trapezoid;
class DagNode{
public:
    DagNode();
    DagNode& operator=(DagNode& other){
        *this = other;
        return *this;
    }
    virtual DagNode*& compareNodeToPoint(const cg3::Point2d& )=0;
    virtual DagNode*& compareNodeToSegment(const cg3::Segment2d& )=0;
    virtual int oneOrBoth(const cg3::Segment2d&)=0;
    DagNode *&getLeftChild() const;
    void setLeftChild(DagNode *value);
    DagNode ** lcPointerAddress();
    DagNode ** rcPointerAddress();
    virtual ~DagNode(){}
    friend class Dag;
    DagNode *&getRightChild() const;
    void setRightChild(DagNode * const &value);

private:
    DagNode * lcValue;
    DagNode * rcValue;
    DagNode*& leftChild;
    DagNode*& rightChild;
};


class DagNodePoint:public DagNode{
public:
    DagNodePoint(const cg3::Point2d& point);
    DagNode *&compareNodeToPoint(const cg3::Point2d& point) override;
    DagNode *&compareNodeToSegment(const cg3::Segment2d& segment) override;
    int oneOrBoth(const cg3::Segment2d&) override;

private:
    cg3::Point2d point;

};

class DagNodeSegment:public DagNode{
public:
    DagNodeSegment(const cg3::Segment2d& segment);
    //template<typename T>
    DagNode *&compareNodeToPoint(const cg3::Point2d& point) override;
    DagNode *&compareNodeToSegment(const cg3::Segment2d& segment) override;
    int oneOrBoth(const cg3::Segment2d&) override;
private:
    cg3::Segment2d segment;
};

class DagNodeArea:public DagNode{
public:
    DagNodeArea& operator=(const DagNodeArea& other){
        *this = other;
        return *this;
    }

    DagNodeArea(Trapezoid& t);
    DagNode *&compareNodeToPoint(const cg3::Point2d& point)override ;
    DagNode *&compareNodeToSegment(const cg3::Segment2d& segment) override;
    int oneOrBoth(const cg3::Segment2d&) override;
    Trapezoid& getT() const;
    //void setT(const Trapezoid& value) const;

    void setTrap(Trapezoid &value);

private:
    Trapezoid& trap;
};

#endif // DAG_NODE_H
