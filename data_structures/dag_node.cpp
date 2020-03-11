#include <data_structures/dag_node.h>

DagNodePoint::DagNodePoint(cg3::Point2d& point) :
    point(point){
    this->setLeftChild(new DagNodeArea());
    this->setRightChild(new DagNodeArea());
}
DagNodeSegment::DagNodeSegment(cg3::Segment2d& segment) :
    segment(segment){
    this->setLeftChild(new DagNodeArea());
    this->setRightChild(new DagNodeArea());
}
DagNodeArea::DagNodeArea(){
    this->setLeftChild(nullptr);
    this->setRightChild(nullptr);
}


//verfica se il punto point è sopra o sotto il segmento calcolando il determinante di [[sx1,sy1,1][sx2,sy2,1][px,py,1]]
double matrixDet(const cg3::Segment2d& segment, const cg3::Point2d& point){
    return (segment.p1().x()*(segment.p2().y()-point.y()))-
            (segment.p1().y()*(segment.p2().x()-point.x()))-
            ((segment.p2().x()*point.y())-(segment.p2().y()*point.x()));
}

//template<typename T>
DagNode *DagNodePoint::compareNodeToPoint(const cg3::Point2d& point){
    return (this->point.x()>point.x())? this->getLeftChild() : this->getRightChild();
}

DagNode *DagNodePoint::compareNodeToSegment(const cg3::Segment2d &segment){
    double res = matrixDet(segment, this->point);
    return (res > 0)? this->getLeftChild():this->getRightChild();

}


int DagNodePoint::oneOrBoth(const cg3::Segment2d & segment)
{
    enum direction { left = 1, right = 2, both = 3 };
    double xValue = this->point.x();
    if(segment.p1().x() < xValue && segment.p2().x() > xValue)
        return both;
    return (this->compareNodeToSegment(segment)==this->getLeftChild())? left : right;
}
//template<typename T>
DagNode *DagNodeSegment::compareNodeToPoint(const cg3::Point2d& point){
    double res;
    res = matrixDet(this->segment, point);
    return (res < 0)? this->getRightChild():this->getLeftChild();
}

DagNode *DagNodeSegment::compareNodeToSegment(const cg3::Segment2d &segment){
    double c1 = matrixDet(segment, this->segment.p1());
    double c2 = matrixDet(segment, this->segment.p2());
    return (c1 < 0 && c2 < 0) ? this->getLeftChild():this->getRightChild();

}


int DagNodeSegment::oneOrBoth(const cg3::Segment2d & segment)
{
    enum direction { left = 1, right = 2, both = 3 };
    return(this->compareNodeToSegment(segment)==this->getLeftChild())? left : right;
}


DagNode * DagNode::getLeftChild() const
{
    return leftChild;
}

void DagNode::setLeftChild(DagNode *value)
{
    leftChild = value;
}

DagNode * DagNode::getRightChild() const
{
    return rightChild;
}

void DagNode::setRightChild(DagNode *value)
{
    rightChild = value;
}

DagNode **DagNode::lcPointerAddress()
{
    return &leftChild;
}

DagNode **DagNode::rcPointerAddress()
{
    return &rightChild;
}



DagNode *DagNodeArea::compareNodeToPoint(const cg3::Point2d &point){}

DagNode *DagNodeArea::compareNodeToSegment(const cg3::Segment2d &segment){}


int DagNodeArea::oneOrBoth(const cg3::Segment2d &){}
