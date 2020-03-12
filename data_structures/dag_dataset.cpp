#include <data_structures/dag_dataset.h>
#include <data_structures/dag_node.h>

DagNode* Dag::dag = nullptr;
std::map<unsigned int, std::pair<DagNode **, DagNode*>> Dag::tempmap = {};
Dag::Dag(){
    dag=nullptr;
    tempmap = {};
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
            DagNode* dg2 = new DagNodePoint(p2);
            dg->setRightChild(dg2);
            DagNode* dgs = new DagNodeSegment(segment);
            dg2->setLeftChild(dgs);
            if(split->getLeftChild() == s1)
                split->setLeftChild(dg);
            if(split->getRightChild() == s1)
                split->setRightChild(dg);

        }
        else{
            //addMultiTrapezoidalSegment(split, segment);
            Dag::tempmap= {};
            innerNodes(split,segment,split);
            addMultiTrapezoidalSegment(segment);
        }
    }
    else{
        cg3::Point2d p = segment.p1();
        DagNode* dg = new DagNodePoint(p);
        cg3::Point2d p2 = segment.p2();
        DagNode* dg2 = new DagNodePoint(p2);
        dg->setRightChild(dg2);
        DagNode* dgs = new DagNodeSegment(segment);
        dg2->setLeftChild(dgs);
        Dag::dag = dg;

    }

}

void Dag::addMultiTrapezoidalSegment(cg3::Segment2d &segment)
{
    std::map<unsigned int, std::pair<DagNode **, DagNode*>>::iterator itr;
    DagNode * lastMeaningful = nullptr;
    bool lastDirectionUp;
    bool currentDirectionUp;
    for (itr = Dag::tempmap.begin(); itr!=Dag::tempmap.end(); ++itr){
        if(itr==Dag::tempmap.begin()){
            cg3::Point2d p = segment.p1();
            DagNode* dg = new DagNodePoint(p);
            dg->setLeftChild(new DagNodeArea());
            dg->setRightChild(new DagNodeSegment(segment));
            *(itr->second.first) = dg;
            lastMeaningful = itr->second.second;
            lastDirectionUp = (lastMeaningful->compareNodeToSegment(segment)==lastMeaningful->getLeftChild())? true:false;
        }
        else if(itr!=Dag::tempmap.begin() && itr!=Dag::tempmap.end() && (next(itr) != Dag::tempmap.end())){
            DagNode* dg = new DagNodeSegment(segment);
            //
            currentDirectionUp = (itr->second.second->compareNodeToSegment(segment)==itr->second.second->getLeftChild())? true:false;
            if(currentDirectionUp!=lastDirectionUp){
                --itr;
                if(itr!=Dag::tempmap.begin())
                    (lastDirectionUp!=true)?dg->setRightChild((*(itr->second.first))->getRightChild()):dg->setLeftChild((*(itr->second.first))->getLeftChild());
                else
                    (lastDirectionUp!=true)?dg->setRightChild((*(itr->second.first))->getRightChild()->getRightChild()):dg->setLeftChild((*(itr->second.first))->getRightChild()->getLeftChild());
                ++itr;
            }
            else{
                --itr;
                if(itr!=Dag::tempmap.begin())
                    (lastDirectionUp!=true)?dg->setRightChild((*(itr->second.first))->getRightChild()):dg->setLeftChild((*(itr->second.first))->getLeftChild());
                else
                    (lastDirectionUp!=true)?dg->setRightChild((*(itr->second.first))->getRightChild()->getRightChild()):dg->setLeftChild((*(itr->second.first))->getRightChild()->getLeftChild());
                ++itr;
            }
            lastMeaningful = itr->second.second;
            lastDirectionUp = currentDirectionUp;
            *(itr->second.first) = dg;

        }
        else if(next(itr) == Dag::tempmap.end()){
            cg3::Point2d p = segment.p2();
            DagNode* dg = new DagNodePoint(p);
            dg->setRightChild(new DagNodeArea());
            DagNode* dgs = new DagNodeSegment(segment);
            currentDirectionUp = (itr->second.second->compareNodeToSegment(segment)==itr->second.second->getLeftChild())? true:false;
            if(currentDirectionUp!=lastDirectionUp){
                --itr;
                if(itr!=Dag::tempmap.begin())
                    (lastDirectionUp!=true)?dgs->setRightChild((*(itr->second.first))->getRightChild()):dgs->setLeftChild((*(itr->second.first))->getLeftChild());
                else
                    (lastDirectionUp!=true)?dgs->setRightChild((*(itr->second.first))->getRightChild()->getRightChild()):dgs->setLeftChild((*(itr->second.first))->getRightChild()->getLeftChild());
                ++itr;
            }
            if(currentDirectionUp==lastDirectionUp){
                --itr;
                if(itr!=Dag::tempmap.begin())
                    (lastDirectionUp!=true)?dgs->setRightChild((*(itr->second.first))->getRightChild()):dgs->setLeftChild((*(itr->second.first))->getLeftChild());
                else
                    (lastDirectionUp!=true)?dgs->setRightChild((*(itr->second.first))->getRightChild()->getRightChild()):dgs->setLeftChild((*(itr->second.first))->getRightChild()->getLeftChild());
                ++itr;
            }
            dg->setLeftChild(dgs);
            *(itr->second.first) = dg;
        }
    }


};

void Dag::innerNodes(DagNode *split, cg3::Segment2d &segment, DagNode* meaningful){
    enum direction { left = 1, right = 2, both = 3 };
    int chosen = split->oneOrBoth(segment);
    if(strcmp(typeid(*meaningful).name(),"12DagNodePoint")==0 && strcmp(typeid(*split).name(),"12DagNodePoint")==0){
        meaningful = split;
    }
    if(strcmp(typeid(*split).name(),"14DagNodeSegment")==0){
        meaningful = split;
    }
    if(chosen==both){
        if(split->getLeftChild()->getLeftChild()==nullptr){
            tempmap.insert(std::make_pair(tempmap.size(),std::make_pair(split->lcPointerAddress(),meaningful)));
            innerNodes(split->getRightChild(),segment,meaningful);
        }
        else if(split->getRightChild()->getRightChild()==nullptr){
            innerNodes(split->getLeftChild(),segment,meaningful);
            tempmap.insert(std::make_pair(tempmap.size(),std::make_pair(split->rcPointerAddress(),meaningful)));

        }
        else{
            innerNodes(split->getLeftChild(),segment,meaningful);
            innerNodes(split->getRightChild(),segment,meaningful);
        }
    }
    if(chosen==right){
        if(split->getRightChild()->getRightChild()==nullptr){
            tempmap.insert(std::make_pair(tempmap.size(),std::make_pair(split->rcPointerAddress(),meaningful)));
        }
        else
            innerNodes(split->getRightChild(),segment,meaningful);
    }
    if(chosen==left){
        if(split->getLeftChild()->getLeftChild()==nullptr)
            tempmap.insert(std::make_pair(tempmap.size(),std::make_pair(split->lcPointerAddress(),meaningful)));
        else
            innerNodes(split->getLeftChild(),segment,meaningful);
    }
}
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














