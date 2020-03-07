#ifndef DAG_NODE_H
#define DAG_NODE_H
#include <cg3/geometry/point2.h>
#include <cg3/geometry/segment2.h>
class DagNode{
public:
    virtual DagNode*const* compareTo(cg3::Point2d& point){}
    DagNode *const*getLeftChild() const;
    void setLeftChild(DagNode *value);
    DagNode *const*getRightChild() const;
    void setRightChild(DagNode *value);
    virtual ~DagNode(){}
private:
    DagNode* leftChild;
    DagNode* rightChild;
};


class DagNodePoint:public DagNode{
public:
    DagNodePoint(cg3::Point2d& point);
    //template<typename T>
    DagNode *const*compareTo(cg3::Point2d& point) override;
private:
    cg3::Point2d* point;

};

class DagNodeSegment:public DagNode{
public:
    DagNodeSegment(cg3::Segment2d* segment);
    //template<typename T>
    DagNode *const*compareTo(cg3::Point2d& point) override;
private:
    cg3::Segment2d* segment;
};


#endif // DAG_NODE_H
