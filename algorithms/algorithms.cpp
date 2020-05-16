#include "algorithms.h"

#define REDUCER 100000.0
#define REDUCER2 1000000.0
#define BOUNDINGBOX (1e+6)+1
cg3::Point2d gas::intersection(const cg3::Segment2d & edge, const  cg3::Point2d & point){
      cg3::Line2 line = cg3::Line2(edge.p1(), edge.p2());
      cg3::Line2 line2 = cg3::Line2(cg3::Point2d(point.x(), -BOUNDINGBOX), cg3::Point2d(point.x(), BOUNDINGBOX));
      cg3::Point2d res = line.intersection(line2);
      res.x() = floor((res.x()*10000)+0.5)/10000.0;
      res.y() = floor((res.y()*10000)+0.5)/10000.0;
      return res;
}


long double gas::matrixDet(const cg3::Segment2d& segment, const cg3::Point2d& point){
    double mat[3][3] = {{(segment.p1().x()/REDUCER),(segment.p1().y()/REDUCER),1.0},
                                           {(segment.p2().x()/REDUCER),(segment.p2().y()/REDUCER), 1.0},
                                           {(point.x()/REDUCER), (point.y()/REDUCER),1.0}};
    return mat[0][0]*((mat[1][1]*mat[2][2]) - (mat[2][1]*mat[1][2]))
            -mat[0][1]*(mat[1][0]*mat[2][2] - mat[2][0]*mat[1][2]) +
            mat[0][2]*(mat[1][0]*mat[2][1] - mat[2][0]*mat[1][1]);
}


void gas::followSegment(const cg3::Segment2d &segment, Trapezoid *trapezoid, TrapezoidalMap* tm, Dag* dag)
{
    Trapezoid * lastDeleted;
    Trapezoid * next;
    std::vector<Trapezoid*> traps;
    DagNode* prevSeg;
    unsigned long iter = 0;
    while(segment.p2().x() > trapezoid->getRightp().x()){
        if(gas::matrixDet(segment, trapezoid->getRightp())<0)
            next = &trapezoid->getRightUp();
        else
            next = &trapezoid->getRightDown();
        DagNode *& copy = (DagNode*&)trapezoid->getNodeRef();
        if(iter==0){
            if(segment.p1()==trapezoid->getLeftp()){
                traps = tm->createLeftMostDegenerate(*trapezoid,segment,lastDeleted);
                copy = dag->createLeftMostDegenerate(segment, traps);
                prevSeg = copy;
            }
            else{
                traps = tm->createLeftMost(*trapezoid,segment,lastDeleted);
                copy = dag->createLeftMost(segment, traps);
                prevSeg=((DagNodePoint*)copy)->getRightChild();
            }
        }
        else{
            traps = tm->createIntermediate(*trapezoid,segment,*(DagNodeSegment*)prevSeg, lastDeleted);
            copy = dag->createIntermediate(segment,*(DagNodeSegment*)prevSeg, traps);
            prevSeg = copy;
        }
        iter++;
        trapezoid = next;
    }
    DagNode *& copy = (DagNode*&)trapezoid->getNodeRef();
    if(segment.p2()==trapezoid->getRightp()){
        traps = tm->createRightMostDegenerate(*trapezoid,segment,*(DagNodeSegment*)prevSeg, lastDeleted);
        copy = dag->createRightMostDegenerate(segment,*(DagNodeSegment*)prevSeg, traps);
    }
    else{
        traps = tm->createRightMost(*trapezoid,segment,*(DagNodeSegment*)prevSeg, lastDeleted);
        copy = dag->createRightMost(segment,*(DagNodeSegment*)prevSeg, traps);
    }

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

    if(seg.p2().x()<seg.p1().x())
        return cg3::Segment2d(seg.p2(),seg.p1());
    else
        return seg;
}
