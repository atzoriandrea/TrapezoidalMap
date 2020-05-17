#include "dag_dataset.h"
#include "algorithms/algorithms.h"

/**
 * @brief Dag constructor
 * @param Trapezoidal Map's trapezoid's reference
 */
Dag::Dag(Trapezoid &link)
{
    dag = link.getNode();
}

/**
 * @brief Bottom-Up construction of the new sub-tree with trapezoid's references in single trapezoid insertion case
 * @param reference to inserted segment
 * @param reference to leftmost trapezoid
 * @param reference to rightmost trapezoid
 * @param reference to above segment's trapezoid
 * @param reference to under segment's trapezoid
 * @return root of the new sub-tree
 */
DagNodePoint* Dag::addSegmentInSingleTrap(const cg3::Segment2d& segment, Trapezoid& a, Trapezoid& d, Trapezoid& b, Trapezoid& c){

    DagNodeSegment ** si = new DagNodeSegment*();
    DagNodePoint ** qi = new DagNodePoint*();
    DagNodePoint ** pi = new DagNodePoint*();
    * si = new DagNodeSegment((DagNode*&)b.getNodeRef(), (DagNode*&)c.getNodeRef(), segment);
    * qi = new DagNodePoint((DagNode*&)*si, (DagNode*&)d.getNodeRef(), segment.p2());
    * pi= new DagNodePoint((DagNode*&)a.getNodeRef(),(DagNode*&)*qi, segment.p1());
    return *pi;
}

/**
 * @brief Bottom-Up construction of the new sub-tree with trapezoid's references in left degenerated single trapezoid insertion case
 * @param reference to inserted segment
 * @param reference to rightmost trapezoid
 * @param reference to above segment's trapezoid
 * @param reference to under segment's trapezoid
 * @return root of the new sub-tree
 */
DagNodePoint *Dag::leftDegenerateSingleInsertion(const cg3::Segment2d &segment, Trapezoid& c , Trapezoid& a , Trapezoid& b)
{
    DagNodeSegment ** si = new DagNodeSegment*();
    DagNodePoint ** qi = new DagNodePoint*();
    * si = new DagNodeSegment((DagNode*&)a.getNodeRef(), (DagNode*&)b.getNodeRef(), segment);
    * qi = new DagNodePoint((DagNode*&)*si, (DagNode*&)c.getNodeRef(), segment.p2());
    return *qi;
}

/**
 * @brief Bottom-Up construction of the new sub-tree with trapezoid's references in right degenerated single trapezoid insertion case
 * @param reference to inserted segment
 * @param reference to leftmost trapezoid
 * @param reference to above segment's trapezoid
 * @param reference to under segment's trapezoid
 * @return root of the new sub-tree
 */
DagNodePoint *Dag::rightDegenerateSingleInsertion(const cg3::Segment2d &segment, Trapezoid& a , Trapezoid& b , Trapezoid& c)
{
    DagNodeSegment ** si = new DagNodeSegment*();
    DagNodePoint ** pi = new DagNodePoint*();
    * si = new DagNodeSegment((DagNode*&)b.getNodeRef(), (DagNode*&)c.getNodeRef(), segment);
    * pi = new DagNodePoint((DagNode*&)a.getNodeRef(),(DagNode*&)*si, segment.p1());
    return *pi;
}

/**
 * @brief Bottom-Up construction of the new sub-tree with trapezoid's references in both left and right degenerated trapezoid insertion case
 * @param reference to inserted segment
 * @param reference to above segment's trapezoid
 * @param reference to under segment's trapezoid
 * @return root of the new sub-tree
 */
DagNodeSegment *Dag::totallyDegenerateSingleInsertion(const cg3::Segment2d &segment, Trapezoid& a, Trapezoid&b)
{
    DagNodeSegment ** si = new DagNodeSegment*();
    * si = new DagNodeSegment((DagNode*&)a.getNodeRef(), (DagNode*&)b.getNodeRef(), segment);
    return *si;
}

/**
 * @return reference to dag's root
 */
DagNode *&Dag::dagRef()
{
    return dag;
}

/**
 * @brief clears every node's childs
 * @param pointer to a dag node
 */
void Dag::clear(DagNode* node)
{
   if(node!= nullptr && node->getType()!=DagNode::Leaf){
       clear(((InnerNodes*)node)->getLeftChild());
       delete ((InnerNodes*)node)->getLeftChild();
       ((InnerNodes*)node)->setLeftChild(nullptr);
       clear(((InnerNodes*)node)->getRightChild());
       delete ((InnerNodes*)node)->getRightChild();
       ((InnerNodes*)node)->setRightChild(nullptr);
   }
}

/**
 * @brief search for the node linking the trapezoid containing the leftpoint of the segment
 * @param reference to inserted segment
 * @return reference to pointer pointing the correct leaf
 */
DagNode*& Dag::searchAndAppend(const cg3::Segment2d &seg)
{
    DagNode* tmpdag=this->dag;
    if(tmpdag->getType()==DagNode::Leaf)
        return this->dag;
    InnerNodes* chosen=(InnerNodes*)this->dag;
    while(tmpdag->getType()!=DagNode::Leaf){
        chosen = (InnerNodes*)tmpdag;
        if(tmpdag->getType()==DagNode::X || ((DagNodeSegment*)chosen)->getSegment().p1()!=seg.p1())
            tmpdag = ((InnerNodes*)tmpdag)->compareNodeToPoint(seg.p1());
        else
            tmpdag = (gas::matrixDet(seg, ((DagNodeSegment*)chosen)->getSegment().p2())<0)?chosen->getLeftChild():chosen->getRightChild();
    }
    return (chosen->leftChild == tmpdag)?chosen->getLeftChild():chosen->getRightChild();
}

/**
 * @brief Leftmost bottom-Up construction of the new sub-tree with trapezoid's references in multi trapezoid insertion case
 * @param reference to inserted segment
 * @param vector of pointers to new trapezoids (leftmost, above segment, under segment)
 * @return root of the new sub-tree
 */
DagNode* Dag::createLeftMost(const cg3::Segment2d &segment, std::vector<Trapezoid*>& traps){
    DagNodeSegment ** si = new DagNodeSegment*();
    DagNodePoint ** pi = new DagNodePoint*();
    * si = new DagNodeSegment((DagNode*&)traps[1]->getNodeRef(), (DagNode*&)traps[2]->getNodeRef(), segment);
    * pi = new DagNodePoint((DagNode*&)traps[0]->getNodeRef(),(DagNode*&)*si,segment.p1());
    return *pi;
}

/**
 * @brief Degenerate leftmost bottom-Up construction of the new sub-tree with trapezoid's references in multi trapezoid insertion case
 * @param reference to inserted segment
 * @param vector of pointers to new trapezoids (above segment, under segment)
 * @return root of the new sub-tree
 */
DagNode *Dag::createLeftMostDegenerate(const cg3::Segment2d &segment, std::vector<Trapezoid*>& traps)
{
    DagNodeSegment ** si = new DagNodeSegment*();
    * si = new DagNodeSegment((DagNode*&)traps[0]->getNodeRef(),(DagNode*&)traps[1]->getNodeRef(), segment);
    return *si;
}

/**
 * @brief Rightmost bottom-Up construction of the new sub-tree with trapezoid's references in multi trapezoid insertion case
 * @param reference to inserted segment
 * @param previous inserted segment node (type Y)
 * @param vector of pointers to new trapezoids (above segment, under segment, rightmost)
 * @return root of the new sub-tree
 */
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

/**
 * @brief Degenerate rightmost bottom-Up construction of the new sub-tree with trapezoid's references in multi trapezoid insertion case
 * @param reference to inserted segment
 * @param previous inserted segment node (type Y)
 * @param vector of pointers to new trapezoids (leftmost, above segment, under segment)
 * @return root of the new sub-tree
 */
DagNode *Dag::createRightMostDegenerate(const cg3::Segment2d &segment, DagNodeSegment& prevSeg, std::vector<Trapezoid*>& traps)
{
    DagNodeSegment ** si = new DagNodeSegment*();
    if(prevSeg.getLeftChild()==traps[0]->getNodeRef())
        *si= new DagNodeSegment((DagNodeArea*&)prevSeg.getLeftChild(), (DagNode*&)traps[1]->getNodeRef(), segment);
    if(prevSeg.getRightChild()==traps[1]->getNodeRef())
        *si = new DagNodeSegment((DagNode*&)traps[0]->getNodeRef(), (DagNodeArea*&)prevSeg.getRightChild(), segment);
    return *si;
}

/**
 * @brief Segment node bottom-Up construction of the new sub-tree with trapezoid's references in multi trapezoid insertion case
 * @param reference to inserted segment
 * @param previous inserted segment node (type Y)
 * @param vector of pointers to new trapezoids (leftmost, above segment, under segment)
 * @return root of the new sub-tree
 */
DagNode *Dag::createIntermediate(const cg3::Segment2d &segment, DagNodeSegment& prevSeg, std::vector<Trapezoid*>& traps)
{
    DagNodeSegment ** si = new DagNodeSegment*();
    if(prevSeg.getLeftChild()==traps[0]->getNodeRef())
        *si= new DagNodeSegment((DagNodeArea*&)prevSeg.getLeftChild(), (DagNode*&)traps[1]->getNodeRef(), segment);
    if(prevSeg.getRightChild()==traps[1]->getNodeRef())
        *si = new DagNodeSegment((DagNode*&)traps[0]->getNodeRef(), (DagNodeArea*&)prevSeg.getRightChild(), segment);
    return *si;
}









