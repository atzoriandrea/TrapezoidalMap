#ifndef DAG_NODE_H
#define DAG_NODE_H
#include <cg3/geometry/point2.h>
#include <cg3/geometry/segment2.h>
#include "trapezoid.h"

class Trapezoid;
class DagNode{
public:
    DagNode();
    virtual ~DagNode(){}
    enum NodeType {X, Y, Leaf};
    enum Bounds {Classic, LeftBounded, RightBounded};
    NodeType type;
    NodeType getType();
};


class DagNodeArea:public DagNode{
public:
    DagNodeArea(Trapezoid& t);
    Trapezoid& getT() const;
    void setTrap(Trapezoid &value) const;
private:
    Trapezoid& trap;
};

class InnerNodes: public DagNode{
public:

    InnerNodes(DagNode*& lc, DagNode*& rc);
    InnerNodes(DagNodeArea*& left, DagNode*& rc);
    InnerNodes(DagNode*& lc, DagNodeArea*& right);

    virtual DagNode* compareNodeToPoint(const cg3::Point2d& )=0;
    virtual DagNode* compareNodeToSegment(const cg3::Segment2d& )=0;
    friend class Dag;

    DagNode *&getLeftChild() const;
    DagNode *&getRightChild() const;

    void setLeftChild(DagNode * const value);

    void setRightChild(DagNode * const value);

private:
    DagNode*& leftChild;
    DagNode*& rightChild;
};

class DagNodePoint:public InnerNodes{
public:
    DagNodePoint(DagNode*& lc, DagNode*& rc, const cg3::Point2d& point);
    DagNodePoint(DagNodeArea*& left, DagNode*& rc, const cg3::Point2d& point);
    DagNodePoint(DagNode*& lc, DagNodeArea*& right, const cg3::Point2d& point);
    DagNode* compareNodeToPoint(const cg3::Point2d& point) override;
    DagNode* compareNodeToSegment(const cg3::Segment2d& segment) override;
    cg3::Point2d getPoint() const;

private:
    const cg3::Point2d point;
};


class DagNodeSegment:public InnerNodes{
public:
    DagNodeSegment(DagNode*& lc, DagNode*& rc, const cg3::Segment2d& segment);
    DagNodeSegment(DagNodeArea*& left, DagNode*& rc, const cg3::Segment2d& segment);
    DagNodeSegment(DagNode*& lc, DagNodeArea*& right, const cg3::Segment2d& segment);
    DagNode* compareNodeToPoint(const cg3::Point2d& point) override;
    DagNode* compareNodeToSegment(const cg3::Segment2d& segment) override;
    cg3::Segment2d getSegment() const;

private:
    cg3::Segment2d segment;
};

#endif // DAG_NODE_H
