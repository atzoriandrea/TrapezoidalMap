#include "dag_dataset.h"


std::vector<std::pair<DagNode **, DagNode*>> Dag::tempmap = {};

Dag::Dag(){  
    tempmap = {};
}
void Dag::addSegment(cg3::Segment2d& segment){ //O (k log k)
    if(Dag::dag!=nullptr && Dag::dag->getLeftChild()!=nullptr){
        DagNode *& ins = searchAndAppend(segment.p1());
        if(segment.p2().x()<dynamic_cast<DagNodeArea*>(ins)->getT().getRightp().x())
            ins = addSegmentInSingleTrap(dynamic_cast<DagNodeArea*>(ins), segment);
        else
            followSegment(segment, dynamic_cast<DagNodeArea&>(*ins));
    }
    else{
        Dag::dag = addSegmentInSingleTrap(dynamic_cast<DagNodeArea*>(Dag::dag), segment);
    }

}


DagNodePoint* Dag::addSegmentInSingleTrap(DagNodeArea * node, cg3::Segment2d& segment){
    std::list<Trapezoid>& ref = TrapezoidalMap::getTrapezoids();
    std::list<Trapezoid>::iterator itr;
    std::vector<Trapezoid*> iterators(4);
    DagNodePoint* dg = new DagNodePoint(segment.p1());
    DagNode* dg2 = new DagNodePoint(segment.p2());
    DagNode* dgs = new DagNodeSegment(segment);
    Trapezoid& trap = {node->getT()};
    std::vector<cg3::Point2d> ints = {intersection(trap.getTop(), segment.p1()),
                                      intersection(trap.getTop(), segment.p2()),
                                      intersection(trap.getBottom(), segment.p1()),
                                      intersection(trap.getBottom(), segment.p2())};
    std::vector<Trapezoid> traps = {Trapezoid(cg3::Segment2d(trap.getTop().p1(),ints[0]),
                                        cg3::Segment2d(trap.getBottom().p1(),ints[2]),trap.getLeftp(), segment.p1(),nullptr),
                                    Trapezoid(cg3::Segment2d(ints[0],ints[1]), cg3::Segment2d(segment.p1(),segment.p2()), segment.p1(), segment.p2(),nullptr),
                                    Trapezoid(cg3::Segment2d(segment.p1(),segment.p2()),cg3::Segment2d(ints[2],ints[3]), segment.p1(), segment.p2(),nullptr),
                                    Trapezoid(cg3::Segment2d(ints[1], trap.getTop().p2()),
                                        cg3::Segment2d(ints[3], trap.getBottom().p2()),segment.p2(), trap.getRightp(),nullptr)};
    TrapezoidalMap::addTrapezoids(traps);
    itr = ref.end();
    iterators[3] = &*(--itr);
    iterators[2] = &*(--itr);
    iterators[1] = &*(--itr);
    iterators[0] = &*(--itr);
    TrapezoidalMap::updateNeighbors(trap, iterators);
    iterators[0]->setNeighbors(trap.getLeftUp(),trap.getLeftDown(),*iterators[1], *iterators[2]);
    iterators[1]->setNeighbors(*iterators[0],*iterators[0], *iterators[3], *iterators[3]);
    iterators[2]->setNeighbors(*iterators[0],*iterators[0], *iterators[3], *iterators[3]);
    iterators[3]->setNeighbors(*iterators[1], *iterators[2], trap.getRightUp(), trap.getRightDown());
    TrapezoidalMap::removeTrapezoid(trap);
    dg->setLeftChild(bind(*iterators[0]));
    dg->setRightChild(dg2);
    dgs->setLeftChild(bind(*iterators[1]));
    dgs->setRightChild(bind(*iterators[2]));
    dg2->setLeftChild(dgs);
    dg2->setRightChild(bind(*iterators[3]));

    return dg;
}


//void Dag::addMultiTrapezoidalSegment(cg3::Segment2d &segment)
//{
//    std::vector<std::pair<DagNode **, DagNode*>>::iterator itr;
//    DagNode * lastMeaningful = nullptr;
//    Trapezoid tempTrap;
//    Trapezoid t;
//    bool lastDirectionUp;
//    bool nextDir;
//    bool currentDirectionUp;
//    std::vector<cg3::Point2d> ints ;
//    std::vector<Trapezoid> traps;
//    for (itr = Dag::tempmap.begin(); itr!=Dag::tempmap.end(); ++itr){
//        if(itr==Dag::tempmap.begin()){
//            /*Traps*/
//            Trapezoid trap = (static_cast<DagNodeArea *>(*itr->first))->getT();
//            traps.push_back(Trapezoid(
//                                cg3::Segment2d(trap.getTop().p1(),intersection(trap.getTop(), segment.p1())),
//                                cg3::Segment2d(trap.getBottom().p1(),intersection(trap.getBottom(),segment.p1())),
//                                trap.getTop().p1() , segment.p1()));
//            Trapezoid t1 = (Trapezoid(
//                                cg3::Segment2d(intersection(trap.getTop(), segment.p1()),trap.getTop().p2()),
//                                cg3::Segment2d(segment.p1(), intersection(segment,trap.getBottom().p2())),
//                                segment.p1(), trap.getTop().p2()));
//            Trapezoid t2 = (Trapezoid(
//                                cg3::Segment2d(segment.p1(), intersection(segment,trap.getBottom().p2())),
//                                cg3::Segment2d(intersection(trap.getBottom(), segment.p1()), trap.getBottom().p2()),
//                                segment.p1(), trap.getBottom().p2()));
//            /*endTraps*/

//            DagNode* dg = new DagNodePoint(segment.p1());
//            dg->setLeftChild(new DagNodeArea(traps[0])); //Aggiungo il trapezoide tutto a sinistra
//            DagNode* dgs = new DagNodeSegment(segment);
//            dgs->setLeftChild(new DagNodeArea(t1));
//            dgs->setRightChild(new DagNodeArea(t2));
//            dg->setRightChild(dgs);
//            *(itr->first) = dg;
//            lastMeaningful = itr->second;
//            lastDirectionUp = (lastMeaningful->compareNodeToSegment(segment)==lastMeaningful->getLeftChild())? true:false;
//            nextDir = (next(itr)->second->compareNodeToSegment(segment)==next(itr)->second->getLeftChild())? true:false;
//            Dag::removeTrapezoid(trap);
//            if((lastDirectionUp != nextDir && !nextDir )|| (lastDirectionUp == nextDir && nextDir)){
//                traps.push_back(t2);
//            }
//            if((lastDirectionUp != nextDir && nextDir )|| (lastDirectionUp == nextDir && !nextDir)){
//                traps.push_back(t1);
//            }

//        }
//        else if(itr!=Dag::tempmap.begin() && itr!=Dag::tempmap.end() && (next(itr) != Dag::tempmap.end())){
//            DagNode* dg = new DagNodeSegment(segment);
//            Trapezoid trap = (static_cast<DagNodeArea *>(*itr->first))->getT();
//            Dag::removeTrapezoid(trap);
//            std::vector<cg3::Point2d> rightInts = {trap.getTop().p2(), intersection(segment, trap.getTop().p2()), trap.getBottom().p2()};
//            currentDirectionUp = (itr->second->compareNodeToSegment(segment)==itr->second->getLeftChild())? true:false;
//            --itr;
//            if(itr!=Dag::tempmap.begin()){
//                if(lastDirectionUp!=true){
//                    (tempTrap.getTop() != static_cast<DagNodeArea *>((*((itr))->first)->getRightChild())->getT().getTop())?traps.push_back(t):traps.push_back(tempTrap);
//                    tempTrap = static_cast<DagNodeArea *>((*itr->first))->getT();
//                    tempTrap.setTop(cg3::Segment2d(tempTrap.getTop().p1(), rightInts[1]));
//                    tempTrap.setBottom(cg3::Segment2d(tempTrap.getBottom().p1(), rightInts[2]));
//                    tempTrap.setRightp(rightInts[2]);
//                    tempTrap.addPointingNodes((*itr->first)->rcPointerAddress());
//                    static_cast<DagNodeArea *>((*itr->first)->getRightChild())->setT(tempTrap);
//                    dg->setRightChild((*(itr->first))->getRightChild());
//                    t = Trapezoid(
//                                trap.getTop(),
//                                cg3::Segment2d(intersection(segment, trap.getTop().p1()),intersection(segment, trap.getTop().p2())),
//                                intersection(segment, trap.getTop().p1()),intersection(segment, trap.getTop().p2()));
//                    traps.push_back(t);

//                    dg->setLeftChild(new DagNodeArea(t));
//                }
//                else{
//                    (tempTrap.getTop() != static_cast<DagNodeArea *>((*((itr))->first)->getLeftChild())->getT().getTop())?traps.push_back(tempTrap):traps.push_back(t);
//                    tempTrap = static_cast<DagNodeArea *>((*itr->first)->getLeftChild())->getT();
//                    tempTrap.setTop(cg3::Segment2d(tempTrap.getTop().p1(), rightInts[0]));
//                    tempTrap.setBottom(cg3::Segment2d(tempTrap.getBottom().p1(), rightInts[1]));
//                    tempTrap.setRightp(rightInts[1]);
//                    tempTrap.addPointingNodes((*itr->first)->lcPointerAddress());
//                    static_cast<DagNodeArea *>((*itr->first)->getLeftChild())->setT(tempTrap);
//                    dg->setLeftChild((*(itr->first))->getLeftChild());
//                    t = Trapezoid( cg3::Segment2d(intersection(segment, trap.getTop().p1()),intersection(segment, trap.getTop().p2())),trap.getBottom(),
//                    intersection(segment, trap.getBottom().p1()),intersection(segment, trap.getBottom().p2()));
//                    traps.push_back(t);

//                    dg->setRightChild(new DagNodeArea(t));
//                }
//            }
//            else{
//                if(lastDirectionUp!=true){
//                    //Corretta!
//                    tempTrap = static_cast<DagNodeArea *>((*(itr->first))->getRightChild()->getRightChild())->getT();
//                    tempTrap.setTop(cg3::Segment2d(tempTrap.getTop().p1(), rightInts[1]));
//                    tempTrap.setBottom(cg3::Segment2d(tempTrap.getBottom().p1(), rightInts[2]));
//                    tempTrap.setRightp(rightInts[2]);
//                    tempTrap.addPointingNodes((*itr->first)->getRightChild()->rcPointerAddress());
//                    static_cast<DagNodeArea *>((*itr->first)->getRightChild()->getRightChild())->setT(tempTrap);
//                    dg->setRightChild((*(itr->first))->getRightChild()->getRightChild());
//                    t = Trapezoid(
//                                trap.getTop(),
//                                cg3::Segment2d(intersection(segment, trap.getTop().p1()),intersection(segment, trap.getTop().p2())),
//                                intersection(segment, trap.getTop().p1()),intersection(segment, trap.getTop().p2()));
//                    dg->setLeftChild(new DagNodeArea(t));
//                }
//                else{
//                     tempTrap = static_cast<DagNodeArea *>((*(itr->first))->getRightChild()->getLeftChild())->getT();
//                     tempTrap.setTop(cg3::Segment2d(tempTrap.getTop().p1(), rightInts[0]));
//                     tempTrap.setBottom(cg3::Segment2d(tempTrap.getBottom().p1(), rightInts[1]));
//                     tempTrap.setRightp(rightInts[1]);
//                     tempTrap.addPointingNodes((*itr->first)->getLeftChild()->lcPointerAddress());
//                     static_cast<DagNodeArea *>((*itr->first)->getRightChild()->getLeftChild())->setT(tempTrap);
//                     dg->setLeftChild((*(itr->first))->getRightChild()->getLeftChild());
//                     t = Trapezoid(
//                                 cg3::Segment2d(intersection(segment, trap.getTop().p1()),intersection(segment, trap.getTop().p2())),
//                                 trap.getBottom(),
//                                 intersection(segment, trap.getBottom().p1()),intersection(segment, trap.getBottom().p2()));
//                     dg->setRightChild(new DagNodeArea(t));

//                }
//            }
//            ++itr;
//            lastMeaningful = itr->second;
//            lastDirectionUp = currentDirectionUp;
//            *(itr->first) = dg;

//        }
//        else if(next(itr) == Dag::tempmap.end()){
//            cg3::Point2d p = segment.p2();
//            DagNode* dg = new DagNodePoint(p);
//            dg->setRightChild(new DagNodeArea());
//            DagNode* dgs = new DagNodeSegment(segment);
//            currentDirectionUp = (itr->second->compareNodeToSegment(segment)==itr->second->getLeftChild())? true:false;
//            Trapezoid trap = (static_cast<DagNodeArea *>(*itr->first))->getT();
//            Dag::removeTrapezoid(trap);
//            std::vector<cg3::Point2d> rightInts = {intersection(trap.getTop(), segment.p2()), segment.p2(), intersection(trap.getBottom(), segment.p2())};
//            --itr;
//            if(itr!=Dag::tempmap.begin()){
//                if(lastDirectionUp!=true){ //Corretta!!
//                    (tempTrap.getTop() != static_cast<DagNodeArea *>((*(itr)->first)->getRightChild())->getT().getTop())?
//                                traps.push_back(tempTrap):traps.push_back(t);
//                    tempTrap = static_cast<DagNodeArea *>((*itr->first)->getRightChild())->getT();

//                    tempTrap.setTop(cg3::Segment2d(tempTrap.getTop().p1(), rightInts[1]));
//                    tempTrap.setBottom(cg3::Segment2d(tempTrap.getBottom().p1(), rightInts[2]));
//                    tempTrap.setRightp(rightInts[2]);
//                    tempTrap.addPointingNodes((*itr->first)->rcPointerAddress());
//                    static_cast<DagNodeArea *>((*itr->first)->getRightChild())->setT(tempTrap);
//                    dgs->setRightChild((*(itr->first))->getRightChild());
//                    t = Trapezoid(cg3::Segment2d(trap.getTop().p1(), intersection(trap.getTop(), segment.p2())),
//                                            cg3::Segment2d(intersection(segment, trap.getTop().p1()), segment.p2()),intersection(segment, trap.getTop().p1()), segment.p2() );
//                    traps.push_back(t);
//                    dgs->setLeftChild(new DagNodeArea(t));
//                }
//                else{
//                    (tempTrap.getTop() != static_cast<DagNodeArea *>((*(itr)->first)->getLeftChild())->getT().getTop())?
//                                traps.push_back(tempTrap):traps.push_back(t);
//                    tempTrap = static_cast<DagNodeArea *>((*itr->first)->getLeftChild())->getT();
//                    tempTrap.setTop(cg3::Segment2d(tempTrap.getTop().p1(), rightInts[0]));
//                    tempTrap.setBottom(cg3::Segment2d(tempTrap.getBottom().p1(), rightInts[1]));
//                    tempTrap.setRightp(rightInts[1]);
//                    tempTrap.addPointingNodes((*itr->first)->lcPointerAddress());
//                    static_cast<DagNodeArea *>((*itr->first)->getLeftChild())->setT(tempTrap);
//                    dgs->setLeftChild((*(itr->first))->getLeftChild());
//                    t = Trapezoid(cg3::Segment2d(intersection(segment, trap.getTop().p1()), segment.p2()),
//                                            cg3::Segment2d(trap.getBottom().p1(), intersection(trap.getBottom(), segment.p2())),intersection(segment, trap.getBottom().p1()), segment.p2());
//                    traps.push_back(t);
//                    dgs->setRightChild(new DagNodeArea(t));
//                }
//            }
//            else{
//                if(lastDirectionUp!=true){

//                    tempTrap = static_cast<DagNodeArea *>((*(itr->first))->getRightChild()->getRightChild())->getT();
//                    tempTrap.setTop(cg3::Segment2d(tempTrap.getTop().p1(), rightInts[1]));
//                    tempTrap.setBottom(cg3::Segment2d(tempTrap.getBottom().p1(), rightInts[2]));
//                    tempTrap.setRightp(rightInts[2]);
//                    tempTrap.addPointingNodes((*itr->first)->getRightChild()->rcPointerAddress());
//                    static_cast<DagNodeArea *>((*itr->first)->getRightChild()->getRightChild())->setT(tempTrap);
//                    dgs->setRightChild((*(itr->first))->getRightChild()->getRightChild());
//                    t = Trapezoid(cg3::Segment2d(trap.getTop().p1(), intersection(trap.getTop(), segment.p2())),
//                                            cg3::Segment2d(intersection(segment, trap.getTop().p1()), segment.p2()),intersection(segment, trap.getTop().p1()), segment.p2() );
//                    traps.push_back(t);
//                    dgs->setLeftChild(new DagNodeArea(t));
//                }
//                else{

//                    tempTrap = static_cast<DagNodeArea *>((*(itr->first))->getRightChild()->getLeftChild())->getT();
//                    tempTrap.setTop(cg3::Segment2d(tempTrap.getTop().p1(), rightInts[0]));
//                    tempTrap.setBottom(cg3::Segment2d(tempTrap.getBottom().p1(), rightInts[1]));
//                    tempTrap.setRightp(rightInts[1]);
//                    tempTrap.addPointingNodes((*itr->first)->getRightChild()->lcPointerAddress());
//                    static_cast<DagNodeArea *>((*itr->first)->getRightChild()->getLeftChild())->setT(tempTrap);
//                    dgs->setLeftChild((*(itr->first))->getRightChild()->getLeftChild());
//                    t = Trapezoid(cg3::Segment2d(intersection(segment, trap.getTop().p1()), segment.p2()),
//                                            cg3::Segment2d(trap.getBottom().p1(), intersection(trap.getBottom(), segment.p2())),intersection(segment, trap.getBottom().p1()), segment.p2());
//                    traps.push_back(t);
//                    dgs->setRightChild(new DagNodeArea(t));
//                }
//            }
//            ++itr;

//            Trapezoid rightMost = Trapezoid(
//                        cg3::Segment2d(intersection(trap.getTop(), segment.p2()), trap.getTop().p2()),
//                        cg3::Segment2d(intersection(trap.getBottom(), segment.p2()),trap.getBottom().p2()),
//                        segment.p2(), trap.getTop().p2());
//            traps.push_back(tempTrap);
//            traps.push_back(rightMost);
//            rightMost.addPointingNodes(dg->rcPointerAddress());
//            dg->setRightChild(new DagNodeArea(rightMost));
//            dg->setLeftChild(dgs);
//            *(itr->first) = dg;
//        }
//    }
//    Dag::addTrapezoids(traps);

//};

//void Dag::innerNodes(DagNode *split, cg3::Segment2d &segment, DagNode* meaningful){
//    enum direction { left = 1, right = 2, both = 3 };
//    int chosen = split->oneOrBoth(segment);
//    if(strcmp(typeid(*meaningful).name(),"12DagNodePoint")==0 && strcmp(typeid(*split).name(),"12DagNodePoint")==0){
//        meaningful = split;
//    }
//    if(strcmp(typeid(*split).name(),"14DagNodeSegment")==0){
//        meaningful = split;
//    }
//     if(chosen==both){
//        if(split->getLeftChild()->getLeftChild()==nullptr){
//            tempmap.push_back(std::make_pair(split->lcPointerAddress(),meaningful));
//            innerNodes(split->getRightChild(),segment,meaningful);
//        }
//        else if(split->getRightChild()->getRightChild()==nullptr){
//            innerNodes(split->getLeftChild(),segment,meaningful);
//            tempmap.push_back(std::make_pair(split->rcPointerAddress(),meaningful));

//        }
//        else{
//            innerNodes(split->getLeftChild(),segment,meaningful);
//            innerNodes(split->getRightChild(),segment,meaningful);
//        }
//    }
//    if(chosen==right){
//        if(split->getRightChild()->getRightChild()==nullptr){
//            tempmap.push_back(std::make_pair(split->rcPointerAddress(),meaningful));
//        }
//        else
//            innerNodes(split->getRightChild(),segment,meaningful);
//    }
//    if(chosen==left){
//        if(split->getLeftChild()->getLeftChild()==nullptr){
//            tempmap.push_back(std::make_pair(split->lcPointerAddress(),meaningful));
//        }
//        else
//            innerNodes(split->getLeftChild(),segment,meaningful);
//    }
//}
DagNode * Dag::searchPoint(const cg3::Point2d& point){ //O(log n)
    DagNode* tmpdag=Dag::dag;
    while(tmpdag->getLeftChild()!=nullptr && tmpdag->getRightChild()!=nullptr){
        tmpdag = tmpdag->compareNodeToPoint(point);
    }
    return tmpdag;

}

DagNode *&Dag::searchAndAppend(const cg3::Point2d &point)
{
    DagNode* tmpdag=Dag::dag;
    DagNode* chosen=Dag::dag;
    while(tmpdag->getLeftChild()!=nullptr && tmpdag->getRightChild()!=nullptr){
        chosen = tmpdag;
        tmpdag = tmpdag->compareNodeToPoint(point);
    }
    if(chosen->lcValue == tmpdag)
        return chosen->getLeftChild();
    if(chosen->rcValue == tmpdag)
        return chosen->getRightChild();
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

void Dag::followSegment(cg3::Segment2d &segment, DagNodeArea &leaf){
    std::vector<const DagNodeArea*> leaves;
    leaves.push_back(&leaf);
    int size = leaves.size();
    while (segment.p2().x() > leaves[size-1]->getT().getRightp().x()) {
        if(matrixDet(segment, leaves[size-1]->getT().getRightp())<0){
            //leaves.push_back(leaves[size-1]->getT().getRightUp().getNode());
        }
        else{
            //leaves.push_back(leaves[size-1]->getT().getRightDown().getNode());
        }

        size++;
    }
}

DagNode* Dag::createLeftMost(cg3::Segment2d &segment, DagNodeArea &leaf){
    std::list<Trapezoid>& ref = TrapezoidalMap::getTrapezoids();
    std::list<Trapezoid>::iterator itr;
    std::vector<Trapezoid*> iterators(4);
    DagNodePoint* pi = new DagNodePoint(segment.p1());
    DagNode* si = new DagNodeSegment(segment);
    Trapezoid& trap = leaf.getT();
    std::vector<cg3::Point2d> ints = {intersection(trap.getTop(), segment.p1()),
                                      intersection(trap.getBottom(), segment.p1()),
                                      intersection(segment, trap.getTop().p2())};
    std::vector<Trapezoid> traps = {Trapezoid(cg3::Segment2d(trap.getTop().p1(),ints[0]),
                                    cg3::Segment2d(trap.getBottom().p1(),ints[1]),
                                    trap.getLeftp() , segment.p1(), nullptr),
                                    Trapezoid(cg3::Segment2d(ints[0],trap.getTop().p2()),
                                    cg3::Segment2d(segment.p1(), intersection(segment,trap.getBottom().p2())),
                                    segment.p1(), ints[2], nullptr),
                                    Trapezoid(cg3::Segment2d(segment.p1(),ints[2]),
                                    cg3::Segment2d(ints[1], trap.getBottom().p2()),
                                    segment.p1(), ints[2],nullptr)};
    TrapezoidalMap::addTrapezoids(traps);
    itr = ref.end();
    iterators[2] = &*(--itr);
    iterators[1] = &*(--itr);
    iterators[0] = &*(--itr);
}

DagNode *Dag::createRightMost(cg3::Segment2d &segment, DagNodeArea &leaf)
{

}

DagNode *Dag::createIntermediate(cg3::Segment2d &segment, DagNodeArea &leaf)
{

}

DagNode *Dag::bind(Trapezoid & trap)
{
    DagNodeArea* first = new DagNodeArea(trap);
    trap.setNode(first);
    return first;
}














