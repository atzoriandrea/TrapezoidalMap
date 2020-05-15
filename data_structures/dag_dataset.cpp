#include "dag_dataset.h"
#include "algorithms/algorithms.h"

//void Dag::addSegment(cg3::Segment2d& segment){ //O (k log k)
//    bool leftDegenerate = false;
//    bool rightDegenerate = false;
//    if(Dag::dag->getType()!=DagNode::Leaf){
//        DagNode*& ins = searchAndAppend(segment);
//        if(segment.p2().x()>((DagNodeArea*)ins)->getT().getRightp().x())
//            followSegment(segment, (DagNodeArea*)ins);
//        else{
//            if(segment.p1() == ((DagNodeArea*)ins)->getT().getLeftp())
//                leftDegenerate=true;
//            if(segment.p2() == ((DagNodeArea*)ins)->getT().getRightp())
//                rightDegenerate=true;
//            if(leftDegenerate && rightDegenerate)
//                ins = totallyDegenerateSingleInsertion(ins, segment);
//            else if(leftDegenerate && !rightDegenerate)
//                ins = leftDegenerateSingleInsertion(ins, segment);
//            else if(!leftDegenerate && rightDegenerate)
//                ins = rightDegenerateSingleInsertion(ins, segment);
//            else
//                ins = addSegmentInSingleTrap(ins, segment);
//        }
//    }
//    else{
//        Dag::dag = addSegmentInSingleTrap(Dag::dag, segment);
//    }

//}


Dag::Dag(Trapezoid &link)
{
    dag = link.getNode();
}

DagNodePoint* Dag::addSegmentInSingleTrap(const cg3::Segment2d& segment, Trapezoid& a, Trapezoid& d, Trapezoid& b, Trapezoid& c){
    DagNodeSegment* si = new DagNodeSegment((DagNode*)b.getNodeRef(), (DagNode*)c.getNodeRef(), segment);
    DagNodePoint * qi = new DagNodePoint((DagNode*)si, (DagNode*)d.getNodeRef(), segment.p2());
    DagNodePoint * pi= new DagNodePoint((DagNode*)a.getNodeRef(),(DagNode*)qi, segment.p1());
    return pi;
}

DagNodePoint *Dag::leftDegenerateSingleInsertion(const cg3::Segment2d &segment, Trapezoid& a , Trapezoid& b , Trapezoid& c)
{
    DagNodeSegment* si = new DagNodeSegment((DagNode*)a.getNodeRef(), (DagNode*)b.getNodeRef(), segment);
    DagNodePoint * qi = new DagNodePoint((DagNode*)si, (DagNode*)c.getNodeRef(), segment.p2());
    return qi;
}

DagNodePoint *Dag::rightDegenerateSingleInsertion(const cg3::Segment2d &segment, Trapezoid& a , Trapezoid& b , Trapezoid& c)
{
    DagNodeSegment* si = new DagNodeSegment((DagNode*)b.getNodeRef(), (DagNode*)c.getNodeRef(), segment);
    DagNodePoint * pi = new DagNodePoint((DagNode*)a.getNodeRef(),(DagNode*)si, segment.p1());
    return pi;
}

DagNodeSegment *Dag::totallyDegenerateSingleInsertion(const cg3::Segment2d &segment, Trapezoid& a, Trapezoid&b)
{
    DagNodeSegment* si = new DagNodeSegment((DagNode*)a.getNodeRef(), (DagNode*)b.getNodeRef(), segment);
    return si;
}

//DagNodeArea *Dag::bindWithParentRef(cg3::Segment2d &top, cg3::Segment2d &bottom, cg3::Point2d &leftp, cg3::Point2d &rightp, DagNode*& ref)
//{
//    Trapezoid trap = Trapezoid(top, bottom, leftp, rightp, (DagNodeArea*&)ref);
//    TrapezoidalMap::addTrapezoid(trap);
//    DagNodeArea* first = new DagNodeArea(TrapezoidalMap::getTrapezoids().back());
//    return first;
//}

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
        chosen = (InnerNodes*)tmpdag;
        if(tmpdag->getType()==DagNode::X || ((DagNodeSegment*)chosen)->getSegment().p1()!=seg.p1())
            tmpdag = ((InnerNodes*)tmpdag)->compareNodeToPoint(seg.p1());
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

//void Dag::followSegment(cg3::Segment2d &segment, DagNodeArea *leaf){
//    std::vector<std::list<Trapezoid>::iterator> garbage;
//    Trapezoid * lastDeleted;
//    DagNodeArea * current;
//    DagNode* prevSeg;
//    unsigned long iter = 0;
//    while (segment.p2().x() > leaf->getT().getRightp().x()) {
//        current = leaf;
//        if(gas::matrixDet(segment, leaf->getT().getRightp())<0){
//            leaf = leaf->getT().getRightUp().getNodeRef();

//        }
//        else{
//            leaf = leaf->getT().getRightDown().getNodeRef();
//        }
//        DagNode *& copy = (DagNode*&)current->getT().getNodeRef();
//        if(iter == 0){
//            if(segment.p1()==(*(DagNodeArea*)copy).getT().getLeftp()){
//                copy = createLeftMostDegenerate(segment, *(DagNodeArea*)copy, lastDeleted);
//                prevSeg = copy;
//            }
//            else{
//                copy = createLeftMost(segment, *(DagNodeArea*)copy, lastDeleted);
//                prevSeg = ((DagNodePoint*)copy)->getRightChild();
//            }
//        }
//        else{
//            copy = createIntermediate(segment, *(DagNodeArea*)copy, *(DagNodeSegment*)prevSeg, lastDeleted);
//            prevSeg = copy;
//        }
//       iter ++;
//    }
//    DagNode*& copy = (DagNode*&)leaf->getT().getNodeRef();
//    if(segment.p2()==(*(DagNodeArea*)copy).getT().getRightp()){
//        copy = createRightMostDegenerate(segment, *(DagNodeArea*)copy, *(DagNodeSegment*)prevSeg, lastDeleted);
//    }
//    else
//        copy = createRightMost(segment, *(DagNodeArea*)copy, *(DagNodeSegment*)prevSeg, lastDeleted);
//}

//DagNode* Dag::createLeftMost(cg3::Segment2d &segment, DagNodeArea &leaf, Trapezoid*& lastDeleted){
//    std::list<Trapezoid>& ref = TrapezoidalMap::getTrapezoids();
//    std::list<Trapezoid>::iterator itr;
//    std::vector<Trapezoid*> iterators(3);
//    DagNode * leafA;
//    DagNode * leafB;
//    DagNode * leafC;
//    DagNodePoint* pi;
//    InnerNodes* si;
//    Trapezoid& trap = leaf.getT();

//    std::vector<cg3::Point2d> ints = {gas::intersection(trap.getTop(), segment.p1()),
//                                      gas::intersection(trap.getBottom(), segment.p1()),
//                                      gas::intersection(segment, trap.getTop().p2())};
//    std::vector<std::tuple<cg3::Segment2d, cg3::Segment2d, cg3::Point2d, cg3::Point2d>> traps = {
//        std::make_tuple(cg3::Segment2d(trap.getTop().p1(),ints[0]),cg3::Segment2d(trap.getBottom().p1(),ints[1]), trap.getLeftp() , segment.p1()),
//        std::make_tuple(cg3::Segment2d(ints[0],trap.getTop().p2()),cg3::Segment2d(segment.p1(), gas::intersection(segment,trap.getBottom().p2())),segment.p1(), ((trap.getRightp().y()>ints[2].y())?trap.getRightp():ints[2])),
//        std::make_tuple(cg3::Segment2d(segment.p1(),ints[2]),cg3::Segment2d(ints[1], trap.getBottom().p2()),segment.p1(), ((trap.getRightp().y()>ints[2].y())?ints[2]:trap.getRightp())),

//    };

//    pi = new DagNodePoint(nullptr, nullptr, segment.p1());
//    leafA = bindWithParentRef(std::get<0>(traps[0]), std::get<1>(traps[0]), std::get<2>(traps[0]), std::get<3>(traps[0]), pi->getLeftChild());
//    pi->setLeftChild(leafA);

//    si = new DagNodeSegment(nullptr, nullptr, segment);
//    leafB = bindWithParentRef(std::get<0>(traps[1]), std::get<1>(traps[1]), std::get<2>(traps[1]), std::get<3>(traps[1]), si->getLeftChild());
//    leafC = bindWithParentRef(std::get<0>(traps[2]), std::get<1>(traps[2]), std::get<2>(traps[2]), std::get<3>(traps[2]), si->getRightChild());
//    pi->setRightChild(si);
//    si->setLeftChild(leafB);
//    si->setRightChild(leafC);
//    itr = ref.end();
//    for(int i=2; i>-1; i--)
//        iterators[i] = &*(--itr);
//    TrapezoidalMap::updateNeighborsMultiTrapezoid(trap, iterators, 0, *(DagNodeSegment*)si, lastDeleted);
//    lastDeleted = &trap;
//    TrapezoidalMap::removeTrapezoid(trap.getItr());
//    return pi;
//}

//DagNode *Dag::createLeftMostDegenerate(cg3::Segment2d &segment, DagNodeArea &leaf, Trapezoid *&lastDeleted)
//{
//    bool aboveleftp;
//    bool aboverightp;
//    bool degenerate;
//    bool triangleOver = false;
//    bool linear = false;
//    std::list<Trapezoid>& ref = TrapezoidalMap::getTrapezoids();
//    std::list<Trapezoid>::iterator itr;
//    std::vector<Trapezoid*> iterators(2);
//    DagNode * leafB;
//    DagNode * leafC;
//    InnerNodes* si;
//    Trapezoid& trap = leaf.getT();
//    Trapezoid* leftUp = &trap.getLeftUp();
//    Trapezoid* leftDown = &trap.getLeftDown();
//    Trapezoid* rightUp = &trap.getRightUp();
//    Trapezoid* rightDown = &trap.getRightDown();
//    std::vector<cg3::Point2d> ints = {gas::intersection(trap.getTop(), segment.p1()),
//                                      gas::intersection(trap.getBottom(), segment.p1()),
//                                      gas::intersection(segment, trap.getTop().p2())};
//    (ints[2].y()>trap.getRightp().y())?aboverightp=true:aboverightp=false;
//    std::vector<std::tuple<cg3::Segment2d, cg3::Segment2d, cg3::Point2d, cg3::Point2d>> traps = {
//        std::make_tuple(trap.getTop(),cg3::Segment2d(segment.p1(),ints[2]), segment.p1() , (trap.getRightp().y()<ints[2].y())?ints[2]:trap.getRightp()),
//        std::make_tuple(cg3::Segment2d(segment.p1(),ints[2]),trap.getBottom(),segment.p1(), (trap.getRightp().y()>ints[2].y())?ints[2]:trap.getRightp()),

//    };
//    if(segment.p1()==trap.getTop().p1())
//        triangleOver = true;
//    if(segment.p1()==trap.getBottom().p1())
//        triangleOver = false;
//    if(segment.p1()!=trap.getTop().p1() && segment.p1()!=trap.getBottom().p1())
//        linear = true;
//    si = new DagNodeSegment(nullptr, nullptr, segment);
//    leafB = bindWithParentRef(std::get<0>(traps[0]), std::get<1>(traps[0]), std::get<2>(traps[0]), std::get<3>(traps[0]), si->getLeftChild());
//    leafC = bindWithParentRef(std::get<0>(traps[1]), std::get<1>(traps[1]), std::get<2>(traps[1]), std::get<3>(traps[1]), si->getRightChild());
//    si->setLeftChild(leafB);
//    si->setRightChild(leafC);
//    itr = ref.end();
//    for(int i=1; i>-1; i--)
//        iterators[i] = &*(--itr);

//    if(trap.getTop().p1()!=trap.getBottom().p1()){
//        if(!linear){
//            if(!trap.leftUpNull() && trap.getLeftUp().getRightUp()==trap && !triangleOver)
//                trap.getLeftUp().setRightUp(*iterators[0]);
//            if(!trap.leftUpNull() && trap.getLeftUp().getRightDown()==trap &&!triangleOver)
//                trap.getLeftUp().setRightDown(*iterators[0]);

//            if(!trap.leftDownNull() && trap.getLeftDown().getRightUp()==trap && triangleOver)
//                trap.getLeftDown().setRightUp(*iterators[1]);
//            if(!trap.leftDownNull() && trap.getLeftDown().getRightDown()==trap && triangleOver)
//                trap.getLeftDown().setRightDown(*iterators[1]);
//            if(!triangleOver){
//                iterators[0]->setNeighbors(trap.getLeftUp(),trap.getLeftUp(), trap.getRightUp(), (aboverightp)?trap.getRightUp():trap.getRightDown());// be careful on right side neighbors
//                iterators[1]->setLeftUpNull();
//                iterators[1]->setLeftDownNull();
//                iterators[1]->setRightUp((aboverightp)?trap.getRightUp():trap.getRightDown());
//                iterators[1]->setRightDown(trap.getRightDown());
//            }else{
//                iterators[0]->setLeftUpNull();
//                iterators[0]->setLeftDownNull();
//                iterators[0]->setRightUp(trap.getRightUp());
//                iterators[0]->setRightDown((aboverightp)?trap.getRightUp():trap.getRightDown());
//                iterators[1]->setNeighbors(trap.getLeftDown(),trap.getLeftDown(), (aboverightp)?trap.getRightUp():trap.getRightDown(), trap.getRightDown());//also there
//            }
//        }
//        else{
//            if(!trap.leftUpNull() && trap.getLeftUp().getRightUp()==trap )
//                trap.getLeftUp().setRightUp(*iterators[0]);
//            if(!trap.leftUpNull() && trap.getLeftUp().getRightDown()==trap )
//                trap.getLeftUp().setRightDown(*iterators[0]);
//            if(!trap.leftDownNull() && trap.getLeftDown().getRightUp()==trap )
//                trap.getLeftDown().setRightUp(*iterators[1]);
//            if(!trap.leftDownNull() && trap.getLeftDown().getRightDown()==trap )
//                trap.getLeftDown().setRightDown(*iterators[1]);

//            iterators[0]->setNeighbors(trap.getLeftUp(),trap.getLeftUp(), trap.getRightUp(), (aboverightp)?trap.getRightUp():trap.getRightDown());// be careful on right side neighbors
//            iterators[1]->setNeighbors(trap.getLeftDown(),trap.getLeftDown(), (aboverightp)?trap.getRightUp():trap.getRightDown(), trap.getRightDown());//also there
//        }
//    }
//    else{
//        iterators[0]->setRightUp(trap.getRightUp());
//        iterators[0]->setRightDown((aboverightp)?trap.getRightUp():trap.getRightDown());
//        iterators[1]->setRightUp((aboverightp)?trap.getRightUp():trap.getRightDown());
//        iterators[1]->setRightDown(trap.getRightDown());
//        iterators[0]->setLeftUpNull();
//        iterators[0]->setLeftDownNull();
//        iterators[1]->setLeftUpNull();
//        iterators[1]->setLeftDownNull();
//    }
//    if(aboverightp){
//        if(rightDown->getLeftUp()==trap)
//            rightDown->setLeftUp(*iterators[1]);
//        if(rightDown->getLeftDown()==trap)
//            rightDown->setLeftDown(*iterators[1]);
//        if(rightUp->getLeftUp()==trap)
//            rightUp->setLeftUp(*iterators[0]);
//        if(rightUp->getLeftDown()==trap)
//            rightUp->setLeftDown(*iterators[1]);
//    }
//    else{
//        if(rightDown->getLeftUp()==trap)
//            rightDown->setLeftUp(*iterators[0]);
//        if(rightDown->getLeftDown()==trap)
//            rightDown->setLeftDown(*iterators[1]);
//        if(rightUp->getLeftUp()==trap)
//            rightUp->setLeftUp(*iterators[0]);
//        if(rightUp->getLeftDown()==trap)
//            rightUp->setLeftDown(*iterators[0]);
//    }

//    //from new to old neighbors

//    lastDeleted = &trap;
//    TrapezoidalMap::removeTrapezoid(trap.getItr());
//    return si;
//}

//DagNode *Dag::createRightMost(cg3::Segment2d &segment, DagNodeArea &leaf,  DagNodeSegment& prevSeg, Trapezoid*& lastDeleted)
//{
//    std::list<Trapezoid>& ref = TrapezoidalMap::getTrapezoids();
//    std::list<Trapezoid>::iterator itr;
//    bool upper;
//    DagNode * leafA;
//    DagNode * leafB;
//    DagNode * leafC;
//    std::vector<Trapezoid*> iterators(3);
//    DagNodePoint* qi;// = new DagNodePoint(segment.p2());
//    InnerNodes* si;// = new DagNodeSegment(segment);
//    Trapezoid& trap = leaf.getT();

//    std::vector<cg3::Point2d> ints = {gas::intersection(trap.getTop(), segment.p2()),
//                                      gas::intersection(trap.getBottom(), segment.p2()),
//                                      gas::intersection(segment, trap.getTop().p1())};

//    std::vector<std::tuple<cg3::Segment2d, cg3::Segment2d, cg3::Point2d, cg3::Point2d>> traps = {
//        std::make_tuple(cg3::Segment2d(trap.getTop().p1(),ints[0]),cg3::Segment2d(ints[2], segment.p2()),(trap.getLeftp().y()<ints[2].y())?ints[2]:trap.getLeftp() , segment.p2()),
//        std::make_tuple(cg3::Segment2d(ints[2],segment.p2()),cg3::Segment2d(trap.getBottom().p1(), ints[1]),(trap.getLeftp().y()>ints[2].y())?ints[2]:trap.getLeftp(), segment.p2()),
//        std::make_tuple(cg3::Segment2d(ints[0],trap.getTop().p2()),cg3::Segment2d(ints[1], trap.getBottom().p2()),segment.p2(), trap.getRightp()),
//    };
//    qi = new DagNodePoint(nullptr, nullptr, segment.p2());
//    leafC = bindWithParentRef(std::get<0>(traps[2]), std::get<1>(traps[2]), std::get<2>(traps[2]), std::get<3>(traps[2]), qi->getRightChild());
//    qi->setRightChild(leafC);
//    itr = ref.end();
//    iterators[2] = &*(--itr);
//    itr = ref.end();
//    if(ints[2].y()>trap.getLeftp().y()){
//        si= new DagNodeSegment((TrapezoidalMap::merge(((DagNodeArea*)prevSeg.getLeftChild())->getT(), traps[0])),nullptr, segment);
//        leafB = bindWithParentRef(std::get<0>(traps[1]), std::get<1>(traps[1]), std::get<2>(traps[1]), std::get<3>(traps[1]), si->getRightChild());
//        si->setRightChild(leafB);
//        upper = true;
//        iterators[1] = &*(--itr);
//        iterators[0] = &((DagNodeArea*)si->getLeftChild())->getT();
//    }
//    else{
//        si = new DagNodeSegment(nullptr, TrapezoidalMap::merge(((DagNodeArea*)prevSeg.getRightChild())->getT(),traps[1]), segment);
//        leafA = bindWithParentRef(std::get<0>(traps[0]), std::get<1>(traps[0]), std::get<2>(traps[0]), std::get<3>(traps[0]), si->getLeftChild());
//        si->setLeftChild(leafA);
//        upper = false;
//        iterators[0] = &*(--itr);
//        iterators[1] = &((DagNodeArea*)si->getRightChild())->getT();

//    }
//    qi->setLeftChild(si);
//    TrapezoidalMap::updateNeighborsMultiTrapezoid(trap, iterators, 2, prevSeg, lastDeleted);
//    if(upper){
//        iterators[0]->setNeighbors(iterators[0]->getLeftUp(),iterators[0]->getLeftDown(),*iterators[2], *iterators[2]);//
//        iterators[1]->setNeighbors((trap.getLeftUp().getRightp().x()<iterators[1]->getRightp().x())?trap.getLeftUp():((DagNodeArea*)prevSeg.getRightChild())->getT(),(trap.getLeftDown()==*lastDeleted)?((DagNodeArea*)prevSeg.getRightChild())->getT():trap.getLeftDown(),*iterators[2], *iterators[2]);
//    }
//    else{
//        iterators[0]->setNeighbors((trap.getLeftUp()==*lastDeleted)?((DagNodeArea*)prevSeg.getLeftChild())->getT():trap.getLeftUp(),(trap.getLeftDown().getRightp().x()<iterators[0]->getRightp().x())?trap.getLeftDown():((DagNodeArea*)prevSeg.getLeftChild())->getT(),*iterators[2], *iterators[2]);//
//        iterators[1]->setNeighbors(iterators[1]->getLeftUp(),iterators[1]->getLeftDown(),*iterators[2], *iterators[2]);
//    }
//    iterators[2]->setNeighbors(*iterators[0],*iterators[1], trap.getRightUp(), trap.getRightDown());//
//    lastDeleted = &trap;
//    TrapezoidalMap::removeTrapezoid(trap.getItr());
//    return qi;
//}

//DagNode *Dag::createRightMostDegenerate(cg3::Segment2d &segment, DagNodeArea &leaf, DagNodeSegment& prevSeg, Trapezoid *&lastDeleted)
//{
//    std::list<Trapezoid>& ref = TrapezoidalMap::getTrapezoids();
//    std::list<Trapezoid>::iterator itr;
//    DagNode * leafA;
//    DagNode * leafB;
//    bool upper;
//    bool triangleOver = false;
//    bool linear = false;
//    std::vector<Trapezoid*> iterators(2);
//    InnerNodes* si;
//    Trapezoid& trap = leaf.getT();
//    std::vector<cg3::Point2d> ints = {gas::intersection(segment, trap.getTop().p1()), gas::intersection(segment, trap.getTop().p2())};
//    std::vector<std::tuple<cg3::Segment2d, cg3::Segment2d, cg3::Point2d, cg3::Point2d>> traps = {
//        std::make_tuple(cg3::Segment2d(trap.getTop().p1(),trap.getTop().p2()),cg3::Segment2d(ints[0], ints[1]),(trap.getLeftp().y()<ints[0].y())?ints[0]:trap.getLeftp(), segment.p2()),
//        std::make_tuple(cg3::Segment2d(ints[0], ints[1]),cg3::Segment2d(trap.getBottom().p1(), trap.getBottom().p2()),(trap.getLeftp().y()>ints[0].y())?ints[0]:trap.getLeftp(), segment.p2()),
//    };
//    if(segment.p2()==trap.getTop().p2())
//        triangleOver = true;
//    if(segment.p2()==trap.getBottom().p2())
//        triangleOver = false;
//    if(segment.p2()!=trap.getTop().p2() && segment.p2()!=trap.getBottom().p2())
//        linear = true;
//    itr = ref.end();
//    if(ints[0].y()>trap.getLeftp().y()){
//        si= new DagNodeSegment((TrapezoidalMap::merge(((DagNodeArea*)prevSeg.getLeftChild())->getT(), traps[0])),nullptr, segment);
//        leafB = bindWithParentRef(std::get<0>(traps[1]), std::get<1>(traps[1]), std::get<2>(traps[1]), std::get<3>(traps[1]), si->getRightChild());
//        si->setRightChild(leafB);
//        upper = true;
//        iterators[1] = &*(--itr);
//        iterators[0] = &((DagNodeArea*)si->getLeftChild())->getT();
//        if(((DagNodeArea*)prevSeg.getRightChild())->getT().getRightUp()==trap)
//            ((DagNodeArea*)prevSeg.getRightChild())->getT().setRightUp(*iterators[1]);
//        if(((DagNodeArea*)prevSeg.getRightChild())->getT().getRightDown()==trap)
//            ((DagNodeArea*)prevSeg.getRightChild())->getT().setRightDown(*iterators[1]);
//        if(trap.getLeftDown()!= *lastDeleted && trap.getLeftDown().getRightUp()==trap)
//            trap.getLeftDown().setRightUp(*iterators[1]);
//        if(trap.getLeftDown()!= *lastDeleted && trap.getLeftDown().getRightDown()==trap)
//            trap.getLeftDown().setRightDown(*iterators[1]);
//    }
//    else{
//        si = new DagNodeSegment(nullptr, TrapezoidalMap::merge(((DagNodeArea*)prevSeg.getRightChild())->getT(),traps[1]), segment);
//        leafA = bindWithParentRef(std::get<0>(traps[0]), std::get<1>(traps[0]), std::get<2>(traps[0]), std::get<3>(traps[0]), si->getLeftChild());
//        si->setLeftChild(leafA);
//        upper = false;
//        iterators[0] = &*(--itr);
//        iterators[1] = &((DagNodeArea*)si->getRightChild())->getT();
//        if(((DagNodeArea*)prevSeg.getLeftChild())->getT().getRightUp()==trap)
//            ((DagNodeArea*)prevSeg.getLeftChild())->getT().setRightUp(*iterators[0]);
//        if(((DagNodeArea*)prevSeg.getLeftChild())->getT().getRightDown()==trap)
//            ((DagNodeArea*)prevSeg.getLeftChild())->getT().setRightDown(*iterators[0]);
//        if(trap.getLeftUp()!= *lastDeleted &&  trap.getLeftUp().getRightUp()==trap)
//            trap.getLeftUp().setRightUp(*iterators[0]);
//        if(trap.getLeftUp()!= *lastDeleted && trap.getLeftUp().getRightDown()==trap)
//            trap.getLeftUp().setRightDown(*iterators[0]);
//    }

//    if(upper){
//        //iterators[0]->setRightUp(trap.getRightUp());
//        //iterators[0]->setRightDown((iterators[0]->getBottom().p2().y()<trap.getRightp().y())?trap.getRightDown():trap.getRightUp());
//        iterators[1]->setLeftUp((trap.getLeftUp().getRightp().x()<iterators[1]->getRightp().x())?trap.getLeftUp():((DagNodeArea*)prevSeg.getRightChild())->getT());
//        iterators[1]->setLeftDown(trap.getLeftDown());

//        /*iterators[1]->setNeighbors((trap.getLeftUp().getRightp().x()<iterators[1]->getRightp().x())?trap.getLeftUp():((DagNodeArea*)prevSeg.getRightChild())->getT(), trap.getLeftDown(),
//                                   (iterators[0]->getBottom().p2().y()<trap.getRightp().y())?trap.getRightDown():trap.getRightUp(),
//                                    trap.getRightDown());//*/
//    }
//    else{
//        iterators[0]->setLeftUp(trap.getLeftUp());
//        iterators[0]->setLeftDown((trap.getLeftDown().getRightp().x()<iterators[0]->getRightp().x())?trap.getLeftDown():((DagNodeArea*)prevSeg.getLeftChild())->getT());
//        /*iterators[0]->setNeighbors(trap.getLeftUp(),(trap.getLeftDown().getRightp().x()<iterators[0]->getRightp().x())?trap.getLeftDown():((DagNodeArea*)prevSeg.getLeftChild())->getT(),trap.getRightUp(),
//                                   (iterators[1]->getTop().p2().y()>trap.getRightp().y())?trap.getRightUp():trap.getRightDown());*/
//        //iterators[1]->setRightUp((iterators[1]->getTop().p2().y()>trap.getRightp().y())?trap.getRightUp():trap.getRightDown());
//        //iterators[1]->setRightDown(trap.getRightDown());
//    }
//    if(trap.getTop().p2()!=trap.getBottom().p2()){
//        if(!linear){
//            if(!trap.rightUpNull() && trap.getRightUp().getLeftUp()==trap && !triangleOver)
//                trap.getRightUp().setLeftUp(*iterators[0]);
//            if(!trap.rightUpNull() && trap.getRightUp().getLeftDown()==trap && !triangleOver)
//                trap.getRightUp().setLeftDown(*iterators[0]);
//            if(!trap.rightDownNull() && trap.getRightDown().getLeftDown()==trap && triangleOver)
//                trap.getRightDown().setLeftDown(*iterators[1]);
//            if(!trap.rightDownNull() && trap.getRightDown().getLeftUp()==trap && triangleOver)
//                trap.getRightDown().setLeftUp(*iterators[1]);
//            if(!triangleOver){
//                iterators[0]->setRightUp(trap.getRightUp());
//                iterators[0]->setRightDown(trap.getRightUp());
//                iterators[1]->setRightUpNull();
//                iterators[1]->setRightDownNull();
////                iterators[0]->setNeighbors(trap.getLeftUp(),trap.getLeftUp(), trap.getRightUp(), trap.getRightUp());// be careful on right side neighbors
////                iterators[1]->setRightUp(trap.getRightDown());
////                iterators[1]->setRightDown(trap.getRightDown());
//            }else{
//                iterators[0]->setRightUpNull();
//                iterators[0]->setRightDownNull();
//                iterators[1]->setRightUp(trap.getRightDown());
//                iterators[1]->setRightDown(trap.getRightDown());
////                iterators[0]->setRightUp(trap.getRightUp());
////                iterators[0]->setRightDown(trap.getRightUp());
////                iterators[1]->setNeighbors(trap.getLeftDown(),trap.getLeftDown(), trap.getRightDown(),trap.getRightDown());//also there
//            }
//        }else{
//            if(!trap.rightUpNull() && trap.getRightUp().getLeftUp()==trap )
//                trap.getRightUp().setLeftUp(*iterators[0]);
//            if(!trap.rightUpNull() && trap.getRightUp().getLeftDown()==trap)
//                trap.getRightUp().setLeftDown(*iterators[0]);

//            if(!trap.rightDownNull() && trap.getRightDown().getLeftDown()==trap )
//                trap.getRightDown().setLeftDown(*iterators[1]);
//            if(!trap.rightDownNull() && trap.getRightDown().getLeftUp()==trap )
//                trap.getRightDown().setLeftUp(*iterators[1]);

//            iterators[0]->setRightUp(trap.getRightUp());
//            iterators[0]->setRightDown(trap.getRightUp());
//            iterators[1]->setRightUp(trap.getRightDown());
//            iterators[1]->setRightDown(trap.getRightDown());

////            iterators[0]->setNeighbors(trap.getLeftUp(),trap.getLeftUp(),trap.getRightUp(), trap.getRightUp()); //A
////            iterators[1]->setNeighbors(trap.getLeftDown(),trap.getLeftDown(),trap.getRightDown(), trap.getRightDown()); //B
//        }
//    }else{
//        iterators[0]->setRightUpNull();
//        iterators[0]->setRightDownNull();
//        iterators[1]->setRightUpNull();
//        iterators[1]->setRightDownNull();

//    }


//    lastDeleted = &trap;

//    TrapezoidalMap::removeTrapezoid(trap.getItr());
//    return si;
//}

//DagNode *Dag::createIntermediate(cg3::Segment2d &segment, DagNodeArea &leaf, DagNodeSegment& prevSeg, Trapezoid*& lastDeleted)
//{
//    std::list<Trapezoid>& ref = TrapezoidalMap::getTrapezoids();
//    std::list<Trapezoid>::iterator itr;
//    DagNode* ex;
//    DagNode * leafA;
//    DagNode * leafB;
//    bool upper;
//    std::vector<Trapezoid*> iterators(2);
//    InnerNodes* si;
//    Trapezoid& trap = leaf.getT();
//    std::vector<cg3::Point2d> ints = {gas::intersection(segment, trap.getTop().p1()), gas::intersection(segment, trap.getTop().p2())};
//    std::vector<std::tuple<cg3::Segment2d, cg3::Segment2d, cg3::Point2d, cg3::Point2d>> traps = {
//        std::make_tuple(cg3::Segment2d(trap.getTop().p1(),trap.getTop().p2()),cg3::Segment2d(ints[0], ints[1]),(trap.getLeftp().y()<ints[0].y())?ints[0]:trap.getLeftp(), (trap.getRightp().y()<ints[1].y())?ints[1]:trap.getRightp()),
//        std::make_tuple(cg3::Segment2d(ints[0], ints[1]),cg3::Segment2d(trap.getBottom().p1(), trap.getBottom().p2()),(trap.getLeftp().y()>ints[0].y())?ints[0]:trap.getLeftp(), (trap.getRightp().y()>ints[1].y())?ints[1]:trap.getRightp()),
//    };
//    itr = ref.end();
//    if(ints[0].y()>trap.getLeftp().y()){
//        si= new DagNodeSegment((TrapezoidalMap::merge(((DagNodeArea*)prevSeg.getLeftChild())->getT(), traps[0])),nullptr, segment);
//        leafB = bindWithParentRef(std::get<0>(traps[1]), std::get<1>(traps[1]), std::get<2>(traps[1]), std::get<3>(traps[1]), si->getRightChild());
//        si->setRightChild(leafB);
//        upper = true;
//        iterators[1] = &*(--itr);
//        iterators[0] = &((DagNodeArea*)si->getLeftChild())->getT();
//    }
//    else{
//        si = new DagNodeSegment(nullptr, TrapezoidalMap::merge(((DagNodeArea*)prevSeg.getRightChild())->getT(),traps[1]), segment);
//        leafA = bindWithParentRef(std::get<0>(traps[0]), std::get<1>(traps[0]), std::get<2>(traps[0]), std::get<3>(traps[0]), si->getLeftChild());
//        si->setLeftChild(leafA);
//        upper = false;
//        iterators[0] = &*(--itr);
//        iterators[1] = &((DagNodeArea*)si->getRightChild())->getT();
//    }
//    TrapezoidalMap::updateNeighborsMultiTrapezoid(trap, iterators, 1, prevSeg, lastDeleted);
//    if(upper){
//        iterators[0]->setRightUp(trap.getRightUp());
//        iterators[0]->setRightDown((iterators[0]->getBottom().p2().y()<trap.getRightp().y())?trap.getRightDown():trap.getRightUp());
//        iterators[1]->setNeighbors((trap.getLeftUp().getRightp().x()<iterators[1]->getRightp().x())?trap.getLeftUp():((DagNodeArea*)prevSeg.getRightChild())->getT(), trap.getLeftDown(),
//                                   (iterators[0]->getBottom().p2().y()<trap.getRightp().y())?trap.getRightDown():trap.getRightUp(),
//                                    trap.getRightDown());//

//    }
//    else{
//        iterators[0]->setNeighbors(trap.getLeftUp(), (trap.getLeftDown().getRightp().x()<iterators[0]->getRightp().x())?trap.getLeftDown():((DagNodeArea*)prevSeg.getLeftChild())->getT(),trap.getRightUp(),
//                                   (iterators[1]->getTop().p2().y()>trap.getRightp().y())?trap.getRightUp():trap.getRightDown());
//        iterators[1]->setRightUp((iterators[1]->getTop().p2().y()>trap.getRightp().y())?trap.getRightUp():trap.getRightDown());
//        iterators[1]->setRightDown(trap.getRightDown());
//    }

//    lastDeleted = &trap;
//    TrapezoidalMap::removeTrapezoid(trap.getItr());
//    return si;
//}









