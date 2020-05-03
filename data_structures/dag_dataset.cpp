#include "dag_dataset.h"


Dag::Dag(){  
    //tempmap = {};
}
void Dag::addSegment(cg3::Segment2d& segment){ //O (k log k)
    bool leftDegenerate = false;
    bool rightDegenerate = false;
    if(Dag::dag->getType()!=DagNode::Leaf){
        DagNode*& ins = searchAndAppend(segment);
        if(segment.p2().x()>((DagNodeArea*)ins)->getT().getRightp().x())
            followSegment(segment, (DagNodeArea*)ins);
        else{
            if(segment.p1() == ((DagNodeArea*)ins)->getT().getLeftp())
                leftDegenerate=true;
            if(segment.p2() == ((DagNodeArea*)ins)->getT().getRightp())
                rightDegenerate=true;
            if(leftDegenerate && rightDegenerate)
                ins = totallyDegenerateSingleInsertion(ins, segment);
            else if(leftDegenerate && !rightDegenerate)
                ins = leftDegenerateSingleInsertion(ins, segment);
            else if(!leftDegenerate && rightDegenerate)
                ins = rightDegenerateSingleInsertion(ins, segment);
            else
                ins = addSegmentInSingleTrap(ins, segment);
        }
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

DagNodePoint *Dag::leftDegenerateSingleInsertion(DagNode *node, cg3::Segment2d &segment)
{
    std::list<Trapezoid>& ref = TrapezoidalMap::getTrapezoids();
    std::list<Trapezoid>::iterator itr;
    std::vector<Trapezoid*> iterators(3);
    bool triangleOver;
    bool linear = false;
    DagNode * leafA;
    DagNode * leafB;
    DagNode * leafC;
    const Trapezoid& trap = {((DagNodeArea*)node)->getT()};
    std::vector<cg3::Point2d> ints = {intersection(trap.getTop(), segment.p2()),intersection(trap.getBottom(), segment.p2())};

    std::vector<std::tuple<cg3::Segment2d, cg3::Segment2d, cg3::Point2d, cg3::Point2d>> traps = {
        std::make_tuple(cg3::Segment2d(trap.getTop().p1(),ints[0]), cg3::Segment2d(segment.p1(), segment.p2()), segment.p1(), segment.p2()),
        std::make_tuple(cg3::Segment2d(segment.p1(),segment.p2()), cg3::Segment2d(trap.getBottom().p1(), ints[1]), segment.p1(), segment.p2()),
        std::make_tuple(cg3::Segment2d(ints[0],trap.getTop().p2()),cg3::Segment2d(ints[1], trap.getBottom().p2()),segment.p2(), trap.getRightp()),

    };
    if(segment.p1()==trap.getTop().p1())
        triangleOver = true;
    if(segment.p1()==trap.getBottom().p1())
        triangleOver = false;
    if(segment.p1()!=trap.getTop().p1() && segment.p1()!=trap.getBottom().p1())
        linear = true;
    DagNodePoint * qi = new DagNodePoint(nullptr, nullptr, segment.p2());
    leafC = bindWithParentRef(std::get<0>(traps[2]), std::get<1>(traps[2]), std::get<2>(traps[2]), std::get<3>(traps[2]), qi->getRightChild());
    qi->setRightChild(leafC);
    DagNodeSegment* si = new DagNodeSegment(nullptr, nullptr, segment);
    leafA = bindWithParentRef(std::get<0>(traps[0]), std::get<1>(traps[0]), std::get<2>(traps[0]), std::get<3>(traps[0]), si->getLeftChild());
    leafB = bindWithParentRef(std::get<0>(traps[1]), std::get<1>(traps[1]), std::get<2>(traps[1]), std::get<3>(traps[1]), si->getRightChild());
    qi->setLeftChild(si);
    si->setLeftChild(leafA);
    si->setRightChild(leafB);

    itr = ref.end();
    iterators[1] = &*(--itr);
    iterators[0] = &*(--itr);
    iterators[2] = &*(--itr);

    iterators[0]->setNeighbors(trap.getLeftUp(),trap.getLeftUp(),*iterators[2], *iterators[2]); //A
    iterators[1]->setNeighbors(trap.getLeftDown(),trap.getLeftDown(),*iterators[2], *iterators[2]); //B
    iterators[2]->setNeighbors(*iterators[0], *iterators[1],trap.getRightUp(), trap.getRightDown());


    if(!trap.rightUpNull() && trap.getRightUp().getLeftUp()==trap)
        trap.getRightUp().setLeftUp(*iterators[2]);
    if(!trap.rightUpNull() && trap.getRightUp().getLeftDown()==trap)
        trap.getRightUp().setLeftDown(*iterators[2]);
    if(!trap.rightDownNull() && trap.getRightDown().getLeftUp()==trap)
        trap.getRightDown().setLeftUp(*iterators[2]);
    if(!trap.rightDownNull() && trap.getRightDown().getLeftDown()==trap)
        trap.getRightDown().setLeftDown(*iterators[2]);
    if(!linear){
        if(!trap.leftUpNull() && trap.getLeftUp().getRightUp()==trap && !triangleOver)
            trap.getLeftUp().setRightUp(*iterators[0]);
        if(!trap.leftUpNull() && !triangleOver)
            trap.getLeftUp().setRightDown(*iterators[0]);

        if(!trap.leftDownNull() && trap.getLeftDown().getRightDown()==trap && triangleOver)
            trap.getLeftDown().setRightDown(*iterators[1]);
        if(!trap.leftDownNull() && triangleOver)
            trap.getLeftDown().setRightUp(*iterators[1]);
    }else{
        if(!trap.leftUpNull() && trap.getLeftUp().getRightUp()==trap )
            trap.getLeftUp().setRightUp(*iterators[0]);
        if(!trap.leftUpNull())
            trap.getLeftUp().setRightDown(*iterators[0]);

        if(!trap.leftDownNull() && trap.getLeftDown().getRightDown()==trap )
            trap.getLeftDown().setRightDown(*iterators[1]);
        if(!trap.leftDownNull())
            trap.getLeftDown().setRightUp(*iterators[1]);
    }

    TrapezoidalMap::removeTrapezoid(trap.getItr());

    return qi;
}

DagNodePoint *Dag::rightDegenerateSingleInsertion(DagNode *node, cg3::Segment2d &segment)
{
    std::list<Trapezoid>& ref = TrapezoidalMap::getTrapezoids();
    std::list<Trapezoid>::iterator itr;
    std::vector<Trapezoid*> iterators(3);
    DagNode * leafA;
    DagNode * leafB;
    DagNode * leafC;
    const Trapezoid& trap = {((DagNodeArea*)node)->getT()};
    std::vector<cg3::Point2d> ints = {intersection(trap.getTop(), segment.p1()),intersection(trap.getBottom(), segment.p1())};

    std::vector<std::tuple<cg3::Segment2d, cg3::Segment2d, cg3::Point2d, cg3::Point2d>> traps = {
        std::make_tuple(cg3::Segment2d(trap.getTop().p1(),ints[0]), cg3::Segment2d(trap.getBottom().p1(), ints[1]), trap.getLeftp(), segment.p1()),
        std::make_tuple(cg3::Segment2d(ints[0],trap.getTop().p2()),cg3::Segment2d(segment.p1(),segment.p2()),segment.p1(), segment.p2()),
        std::make_tuple(cg3::Segment2d(segment.p1(),segment.p2()), cg3::Segment2d(ints[1],trap.getBottom().p2()), segment.p1(), segment.p2())
    };
    DagNodePoint * pi = new DagNodePoint(nullptr, nullptr, segment.p1());
    leafA = bindWithParentRef(std::get<0>(traps[0]), std::get<1>(traps[0]), std::get<2>(traps[0]), std::get<3>(traps[0]), pi->getLeftChild());
    pi->setLeftChild(leafA);
    DagNodeSegment* si = new DagNodeSegment(nullptr, nullptr, segment);
    leafB = bindWithParentRef(std::get<0>(traps[1]), std::get<1>(traps[1]), std::get<2>(traps[1]), std::get<3>(traps[1]), si->getLeftChild());
    leafC = bindWithParentRef(std::get<0>(traps[2]), std::get<1>(traps[2]), std::get<2>(traps[2]), std::get<3>(traps[2]), si->getRightChild());
    pi->setRightChild(si);
    si->setLeftChild(leafB);
    si->setRightChild(leafC);
    itr = ref.end();
    iterators[2] = &*(--itr);
    iterators[1] = &*(--itr);
    iterators[0] = &*(--itr);

    iterators[0]->setNeighbors(trap.getLeftUp(),trap.getLeftDown(),*iterators[1], *iterators[2]); //A
    iterators[1]->setNeighbors(*iterators[0], *iterators[0],trap.getRightUp(), trap.getRightUp()); //B
    iterators[2]->setNeighbors(*iterators[0], *iterators[0],trap.getRightDown(), trap.getRightDown());

    if(!trap.leftUpNull() && trap.getLeftUp().getRightUp()==trap)
        trap.getLeftUp().setRightUp(*iterators[0]);
    if(!trap.leftUpNull() && trap.getLeftUp().getRightDown()==trap)
        trap.getLeftUp().setRightDown(*iterators[0]);
    if(!trap.leftDownNull() && trap.getLeftDown().getRightUp()==trap)
        trap.getLeftDown().setRightUp(*iterators[0]);
    if(!trap.leftDownNull() && trap.getLeftDown().getRightDown()==trap)
        trap.getLeftDown().setRightDown(*iterators[0]);

    if(!trap.rightUpNull() && trap.getRightUp().getLeftUp()==trap)
        trap.getRightUp().setLeftUp(*iterators[1]);
    if(!trap.rightUpNull())
        trap.getRightUp().setLeftDown(*iterators[1]);
    if(!trap.rightDownNull())
        trap.getRightDown().setLeftUp(*iterators[2]);
    if(!trap.rightDownNull() && trap.getRightDown().getLeftDown()==trap)
        trap.getRightDown().setLeftDown(*iterators[2]);
    TrapezoidalMap::removeTrapezoid(trap.getItr());

    return pi;
}

DagNodeSegment *Dag::totallyDegenerateSingleInsertion(DagNode *node, cg3::Segment2d &segment)
{
    std::list<Trapezoid>& ref = TrapezoidalMap::getTrapezoids();
    std::list<Trapezoid>::iterator itr;
    std::vector<Trapezoid*> iterators(2);
    DagNode * leafA;
    DagNode * leafB;
    const Trapezoid& trap = {((DagNodeArea*)node)->getT()};
    std::vector<std::tuple<cg3::Segment2d, cg3::Segment2d, cg3::Point2d, cg3::Point2d>> traps = {
        std::make_tuple(cg3::Segment2d(trap.getTop().p1(),trap.getTop().p2()),cg3::Segment2d(segment.p1(),segment.p2()),segment.p1(), segment.p2()),
        std::make_tuple(cg3::Segment2d(segment.p1(),segment.p2()), cg3::Segment2d(trap.getBottom().p1(),trap.getBottom().p2()), segment.p1(), segment.p2())
    };
    DagNodeSegment* si = new DagNodeSegment(nullptr, nullptr, segment);
    leafA = bindWithParentRef(std::get<0>(traps[0]), std::get<1>(traps[0]), std::get<2>(traps[0]), std::get<3>(traps[0]), si->getLeftChild());
    leafB = bindWithParentRef(std::get<0>(traps[1]), std::get<1>(traps[1]), std::get<2>(traps[1]), std::get<3>(traps[1]), si->getRightChild());
    itr = ref.end();
    iterators[0] = &*(--itr);
    iterators[1] = &*(--itr);
    iterators[0]->setNeighbors(trap.getLeftUp(),trap.getLeftUp(),trap.getRightUp(), trap.getRightUp()); //A
    iterators[1]->setNeighbors(trap.getLeftDown(),trap.getLeftDown(),trap.getRightDown(), trap.getRightDown()); //B
    if(!trap.leftUpNull() && trap.getLeftUp().getRightUp()==trap)
        trap.getLeftUp().setRightUp(*iterators[0]);
    if(!trap.leftUpNull())
        trap.getLeftUp().setRightDown(*iterators[0]);
    if(trap.getRightUp().getLeftUp()==trap)
        trap.getRightUp().setLeftUp(*iterators[0]);
    if(!trap.rightUpNull())
        trap.getRightUp().setLeftDown(*iterators[0]);
    if(!trap.leftDownNull())
        trap.getLeftDown().setRightUp(*iterators[1]);
    if(trap.getLeftDown().getRightDown()==trap)
        trap.getLeftDown().setRightDown(*iterators[1]);
    if(!trap.rightDownNull())
        trap.getRightDown().setLeftUp(*iterators[1]);
    if(trap.getRightDown().getLeftDown()==trap)
        trap.getRightDown().setLeftDown(*iterators[1]);
    TrapezoidalMap::removeTrapezoid(trap.getItr());
    return si;
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

DagNode*& Dag::searchAndAppend(const cg3::Segment2d &seg)
{
    DagNode* tmpdag=Dag::dag;
    InnerNodes* chosen=(InnerNodes*)Dag::dag;
    while(tmpdag->getType()!=DagNode::Leaf){
        chosen = (InnerNodes*)tmpdag;
        if(tmpdag->getType()==DagNode::X || ((DagNodeSegment*)chosen)->getSegment().p1()!=seg.p1())
            tmpdag = ((InnerNodes*)tmpdag)->compareNodeToPoint(seg.p1());
        else
            tmpdag = (matrixDet(seg, ((DagNodeSegment*)chosen)->getSegment().p2())<0)?chosen->getLeftChild():chosen->getRightChild();
    }
//    if(chosen->getLeftChild()->getType()==DagNode::Leaf &&
//       chosen->getRightChild()->getType()==DagNode::Leaf &&
//       ((DagNodeArea*)chosen->getLeftChild())->getT().getLeftp()== seg.p1() &&
//       ((DagNodeArea*)chosen->getRightChild())->getT().getLeftp()== seg.p1()){
//       return (matrixDet(seg,((DagNodeArea*)chosen->getLeftChild())->getT().getBottom().p2()) < 0)?chosen->getLeftChild():chosen->getRightChild();
//    }
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
    Trapezoid * lastDeleted;
    DagNodeArea * current;
    DagNode* prevSeg;
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
            if(segment.p1()==(*(DagNodeArea*)copy).getT().getLeftp()){
                copy = createLeftMostDegenerate(segment, *(DagNodeArea*)copy, lastDeleted);
                prevSeg = copy;
            }
            else{
                copy = createLeftMost(segment, *(DagNodeArea*)copy, lastDeleted);
                prevSeg = ((DagNodePoint*)copy)->getRightChild();
            }
        }
        else{
            copy = createIntermediate(segment, *(DagNodeArea*)copy, *(DagNodeSegment*)prevSeg, lastDeleted);
            prevSeg = copy;
        }
       iter ++;
    }
    DagNode*& copy = (DagNode*&)leaf->getT().getNodeRef();
    if(segment.p2()==(*(DagNodeArea*)copy).getT().getRightp()){
        printf("eccolo!");
        copy = createRightMostDegenerate(segment, *(DagNodeArea*)copy, *(DagNodeSegment*)prevSeg, lastDeleted);
    }
    else
        copy = createRightMost(segment, *(DagNodeArea*)copy, *(DagNodeSegment*)prevSeg, lastDeleted);
}

DagNode* Dag::createLeftMost(cg3::Segment2d &segment, DagNodeArea &leaf, Trapezoid*& lastDeleted){
    std::list<Trapezoid>& ref = TrapezoidalMap::getTrapezoids();
    std::list<Trapezoid>::iterator itr;
    std::vector<Trapezoid*> iterators(3);
    DagNode * leafA;
    DagNode * leafB;
    DagNode * leafC;
    DagNodePoint* pi;// = new DagNodePoint(nullptr, nullptr, segment.p1());
    InnerNodes* si;// = new DagNodeSegment(segment);
    Trapezoid& trap = leaf.getT();

    std::vector<cg3::Point2d> ints = {intersection(trap.getTop(), segment.p1()),
                                      intersection(trap.getBottom(), segment.p1()),
                                      intersection(segment, trap.getTop().p2())};
    std::vector<std::tuple<cg3::Segment2d, cg3::Segment2d, cg3::Point2d, cg3::Point2d>> traps = {
        std::make_tuple(cg3::Segment2d(trap.getTop().p1(),ints[0]),cg3::Segment2d(trap.getBottom().p1(),ints[1]), trap.getLeftp() , segment.p1()),
        std::make_tuple(cg3::Segment2d(ints[0],trap.getTop().p2()),cg3::Segment2d(segment.p1(), intersection(segment,trap.getBottom().p2())),segment.p1(), ((trap.getRightp().y()>ints[2].y())?trap.getRightp():ints[2])),
        std::make_tuple(cg3::Segment2d(segment.p1(),ints[2]),cg3::Segment2d(ints[1], trap.getBottom().p2()),segment.p1(), ((trap.getRightp().y()>ints[2].y())?ints[2]:trap.getRightp())),

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
    itr = ref.end();
    for(int i=2; i>-1; i--)
        iterators[i] = &*(--itr);
    //TODO
    TrapezoidalMap::updateNeighborsMultiTrapezoid(trap, iterators, 0, *(DagNodeSegment*)si, lastDeleted);
    lastDeleted = &trap;
    TrapezoidalMap::removeTrapezoid(trap.getItr());
    return pi;
}

DagNode *Dag::createLeftMostDegenerate(cg3::Segment2d &segment, DagNodeArea &leaf, Trapezoid *&lastDeleted)
{
    bool aboveleftp;
    bool aboverightp;
    bool degenerate;
    bool triangleOver = false;
    bool linear = false;
    std::list<Trapezoid>& ref = TrapezoidalMap::getTrapezoids();
    std::list<Trapezoid>::iterator itr;
    std::vector<Trapezoid*> iterators(2);
    DagNode * leafB;
    DagNode * leafC;
    InnerNodes* si;// = new DagNodeSegment(segment);
    Trapezoid& trap = leaf.getT();
    Trapezoid* leftUp = &trap.getLeftUp();
    Trapezoid* leftDown = &trap.getLeftDown();
    Trapezoid* rightUp = &trap.getRightUp();
    Trapezoid* rightDown = &trap.getRightDown();
    std::vector<cg3::Point2d> ints = {intersection(trap.getTop(), segment.p1()),
                                      intersection(trap.getBottom(), segment.p1()),
                                      intersection(segment, trap.getTop().p2())};
    (ints[2].y()>trap.getRightp().y())?aboverightp=true:aboverightp=false;
    std::vector<std::tuple<cg3::Segment2d, cg3::Segment2d, cg3::Point2d, cg3::Point2d>> traps = {
        std::make_tuple(trap.getTop(),cg3::Segment2d(segment.p1(),ints[2]), segment.p1() , ints[2]),
        std::make_tuple(cg3::Segment2d(segment.p1(),ints[2]),trap.getBottom(),segment.p1(), ints[2]),

    };
    if(segment.p1()==trap.getTop().p1())
        triangleOver = true;
    if(segment.p1()==trap.getBottom().p1())
        triangleOver = false;
    if(segment.p1()!=trap.getTop().p1() && segment.p1()!=trap.getBottom().p1())
        linear = true;
    si = new DagNodeSegment(nullptr, nullptr, segment);
    leafB = bindWithParentRef(std::get<0>(traps[0]), std::get<1>(traps[0]), std::get<2>(traps[0]), std::get<3>(traps[0]), si->getLeftChild());
    leafC = bindWithParentRef(std::get<0>(traps[1]), std::get<1>(traps[1]), std::get<2>(traps[1]), std::get<3>(traps[1]), si->getRightChild());
    //pi->setRightChild(si);
    si->setLeftChild(leafB);
    si->setRightChild(leafC);
    itr = ref.end();
    for(int i=1; i>-1; i--)
        iterators[i] = &*(--itr);

    if(!linear){
        if(!trap.leftUpNull() && trap.getLeftUp().getRightUp()==trap && !triangleOver)
            trap.getLeftUp().setRightUp(*iterators[0]);
        if(!trap.leftUpNull() && trap.getLeftUp().getRightDown()==trap &&!triangleOver)
            trap.getLeftUp().setRightDown(*iterators[0]);

        if(!trap.leftDownNull() && trap.getLeftDown().getRightUp()==trap && triangleOver)
            trap.getLeftDown().setRightUp(*iterators[1]);
        if(!trap.leftDownNull() && trap.getLeftDown().getRightDown()==trap && triangleOver)
            trap.getLeftDown().setRightDown(*iterators[1]);
    }
    else{
        trap.getLeftUp().setRightUp(*iterators[0]);
        trap.getLeftUp().setRightDown(*iterators[0]);

        trap.getLeftDown().setRightUp(*iterators[1]);
        trap.getLeftDown().setRightDown(*iterators[1]);
    }
    if(aboverightp){
        if(rightDown->getLeftUp()==trap)
            rightDown->setLeftUp(*iterators[1]);
        if(rightDown->getLeftDown()==trap)
            rightDown->setLeftDown(*iterators[1]);
        if(rightUp->getLeftUp()==trap)
            rightUp->setLeftUp(*iterators[0]);
        if(rightUp->getLeftDown()==trap)
            rightUp->setLeftDown(*iterators[1]);
    }
    else{
        if(rightDown->getLeftUp()==trap)
            rightDown->setLeftUp(*iterators[0]);
        if(rightDown->getLeftDown()==trap)
            rightDown->setLeftDown(*iterators[1]);
        if(rightUp->getLeftUp()==trap)
            rightUp->setLeftUp(*iterators[0]);
        if(rightUp->getLeftDown()==trap)
            rightUp->setLeftDown(*iterators[0]);
    }

    //from new to old neighbors   
    iterators[0]->setNeighbors(trap.getLeftUp(),trap.getLeftUp(), trap.getRightUp(), (aboverightp)?trap.getRightUp():trap.getRightDown());// be careful on right side neighbors
    iterators[1]->setNeighbors(trap.getLeftDown(),trap.getLeftDown(), (aboverightp)?trap.getRightUp():trap.getRightDown(), trap.getRightDown());//also there
    lastDeleted = &trap;
    TrapezoidalMap::removeTrapezoid(trap.getItr());
    return si;
}

DagNode *Dag::createRightMost(cg3::Segment2d &segment, DagNodeArea &leaf,  DagNodeSegment& prevSeg, Trapezoid*& lastDeleted)
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
    Trapezoid& trap = leaf.getT();

    std::vector<cg3::Point2d> ints = {intersection(trap.getTop(), segment.p2()),
                                      intersection(trap.getBottom(), segment.p2()),
                                      intersection(segment, trap.getTop().p1())};

    std::vector<std::tuple<cg3::Segment2d, cg3::Segment2d, cg3::Point2d, cg3::Point2d>> traps = {
        std::make_tuple(cg3::Segment2d(trap.getTop().p1(),ints[0]),cg3::Segment2d(ints[2], segment.p2()),(trap.getLeftp().y()<ints[2].y())?ints[2]:trap.getLeftp() , segment.p2()),
        std::make_tuple(cg3::Segment2d(ints[2],segment.p2()),cg3::Segment2d(trap.getBottom().p1(), ints[1]),(trap.getLeftp().y()>ints[2].y())?ints[2]:trap.getLeftp(), segment.p2()),
        std::make_tuple(cg3::Segment2d(ints[0],trap.getTop().p2()),cg3::Segment2d(ints[1], trap.getBottom().p2()),segment.p2(), trap.getRightp()),
    };
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
    TrapezoidalMap::updateNeighborsMultiTrapezoid(trap, iterators, 2, prevSeg, lastDeleted);
    if(upper){
        iterators[0]->setNeighbors(iterators[0]->getLeftUp(),iterators[0]->getLeftDown(),*iterators[2], *iterators[2]);//
        iterators[1]->setNeighbors(((DagNodeArea*)prevSeg.getRightChild())->getT(),(trap.getLeftDown()==*lastDeleted)?((DagNodeArea*)prevSeg.getRightChild())->getT():trap.getLeftDown(),*iterators[2], *iterators[2]);   
    }
    else{
        iterators[0]->setNeighbors((trap.getLeftUp()==*lastDeleted)?((DagNodeArea*)prevSeg.getLeftChild())->getT():trap.getLeftUp(),((DagNodeArea*)prevSeg.getLeftChild())->getT(),*iterators[2], *iterators[2]);//
        iterators[1]->setNeighbors(iterators[1]->getLeftUp(),iterators[1]->getLeftDown(),*iterators[2], *iterators[2]);
    }
    iterators[2]->setNeighbors(*iterators[0],*iterators[1], trap.getRightUp(), trap.getRightDown());//
    lastDeleted = &trap;
    TrapezoidalMap::removeTrapezoid(trap.getItr());
    return qi;
}

DagNode *Dag::createRightMostDegenerate(cg3::Segment2d &segment, DagNodeArea &leaf, DagNodeSegment& prevSeg, Trapezoid *&lastDeleted)
{
    std::list<Trapezoid>& ref = TrapezoidalMap::getTrapezoids();
    std::list<Trapezoid>::iterator itr;
    DagNode* ex;
    DagNode * leafA;
    DagNode * leafB;
    bool upper;
    std::vector<Trapezoid*> iterators(2);
    InnerNodes* si;// = new DagNodeSegment(segment);
    Trapezoid& trap = leaf.getT();
    std::vector<cg3::Point2d> ints = {intersection(segment, trap.getTop().p1()), intersection(segment, trap.getTop().p2())};
    std::vector<std::tuple<cg3::Segment2d, cg3::Segment2d, cg3::Point2d, cg3::Point2d>> traps = {
        std::make_tuple(cg3::Segment2d(trap.getTop().p1(),trap.getTop().p2()),cg3::Segment2d(ints[0], ints[1]),(trap.getLeftp().y()<ints[0].y())?ints[0]:trap.getLeftp(), segment.p2()),
        std::make_tuple(cg3::Segment2d(ints[0], ints[1]),cg3::Segment2d(trap.getBottom().p1(), trap.getBottom().p2()),(trap.getLeftp().y()>ints[0].y())?ints[0]:trap.getLeftp(), segment.p2()),
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
        if(((DagNodeArea*)prevSeg.getRightChild())->getT().getRightUp()==trap)
            ((DagNodeArea*)prevSeg.getRightChild())->getT().setRightUp(*iterators[1]);
        if(((DagNodeArea*)prevSeg.getRightChild())->getT().getRightDown()==trap)
            ((DagNodeArea*)prevSeg.getRightChild())->getT().setRightDown(*iterators[1]);
        if(trap.getLeftDown()!= *lastDeleted && trap.getLeftDown().getRightUp()==trap)
            trap.getLeftDown().setRightUp(*iterators[1]);
        if(trap.getLeftDown()!= *lastDeleted && trap.getLeftDown().getRightDown()==trap)
            trap.getLeftDown().setRightDown(*iterators[1]);
    }
    else{
        si = new DagNodeSegment(nullptr, TrapezoidalMap::merge(((DagNodeArea*)prevSeg.getRightChild())->getT(),traps[1]), segment);
        leafA = bindWithParentRef(std::get<0>(traps[0]), std::get<1>(traps[0]), std::get<2>(traps[0]), std::get<3>(traps[0]), si->getLeftChild());
        si->setLeftChild(leafA);
        upper = false;
        iterators[0] = &*(--itr);
        iterators[1] = &((DagNodeArea*)si->getRightChild())->getT();
        if(((DagNodeArea*)prevSeg.getLeftChild())->getT().getRightUp()==trap)
            ((DagNodeArea*)prevSeg.getLeftChild())->getT().setRightUp(*iterators[0]);
        if(((DagNodeArea*)prevSeg.getLeftChild())->getT().getRightDown()==trap)
            ((DagNodeArea*)prevSeg.getLeftChild())->getT().setRightDown(*iterators[0]);
        if(trap.getLeftUp()!= *lastDeleted &&  trap.getLeftUp().getRightUp()==trap)
            trap.getLeftUp().setRightUp(*iterators[0]);
        if(trap.getLeftUp()!= *lastDeleted && trap.getLeftUp().getRightDown()==trap)
            trap.getLeftUp().setRightDown(*iterators[0]);
    }

    if(upper){
        iterators[0]->setRightUp(trap.getRightUp());
        iterators[0]->setRightDown((iterators[0]->getBottom().p2().y()<trap.getRightp().y())?trap.getRightDown():trap.getRightUp());
        iterators[1]->setNeighbors(trap.getLeftUp(), trap.getLeftDown(),
                                   (iterators[0]->getBottom().p2().y()<trap.getRightp().y())?trap.getRightDown():trap.getRightUp(),
                                    trap.getRightDown());//
    }
    else{
        iterators[0]->setNeighbors(trap.getLeftUp(), trap.getLeftDown(),trap.getRightUp(),
                                   (iterators[1]->getTop().p2().y()>trap.getRightp().y())?trap.getRightUp():trap.getRightDown());
        iterators[1]->setRightUp((iterators[1]->getTop().p2().y()>trap.getRightp().y())?trap.getRightUp():trap.getRightDown());
        iterators[1]->setRightDown(trap.getRightDown());
    }
    if(!trap.rightUpNull() && trap.getRightUp().getLeftUp()==trap)
        trap.getRightUp().setLeftUp(*iterators[0]);
    if(!trap.rightUpNull())
        trap.getRightUp().setLeftDown(*iterators[0]);
    if(!trap.rightDownNull())
        trap.getRightDown().setLeftUp(*iterators[1]);
    if(!trap.rightDownNull() && trap.getRightDown().getLeftDown()==trap)
        trap.getRightDown().setLeftDown(*iterators[1]);


    //TrapezoidalMap::updateNeighborsMultiTrapezoid(trap, iterators, 1, prevSeg, lastDeleted);
    lastDeleted = &trap;

    TrapezoidalMap::removeTrapezoid(trap.getItr());
    return si;
}

DagNode *Dag::createIntermediate(cg3::Segment2d &segment, DagNodeArea &leaf, DagNodeSegment& prevSeg, Trapezoid*& lastDeleted)
{
    std::list<Trapezoid>& ref = TrapezoidalMap::getTrapezoids();
    std::list<Trapezoid>::iterator itr;
    DagNode* ex;
    DagNode * leafA;
    DagNode * leafB;
    bool upper;
    std::vector<Trapezoid*> iterators(2);
    InnerNodes* si;// = new DagNodeSegment(segment);
    Trapezoid& trap = leaf.getT();
    std::vector<cg3::Point2d> ints = {intersection(segment, trap.getTop().p1()), intersection(segment, trap.getTop().p2())};
    std::vector<std::tuple<cg3::Segment2d, cg3::Segment2d, cg3::Point2d, cg3::Point2d>> traps = {
        std::make_tuple(cg3::Segment2d(trap.getTop().p1(),trap.getTop().p2()),cg3::Segment2d(ints[0], ints[1]),(trap.getLeftp().y()<ints[0].y())?ints[0]:trap.getLeftp(), (trap.getRightp().y()<ints[1].y())?ints[1]:trap.getRightp()),
        std::make_tuple(cg3::Segment2d(ints[0], ints[1]),cg3::Segment2d(trap.getBottom().p1(), trap.getBottom().p2()),(trap.getLeftp().y()>ints[0].y())?ints[0]:trap.getLeftp(), (trap.getRightp().y()>ints[1].y())?ints[1]:trap.getRightp()),
    };
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
    TrapezoidalMap::updateNeighborsMultiTrapezoid(trap, iterators, 1, prevSeg, lastDeleted);
    if(upper){
        iterators[0]->setRightUp(trap.getRightUp());
        iterators[0]->setRightDown((iterators[0]->getBottom().p2().y()<trap.getRightp().y())?trap.getRightDown():trap.getRightUp());
        iterators[1]->setNeighbors(trap.getLeftUp(), trap.getLeftDown(),
                                   (iterators[0]->getBottom().p2().y()<trap.getRightp().y())?trap.getRightDown():trap.getRightUp(),
                                    trap.getRightDown());//

    }
    else{
        iterators[0]->setNeighbors(trap.getLeftUp(), trap.getLeftDown(),trap.getRightUp(),
                                   (iterators[1]->getTop().p2().y()>trap.getRightp().y())?trap.getRightUp():trap.getRightDown());
        iterators[1]->setRightUp((iterators[1]->getTop().p2().y()>trap.getRightp().y())?trap.getRightUp():trap.getRightDown());
        iterators[1]->setRightDown(trap.getRightDown());
    }

    lastDeleted = &trap;
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
















