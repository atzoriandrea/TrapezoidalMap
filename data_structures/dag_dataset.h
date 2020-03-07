#ifndef DAG_DATASET_H
#define DAG_DATASET_H

#include "data_structures/dag_node.h"
#include <cg3/geometry/point2.h>
#include <cg3/geometry/segment2.h>

class Dag{
public:
    Dag();
    static void addSegment(cg3::Segment2d* segment);
    static DagNode * searchPoint(cg3::Point2d point);
    static DagNode dag;
    static DagNode* getInstance();

};

#endif // DAG_DATASET_H
