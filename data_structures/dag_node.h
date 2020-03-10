#ifndef DAG_NODE_H
#define DAG_NODE_H
#include <cg3/geometry/point2.h>
#include <cg3/geometry/segment2.h>
class DagNode{
public:
    virtual DagNode* compareNodeToPoint(const cg3::Point2d& ){}
    virtual DagNode* compareNodeToSegment(const cg3::Segment2d& ){}
    virtual int oneOrBoth(const cg3::Segment2d&){}
    DagNode *getLeftChild() const;
    void setLeftChild(DagNode *value);
    DagNode *getRightChild() const;
    void setRightChild(DagNode *value);
    DagNode ** lcPointerAddress();
    DagNode ** rcPointerAddress();
    virtual ~DagNode(){}
    friend class Dag;
private:
    DagNode* leftChild;
    DagNode* rightChild;
};


class DagNodePoint:public DagNode{
public:
    DagNodePoint(cg3::Point2d& point);
    DagNode *compareNodeToPoint(const cg3::Point2d& point) override;
    DagNode *compareNodeToSegment(const cg3::Segment2d& segment) override;
    int oneOrBoth(const cg3::Segment2d&) override;

private:
    cg3::Point2d* point;

};

class DagNodeSegment:public DagNode{
public:
    DagNodeSegment(cg3::Segment2d* segment);
    //template<typename T>
    DagNode *compareNodeToPoint(const cg3::Point2d& point) override;
    DagNode *compareNodeToSegment(const cg3::Segment2d& segment) override;
    int oneOrBoth(const cg3::Segment2d&) override;
private:
    cg3::Segment2d* segment;
};

class DagNodeArea:public DagNode{
public:
    DagNodeArea();
    DagNode *compareNodeToPoint(const cg3::Point2d& point)override ;
    DagNode *compareNodeToSegment(const cg3::Segment2d& segment) override;
    int oneOrBoth(const cg3::Segment2d&) override;
private:
    std::string label;
};

#endif // DAG_NODE_H
