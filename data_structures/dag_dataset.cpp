#include <data_structures/dag_dataset.h>
#include <data_structures/dag_node.h>

DagNode* Dag::dag = nullptr;

Dag::Dag(){
    dag=nullptr;
}
void Dag::addSegment(cg3::Segment2d& segment){
    DagNode* split;
    DagNode* s1;
    DagNode* s2;
    if(Dag::dag!=nullptr){
        split = splitNode(segment);
        s1 =split->compareTo(segment.p1());
        s2 =split->compareTo(segment.p2());
    }
    else{
        cg3::Point2d p = segment.p1();
        DagNode* dg = new DagNodePoint(p);
        cg3::Point2d p2 = segment.p2();
        DagNode* dg2 = new DagNodePoint(p);
        dg->setRightChild(dg2);
        DagNode* dgs = new DagNodeSegment(&segment);
        dg2->setLeftChild(dgs);
        Dag::dag = dg;

    }
    if(s1 == s2 && s1->getLeftChild()==nullptr){

        cg3::Point2d p = segment.p1();
        DagNode* dg = new DagNodePoint(p);
        cg3::Point2d p2 = segment.p2();
        DagNode* dg2 = new DagNodePoint(p);
        dg->setRightChild(dg2);
        DagNode* dgs = new DagNodeSegment(&segment);
        dg2->setLeftChild(dgs);
        if(split->getLeftChild() == s1)
            split->setLeftChild(dg);
        if(split->getRightChild() == s1)
            split->setRightChild(dg);

    }
    else{

    }
};
DagNode* Dag::searchPoint(const cg3::Point2d& point){
    DagNode* tmpdag=Dag::dag;
    if (tmpdag == nullptr)
        return nullptr;
    while(tmpdag->getLeftChild()!=nullptr && tmpdag->getRightChild()!=nullptr){
        tmpdag = tmpdag->compareTo(point);
    }
    return tmpdag;

}

DagNode *Dag::splitNode(cg3::Segment2d &segment)
{
    DagNode* tmpdag=Dag::dag;
    DagNode* c1=nullptr;
    DagNode* c2=nullptr;
    bool splitted = false;
    if (tmpdag == nullptr)
        return nullptr;
    while(!splitted){
        c1 = tmpdag->compareTo(segment.p1());
        c2 = tmpdag->compareTo(segment.p2());
        if(c1==c2 && c1->getLeftChild()!=nullptr && c1->getRightChild()!=nullptr)
            tmpdag = c1;
        else
            splitted = true;
    }
    return tmpdag;
}














