#include <data_structures/dag_node.h>
#include "algorithms/algorithms.h"

/**
 * @brief DagNode constructor
 */
DagNode::DagNode(){}

/**
 * @return node's type
 */
DagNode::NodeType DagNode::getType()
{
    return type;
}

/**
 * @brief Leaf node constructor
 * @param Trapezoidal Map's trapezoid reference
 */
DagNodeArea::DagNodeArea(Trapezoid &t) :trap(t){ type = NodeType::Leaf; };


/**
 * @brief Internal node constructor
 * @param reference to left child node
 * @param reference to right child node
 */
InnerNodes::InnerNodes(DagNode*& lc, DagNode*& rc): leftChild(lc), rightChild(rc){}
InnerNodes::InnerNodes(DagNodeArea*& left, DagNode*& rc): leftChild((DagNode*&)left), rightChild(rc) {}
InnerNodes::InnerNodes(DagNode*& lc, DagNodeArea*& right): leftChild(lc), rightChild((DagNode*&)right){}

/**
 * @brief Type X node constructor
 * @param reference to left child node
 * @param reference to right child node
 */
DagNodePoint::DagNodePoint(DagNode *&lc, DagNode *&rc, const cg3::Point2d &point):InnerNodes(lc, rc),point(point){ type = NodeType::X; }
DagNodePoint::DagNodePoint(DagNodeArea *&left, DagNode *&rc, const cg3::Point2d &point):InnerNodes(left, rc),point(point){type = NodeType::X; }
DagNodePoint::DagNodePoint(DagNode *&lc, DagNodeArea *&right, const cg3::Point2d &point):InnerNodes(lc, right),point(point){type = NodeType::X; }

/**
 * @brief Type Y node constructor
 * @param reference to left child node
 * @param reference to right child node
 */
DagNodeSegment::DagNodeSegment(DagNode*& lc, DagNode*& rc, const cg3::Segment2d& segment) :InnerNodes(lc, rc),segment(segment){ type = NodeType::Y; }
DagNodeSegment::DagNodeSegment(DagNodeArea *&left, DagNode *&rc, const cg3::Segment2d &segment):InnerNodes(left, rc),segment(segment){type = NodeType::Y; }
DagNodeSegment::DagNodeSegment(DagNode *&lc, DagNodeArea *&right, const cg3::Segment2d &segment):InnerNodes(lc, right),segment(segment){type = NodeType::Y; }





/**
 * @brief X-coordinate point comparison
 * @param reference to a point
 * @return correct child of type X node
 */
DagNode* DagNodePoint::compareNodeToPoint(const cg3::Point2d& point){
    return (point.x()<this->point.x())? this->getLeftChild() : this->getRightChild();
}

/**
 * @brief X-coordinate segment comparison
 * @param reference to a Segment
 * @return correct child of type X node
 */
DagNode* DagNodePoint::compareNodeToSegment(const cg3::Segment2d &segment){
    if(segment.p2().x()<=this->point.x())
        return this->getLeftChild();
    if(segment.p1().x()>=this->point.x())
        return this->getRightChild();
    long double res = gas::matrixDet(segment, this->point);
    return (res < 0)? this->getLeftChild():this->getRightChild();

}

/**
 * @brief get Point in type X node
 * @return point contained
 */
cg3::Point2d DagNodePoint::getPoint() const
{
    return point;
}

/**
 * @brief get Segment in type Y node
 * @return Segment contained
 */
cg3::Segment2d DagNodeSegment::getSegment() const
{
    return segment;
}

/**
 * @brief check if the point is under or above the segment
 * @param reference to a Point
 * @return correct child of type Y node
 */
DagNode* DagNodeSegment::compareNodeToPoint(const cg3::Point2d& point){
    long double res;
    res = gas::matrixDet(this->segment, point);

    return (res < 0)? this->getRightChild():this->getLeftChild();
}

/**
 * @brief check if the segment is under or above another segment
 * @param reference to a Segment
 * @return correct child of type Y node
 */
DagNode* DagNodeSegment::compareNodeToSegment(const cg3::Segment2d &segment){
    long double c1 = gas::matrixDet(segment, this->segment.p1());
    long double c2 = gas::matrixDet(segment, this->segment.p2());
    if((c1 < 0 && c2 > 0) ||( c1 > 0 && c2 < 0 )){
        return(abs(c1) > abs(c2))?this->getLeftChild():this->getRightChild();
    }
    return (c1 < 0 && c2 < 0) ? this->getLeftChild():this->getRightChild();

}

/**
 * @return reference to left child pointer
 */
DagNode *&InnerNodes::getLeftChild() const
{
    return leftChild;
}

/**
 * @return reference to right child pointer
 */
DagNode *&InnerNodes::getRightChild() const
{
    return rightChild;
}


/**
 * @brief set left child of current node
 * @param pointer to a node
 */
void InnerNodes::setLeftChild(DagNode * const value)
{
    leftChild = value;
}

/**
 * @brief set right child of current node
 * @param pointer to a node
 */
void InnerNodes::setRightChild(DagNode * const value)
{
    rightChild = value;
}

/**
 * @return reference to contained Trapezoid
 */
Trapezoid& DagNodeArea::getT() const
{
    return trap;
}


