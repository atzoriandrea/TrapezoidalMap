#include <data_structures/dag_node.h>

DagNodePoint::DagNodePoint(cg3::Point2d* point){
    this->point = point;
}
DagNodeSegment::DagNodeSegment(cg3::Segment2d* segment){
    this->segment = segment;
}
