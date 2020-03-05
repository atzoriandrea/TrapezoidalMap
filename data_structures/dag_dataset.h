#ifndef DAG_DATASET_H
#define DAG_DATASET_H

#include "data_structures/dag_node.h"

class Dag{
public:
    Dag();
    void addSegment(cg3::Segment2d* segment);
    DagNode searchPoint(cg3::Point2d* point);
private:
    DagNode dag;
};

#endif // DAG_DATASET_H
