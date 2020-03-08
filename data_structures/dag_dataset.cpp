#include <data_structures/dag_dataset.h>
#include <data_structures/dag_node.h>

DagNode* Dag::dag = nullptr;

Dag::Dag(){
    dag=nullptr;
}
void Dag::addSegment(cg3::Segment2d& segment){ //O (k log k)
    DagNode* split;
    DagNode* s1;
    DagNode* s2;
    if(Dag::dag!=nullptr){
        split = splitNode(segment);
        s1 =split->compareNodeToPoint(segment.p1());
        s2 =split->compareNodeToPoint(segment.p2());
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
            addMultiTrapezoidalSegment(split, segment);
        }
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

}

void Dag::addMultiTrapezoidalSegment(DagNode *split, cg3::Segment2d &segment)
{

    DagNode * choose = split->compareNodeToSegment(segment);
    //if(choose->getLeftChild()!=nullptr || split==Dag::dag){
    if((split->getLeftChild())->getLeftChild() != nullptr)
        addMultiTrapezoidalSegment(split->getLeftChild(), segment);
    if((split->getRightChild())->getLeftChild() != nullptr)
        addMultiTrapezoidalSegment(split->getRightChild(), segment);
    //}
    if(choose->getLeftChild()==nullptr){
        if(choose == searchPoint(segment.p1())){
            cg3::Point2d p = segment.p1();
            DagNode* dg = new DagNodePoint(p);
            DagNode * seg = new DagNodeSegment(&segment);
            dg->setLeftChild(new DagNodeArea());
            seg->setLeftChild(new DagNodeArea());
            seg->setRightChild(new DagNodeArea());
            dg->setRightChild(seg);
            if(split->getLeftChild()==choose)
                split->setLeftChild(dg);
            if(split->getRightChild()==choose)
                split->setRightChild(dg);
        }
        else if(choose == searchPoint(segment.p2())){
            cg3::Point2d p = segment.p2();
            DagNode* dg = new DagNodePoint(p);
            DagNode * seg = new DagNodeSegment(&segment);
            dg->setRightChild(new DagNodeArea());
            seg->setLeftChild(new DagNodeArea());
            seg->setRightChild(new DagNodeArea());
            dg->setLeftChild(seg);
            if(split->getLeftChild()==choose)
                split->setLeftChild(dg);
            if(split->getRightChild()==choose)
                split->setRightChild(dg);
        }
        else{
            DagNode* dg = new DagNodeSegment(&segment);
            if(split->getLeftChild()==choose)
                split->setLeftChild(dg);
            if(split->getRightChild()==choose)
                split->setRightChild(dg);
        }

    }

};
DagNode* Dag::searchPoint(const cg3::Point2d& point){ //O(log n)
    DagNode* tmpdag=Dag::dag;
    if (tmpdag == nullptr)
        return nullptr;
    while(tmpdag->getLeftChild()!=nullptr && tmpdag->getRightChild()!=nullptr){
        tmpdag = tmpdag->compareNodeToPoint(point);
    }
    return tmpdag;

}

DagNode *Dag::splitNode(cg3::Segment2d &segment) //O(log n)
{
    DagNode* tmpdag=Dag::dag;
    DagNode* c1=nullptr;
    DagNode* c2=nullptr;
    bool splitted = false;
    if (tmpdag == nullptr)
        return nullptr;
    while(!splitted){
        c1 = tmpdag->compareNodeToPoint(segment.p1());
        c2 = tmpdag->compareNodeToPoint(segment.p2());
        if(c1==c2 && c1->getLeftChild()!=nullptr && c1->getRightChild()!=nullptr)
            tmpdag = c1;
        else
            splitted = true;
    }
    return tmpdag;
}














