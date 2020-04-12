#include <data_structures/dag_node.h>

DagNode::DagNode(): lcValue(nullptr), rcValue(nullptr), leftChild(lcValue), rightChild(rcValue){}
DagNodePoint::DagNodePoint(const cg3::Point2d& point) :point(point){}
DagNodeSegment::DagNodeSegment(const cg3::Segment2d& segment) :segment(segment){}
DagNodeArea::DagNodeArea(Trapezoid &t) : trap(t){}

DagNode *&DagNodePoint::compareNodeToPoint(const cg3::Point2d& point){
    return (this->point.x()>point.x())? this->getLeftChild() : this->getRightChild();
}

DagNode *&DagNodePoint::compareNodeToSegment(const cg3::Segment2d &segment){
    if(segment.p2().x()<this->point.x())
        return this->getLeftChild();
    if(segment.p1().x()>this->point.x())
        return this->getRightChild();
    long double res = matrixDet(segment, this->point);
    return (res < 0)? this->getLeftChild():this->getRightChild();

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
DagNode *& DagNodeSegment::compareNodeToPoint(const cg3::Point2d& point){
    long double res;
    res = matrixDet(this->segment, point);
    return (res < 0)? this->getRightChild():this->getLeftChild();
}

DagNode *&DagNodeSegment::compareNodeToSegment(const cg3::Segment2d &segment){
    long double c1 = matrixDet(segment, this->segment.p1());
    long double c2 = matrixDet(segment, this->segment.p2());
    if((c1 < 0 && c2 > 0) ||( c1 > 0 && c2 < 0 )){
        return(abs(c1) > abs(c2))?this->getLeftChild():this->getRightChild();
    }
    return (c1 < 0 && c2 < 0) ? this->getLeftChild():this->getRightChild();

}


int DagNodeSegment::oneOrBoth(const cg3::Segment2d & segment)
{
    enum direction { left = 1, right = 2, both = 3 };
    return(this->compareNodeToSegment(segment)==this->getLeftChild())? left : right;
}


DagNode *& DagNode::getLeftChild() const
{
    return leftChild;
}

//DagNode* const & DagNode::getAddress()
//{
//    return %this;
//}

void DagNode::setLeftChild(DagNode * const value)
{
    lcValue = value;
}



DagNode **DagNode::lcPointerAddress()
{
    return &leftChild;
}

DagNode **DagNode::rcPointerAddress()
{
    return &rightChild;
}

DagNode *&DagNode::getRightChild() const
{
    return rightChild;
}

void DagNode::setRightChild(DagNode * const &value)
{
    rcValue = value;
}



DagNode *&DagNodeArea::compareNodeToPoint(const cg3::Point2d &point){}

DagNode *&DagNodeArea::compareNodeToSegment(const cg3::Segment2d &segment){}


int DagNodeArea::oneOrBoth(const cg3::Segment2d &){}

Trapezoid& DagNodeArea::getT() const
{
    return trap;
}

void DagNodeArea::setTrap(Trapezoid &value) const
{
    trap=value;
}


