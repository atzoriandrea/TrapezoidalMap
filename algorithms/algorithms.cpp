#include "algorithms.h"

#define REDUCER 100000.0
#define BOUNDINGBOX (1e+6)+1

/**
 * @brief computes intersection point between a segment and a point (it will be treated as a vertical line passng through the point)
 * @param reference to segment
 * @param reference to point
 * @return intersection point
 */
cg3::Point2d gas::intersection(const cg3::Segment2d & edge, const  cg3::Point2d & point){
      cg3::Line2 line = cg3::Line2(edge.p1(), edge.p2());
      cg3::Line2 line2 = cg3::Line2(cg3::Point2d(point.x(), -BOUNDINGBOX), cg3::Point2d(point.x(), BOUNDINGBOX));
      cg3::Point2d res = line.intersection(line2);
      res.x() = point.x(); //performing this operation avoids rounding errors (causing pivoting issues)
      res.y() = std::round((res.y()*10000))/10000.0;
      return res;
}

/**
 * @brief computes determinant to detect if a point is under, above or inline with a segment
 * @param reference to segment
 * @param reference to point
 * @return matrix determinant
 */
long double gas::matrixDet(const cg3::Segment2d& segment, const cg3::Point2d& point){
    double mat[3][3] = {{(segment.p1().x()/REDUCER),(segment.p1().y()/REDUCER),1.0},
                                           {(segment.p2().x()/REDUCER),(segment.p2().y()/REDUCER), 1.0},
                                           {(point.x()/REDUCER), (point.y()/REDUCER),1.0}};
    return mat[0][0]*((mat[1][1]*mat[2][2]) - (mat[2][1]*mat[1][2]))
            -mat[0][1]*(mat[1][0]*mat[2][2] - mat[2][0]*mat[1][2]) +
            mat[0][2]*(mat[1][0]*mat[2][1] - mat[2][0]*mat[1][1]);
}

/**
 * @brief follows the segment that wants to be added through the trapezoids it traverses and updates, at every step, trapezoidal map and dag
 * @param segment
 * @param starting trapezoid
 * @param reference to trapezoidal map
 * @param reference to dag
 */
void gas::followSegment(const cg3::Segment2d &segment, Trapezoid *trapezoid, TrapezoidalMap& tm, Dag& dag)
{
    Trapezoid * lastDeleted;
    Trapezoid * next;
    std::vector<Trapezoid*> traps;
    DagNode* prevSeg = nullptr;
    DagNode* to_be_deleted = nullptr;
    unsigned long iter = 0;
    while(segment.p2().x() > trapezoid->getRightp().x()){
        if(gas::matrixDet(segment, trapezoid->getRightp())<0)
            next = &trapezoid->getRightUp(); //next trapezoid according to segment direction
        else
            next = &trapezoid->getRightDown();
        DagNode *& copy = (DagNode*&)trapezoid->getNodeRef();
        to_be_deleted = &*copy;
        if(iter==0){
            if(segment.p1()==trapezoid->getLeftp()){ //leftmost trapezoid update - degenerate case
                traps = tm.createLeftMostDegenerate(*trapezoid,segment,lastDeleted); //tm update
                copy = dag.createLeftMostDegenerate(segment, traps); //dag update according to tm
                prevSeg = copy; //last created type Y node
            }
            else{ // leftmost trapezoid update - standard case
                traps = tm.createLeftMost(*trapezoid,segment,lastDeleted); //tm update
                copy = dag.createLeftMost(segment, traps); //dag update according to tm
                prevSeg=((DagNodePoint*)copy)->getRightChild(); //last created type Y node
            }
        }
        else{ // intermediate trapezoid update
            traps = tm.createIntermediate(*trapezoid,segment,*(DagNodeSegment*)prevSeg, lastDeleted); //tm update
            copy = dag.createIntermediate(segment,*(DagNodeSegment*)prevSeg, traps); //dag update according to tm
            prevSeg = copy; //last created type Y node
        }
        iter++;
        trapezoid = next;
        delete to_be_deleted;
    }
    DagNode *& copy = (DagNode*&)trapezoid->getNodeRef();
    to_be_deleted = &*copy;
    if(segment.p2()==trapezoid->getRightp()){//rightmost trapezoid update - degenerate case
        traps = tm.createRightMostDegenerate(*trapezoid,segment,*(DagNodeSegment*)prevSeg, lastDeleted);//tm update
        copy = dag.createRightMostDegenerate(segment,*(DagNodeSegment*)prevSeg, traps);//dag update according to tm
    }
    else{//leftmost trapezoid update - standard case
        traps = tm.createRightMost(*trapezoid,segment,*(DagNodeSegment*)prevSeg, lastDeleted);//tm update
        copy = dag.createRightMost(segment,*(DagNodeSegment*)prevSeg, traps);//dag update according to tm
    }
    delete to_be_deleted;
}

/**
 * @brief creates a new trapezoid and binds it to the node that will link it, creating the cross link.
 * @param trapezoid construction directives
 * @param reference to trapezoidal map
 */
void gas::bind(cg3::Segment2d &top, cg3::Segment2d &bottom, cg3::Point2d &leftp, cg3::Point2d &rightp, TrapezoidalMap& tm)
{
    DagNodeArea ** first = new DagNodeArea*(); //create new Leaf Node pointer
    Trapezoid trap = Trapezoid(top, bottom, leftp, rightp, (DagNodeArea*&)*first); //new trapezoid creation, linked to just created node
    tm.addTrapezoid(trap); //trapezoid insertion in trapezoidal map
    tm.traps().back().getNodeRef() = new DagNodeArea(tm.traps().back()); //links the node to the trapezoid and adds its iterator
}

/**
 * @brief normalize a segment in order to make p1.x() < p2.x()
 * @param reference to old trapezoid
 * @return list of pointers to new trapezoids
 */
const cg3::Segment2d gas::normalizeSegment(const cg3::Segment2d& seg)
{
    if(seg.p2().x()<seg.p1().x())
        return cg3::Segment2d(seg.p2(),seg.p1());
    else
        return seg;
}


