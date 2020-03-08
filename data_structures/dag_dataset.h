#ifndef DAG_DATASET_H
#define DAG_DATASET_H

#include "data_structures/dag_node.h"
#include <cg3/geometry/point2.h>
#include <cg3/geometry/segment2.h>

class Dag{
public:
    Dag();
    static void addSegment(cg3::Segment2d& segment);
    static DagNode * searchPoint(const cg3::Point2d& point);
    static DagNode * splitNode(cg3::Segment2d& segment);
    static DagNode *getDag();
private:
    static DagNode * dag;
};

//DagNode* Dag::dag = nullptr;
#endif // DAG_DATASET_H
