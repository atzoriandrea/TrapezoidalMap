#include "dag_dataset.h"

//Dag::nodes
//std::vector<std::pair<DagNode **, DagNode*>> Dag::tempmap = {};

Dag::Dag(){  
    //tempmap = {};
}
void Dag::addSegment(cg3::Segment2d& segment){ //O (k log k)
    if(Dag::dag.getType()!=DagNode::Leaf){
        DagNode& ins = searchPoint(segment.p1());
        if(segment.p2().x()<((DagNodeArea&)ins).getT().getRightp().x())
            ins = addSegmentInSingleTrap((DagNodeArea&)ins, segment);
        //else
            //followSegment(segment, reinterpret_cast<DagNodeArea&>(*ins));
    }
    else{
        Dag::dag = addSegmentInSingleTrap((DagNodeArea&)Dag::dag, segment);
    }

}


DagNodePoint& Dag::addSegmentInSingleTrap(DagNode& node, cg3::Segment2d& segment){
    std::list<Trapezoid>& ref = TrapezoidalMap::getTrapezoids();
    std::list<boost::any>& nodes = Dag::getNodes();
    std::list<Trapezoid>::iterator itr;
    std::vector<Trapezoid*> iterators(4);
     // = new DagNodePoint(segment.p1());
     // = new DagNodePoint(segment.p2());
     // = new DagNodeSegment(segment);
    //DagNodeArea* leafA;
    //DagNodeArea* leafB;
    //DagNodeArea* leafC;
    //DagNodeArea* leafD;
    const Trapezoid& trap = {((DagNodeArea&)node).getT()};
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
    //iterators[0]->setItr(itr);
    TrapezoidalMap::updateNeighbors(trap, iterators);
    iterators[0]->setNeighbors(trap.getLeftUp(),trap.getLeftDown(),*iterators[1], *iterators[2]);
    iterators[1]->setNeighbors(*iterators[0],*iterators[0], *iterators[3], *iterators[3]);
    iterators[2]->setNeighbors(*iterators[0],*iterators[0], *iterators[3], *iterators[3]);
    iterators[3]->setNeighbors(*iterators[1], *iterators[2], trap.getRightUp(), trap.getRightDown());
    TrapezoidalMap::removeTrapezoid(trap.getItr());
    DagNode& leafA = bind(iterators[0]);
    DagNode& leafB = bind(iterators[1]);
    DagNode& leafC = bind(iterators[2]);
    DagNode& leafD = bind(iterators[3]);
    DagNodeSegment si = DagNodeSegment(leafB, leafC, segment);
    Dag::addNode(si);
    DagNodePoint qi = DagNodePoint((DagNodeSegment&)nodes.back(), leafD, segment.p2());
    Dag::addNode(qi);
    DagNodePoint pi= DagNodePoint(leafA,(DagNodePoint&)nodes.back(), segment.p1());
    node = pi;
    //return (DagNodePoint&)*pi;
}

DagNode& Dag::bind(Trapezoid * trap)
{
    DagNodeArea* last;
    std::list<boost::any>& ref = Dag::getNodes();
    DagNodeArea first = DagNodeArea(*trap);
    Dag::addNode(first);
    last = (DagNodeArea*)&ref.back();
    last->setTrap(*trap);
    return (DagNode&)ref.back();
}

std::list<boost::any>& Dag::getNodes()
{
    return nodes;
}



void Dag::addNode(DagNode& node){
    nodes.push_back(node);
}



DagNode& Dag::searchPoint(const cg3::Point2d& point){ //O(log n)
    DagNode* tmpdag= &Dag::dag;

    while(tmpdag->getType()!=DagNode::Leaf){
        tmpdag = &((InnerNodes*)tmpdag)->compareNodeToPoint(point);
    }
    return *tmpdag;

}


//DagNode& Dag::searchAndAppend(const cg3::Point2d &point)
//{
//    DagNode& tmpdag=Dag::dag;
//    InnerNodes& chosen=(InnerNodes&)Dag::dag;
//    while(tmpdag.getType()!=DagNode::Leaf){
//        chosen = (InnerNodes&)tmpdag;
//        tmpdag = ((InnerNodes*)tmpdag)->compareNodeToPoint(point);
//    }
//    if(chosen.leftChild == tmpdag)
//        return chosen.getLeftChild();
//    if(chosen.rightChild == tmpdag)
//        return chosen.getRightChild();
//}

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

void Dag::followSegment(cg3::Segment2d &segment, DagNodeArea &leaf){
    std::vector<DagNodeArea *> leaves;
    std::vector<std::list<Trapezoid>::iterator> garbage;
    leaves.push_back(&leaf);
    int size = leaves.size();
    unsigned long iter = 0;
    while (segment.p2().x() > leaves[size-1]->getT().getRightp().x()) {
        garbage.push_back(leaves[size-1]->getT().getItr());
        if(matrixDet(segment, leaves[size-1]->getT().getRightp())<0){
            leaves.push_back(leaves[size-1]->getT().getRightUp().getNode());

        }
        else{
            leaves.push_back(leaves[size-1]->getT().getRightDown().getNode());
        }
        size++;
    }

//    for(DagNodeArea* lf : leaves){
//        DagNodeArea *& l = lf;
//        DagNode *& ref = (DagNode*&)l;
//        if(iter==0)
//            ref = createLeftMost(segment, *lf, garbage);
//        else if(iter<leaves.size()-1)
//            ref = createIntermediate(segment, *lf, garbage);
//        else
//            ref = createRightMost(segment, *lf, garbage);

//        iter++;
//    }
//    TrapezoidalMap::deleteGarbage(garbage);
//}

//DagNode* Dag::createLeftMost(cg3::Segment2d &segment, DagNodeArea &leaf, std::vector<std::list<Trapezoid>::iterator>& garbageCollector){
//    std::list<Trapezoid>& ref = TrapezoidalMap::getTrapezoids();
//    std::list<Trapezoid>::iterator itr;
//    std::vector<Trapezoid*> iterators(3);
//    DagNodePoint* pi = new DagNodePoint(segment.p1());
//    InnerNodes* si = new DagNodeSegment(segment);
//    const Trapezoid& trap = leaf.getT();
//    std::vector<cg3::Point2d> ints = {intersection(trap.getTop(), segment.p1()),
//                                      intersection(trap.getBottom(), segment.p1()),
//                                      intersection(segment, trap.getTop().p2())};
//    std::vector<Trapezoid> traps = {Trapezoid(cg3::Segment2d(trap.getTop().p1(),ints[0]),
//                                    cg3::Segment2d(trap.getBottom().p1(),ints[1]),
//                                    trap.getLeftp() , segment.p1(), nullptr),
//                                    Trapezoid(cg3::Segment2d(ints[0],trap.getTop().p2()),
//                                    cg3::Segment2d(segment.p1(), intersection(segment,trap.getBottom().p2())),
//                                    segment.p1(), ints[2], nullptr),
//                                    Trapezoid(cg3::Segment2d(segment.p1(),ints[2]),
//                                    cg3::Segment2d(ints[1], trap.getBottom().p2()),
//                                    segment.p1(), ints[2],nullptr)};
//    TrapezoidalMap::addTrapezoids(traps);
//    itr = ref.end();
//    iterators[2] = &*(--itr);
//    iterators[2]->setItr(itr);
//    iterators[1] = &*(--itr);
//    iterators[1]->setItr(itr);
//    iterators[0] = &*(--itr);
//    iterators[0]->setItr(itr);
//    //TODO
//    garbageCollector.push_back(trap.getItr());
//    TrapezoidalMap::updateNeighborsMultiTrapezoid(trap, iterators, 0);
//    iterators[0]->setNeighbors(trap.getLeftUp(),trap.getLeftDown(),*iterators[1], *iterators[2]); //
//    iterators[1]->setNeighbors(*iterators[0],*iterators[0], trap.getRightUp(), trap.getRightUp());// be careful on right side neighbors
//    iterators[2]->setNeighbors(*iterators[0],*iterators[0], trap.getRightDown(), trap.getRightDown());//also there

//    pi->setLeftChild(bind(iterators[0]));
//    pi->setRightChild(si);
//    si->setLeftChild(bind(iterators[1]));
//    si->setRightChild(bind(iterators[2]));
//    return pi;
//}

//DagNode *Dag::createRightMost(cg3::Segment2d &segment, DagNodeArea &leaf, std::vector<std::list<Trapezoid>::iterator>& garbageCollector)
//{
//    std::list<Trapezoid>& ref = TrapezoidalMap::getTrapezoids();
//    std::list<Trapezoid>::iterator itr;
//    bool upper;
//    std::vector<Trapezoid*> iterators(3);
//    DagNodePoint* qi = new DagNodePoint(segment.p2());
//    InnerNodes* si = new DagNodeSegment(segment);
//    const Trapezoid& trap = leaf.getT();
//    std::vector<cg3::Point2d> ints = {intersection(trap.getTop(), segment.p2()),
//                                      intersection(trap.getBottom(), segment.p2()),
//                                      intersection(segment, trap.getTop().p1())};
//    std::vector<Trapezoid> traps = {Trapezoid(cg3::Segment2d(trap.getTop().p1(),ints[0]),
//                                    cg3::Segment2d(ints[3], segment.p2()),
//                                    (trap.getLeftp().y()<ints[3].y())?ints[3]:trap.getLeftp() , segment.p2(), nullptr),
//                                    Trapezoid(cg3::Segment2d(ints[3],segment.p2()),
//                                    cg3::Segment2d(trap.getBottom().p1(), ints[1]),
//                                    (trap.getLeftp().y()>ints[3].y())?ints[3]:trap.getLeftp(), ints[2], nullptr),
//                                    Trapezoid(cg3::Segment2d(ints[0],trap.getTop().p2()),
//                                    cg3::Segment2d(ints[1], trap.getBottom().p2()),
//                                    segment.p2(), trap.getRightp(),nullptr)};
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
//    iterators[2] = &*(--itr);
//    iterators[2]->setItr(itr);
//    iterators[1] = &*(--itr);
//    iterators[1]->setItr(itr);
//    iterators[0] = &*(--itr);
//    iterators[0]->setItr(itr);
//    //TODO
//    garbageCollector.push_back(trap.getItr());
//    TrapezoidalMap::updateNeighborsMultiTrapezoid(trap, iterators, 2);
//    iterators[0]->setNeighbors(trap.getLeftUp(),trap.getLeftUp(),*iterators[2], *iterators[2]);//
//    iterators[1]->setNeighbors(trap.getLeftDown(),trap.getLeftDown(),*iterators[2], *iterators[2]);//
//    iterators[2]->setNeighbors(*iterators[0],*iterators[1], trap.getRightUp(), trap.getRightDown());//

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
//    return qi;
//}

//DagNode *Dag::createIntermediate(cg3::Segment2d &segment, DagNodeArea &leaf, std::vector<std::list<Trapezoid>::iterator>& garbageCollector)
//{
//    std::list<Trapezoid>& ref = TrapezoidalMap::getTrapezoids();
//    std::list<Trapezoid>::iterator itr;
//    DagNode* ex;
//    bool upper;
//    std::vector<Trapezoid*> iterators(2);
//    InnerNodes* si = new DagNodeSegment(segment);
//    const Trapezoid& trap = leaf.getT();
//    std::vector<cg3::Point2d> ints = {intersection(segment, trap.getTop().p1()), intersection(segment, trap.getTop().p2())};
//    std::vector<Trapezoid> traps = {Trapezoid(cg3::Segment2d(trap.getTop().p1(),trap.getTop().p2()),
//                                    cg3::Segment2d(ints[0], ints[1]),
//                                    (trap.getLeftp().y()<ints[0].y())?ints[0]:trap.getLeftp(), (trap.getRightp().y()<ints[1].y())?ints[1]:trap.getRightp(), nullptr),
//                                    Trapezoid(cg3::Segment2d(ints[0], ints[1]),
//                                    cg3::Segment2d(trap.getBottom().p1(), trap.getBottom().p2()),
//                                    (trap.getLeftp().y()>ints[1].y())?ints[0]:trap.getLeftp(), (trap.getRightp().y()>ints[1].y())?ints[1]:trap.getRightp(), nullptr)};
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

//    garbageCollector.push_back(trap.getItr());
//    TrapezoidalMap::updateNeighborsMultiTrapezoid(trap, iterators, 1);

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
//    //iterators[0]->setNeighbors(trap.getLeftUp(),trap.getLeftUp(),*iterators[2], *iterators[2]);//
//    //iterators[1]->setNeighbors(trap.getLeftDown(),trap.getLeftDown(),*iterators[2], *iterators[2]);//

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
//    return si;
//}















}
