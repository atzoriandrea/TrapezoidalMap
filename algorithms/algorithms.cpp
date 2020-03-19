#include "algorithms.h"
#include "cg3/geometry/point2.h"
#include "cg3/geometry/segment2.h"
#include "cg3/geometry/line2.h"
#define BOUNDINGBOX (1e+6)+1
cg3::Point2d intersection(const cg3::Segment2d & edge, const  cg3::Point2d & point)

{
      cg3::Line2 line = cg3::Line2(edge);
      cg3::Line2 line2 = cg3::Line2(cg3::Point2d(point.x(), -BOUNDINGBOX), cg3::Point2d(point.x(), BOUNDINGBOX));
      cg3::Point2d res = line.intersection(line2);
      return res;
//    long double a1=(edge.p2().y()-edge.p1().y());
//    long double b1=-1*(edge.p2().x()-edge.p1().x());
//    long double c1=-1+edge.p1().x()*(edge.p2().y()-edge.p1().y())+edge.p1().y()*(edge.p2().x()-edge.p1().x());
//    long double a2=(BOUNDINGBOX-(-BOUNDINGBOX));
//    long double b2= -1*(point.x()-point.x());
//    long double c2=-1*point.x()*(BOUNDINGBOX-(-BOUNDINGBOX))+(-BOUNDINGBOX)*(point.x()-point.x());
//    long double den=(a1*b2-a2*b1);
//    double y=(a2*c1-a1*c2)/den;
//    double x=(b1*c2 - b2*c1)/den;
//    return cg3::Point2d(x,y);

}
