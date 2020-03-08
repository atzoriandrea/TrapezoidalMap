#include <data_structures/dag_node.h>

DagNodePoint::DagNodePoint(cg3::Point2d& point){
    this->point = &point;
    this->setLeftChild(new DagNodeArea());
    this->setRightChild(new DagNodeArea());
}
DagNodeSegment::DagNodeSegment(cg3::Segment2d* segment){
    this->segment = segment;
    this->setLeftChild(new DagNodeArea());
    this->setRightChild(new DagNodeArea());
}
DagNodeArea::DagNodeArea(){
    this->setLeftChild(nullptr);
    this->setRightChild(nullptr);
}


//verfica se il punto point è sopra o sotto il segmento
double matrixDet(cg3::Segment2d* segment, const cg3::Point2d& point){
    return (segment->p1().x()*(segment->p2().y()-point.y()))-
            (segment->p1().y()*(segment->p2().x()-point.x()))-
            ((segment->p2().x()*point.y())-(segment->p2().y()*point.x()));
}

//template<typename T>
DagNode *DagNodePoint::compareTo(const cg3::Point2d& point){
    return (point.x()<this->point->x())? this->getLeftChild() : this->getRightChild();
}
//template<typename T>
DagNode *DagNodeSegment::compareTo(const cg3::Point2d& point){
    double res;
    res = matrixDet(this->segment, point);
    return (res < 0)? this->getRightChild():this->getLeftChild();
}

//rende l'indirizzo del puntatore al figlio sinistro. Modificandone il valore, verrà modificato solo il figlio puntato
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



DagNode *DagNodeArea::compareTo(const cg3::Point2d &point){}
