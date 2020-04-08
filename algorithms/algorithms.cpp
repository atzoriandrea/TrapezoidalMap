#include "algorithms.h"


#define BOUNDINGBOX (1e+6)+1
cg3::Point2d intersection(const cg3::Segment2d & edge, const  cg3::Point2d & point){
      cg3::Line2 line = cg3::Line2(edge);
      cg3::Line2 line2 = cg3::Line2(cg3::Point2d(point.x(), -BOUNDINGBOX), cg3::Point2d(point.x(), BOUNDINGBOX));
      cg3::Point2d res = line.intersection(line2);
      return res;
}


//verfica se il punto point è sopra o sotto il segmento calcolando il determinante di [[sx1,sy1,1][sx2,sy2,1][px,py,1]]
long double matrixDet(const cg3::Segment2d& segment, const cg3::Point2d& point){
    double reducer = 100000.0;
    double mat[3][3] = {{(segment.p1().x()/reducer),(segment.p1().y()/reducer),1.0},
                                           {(segment.p2().x()/reducer),(segment.p2().y()/reducer), 1.0},
                                           {(point.x()/reducer), (point.y()/reducer),1.0}};
    return mat[0][0]*((mat[1][1]*mat[2][2]) - (mat[2][1]*mat[1][2]))
            -mat[0][1]*(mat[1][0]*mat[2][2] - mat[2][0]*mat[1][2]) +
            mat[0][2]*(mat[1][0]*mat[2][1] - mat[2][0]*mat[1][1]);
}
