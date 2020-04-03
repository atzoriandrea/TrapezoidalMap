#include "algorithms.h"


#define BOUNDINGBOX (1e+6)+1
cg3::Point2d intersection(const cg3::Segment2d & edge, const  cg3::Point2d & point)

{
      cg3::Line2 line = cg3::Line2(edge);
      cg3::Line2 line2 = cg3::Line2(cg3::Point2d(point.x(), -BOUNDINGBOX), cg3::Point2d(point.x(), BOUNDINGBOX));
      cg3::Point2d res = line.intersection(line2);
      return res;
}

void bind(Trapezoid& t, DagNodeArea* node){

}
