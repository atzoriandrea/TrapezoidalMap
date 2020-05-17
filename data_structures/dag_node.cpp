#include <data_structures/dag_node.h>
#include "algorithms/algorithms.h"

DagNode::DagNode(){}

DagNode::NodeType DagNode::getType()
{
    return type;
}

DagNodeArea::DagNodeArea(Trapezoid &t) :trap(t){ type = NodeType::Leaf; };


InnerNodes::InnerNodes(DagNode*& lc, DagNode*& rc): /*lcValue(lc), rcValue(rc),*/ leftChild(lc), rightChild(rc){}
InnerNodes::InnerNodes(DagNodeArea*& left, DagNode*& rc): leftChild((DagNode*&)left), rightChild(rc) {}
InnerNodes::InnerNodes(DagNode*& lc, DagNodeArea*& right): leftChild(lc), rightChild((DagNode*&)right){}


DagNodePoint::DagNodePoint(DagNode *&lc, DagNode *&rc, const cg3::Point2d &point):InnerNodes(lc, rc),point(point){ type = NodeType::X; }
DagNodePoint::DagNodePoint(DagNodeArea *&left, DagNode *&rc, const cg3::Point2d &point):InnerNodes(left, rc),point(point){type = NodeType::X; }
DagNodePoint::DagNodePoint(DagNode *&lc, DagNodeArea *&right, const cg3::Point2d &point):InnerNodes(lc, right),point(point){type = NodeType::X; }

DagNodeSegment::DagNodeSegment(DagNode*& lc, DagNode*& rc, const cg3::Segment2d& segment) :InnerNodes(lc, rc),segment(segment){ type = NodeType::Y; }
DagNodeSegment::DagNodeSegment(DagNodeArea *&left, DagNode *&rc, const cg3::Segment2d &segment):InnerNodes(left, rc),segment(segment){type = NodeType::Y; }
DagNodeSegment::DagNodeSegment(DagNode *&lc, DagNodeArea *&right, const cg3::Segment2d &segment):InnerNodes(lc, right),segment(segment){type = NodeType::Y; }






DagNode* DagNodePoint::compareNodeToPoint(const cg3::Point2d& point){
    return (point.x()<this->point.x())? this->getLeftChild() : this->getRightChild();
}

DagNode* DagNodePoint::compareNodeToSegment(const cg3::Segment2d &segment){
    if(segment.p2().x()<=this->point.x())
        return this->getLeftChild();
    if(segment.p1().x()>=this->point.x())
        return this->getRightChild();
    long double res = gas::matrixDet(segment, this->point);
    return (res < 0)? this->getLeftChild():this->getRightChild();

}

std::list<DagNodePoint>::iterator &DagNodePoint::getItr()
{
    return itr;
}


std::list<DagNodeArea>::iterator &DagNodeArea::getItr()
{
    return itr;
}
std::list<DagNodeSegment>::iterator &DagNodeSegment::getItr()
{
    return itr;
}


void DagNodePoint::setItr(const std::list<DagNodePoint>::iterator &value)
{
    itr = value;
}

cg3::Point2d DagNodePoint::getPoint() const
{
    return point;
}

void DagNodeSegment::setItr(const std::list<DagNodeSegment>::iterator &value)
{
    itr = value;
}

cg3::Segment2d DagNodeSegment::getSegment() const
{
    return segment;
}

void DagNodeArea::setItr(const std::list<DagNodeArea>::iterator &value)
{
    itr = value;
}


DagNode* DagNodeSegment::compareNodeToPoint(const cg3::Point2d& point){
    long double res;
    res = gas::matrixDet(this->segment, point);

    return (res < 0)? this->getRightChild():this->getLeftChild();
}

DagNode* DagNodeSegment::compareNodeToSegment(const cg3::Segment2d &segment){
    long double c1 = gas::matrixDet(segment, this->segment.p1());
    long double c2 = gas::matrixDet(segment, this->segment.p2());
    if((c1 < 0 && c2 > 0) ||( c1 > 0 && c2 < 0 )){
        return(abs(c1) > abs(c2))?this->getLeftChild():this->getRightChild();
    }
    return (c1 < 0 && c2 < 0) ? this->getLeftChild():this->getRightChild();

}

DagNode *&InnerNodes::getLeftChild() const
{
    return leftChild;
}

DagNode *&InnerNodes::getRightChild() const
{
    return rightChild;
}

void InnerNodes::setLeftChild(DagNode * const value)
{
    leftChild = value;
}

void InnerNodes::setRightChild(DagNode * const value)
{
    rightChild = value;
}


Trapezoid& DagNodeArea::getT() const
{
    return trap;
}

void DagNodeArea::setTrap(Trapezoid &value) const
{
    trap=value;
}


