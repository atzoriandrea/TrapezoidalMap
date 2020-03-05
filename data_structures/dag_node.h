#ifndef DAG_NODE_H
#define DAG_NODE_H

#include <cg3/geometry/point2.h>
#include <cg3/geometry/segment2.h>

class DagNode{

};

class DagNodePoint:DagNode  {
public:
    DagNodePoint(cg3::Point2d* point);

private:
    cg3::Point2d* point;
    DagNode leftChild;
    DagNode rightChild;
};

class DagNodeSegment:DagNode{
public:
    DagNodeSegment(cg3::Segment2d* segment);

private:
    cg3::Segment2d* segment;
    DagNode leftChild;
    DagNode rightChild;

};

class DagNodeArea:DagNode{


};
#endif // DAG_NODE_H
