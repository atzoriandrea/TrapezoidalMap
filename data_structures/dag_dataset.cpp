#include "dag_dataset.h"


Dag::Dag(){  
    //tempmap = {};
}
void Dag::addSegment(cg3::Segment2d& segment){ //O (k log k)
    if(Dag::dag->getType()!=DagNode::Leaf){
        DagNode*& ins = searchAndAppend(segment.p1());
        if(segment.p2().x()>((DagNodeArea*)ins)->getT().getRightp().x())
            followSegment(segment, (DagNodeArea*)ins);
        else
            ins = addSegmentInSingleTrap(ins, segment);
    }
    else{
        Dag::dag = addSegmentInSingleTrap(Dag::dag, segment);
    }

}


DagNodePoint* Dag::addSegmentInSingleTrap(DagNode* node, cg3::Segment2d& segment){
    std::list<Trapezoid>& ref = TrapezoidalMap::getTrapezoids();
    std::list<Trapezoid>::iterator itr;
    std::vector<Trapezoid*> iterators(4);
    DagNode * leafA;
    DagNode * leafB;
    DagNode * leafC;
    DagNode * leafD;
    const Trapezoid& trap = {((DagNodeArea*)node)->getT()};
    std::vector<cg3::Point2d> ints = {intersection(trap.getTop(), segment.p1()),
                                      intersection(trap.getTop(), segment.p2()),
                                      intersection(trap.getBottom(), segment.p1()),
                                      intersection(trap.getBottom(), segment.p2())};

    std::vector<std::tuple<cg3::Segment2d, cg3::Segment2d, cg3::Point2d, cg3::Point2d>> traps = {
        std::make_tuple(cg3::Segment2d(trap.getTop().p1(),ints[0]),cg3::Segment2d(trap.getBottom().p1(),ints[2]),trap.getLeftp(), segment.p1()),
        std::make_tuple(cg3::Segment2d(ints[0],ints[1]), cg3::Segment2d(segment.p1(),segment.p2()), segment.p1(), segment.p2()),
        std::make_tuple(cg3::Segment2d(segment.p1(),segment.p2()),cg3::Segment2d(ints[2],ints[3]), segment.p1(), segment.p2()),
        std::make_tuple(cg3::Segment2d(ints[1], trap.getTop().p2()),cg3::Segment2d(ints[3], trap.getBottom().p2()),segment.p2(), trap.getRightp()),
    };

    DagNodePoint * pi= new DagNodePoint(nullptr,nullptr, segment.p1());
    leafA = bindWithParentRef(std::get<0>(traps[0]), std::get<1>(traps[0]), std::get<2>(traps[0]), std::get<3>(traps[0]), pi->getLeftChild());
    pi->setLeftChild(leafA);

    DagNodePoint * qi = new DagNodePoint(nullptr, nullptr, segment.p2());
    leafD = bindWithParentRef(std::get<0>(traps[3]), std::get<1>(traps[3]), std::get<2>(traps[3]), std::get<3>(traps[3]), qi->getRightChild());
    pi->setRightChild(qi);
    qi->setRightChild(leafD);

    DagNodeSegment* si = new DagNodeSegment(nullptr, nullptr, segment);
    leafB = bindWithParentRef(std::get<0>(traps[1]), std::get<1>(traps[1]), std::get<2>(traps[1]), std::get<3>(traps[1]), si->getLeftChild());
    leafC = bindWithParentRef(std::get<0>(traps[2]), std::get<1>(traps[2]), std::get<2>(traps[2]), std::get<3>(traps[2]), si->getRightChild());
    qi->setLeftChild(si);
    si->setLeftChild(leafB);
    si->setRightChild(leafC);

    itr = ref.end();
//    for(int i=3; i>-1; i--)
//        iterators[i] = &*(--itr);
    iterators[2] = &*(--itr);
    iterators[1] = &*(--itr);
    iterators[3] = &*(--itr);
    iterators[0] = &*(--itr);
    TrapezoidalMap::updateNeighbors(trap, iterators);
    iterators[0]->setNeighbors(trap.getLeftUp(),trap.getLeftDown(),*iterators[1], *iterators[2]); //A
    iterators[1]->setNeighbors(*iterators[0],*iterators[0], *iterators[3], *iterators[3]); //B
    iterators[2]->setNeighbors(*iterators[0],*iterators[0], *iterators[3], *iterators[3]); //C
    iterators[3]->setNeighbors(*iterators[1], *iterators[2], trap.getRightUp(), trap.getRightDown()); //D
    TrapezoidalMap::removeTrapezoid(trap.getItr());

    return pi;
}


DagNodeArea *Dag::bindWithParentRef(cg3::Segment2d &top, cg3::Segment2d &bottom, cg3::Point2d &leftp, cg3::Point2d &rightp, DagNode*& ref)
{
    Trapezoid trap = Trapezoid(top, bottom, leftp, rightp, (DagNodeArea*&)ref);
    TrapezoidalMap::addTrapezoid(trap);
    DagNodeArea* first = new DagNodeArea(TrapezoidalMap::getTrapezoids().back());
    //TrapezoidalMap::getTrapezoids().back().setItr(prev(TrapezoidalMap::getTrapezoids().end()));

    return first;
}

DagNode *&Dag::dagRef()
{
    return Dag::dag;
}

DagNode* Dag::searchPoint(const cg3::Point2d& point){ //O(log n)
    DagNode* tmpdag= Dag::dag;

    while(tmpdag->getType()!=DagNode::Leaf){
        tmpdag = ((InnerNodes*)tmpdag)->compareNodeToPoint(point);
    }
    return tmpdag;

}

DagNode*& Dag::searchAndAppend(const cg3::Point2d &point)
{
    DagNode* tmpdag=Dag::dag;
    InnerNodes* chosen=(InnerNodes*)Dag::dag;
    while(tmpdag->getType()!=DagNode::Leaf){
        chosen = (InnerNodes*)tmpdag;
        tmpdag = ((InnerNodes*)tmpdag)->compareNodeToPoint(point);
    }
    if(chosen->leftChild == tmpdag)
        return chosen->getLeftChild();
    if(chosen->rightChild == tmpdag)
        return chosen->getRightChild();
}

//DagNode *Dag::splitNode(cg3::Segment2d &segment) //O(log n)
//{
//    DagNode* tmpdag=Dag::dag;
//    DagNode* c1=nullptr;
//    DagNode* c2=nullptr;
//    bool splitted = false;
//    if (tmpdag == nullptr)
//        return nullptr;
//    while(!splitted){
//        c1 = tmpdag->compareNodeToPoint(segment.p1());
//        c2 = tmpdag->compareNodeToPoint(segment.p2());
//        if(c1==c2 && c1->getLeftChild()!=nullptr && c1->getRightChild()!=nullptr)
//            tmpdag = c1;
//        else
//            splitted = true;
//    }
//    return tmpdag;
//}

void Dag::followSegment(cg3::Segment2d &segment, DagNodeArea *leaf){
    //std::list<DagNodeArea *> leaves;

    std::vector<std::list<Trapezoid>::iterator> garbage;

    DagNodeArea * current;
    //copy = createLeftMost(segment, *(DagNodeArea*)copy, garbage);
    DagNode* prevSeg;
    //leaves.push_back(leaf);
    //int size = leaves.size();
    unsigned long iter = 0;
    while (segment.p2().x() > leaf->getT().getRightp().x()) {
        current = leaf;
        //garbage.push_back(leaf->getT().getItr());
        if(matrixDet(segment, leaf->getT().getRightp())<0){
            leaf = leaf->getT().getRightUp().getNodeRef();

        }
        else{
            leaf = leaf->getT().getRightDown().getNodeRef();
        }
        DagNode *& copy = (DagNode*&)current->getT().getNodeRef();
        //if(segment.p2().x() > current->getT().getRightp().x()){
        if(iter == 0){
            copy = createLeftMost(segment, *(DagNodeArea*)copy);
            prevSeg = ((DagNodePoint*)copy)->getRightChild();
        }
        else{
            copy = createIntermediate(segment, *(DagNodeArea*)copy, *(DagNodeSegment*)prevSeg);
            prevSeg = copy;
        }
//        }else{
//            copy = createRightMost(segment, *(DagNodeArea*)copy, garbage);
//        }
        iter ++;
    }
    DagNode*& copy = (DagNode*&)leaf->getT().getNodeRef();
    copy = createRightMost(segment, *(DagNodeArea*)copy, *(DagNodeSegment*)prevSeg);
    //garbage.push_back(leaf->getT().getItr());
//    for(DagNodeArea* lf : leaves){
//        //DagNodeArea *& l = lf;
//        DagNode *& ref = (DagNode*&)lf->getT().getNodeRef();
//        if(iter==0)
//            ref = createLeftMost(segment, *lf, garbage);
//        //else if(iter<leaves.size()-1)
//        //    ref = createIntermediate(segment, *lf, garbage);
//        else
//            ref = createRightMost(segment, *lf, garbage);

//        iter++;
//    }
    //TrapezoidalMap::deleteGarbage(garbage);
}

DagNode* Dag::createLeftMost(cg3::Segment2d &segment, DagNodeArea &leaf){
    std::list<Trapezoid>& ref = TrapezoidalMap::getTrapezoids();
    std::list<Trapezoid>::iterator itr;
    std::vector<Trapezoid*> iterators(3);
    DagNode * leafA;
    DagNode * leafB;
    DagNode * leafC;
    DagNodePoint* pi;// = new DagNodePoint(nullptr, nullptr, segment.p1());
    InnerNodes* si;// = new DagNodeSegment(segment);
    const Trapezoid& trap = leaf.getT();
    std::vector<cg3::Point2d> ints = {intersection(trap.getTop(), segment.p1()),
                                      intersection(trap.getBottom(), segment.p1()),
                                      intersection(segment, trap.getTop().p2())};
    std::vector<std::tuple<cg3::Segment2d, cg3::Segment2d, cg3::Point2d, cg3::Point2d>> traps = {
        std::make_tuple(cg3::Segment2d(trap.getTop().p1(),ints[0]),cg3::Segment2d(trap.getBottom().p1(),ints[1]), trap.getLeftp() , segment.p1()),
        std::make_tuple(cg3::Segment2d(ints[0],trap.getTop().p2()),cg3::Segment2d(segment.p1(), intersection(segment,trap.getBottom().p2())),segment.p1(), ints[2]),
        std::make_tuple(cg3::Segment2d(segment.p1(),ints[2]),cg3::Segment2d(ints[1], trap.getBottom().p2()),segment.p1(), ints[2]),

    };

    pi = new DagNodePoint(nullptr, nullptr, segment.p1());
    leafA = bindWithParentRef(std::get<0>(traps[0]), std::get<1>(traps[0]), std::get<2>(traps[0]), std::get<3>(traps[0]), pi->getLeftChild());
    pi->setLeftChild(leafA);

    si = new DagNodeSegment(nullptr, nullptr, segment);
    leafB = bindWithParentRef(std::get<0>(traps[1]), std::get<1>(traps[1]), std::get<2>(traps[1]), std::get<3>(traps[1]), si->getLeftChild());
    leafC = bindWithParentRef(std::get<0>(traps[2]), std::get<1>(traps[2]), std::get<2>(traps[2]), std::get<3>(traps[2]), si->getRightChild());
    pi->setRightChild(si);
    si->setLeftChild(leafB);
    si->setRightChild(leafC);

//    std::vector<Trapezoid> traps = {Trapezoid(ctrap.getRightp()g3::Segment2d(trap.getTop().p1(),ints[0]),
//                                    cg3::Segment2d(trap.getBottom().p1(),ints[1]),
//                                    trap.getLeftp() , segment.p1(), nullptr),
//                                    Trapezoid(cg3::Segment2d(ints[0],trap.getTop().p2()),
//                                    cg3::Segment2d(segment.p1(), intersection(segment,trap.getBottom().p2())),
//                                    segment.p1(), ints[2], nullptr),
//                                    Trapezoid(cg3::Segment2d(segment.p1(),ints[2]),
//                                    cg3::Segment2d(ints[1], trap.getBottom().p2()),
//                                    sTrapezoidalMap::removeTrapezoid(trap.getItr());egment.p1(), ints[2],nullptr)};
    //TrapezoidalMap::addTrapezoids(traps);

    itr = ref.end();
    for(int i=2; i>-1; i--)
        iterators[i] = &*(--itr);
    //TODO
    TrapezoidalMap::updateNeighborsMultiTrapezoid(trap, iterators, 0, *(DagNodeSegment*)si);
    iterators[0]->setNeighbors(trap.getLeftUp(),trap.getLeftDown(),*iterators[1], *iterators[2]); //
    iterators[1]->setNeighbors(*iterators[0],*iterators[0], trap.getRightUp(), trap.getRightUp());// be careful on right side neighbors
    iterators[2]->setNeighbors(*iterators[0],*iterators[0], trap.getRightDown(), trap.getRightDown());//also there
    TrapezoidalMap::removeTrapezoid(trap.getItr());
    return pi;
}

DagNode *Dag::createRightMost(cg3::Segment2d &segment, DagNodeArea &leaf,  DagNodeSegment& prevSeg)
{
    std::list<Trapezoid>& ref = TrapezoidalMap::getTrapezoids();
    std::list<Trapezoid>::iterator itr;
    bool upper;
    DagNode * leafA;
    DagNode * leafB;
    DagNode * leafC;
    std::vector<Trapezoid*> iterators(3);
    DagNodePoint* qi;// = new DagNodePoint(segment.p2());
    InnerNodes* si;// = new DagNodeSegment(segment);
    const Trapezoid& trap = leaf.getT();
    std::vector<cg3::Point2d> ints = {intersection(trap.getTop(), segment.p2()),
                                      intersection(trap.getBottom(), segment.p2()),
                                      intersection(segment, trap.getTop().p1())};

    std::vector<std::tuple<cg3::Segment2d, cg3::Segment2d, cg3::Point2d, cg3::Point2d>> traps = {
        std::make_tuple(cg3::Segment2d(trap.getTop().p1(),ints[0]),cg3::Segment2d(ints[2], segment.p2()),(trap.getLeftp().y()<ints[2].y())?ints[2]:trap.getLeftp() , segment.p2()),
        std::make_tuple(cg3::Segment2d(ints[2],segment.p2()),cg3::Segment2d(trap.getBottom().p1(), ints[1]),(trap.getLeftp().y()>ints[2].y())?ints[2]:trap.getLeftp(), segment.p2()),
        std::make_tuple(cg3::Segment2d(ints[0],trap.getTop().p2()),cg3::Segment2d(ints[1], trap.getBottom().p2()),segment.p2(), trap.getRightp()),
    };
//    std::vector<Trapezoid> traps = {Trapezoid(cg3::Segment2d(trap.getTop().p1(),ints[0]),
//                                    cg3::Segment2d(ints[3], segment.p2()),
//                                    (trap.getLeftp().y()<ints[3].y())?ints[3]:trap.getLeftp() , segment.p2(), nullptr),
//                                    Trapezoid(cg3::Segment2d(ints[3],segment.p2()),
//                                    cg3::Segment2d(trap.getBottom().p1(), ints[1]),
//                                    (trap.getLeftp().y()>ints[3].y())?ints[3]:trap.getLeftp(), ints[2], nullptr),
//                                    Trapezoid(cg3::Segment2d(ints[0],trap.getTop().p2()),
//                                    cg3::Segment2d(ints[1], trap.getBottom().p2()),
//                                    segment.p2(), trap.getRightp(),nullptr)};
    qi = new DagNodePoint(nullptr, nullptr, segment.p2());
    leafC = bindWithParentRef(std::get<0>(traps[2]), std::get<1>(traps[2]), std::get<2>(traps[2]), std::get<3>(traps[2]), qi->getRightChild());
    qi->setRightChild(leafC);


    itr = ref.end();
    iterators[2] = &*(--itr);
    itr = ref.end();
    if(ints[2].y()>trap.getLeftp().y()){

        si= new DagNodeSegment((TrapezoidalMap::merge(((DagNodeArea*)prevSeg.getLeftChild())->getT(), traps[0])),nullptr, segment);
        leafB = bindWithParentRef(std::get<0>(traps[1]), std::get<1>(traps[1]), std::get<2>(traps[1]), std::get<3>(traps[1]), si->getRightChild());
        si->setRightChild(leafB);
        upper = true;
        iterators[1] = &*(--itr);
        iterators[0] = &((DagNodeArea*)si->getLeftChild())->getT();
    }
    else{
        si = new DagNodeSegment(nullptr, TrapezoidalMap::merge(((DagNodeArea*)prevSeg.getRightChild())->getT(),traps[1]), segment);
        leafA = bindWithParentRef(std::get<0>(traps[0]), std::get<1>(traps[0]), std::get<2>(traps[0]), std::get<3>(traps[0]), si->getLeftChild());
        si->setLeftChild(leafA);
        upper = false;
        iterators[0] = &*(--itr);
        iterators[1] = &((DagNodeArea*)si->getRightChild())->getT();

    }
    qi->setLeftChild(si);
    //TrapezoidalMap::addTrapezoids(traps);

//    for(int i=2; i>-1; i--)
//        iterators[i] = &*(--itr);
//    iterators[2] = &*(--itr);
//    iterators[2]->setItr(itr);
//    iterators[1] = &*(--itr);
//    iterators[1]->setItr(itr);
//    iterators[0] = &*(--itr);
//    iterators[0]->setItr(itr);
    //TODO
    //garbageCollector.push_back(trap.getItr());
    TrapezoidalMap::updateNeighborsMultiTrapezoid(trap, iterators, 2, prevSeg);
    if(upper){
        iterators[0]->setNeighbors(iterators[0]->getLeftUp(),iterators[0]->getLeftDown(),*iterators[2], *iterators[2]);//
        iterators[1]->setNeighbors(((DagNodeArea*)prevSeg.getRightChild())->getT(),trap.getLeftDown(),*iterators[2], *iterators[2]);
    }
    else{
        iterators[0]->setNeighbors(trap.getLeftUp(),((DagNodeArea*)prevSeg.getLeftChild())->getT(),*iterators[2], *iterators[2]);//
        iterators[1]->setNeighbors(iterators[1]->getLeftUp(),iterators[1]->getLeftDown(),*iterators[2], *iterators[2]);
    }
    iterators[2]->setNeighbors(*iterators[0],*iterators[1], trap.getRightUp(), trap.getRightDown());//

//    qi->setRightChild(bind(iterators[2]));
//    qi->setLeftChild(si);
//    si->setLeftChild(bind(iterators[0]));
//    si->setRightChild(bind(iterators[1]));

//    if(upper){
//        DagNode* ex = reinterpret_cast< DagNode*>(trap.getLeftUp().getLeftUp().getRightUp().getNode());
//        ex = si->getLeftChild();
//    }
//    else{
//         DagNode* ex = reinterpret_cast<DagNode*>(trap.getLeftDown().getLeftDown().getRightDown().getNode());
//        ex = si->getRightChild();
//    }
    TrapezoidalMap::removeTrapezoid(trap.getItr());
    return qi;
}

DagNode *Dag::createIntermediate(cg3::Segment2d &segment, DagNodeArea &leaf, DagNodeSegment& prevSeg)
{
    std::list<Trapezoid>& ref = TrapezoidalMap::getTrapezoids();
    std::list<Trapezoid>::iterator itr;
    DagNode* ex;
    DagNode * leafA;
    DagNode * leafB;
    bool upper;
    std::vector<Trapezoid*> iterators(2);
    InnerNodes* si;// = new DagNodeSegment(segment);
    const Trapezoid& trap = leaf.getT();
    std::vector<cg3::Point2d> ints = {intersection(segment, trap.getTop().p1()), intersection(segment, trap.getTop().p2())};
    std::vector<std::tuple<cg3::Segment2d, cg3::Segment2d, cg3::Point2d, cg3::Point2d>> traps = {
        std::make_tuple(cg3::Segment2d(trap.getTop().p1(),trap.getTop().p2()),cg3::Segment2d(ints[0], ints[1]),(trap.getLeftp().y()<ints[0].y())?ints[0]:trap.getLeftp(), (trap.getRightp().y()<ints[1].y())?ints[1]:trap.getRightp()),
        std::make_tuple(cg3::Segment2d(ints[0], ints[1]),cg3::Segment2d(trap.getBottom().p1(), trap.getBottom().p2()),(trap.getLeftp().y()>ints[1].y())?ints[0]:trap.getLeftp(), (trap.getRightp().y()>ints[1].y())?ints[1]:trap.getRightp()),
    };
//    std::vector<Trapezoid> traps = {Trapezoid(cg3::Segment2d(trap.getTop().p1(),trap.getTop().p2()),
//                                    cg3::Segment2d(ints[0], ints[1]),
//                                    (trap.getLeftp().y()<ints[0].y())?ints[0]:trap.getLeftp(), (trap.getRightp().y()<ints[1].y())?ints[1]:trap.getRightp(), nullptr),
//                                    Trapezoid(cg3::Segment2d(ints[0], ints[1]),
//                                    cg3::Segment2d(trap.getBottom().p1(), trap.getBottom().p2()),
//                                    (trap.getLeftp().y()>ints[1].y())?ints[0]:trap.getLeftp(), (trap.getRightp().y()>ints[1].y())?ints[1]:trap.getRightp(), nullptr)};

    itr = ref.end();
    if(ints[0].y()>trap.getLeftp().y()){
        si= new DagNodeSegment((TrapezoidalMap::merge(((DagNodeArea*)prevSeg.getLeftChild())->getT(), traps[0])),nullptr, segment);
        leafB = bindWithParentRef(std::get<0>(traps[1]), std::get<1>(traps[1]), std::get<2>(traps[1]), std::get<3>(traps[1]), si->getRightChild());
        si->setRightChild(leafB);
        upper = true;
        iterators[1] = &*(--itr);
        iterators[0] = &((DagNodeArea*)si->getLeftChild())->getT();
    }
    else{
        si = new DagNodeSegment(nullptr, TrapezoidalMap::merge(((DagNodeArea*)prevSeg.getRightChild())->getT(),traps[1]), segment);
        leafA = bindWithParentRef(std::get<0>(traps[0]), std::get<1>(traps[0]), std::get<2>(traps[0]), std::get<3>(traps[0]), si->getLeftChild());
        si->setLeftChild(leafA);
        upper = false;
        iterators[0] = &*(--itr);
        iterators[1] = &((DagNodeArea*)si->getRightChild())->getT();
    }



//    for(int i=1; i>-1; i--)
//        iterators[i] = &*(--itr);

//    if(ints[0].y()>trap.getLeftp().y()){
//        TrapezoidalMap::merge(trap.getLeftUp().getLeftUp().getRightUp(), traps[0], garbageCollector);
//        upper = true;
//    }
//    else{
//        upper = false;
//        TrapezoidalMap::merge(trap.getLeftDown().getLeftDown().getRightDown(),traps[1], garbageCollector);
//    }
//    TrapezoidalMap::addTrapezoids(traps);
//    itr = ref.end();
//    iterators[1] = &*(--itr);
//    iterators[1]->setItr(itr);
//    iterators[0] = &DagNode& Dag::bind(Trapezoid * trap)

//    iterators[0]->setItr(itr);

    //garbageCollector.push_back(trap.getItr());
    TrapezoidalMap::updateNeighborsMultiTrapezoid(trap, iterators, 1, prevSeg);

//    if(upper){
//        iterators[0]->setRightUp(trap.getRightUp());
//        iterators[0]->setRightDown((iterators[0]->getBottom().p2().y()<trap.getRightp().y())?trap.getRightDown():trap.getRightUp());
//        iterators[1]->setNeighbors(trap.getLeftUp(), trap.getLeftDown(),
//                                   (iterators[0]->getBottom().p2().y()<trap.getRightp().y())?trap.getRightDown():trap.getRightUp(),
//                                    trap.getRightDown());//
//    }
//    else{
//        iterators[0]->setNeighbors(trap.getLeftUp(), trap.getLeftDown(),trap.getRightUp(),
//                                   (iterators[1]->getTop().p2().y()>trap.getRightp().y())?trap.getRightUp():trap.getRightDown());
//        iterators[1]->setRightUp((iterators[1]->getTop().p2().y()>trap.getRightp().y())?trap.getRightUp():trap.getRightDown());
//        iterators[1]->setRightDown(trap.getRightDown());
//    }
    //iterators[0]->setNeighbors(trap.getLeftUp(),trap.getLeftUp(),*iterators[2], *iterators[2]);//
    //iterators[1]->setNeighbors(trap.getLeftDown(),trap.getLeftDown(),*iterators[2], *iterators[2]);//

//    si->setLeftChild(bind(iterators[0]));
//    si->setRightChild(bind(iterators[1]));

//    if(upper){
//        (trap.getLeftUp().getLeftUp().getRightUp().setNode(dynamic_cast<DagNodeArea*>(si->getLeftChild())));
//        //ex = si->getLeftChild();
//    }
//    else{
//        (trap.getLeftDown().getLeftDown().getRightDown().setNode(dynamic_cast<DagNodeArea*>(si->getRightChild())));
//        //ex = si->getRightChild();
//    }
    TrapezoidalMap::removeTrapezoid(trap.getItr());
    return si;
}
















