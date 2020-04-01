#include "trapezoidalmap_dataset.h"
#include "trapezoid.h"
#include "algorithms/algorithms.h"
#include <data_structures/dag_dataset.h>
#include <data_structures/dag_node.h>

#define START 0

std::vector<std::pair<DagNode **, DagNode*>> Dag::tempmap = {};
cg3::Segment2d topbb = cg3::Segment2d(cg3::Point2d(-1000000.0, 1000000.0),cg3::Point2d(1000000.0, 1000000.0));
cg3::Segment2d botbb = cg3::Segment2d(cg3::Point2d(-1000000.0, -1000000.0),cg3::Point2d(1000000.0, -1000000.0));
cg3::Point2d leftp = cg3::Point2d(-1000000.0, 1000000.0);
cg3::Point2d rightp = cg3::Point2d(1000000.0, 1000000.0);
Trapezoid t = Trapezoid(topbb, botbb, leftp, rightp);
DagNode* Dag::dag= new DagNodeArea(t);
std::set<Trapezoid> Dag::trapezoids = {};
std::map<Trapezoid*, std::vector<DagNode **>> Dag::multiPointed = {};
Dag::Dag(){  
    tempmap = {};
}
void Dag::addSegment(cg3::Segment2d& segment){ //O (k log k)
    DagNode* split;
    DagNode* s1;
    DagNode* s2;
    DagNode* dg;
    if(Dag::dag!=nullptr && Dag::dag->getLeftChild()!=nullptr){
        split = splitNode(segment);
        s1 =split->compareNodeToPoint(segment.p1());
        s2 =split->compareNodeToPoint(segment.p2());
        if(s1 == s2 && s1->getLeftChild()==nullptr){
            dg = addSegmentInSingleTrap(s1, segment);
            if(split->getLeftChild() == s1)
                split->setLeftChild(dg);
            if(split->getRightChild() == s1)
                split->setRightChild(dg);

        }
        else{
            Dag::tempmap= {};
            innerNodes(split,segment,split);
            addMultiTrapezoidalSegment(segment);
        }
    }
    else{
        Dag::dag = addSegmentInSingleTrap(Dag::dag, segment);

    }

}
DagNode* Dag::addSegmentInSingleTrap(DagNode * node, cg3::Segment2d& segment){
    Trapezoid trap = {static_cast<DagNodeArea *>(node)->getT()};
    std::vector<cg3::Point2d> ints = {intersection(trap.getTop(), segment.p1()),
                                      intersection(trap.getTop(), segment.p2()),
                                      intersection(trap.getBottom(), segment.p1()),
                                      intersection(trap.getBottom(), segment.p2())};
    std::vector<Trapezoid> traps = {Trapezoid(cg3::Segment2d(trap.getTop().p1(),ints[0]),
                                        cg3::Segment2d(trap.getBottom().p1(),ints[2]),trap.getTop().p1() , segment.p1()),
                                    Trapezoid(cg3::Segment2d(ints[0],ints[1]), cg3::Segment2d(segment.p1(),segment.p2()), segment.p1(), segment.p2()),
                                    Trapezoid(cg3::Segment2d(segment.p1(),segment.p2()),cg3::Segment2d(ints[2],ints[3]), segment.p1(), segment.p2()),
                                    Trapezoid(cg3::Segment2d(ints[1], trap.getTop().p2()),
                                        cg3::Segment2d(ints[3], trap.getBottom().p2()),segment.p2(), trap.getTop().p2())};

    DagNode* dg = new DagNodePoint(segment.p1());
    dg->setLeftChild(new DagNodeArea(traps[0]));
    DagNode* dg2 = new DagNodePoint(segment.p2());
    dg->setRightChild(dg2);
    DagNode* dgs = new DagNodeSegment(segment);
    dgs->setLeftChild(new DagNodeArea(traps[1]));
    dgs->setRightChild(new DagNodeArea(traps[2]));
    dg2->setLeftChild(dgs);
    dg2->setRightChild(new DagNodeArea(traps[3]));
    Dag::removeTrapezoid(static_cast<DagNodeArea *>(node)->getT());
    Dag::addTrapezoids(traps);
    return dg;
}


void Dag::addMultiTrapezoidalSegment(cg3::Segment2d &segment)
{
    std::vector<std::pair<DagNode **, DagNode*>>::iterator itr;
    DagNode * lastMeaningful = nullptr;
    DagNode ** lastTouched = nullptr;
    Trapezoid tempTrap;
    Trapezoid t;
    bool lastDirectionUp;
    bool nextDir;
    bool currentDirectionUp;
    std::vector<DagNode**> pointers;
    std::vector<cg3::Point2d> ints ;
    std::vector<Trapezoid> traps;
    for (itr = Dag::tempmap.begin(); itr!=Dag::tempmap.end(); ++itr){
        if(itr==Dag::tempmap.begin()){
            /*Traps*/
            Trapezoid trap = (static_cast<DagNodeArea *>(*itr->first))->getT();
            traps.push_back(Trapezoid(
                                cg3::Segment2d(trap.getTop().p1(),intersection(trap.getTop(), segment.p1())),
                                cg3::Segment2d(trap.getBottom().p1(),intersection(trap.getBottom(),segment.p1())),
                                trap.getTop().p1() , segment.p1()));
            Trapezoid t1 = (Trapezoid(
                                cg3::Segment2d(intersection(trap.getTop(), segment.p1()),trap.getTop().p2()),
                                cg3::Segment2d(segment.p1(), intersection(segment,trap.getBottom().p2())),
                                segment.p1(), trap.getTop().p2()));
            Trapezoid t2 = (Trapezoid(
                                cg3::Segment2d(segment.p1(), intersection(segment,trap.getBottom().p2())),
                                cg3::Segment2d(intersection(trap.getBottom(), segment.p1()), trap.getBottom().p2()),
                                segment.p1(), trap.getBottom().p2()));
            /*endTraps*/

            DagNode* dg = new DagNodePoint(segment.p1());
            dg->setLeftChild(new DagNodeArea(traps[0])); //Aggiungo il trapezoide tutto a sinistra
            DagNode* dgs = new DagNodeSegment(segment);
            dgs->setLeftChild(new DagNodeArea(t1));
            dgs->setRightChild(new DagNodeArea(t2));
            dg->setRightChild(dgs);
            *(itr->first) = dg;
            lastMeaningful = itr->second;
            lastDirectionUp = (lastMeaningful->compareNodeToSegment(segment)==lastMeaningful->getLeftChild())? true:false;
            nextDir = (next(itr)->second->compareNodeToSegment(segment)==next(itr)->second->getLeftChild())? true:false;
            Dag::removeTrapezoid(trap);
            if((lastDirectionUp != nextDir && !nextDir )|| (lastDirectionUp == nextDir && nextDir)){
                traps.push_back(t2);
                pointers.push_back(dgs->lcPointerAddress());
                lastTouched = dgs->lcPointerAddress();
            }
            if((lastDirectionUp != nextDir && nextDir )|| (lastDirectionUp == nextDir && !nextDir)){
                traps.push_back(t1);
                pointers.push_back(dgs->rcPointerAddress());
                lastTouched = dgs->rcPointerAddress();
            }

        }
        else if(itr!=Dag::tempmap.begin() && itr!=Dag::tempmap.end() && (next(itr) != Dag::tempmap.end())){
            DagNode* dg = new DagNodeSegment(segment);
            Trapezoid trap = (static_cast<DagNodeArea *>(*itr->first))->getT();
            Dag::removeTrapezoid(trap);
            std::vector<cg3::Point2d> rightInts = {trap.getTop().p2(), intersection(segment, trap.getTop().p2()), trap.getBottom().p2()};
            currentDirectionUp = (itr->second->compareNodeToSegment(segment)==itr->second->getLeftChild())? true:false;
            --itr;
            if(itr!=Dag::tempmap.begin()){
                if(lastDirectionUp!=true){
                    (tempTrap.getTop() != static_cast<DagNodeArea *>((*((itr))->first)->getRightChild())->getT().getTop())?traps.push_back(t):traps.push_back(tempTrap);
                    tempTrap = static_cast<DagNodeArea *>((*itr->first))->getT();
                    tempTrap.setTop(cg3::Segment2d(tempTrap.getTop().p1(), rightInts[1]));
                    tempTrap.setBottom(cg3::Segment2d(tempTrap.getBottom().p1(), rightInts[2]));
                    tempTrap.setRightp(rightInts[2]);
                    static_cast<DagNodeArea *>((*itr->first)->getRightChild())->setT(tempTrap);
                    dg->setRightChild((*(itr->first))->getRightChild());
                    if(dg->getRightChild()==*pointers[pointers.size()-1]){
                        pointers.push_back(dg->rcPointerAddress());
                    }
                    else{
                        Dag::multiPointed.insert(std::make_pair<>(&tempTrap, pointers));
                    }
                    t = Trapezoid(
                                trap.getTop(),
                                cg3::Segment2d(intersection(segment, trap.getTop().p1()),intersection(segment, trap.getTop().p2())),
                                intersection(segment, trap.getTop().p1()),intersection(segment, trap.getTop().p2()));
                    traps.push_back(t);

                    dg->setLeftChild(new DagNodeArea(t));
                }
                else{
                    (tempTrap.getTop() != static_cast<DagNodeArea *>((*((itr))->first)->getLeftChild())->getT().getTop())?traps.push_back(tempTrap):traps.push_back(t);
                    tempTrap = static_cast<DagNodeArea *>((*itr->first)->getLeftChild())->getT();
                    tempTrap.setTop(cg3::Segment2d(tempTrap.getTop().p1(), rightInts[0]));
                    tempTrap.setBottom(cg3::Segment2d(tempTrap.getBottom().p1(), rightInts[1]));
                    tempTrap.setRightp(rightInts[1]);
                    static_cast<DagNodeArea *>((*itr->first)->getLeftChild())->setT(tempTrap);
                    dg->setLeftChild((*(itr->first))->getLeftChild());
                    t = Trapezoid( cg3::Segment2d(intersection(segment, trap.getTop().p1()),intersection(segment, trap.getTop().p2())),trap.getBottom(),
                    intersection(segment, trap.getBottom().p1()),intersection(segment, trap.getBottom().p2()));
                    traps.push_back(t);

                    dg->setRightChild(new DagNodeArea(t));
                }
            }
            else{
                if(lastDirectionUp!=true){
                    //Corretta!
                    tempTrap = static_cast<DagNodeArea *>((*(itr->first))->getRightChild()->getRightChild())->getT();
                    tempTrap.setTop(cg3::Segment2d(tempTrap.getTop().p1(), rightInts[1]));
                    tempTrap.setBottom(cg3::Segment2d(tempTrap.getBottom().p1(), rightInts[2]));
                    tempTrap.setRightp(rightInts[2]);
                    static_cast<DagNodeArea *>((*itr->first)->getRightChild()->getRightChild())->setT(tempTrap);
                    dg->setRightChild((*(itr->first))->getRightChild()->getRightChild());
                    t = Trapezoid(
                                trap.getTop(),
                                cg3::Segment2d(intersection(segment, trap.getTop().p1()),intersection(segment, trap.getTop().p2())),
                                intersection(segment, trap.getTop().p1()),intersection(segment, trap.getTop().p2()));
                    dg->setLeftChild(new DagNodeArea(t));
                }
                else{
                     tempTrap = static_cast<DagNodeArea *>((*(itr->first))->getRightChild()->getLeftChild())->getT();
                     tempTrap.setTop(cg3::Segment2d(tempTrap.getTop().p1(), rightInts[0]));
                     tempTrap.setBottom(cg3::Segment2d(tempTrap.getBottom().p1(), rightInts[1]));
                     tempTrap.setRightp(rightInts[1]);
                     static_cast<DagNodeArea *>((*itr->first)->getRightChild()->getLeftChild())->setT(tempTrap);
                     dg->setLeftChild((*(itr->first))->getRightChild()->getLeftChild());
                     t = Trapezoid(
                                 cg3::Segment2d(intersection(segment, trap.getTop().p1()),intersection(segment, trap.getTop().p2())),
                                 trap.getBottom(),
                                 intersection(segment, trap.getBottom().p1()),intersection(segment, trap.getBottom().p2()));
                     dg->setRightChild(new DagNodeArea(t));

                }
            }
            ++itr;
            lastMeaningful = itr->second;
            lastDirectionUp = currentDirectionUp;
            *(itr->first) = dg;

        }
        else if(next(itr) == Dag::tempmap.end()){
            cg3::Point2d p = segment.p2();
            DagNode* dg = new DagNodePoint(p);
            dg->setRightChild(new DagNodeArea());
            DagNode* dgs = new DagNodeSegment(segment);
            currentDirectionUp = (itr->second->compareNodeToSegment(segment)==itr->second->getLeftChild())? true:false;
            Trapezoid trap = (static_cast<DagNodeArea *>(*itr->first))->getT();
            Dag::removeTrapezoid(trap);
            std::vector<cg3::Point2d> rightInts = {intersection(trap.getTop(), segment.p2()), segment.p2(), intersection(trap.getBottom(), segment.p2())};
            --itr;
            if(itr!=Dag::tempmap.begin()){
                if(lastDirectionUp!=true){ //Corretta!!
                    (tempTrap.getTop() != static_cast<DagNodeArea *>((*(itr)->first)->getRightChild())->getT().getTop())?
                                traps.push_back(tempTrap):traps.push_back(t);
                    tempTrap = static_cast<DagNodeArea *>((*itr->first)->getRightChild())->getT();

                    tempTrap.setTop(cg3::Segment2d(tempTrap.getTop().p1(), rightInts[1]));
                    tempTrap.setBottom(cg3::Segment2d(tempTrap.getBottom().p1(), rightInts[2]));
                    tempTrap.setRightp(rightInts[2]);
                    static_cast<DagNodeArea *>((*itr->first)->getRightChild())->setT(tempTrap);
                    dgs->setRightChild((*(itr->first))->getRightChild());
                    t = Trapezoid(cg3::Segment2d(trap.getTop().p1(), intersection(trap.getTop(), segment.p2())),
                                            cg3::Segment2d(intersection(segment, trap.getTop().p1()), segment.p2()),intersection(segment, trap.getTop().p1()), segment.p2() );
                    traps.push_back(t);
                    dgs->setLeftChild(new DagNodeArea(t));
                }
                else{
                    (tempTrap.getTop() != static_cast<DagNodeArea *>((*(itr)->first)->getLeftChild())->getT().getTop())?
                                traps.push_back(tempTrap):traps.push_back(t);
                    tempTrap = static_cast<DagNodeArea *>((*itr->first)->getLeftChild())->getT();
                    tempTrap.setTop(cg3::Segment2d(tempTrap.getTop().p1(), rightInts[0]));
                    tempTrap.setBottom(cg3::Segment2d(tempTrap.getBottom().p1(), rightInts[1]));
                    tempTrap.setRightp(rightInts[1]);
                    static_cast<DagNodeArea *>((*itr->first)->getLeftChild())->setT(tempTrap);
                    dgs->setLeftChild((*(itr->first))->getLeftChild());
                    t = Trapezoid(cg3::Segment2d(intersection(segment, trap.getTop().p1()), segment.p2()),
                                            cg3::Segment2d(trap.getBottom().p1(), intersection(trap.getBottom(), segment.p2())),intersection(segment, trap.getBottom().p1()), segment.p2());
                    traps.push_back(t);
                    dgs->setRightChild(new DagNodeArea(t));
                }
            }
            else{
                if(lastDirectionUp!=true){

                    tempTrap = static_cast<DagNodeArea *>((*(itr->first))->getRightChild()->getRightChild())->getT();
                    tempTrap.setTop(cg3::Segment2d(tempTrap.getTop().p1(), rightInts[1]));
                    tempTrap.setBottom(cg3::Segment2d(tempTrap.getBottom().p1(), rightInts[2]));
                    tempTrap.setRightp(rightInts[2]);
                    static_cast<DagNodeArea *>((*itr->first)->getRightChild()->getRightChild())->setT(tempTrap);
                    dgs->setRightChild((*(itr->first))->getRightChild()->getRightChild());
                    t = Trapezoid(cg3::Segment2d(trap.getTop().p1(), intersection(trap.getTop(), segment.p2())),
                                            cg3::Segment2d(intersection(segment, trap.getTop().p1()), segment.p2()),intersection(segment, trap.getTop().p1()), segment.p2() );
                    traps.push_back(t);
                    dgs->setLeftChild(new DagNodeArea(t));
                }
                else{

                    tempTrap = static_cast<DagNodeArea *>((*(itr->first))->getRightChild()->getLeftChild())->getT();
                    tempTrap.setTop(cg3::Segment2d(tempTrap.getTop().p1(), rightInts[0]));
                    tempTrap.setBottom(cg3::Segment2d(tempTrap.getBottom().p1(), rightInts[1]));
                    tempTrap.setRightp(rightInts[1]);
                    static_cast<DagNodeArea *>((*itr->first)->getRightChild()->getLeftChild())->setT(tempTrap);
                    dgs->setLeftChild((*(itr->first))->getRightChild()->getLeftChild());
                    t = Trapezoid(cg3::Segment2d(intersection(segment, trap.getTop().p1()), segment.p2()),
                                            cg3::Segment2d(trap.getBottom().p1(), intersection(trap.getBottom(), segment.p2())),intersection(segment, trap.getBottom().p1()), segment.p2());
                    traps.push_back(t);
                    dgs->setRightChild(new DagNodeArea(t));
                }
            }
            ++itr;

            Trapezoid rightMost = Trapezoid(
                        cg3::Segment2d(intersection(trap.getTop(), segment.p2()), trap.getTop().p2()),
                        cg3::Segment2d(intersection(trap.getBottom(), segment.p2()),trap.getBottom().p2()),
                        segment.p2(), trap.getTop().p2());
            traps.push_back(tempTrap);
            traps.push_back(rightMost);
            dg->setRightChild(new DagNodeArea(rightMost));
            dg->setLeftChild(dgs);
            *(itr->first) = dg;
        }
    }
    Dag::addTrapezoids(traps);

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
            tempmap.push_back(std::make_pair(split->lcPointerAddress(),meaningful));
            innerNodes(split->getRightChild(),segment,meaningful);
        }
        else if(split->getRightChild()->getRightChild()==nullptr){
            innerNodes(split->getLeftChild(),segment,meaningful);
            tempmap.push_back(std::make_pair(split->rcPointerAddress(),meaningful));

        }
        else{
            innerNodes(split->getLeftChild(),segment,meaningful);
            innerNodes(split->getRightChild(),segment,meaningful);
        }
    }
    if(chosen==right){
        if(split->getRightChild()->getRightChild()==nullptr){
            tempmap.push_back(std::make_pair(split->rcPointerAddress(),meaningful));
        }
        else
            innerNodes(split->getRightChild(),segment,meaningful);
    }
    if(chosen==left){
        if(split->getLeftChild()->getLeftChild()==nullptr){
            tempmap.push_back(std::make_pair(split->lcPointerAddress(),meaningful));
        }
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

std::set<Trapezoid>& Dag::getTrapezoids()
{
    return  trapezoids;
}


void Dag::addTrapezoids(std::vector<Trapezoid> traps)
{
    //std::copy( vec.begin(), vec.end(), std::inserter( trapezoids, trapezoids.end() ) );
    for (unsigned int i = 0; i<traps.size();i++) {
        trapezoids.insert(traps[i]);
    }

}

void Dag::removeTrapezoid(const Trapezoid &tr)
{
    std::set<Trapezoid> copy = trapezoids;
    trapezoids.clear();
    for (auto t : copy){
        if(t!=tr)
            trapezoids.insert(t);
    }

}

std::map<Trapezoid*, std::vector<DagNode **>>& Dag::getMultiPointed()
{
    return multiPointed;
}












