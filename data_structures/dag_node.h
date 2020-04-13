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
    virtual ~DagNode(){}
    enum NodeType {X, Y, Leaf};
    NodeType type;
    NodeType getType();
};

class InnerNodes: public DagNode{
public:
    InnerNodes(DagNode& lc, DagNode& rc);
    virtual DagNode& compareNodeToPoint(const cg3::Point2d& )=0;
    virtual DagNode& compareNodeToSegment(const cg3::Segment2d& )=0;
    //virtual int oneOrBoth(const cg3::Segment2d&)=0;
    DagNode& getLeftChild() const;
    void setLeftChild(DagNode& value);
    friend class Dag;
    DagNode& getRightChild() const;
    void setRightChild(DagNode& value);
    DagNode*& getAddress();
private:
//    DagNode * lcValue;
//    DagNode * rcValue;
//    DagNode*& leftChild;
//    DagNode*& rightChild;
    DagNode& leftChild;
    DagNode& rightChild;
};

class DagNodePoint:public InnerNodes{
public:
    DagNodePoint(DagNode& lc, DagNode& rc, const cg3::Point2d& point);
    DagNode& compareNodeToPoint(const cg3::Point2d& point) override;
    DagNode& compareNodeToSegment(const cg3::Segment2d& segment) override;
    //int oneOrBoth(const cg3::Segment2d&) override;

private:
    cg3::Point2d point;

};


class DagNodeSegment:public InnerNodes{
public:
    DagNodeSegment(DagNode& lc, DagNode& rc, const cg3::Segment2d& segment);
    DagNode& compareNodeToPoint(const cg3::Point2d& point) override;
    DagNode& compareNodeToSegment(const cg3::Segment2d& segment) override;
    //int oneOrBoth(const cg3::Segment2d&) override;
private:
    cg3::Segment2d segment;
};

class DagNodeArea:public DagNode{
public:
    DagNodeArea& operator=(const DagNodeArea& other){
        *this = other;
        return *this;
    }
    DagNode* operator=(DagNodePoint * other){
        *this = other;
        return this;
    }
    DagNodeArea& operator=(DagNode * other){
        *this = other;
        return *this;
    }
    DagNodeArea(Trapezoid& t);
    Trapezoid& getT() const;
    void setTrap(Trapezoid &value) const;

private:
    Trapezoid& trap;
};

#endif // DAG_NODE_H
