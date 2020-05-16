#include "dag_dataset.h"
#include "algorithms/algorithms.h"


Dag::Dag(Trapezoid &link)
{
    dag = link.getNode();
}

DagNodePoint* Dag::addSegmentInSingleTrap(const cg3::Segment2d& segment, Trapezoid& a, Trapezoid& d, Trapezoid& b, Trapezoid& c){
    DagNodeSegment ** si = new DagNodeSegment*();
    DagNodePoint ** qi = new DagNodePoint*();
    DagNodePoint ** pi = new DagNodePoint*();
    * si = new DagNodeSegment((DagNode*&)b.getNodeRef(), (DagNode*&)c.getNodeRef(), segment);
    * qi = new DagNodePoint((DagNode*&)*si, (DagNode*&)d.getNodeRef(), segment.p2());
    * pi= new DagNodePoint((DagNode*&)a.getNodeRef(),(DagNode*&)*qi, segment.p1());
    return *pi;
}

DagNodePoint *Dag::leftDegenerateSingleInsertion(const cg3::Segment2d &segment, Trapezoid& c , Trapezoid& a , Trapezoid& b)
{
    DagNodeSegment ** si = new DagNodeSegment*();
    DagNodePoint ** qi = new DagNodePoint*();
    * si = new DagNodeSegment((DagNode*&)a.getNodeRef(), (DagNode*&)b.getNodeRef(), segment);
    * qi = new DagNodePoint((DagNode*&)*si, (DagNode*&)c.getNodeRef(), segment.p2());
    return *qi;
}

DagNodePoint *Dag::rightDegenerateSingleInsertion(const cg3::Segment2d &segment, Trapezoid& a , Trapezoid& b , Trapezoid& c)
{
    DagNodeSegment ** si = new DagNodeSegment*();
    DagNodePoint ** pi = new DagNodePoint*();
    * si = new DagNodeSegment((DagNode*&)b.getNodeRef(), (DagNode*&)c.getNodeRef(), segment);
    * pi = new DagNodePoint((DagNode*&)a.getNodeRef(),(DagNode*&)*si, segment.p1());
    return *pi;
}

DagNodeSegment *Dag::totallyDegenerateSingleInsertion(const cg3::Segment2d &segment, Trapezoid& a, Trapezoid&b)
{
    DagNodeSegment ** si = new DagNodeSegment*();
    * si = new DagNodeSegment((DagNode*&)a.getNodeRef(), (DagNode*&)b.getNodeRef(), segment);
    return *si;
}

DagNode *&Dag::dagRef()
{
    return dag;
}

//DagNode* Dag::searchPoint(const cg3::Point2d& point){ //O(log n)
//    DagNode* tmpdag= Dag::dag;

//    while(tmpdag->getType()!=DagNode::Leaf){
//        tmpdag = ((InnerNodes*)tmpdag)->compareNodeToPoint(point);
//    }
//    return tmpdag;

//}

DagNode*& Dag::searchAndAppend(const cg3::Segment2d &seg)
{
    DagNode* tmpdag=this->dag;
    if(tmpdag->getType()==DagNode::Leaf)
        return this->dag;
    InnerNodes* chosen=(InnerNodes*)this->dag;
    while(tmpdag->getType()!=DagNode::Leaf){
        chosen = (InnerNodes*&)tmpdag;
        if(tmpdag->getType()==DagNode::X || ((DagNodeSegment*)chosen)->getSegment().p1()!=seg.p1())
            tmpdag = ((InnerNodes*&)tmpdag)->compareNodeToPoint(seg.p1());
        else
            tmpdag = (gas::matrixDet(seg, ((DagNodeSegment*)chosen)->getSegment().p2())<0)?chosen->getLeftChild():chosen->getRightChild();
    }
        if(chosen->getLeftChild()->getType()==DagNode::Leaf &&
           chosen->getRightChild()->getType()==DagNode::Leaf &&
           ((DagNodeArea*)chosen->getLeftChild())->getT().getLeftp()== seg.p1() &&
           ((DagNodeArea*)chosen->getRightChild())->getT().getLeftp()== seg.p1()){
           return (gas::matrixDet(seg,((DagNodeArea*)chosen->getLeftChild())->getT().getBottom().p2()) < 0)?chosen->getLeftChild():chosen->getRightChild();
        }
    if(chosen->leftChild == tmpdag)
        return chosen->getLeftChild();
    if(chosen->rightChild == tmpdag)
        return chosen->getRightChild();
}

DagNode* Dag::createLeftMost(const cg3::Segment2d &segment, std::vector<Trapezoid*>& traps){
    DagNodeSegment ** si = new DagNodeSegment*();
    DagNodePoint ** pi = new DagNodePoint*();
    * si = new DagNodeSegment((DagNode*&)traps[1]->getNodeRef(), (DagNode*&)traps[2]->getNodeRef(), segment);
    * pi = new DagNodePoint((DagNode*&)traps[0]->getNodeRef(),(DagNode*&)*si,segment.p1());
    return *pi;
}
DagNode *Dag::createLeftMostDegenerate(const cg3::Segment2d &segment, std::vector<Trapezoid*>& traps)
{
    DagNodeSegment ** si = new DagNodeSegment*();
    * si = new DagNodeSegment((DagNode*&)traps[0]->getNodeRef(),(DagNode*&)traps[1]->getNodeRef(), segment);
    return *si;
}

DagNode *Dag::createRightMost(const cg3::Segment2d &segment, DagNodeSegment& prevSeg, std::vector<Trapezoid*>& traps)
{
    DagNodeSegment ** si = new DagNodeSegment*();
    DagNodePoint ** qi = new DagNodePoint*();
    if(prevSeg.getLeftChild()==traps[0]->getNodeRef())
        *si= new DagNodeSegment((DagNodeArea*&)prevSeg.getLeftChild(), (DagNode*&)traps[1]->getNodeRef(), segment);
    if(prevSeg.getRightChild()==traps[1]->getNodeRef())
        *si = new DagNodeSegment((DagNode*&)traps[0]->getNodeRef(), (DagNodeArea*&)prevSeg.getRightChild(), segment);
    *qi = new DagNodePoint((DagNode*&)*si, (DagNode*&)traps[2]->getNodeRef(), segment.p2());
    return *qi;
}

DagNode *Dag::createRightMostDegenerate(const cg3::Segment2d &segment, DagNodeSegment& prevSeg, std::vector<Trapezoid*>& traps)
{
    DagNodeSegment ** si = new DagNodeSegment*();
    if(prevSeg.getLeftChild()==traps[0]->getNodeRef())
        *si= new DagNodeSegment((DagNodeArea*&)prevSeg.getLeftChild(), (DagNode*&)traps[1]->getNodeRef(), segment);
    if(prevSeg.getRightChild()==traps[1]->getNodeRef())
        *si = new DagNodeSegment((DagNode*&)traps[0]->getNodeRef(), (DagNodeArea*&)prevSeg.getRightChild(), segment);
    return *si;
}

DagNode *Dag::createIntermediate(const cg3::Segment2d &segment, DagNodeSegment& prevSeg, std::vector<Trapezoid*>& traps)
{
    DagNodeSegment ** si = new DagNodeSegment*();
    if(prevSeg.getLeftChild()==traps[0]->getNodeRef())
        *si= new DagNodeSegment((DagNodeArea*&)prevSeg.getLeftChild(), (DagNode*&)traps[1]->getNodeRef(), segment);
    if(prevSeg.getRightChild()==traps[1]->getNodeRef())
        *si = new DagNodeSegment((DagNode*&)traps[0]->getNodeRef(), (DagNodeArea*&)prevSeg.getRightChild(), segment);
    return *si;
}









