#ifndef DAG_NODE_H
#define DAG_NODE_H
#include <cg3/geometry/point2.h>
#include <cg3/geometry/segment2.h>
class DagNode{
public:
    virtual DagNode* compareTo(const cg3::Point2d& ){}
    DagNode *getLeftChild() const;
    void setLeftChild(DagNode *value);
    DagNode *getRightChild() const;
    void setRightChild(DagNode *value);
    virtual ~DagNode(){}
    friend class Dag;
private:
    DagNode* leftChild;
    DagNode* rightChild;
};


class DagNodePoint:public DagNode{
public:
    DagNodePoint(cg3::Point2d& point);
    //template<typename T>
    DagNode *compareTo(const cg3::Point2d& point) override;
private:
    cg3::Point2d* point;

};

class DagNodeSegment:public DagNode{
public:
    DagNodeSegment(cg3::Segment2d* segment);
    //template<typename T>
    DagNode *compareTo(const cg3::Point2d& point) override;
private:
    cg3::Segment2d* segment;
};

class DagNodeArea:public DagNode{
public:
    DagNodeArea();
    DagNode *compareTo(const cg3::Point2d& point)override ;
private:
    std::string label;
};

#endif // DAG_NODE_H
