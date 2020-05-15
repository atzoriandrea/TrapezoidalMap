#include "algorithms.h"


#define BOUNDINGBOX (1e+6)+1
cg3::Point2d gas::intersection(const cg3::Segment2d & edge, const  cg3::Point2d & point){
      cg3::Line2 line = cg3::Line2(edge);
      cg3::Line2 line2 = cg3::Line2(cg3::Point2d(point.x(), -BOUNDINGBOX), cg3::Point2d(point.x(), BOUNDINGBOX));
      cg3::Point2d res = line.intersection(line2);
      return res;
}


//verfica se il punto point è sopra o sotto il segmento calcolando il determinante di [[sx1,sy1,1][sx2,sy2,1][px,py,1]]
long double gas::matrixDet(const cg3::Segment2d& segment, const cg3::Point2d& point){
    double reducer = 100000.0;
    double mat[3][3] = {{(segment.p1().x()/reducer),(segment.p1().y()/reducer),1.0},
                                           {(segment.p2().x()/reducer),(segment.p2().y()/reducer), 1.0},
                                           {(point.x()/reducer), (point.y()/reducer),1.0}};
    return mat[0][0]*((mat[1][1]*mat[2][2]) - (mat[2][1]*mat[1][2]))
            -mat[0][1]*(mat[1][0]*mat[2][2] - mat[2][0]*mat[1][2]) +
            mat[0][2]*(mat[1][0]*mat[2][1] - mat[2][0]*mat[1][1]);
}


void gas::followSegment(const cg3::Segment2d &segment, Trapezoid *trapezoid, TrapezoidalMap& tm, Dag& dag)
{
    Trapezoid * lastDeleted;
    DagNodeArea * current;
    Trapezoid * next;
    DagNode* prevSeg;
    unsigned long iter = 0;
    while(segment.p2().x() > trapezoid->getRightp().x()){
        if(gas::matrixDet(segment, trapezoid->getRightp().x())<0)
            next = &trapezoid->getRightUp();
        else
            next = &trapezoid->getRightDown();
        DagNode *& copy = (DagNode*&)*trapezoid->getNodeRef();
        if(iter==0){
            if(segment.p1()==trapezoid->getLeftp()){
                //crea nuovi trapezoidi DEG
                //crea nuovi nodi nel dag relativi ai nuovi trapezoidi DEG
                prevSeg = copy;
            }
            else{
                //crea nuovi trapezoidi
                //crea nuovi nodi nel dag relativi ai nuovi trapezoidi
                prevSeg=copy;
            }
        }
        else{
            //crea nuovi trapezoidi intermedi
            //crea nuovi nodi nel dag relativi ai nuovi trapezoidi intermedi
            prevSeg = copy;
        }
        iter++;
        trapezoid = next;
    }
    DagNode *& copy = (DagNode*&)*trapezoid->getNodeRef();
    if(segment.p2()==trapezoid->getRightp()){
        //crea nuovi trapezoidi destri DEG
        //crea nuovi nodi nel dag relativi ai nuovi trapezoidi destri DEGe
    }
    else{
        //crea nuovi trapezoidi destri
        //crea nuovi nodi nel dag relativi ai nuovi trapezoidi destri
    }
//    while (segment.p2().x() > leaf->getT().getRightp().x()) {
//        current = leaf;
//        if(matrixDet(segment, leaf->getT().getRightp())<0){
//            leaf = leaf->getT().getRightUp().getNodeRef();

//        }
//        else{
//            leaf = leaf->getT().getRightDown().getNodeRef();
//        }
//        DagNode *& copy = (DagNode*&)current->getT().getNodeRef();
//        if(iter == 0){
//            if(segment.p1()==(*(DagNodeArea*)copy).getT().getLeftp()){
//                copy = createLeftMostDegenerate(segment, *(DagNodeArea*)copy, lastDeleted);
//                prevSeg = copy;
//            }
//            else{
//                copy = createLeftMost(segment, *(DagNodeArea*)copy, lastDeleted);
//                prevSeg = ((DagNodePoint*)copy)->getRightChild();
//            }
//        }
//        else{
//            copy = createIntermediate(segment, *(DagNodeArea*)copy, *(DagNodeSegment*)prevSeg, lastDeleted);
//            prevSeg = copy;
//        }
//       iter ++;
//    }
//    DagNode*& copy = (DagNode*&)leaf->getT().getNodeRef();
//    if(segment.p2()==(*(DagNodeArea*)copy).getT().getRightp()){
//        copy = createRightMostDegenerate(segment, *(DagNodeArea*)copy, *(DagNodeSegment*)prevSeg, lastDeleted);
//    }
//    else
//        copy = createRightMost(segment, *(DagNodeArea*)copy, *(DagNodeSegment*)prevSeg, lastDeleted);
}

void gas::bind(cg3::Segment2d &top, cg3::Segment2d &bottom, cg3::Point2d &leftp, cg3::Point2d &rightp, TrapezoidalMap& tm)
{
    DagNodeArea ** first = new DagNodeArea*();
    Trapezoid trap = Trapezoid(top, bottom, leftp, rightp, (DagNodeArea*&)*first);
    tm.addTrapezoid(trap);
    tm.traps().back().getNodeRef() = new DagNodeArea(tm.traps().back());
}

const cg3::Segment2d gas::normalizeSegment(const cg3::Segment2d& seg)
{

    if(seg.p2().x()>seg.p1().x())
        return cg3::Segment2d(seg.p2(),seg.p1());
    else
        return seg;
}
