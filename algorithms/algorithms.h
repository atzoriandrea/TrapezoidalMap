#ifndef ALGORITHMS_H
#define ALGORITHMS_H
#include <iomanip>
#include <cg3/geometry/point2.h>
#include <cg3/geometry/segment2.h>
#include "cg3/geometry/line2.h"
#include <data_structures/trapezoidalmap.h>
#include <data_structures/dag_dataset.h>

namespace gas{
    cg3::Point2d intersection(const cg3::Segment2d & edge, const cg3::Point2d &point);
    long double matrixDet(const cg3::Segment2d& segment, const cg3::Point2d& point);
    void followSegment(const cg3::Segment2d &segment, Trapezoid* trapezoid, TrapezoidalMap& tm, Dag& dag);
    void bind(cg3::Segment2d &top, cg3::Segment2d &bottom, cg3::Point2d &leftp, cg3::Point2d &rightp, TrapezoidalMap& tm);
    const cg3::Segment2d normalizeSegment(const cg3::Segment2d& seg);
}



#endif // ALGORITHMS_H
