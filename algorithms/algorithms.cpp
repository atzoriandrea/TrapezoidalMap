#include "algorithms.h"
#define BOUNDINGBOX 1e+6
cg3::Point2d intersection(cg3::Segment2d edge, cg3::Point2d point)
{
    double a1=(edge.p2().y()-edge.p1().y());
    double b1=-1*(edge.p2().x()-edge.p1().x());
    double c1=-1+edge.p1().x()*(edge.p2().y()-edge.p1().y())+edge.p1().y()*(edge.p2().x()-edge.p1().x());
    double a2=(BOUNDINGBOX-(-BOUNDINGBOX));
    double b2= 0;//-1*(x4-x3);
    double c2=-1*point.x()*(BOUNDINGBOX-(-BOUNDINGBOX))+(-BOUNDINGBOX)*0;
    double den=(a1*b2-a2*b1);
    double y=(a2*c1-a1*c2)/den;
    double x=(b1*c2 - b2*c1)/den;
    return cg3::Point2d(x,y);

}
