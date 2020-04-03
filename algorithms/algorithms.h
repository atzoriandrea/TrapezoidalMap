#ifndef ALGORITHMS_H
#define ALGORITHMS_H
#include <cg3/geometry/point2.h>
#include <cg3/geometry/segment2.h>
#include "cg3/geometry/line2.h"
#include <data_structures/dag_node.h>
#include <data_structures/trapezoid.h>


cg3::Point2d intersection(const cg3::Segment2d & edge, const cg3::Point2d &point);
//void bind(Trapezoid& t, DagNodeArea* node);


#endif // ALGORITHMS_H
