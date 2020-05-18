
#include "trapezoidalmap.h"
#include "algorithms/algorithms.h"
#include "trapezoid.h"

/**
 * @brief Trapezoidal map constructor. Creates bounding box trapezoid and cross-links it to the node that will be the root of the dag
 */
TrapezoidalMap::TrapezoidalMap(){
    DagNodeArea ** node = new DagNodeArea*();
    trapezoids.emplace_back(cg3::Segment2d(cg3::Point2d(-1000000.0, 1000000.0),cg3::Point2d(1000000.0, 1000000.0)),
                            cg3::Segment2d(cg3::Point2d(-1000000.0, -1000000.0),cg3::Point2d(1000000.0, -1000000.0)),
                            cg3::Point2d(-1000000.0, 1000000.0), cg3::Point2d(1000000.0, -1000000.0), (DagNodeArea*&)*node);
    trapezoids.back().setItr(prev(trapezoids.end()));
    trapezoids.back().getNodeRef() = new DagNodeArea(trapezoids.back());
}

/**
 * @brief insert a new portion of the trapezoidal map, substituing the one founded by point location
 * @param reference to trapezoid founded by point location
 * @param reference to new segment
 * @return iterator of trapezoidal map
 */
std::list<Trapezoid>::iterator TrapezoidalMap::addSegmentInSingleTrap(Trapezoid &trap, const cg3::Segment2d &segment)
{

    std::list<Trapezoid>& ref = TrapezoidalMap::traps();
    std::list<Trapezoid>::iterator itr;
    std::vector<Trapezoid*> iterators(4);
    std::vector<cg3::Point2d> ints = {gas::intersection(trap.getTop(), segment.p1()), //compute upper and lower segment intersections
                                      gas::intersection(trap.getTop(), segment.p2()),
                                      gas::intersection(trap.getBottom(), segment.p1()),
                                      gas::intersection(trap.getBottom(), segment.p2())};

    std::vector<std::tuple<cg3::Segment2d, cg3::Segment2d, cg3::Point2d, cg3::Point2d>> traps = { //making trapezoids templates
        std::make_tuple(cg3::Segment2d(trap.getTop().p1(),ints[0]),cg3::Segment2d(trap.getBottom().p1(),ints[2]),trap.getLeftp(), segment.p1()),
        std::make_tuple(cg3::Segment2d(ints[0],ints[1]), cg3::Segment2d(segment.p1(),segment.p2()), segment.p1(), segment.p2()),
        std::make_tuple(cg3::Segment2d(segment.p1(),segment.p2()),cg3::Segment2d(ints[2],ints[3]), segment.p1(), segment.p2()),
        std::make_tuple(cg3::Segment2d(ints[1], trap.getTop().p2()),cg3::Segment2d(ints[3], trap.getBottom().p2()),segment.p2(), trap.getRightp()),
    };
    gas::bind(std::get<0>(traps[0]), std::get<1>(traps[0]), std::get<2>(traps[0]), std::get<3>(traps[0]), *this);
    gas::bind(std::get<0>(traps[3]), std::get<1>(traps[3]), std::get<2>(traps[3]), std::get<3>(traps[3]), *this);
    gas::bind(std::get<0>(traps[1]), std::get<1>(traps[1]), std::get<2>(traps[1]), std::get<3>(traps[1]), *this);
    gas::bind(std::get<0>(traps[2]), std::get<1>(traps[2]), std::get<2>(traps[2]), std::get<3>(traps[2]), *this);
    itr = ref.end(); //code below recovers last inserted trapezoids in reverse order
    iterators[2] = &*(--itr);
    iterators[1] = &*(--itr);
    iterators[3] = &*(--itr);
    iterators[0] = &*(--itr);
    updateNeighbors(trap, iterators); //neighbors' neighborhood update according to new trapezoids
    //new trapezoids neighborhood update
    iterators[0]->setNeighbors(trap.getLeftUp(),trap.getLeftDown(),*iterators[1], *iterators[2]);
    iterators[1]->setNeighbors(*iterators[0],*iterators[0], *iterators[3], *iterators[3]);
    iterators[2]->setNeighbors(*iterators[0],*iterators[0], *iterators[3], *iterators[3]);
    iterators[3]->setNeighbors(*iterators[1], *iterators[2], trap.getRightUp(), trap.getRightDown());
    removeTrapezoid(trap.getItr()); //old trapezoid removal
    return ref.end();
}



/**
 * @brief insert a new portion of the trapezoidal map, substituing the one founded by point location (left degenerate case)
 * @param reference to trapezoid founded by point location
 * @param reference to new segment
 * @return iterator of trapezoidal map
 */
std::list<Trapezoid>::iterator TrapezoidalMap::leftDegenerateSingleInsertion(Trapezoid &trap, const cg3::Segment2d &segment)
{
    std::list<Trapezoid>& ref = TrapezoidalMap::traps();
    std::list<Trapezoid>::iterator itr;
    std::vector<Trapezoid*> iterators(3);
    bool triangleOver;
    bool linear = false;
    std::vector<cg3::Point2d> ints = {gas::intersection(trap.getTop(), segment.p2()),gas::intersection(trap.getBottom(), segment.p2())}; //compute upper and lower right endpoint intersections

    std::vector<std::tuple<cg3::Segment2d, cg3::Segment2d, cg3::Point2d, cg3::Point2d>> traps = { //making trapezoids templates
        std::make_tuple(cg3::Segment2d(trap.getTop().p1(),ints[0]), cg3::Segment2d(segment.p1(), segment.p2()), segment.p1(), segment.p2()),
        std::make_tuple(cg3::Segment2d(segment.p1(),segment.p2()), cg3::Segment2d(trap.getBottom().p1(), ints[1]), segment.p1(), segment.p2()),
        std::make_tuple(cg3::Segment2d(ints[0],trap.getTop().p2()),cg3::Segment2d(ints[1], trap.getBottom().p2()),segment.p2(), trap.getRightp()),
    };
    //check degenerate details
    if(segment.p1()==trap.getTop().p1())
        triangleOver = true;
    if(segment.p1()==trap.getBottom().p1())
        triangleOver = false;
    if(segment.p1()!=trap.getTop().p1() && segment.p1()!=trap.getBottom().p1())
        linear = true;
    gas::bind(std::get<0>(traps[2]), std::get<1>(traps[2]), std::get<2>(traps[2]), std::get<3>(traps[2]),*this);
    gas::bind(std::get<0>(traps[0]), std::get<1>(traps[0]), std::get<2>(traps[0]), std::get<3>(traps[0]),*this);
    gas::bind(std::get<0>(traps[1]), std::get<1>(traps[1]), std::get<2>(traps[1]), std::get<3>(traps[1]),*this);

    itr = ref.end();
    iterators[1] = &*(--itr);
    iterators[0] = &*(--itr);
    iterators[2] = &*(--itr);

    iterators[2]->setNeighbors(*iterators[0], *iterators[1],trap.getRightUp(), trap.getRightDown());
    iterators[1]->setRightUp(*iterators[2]);
    iterators[1]->setRightDown(*iterators[2]);
    iterators[0]->setRightUp(*iterators[2]);
    iterators[0]->setRightDown(*iterators[2]);

    leftSingle(trap, iterators, triangleOver, linear); //update neighborhoods
    removeTrapezoid(trap.getItr());
    return ref.end();
}

/**
 * @brief insert a new portion of the trapezoidal map, substituing the one founded by point location (right degenerate case)
 * @param reference to trapezoid founded by point location
 * @param reference to new segment
 * @return iterator of trapezoidal map
 */
std::list<Trapezoid>::iterator TrapezoidalMap::rightDegenerateSingleInsertion(Trapezoid &trap, const cg3::Segment2d &segment)
{
        std::list<Trapezoid>& ref = traps();
        std::list<Trapezoid>::iterator itr;
        std::vector<Trapezoid*> iterators(3);
        bool triangleOver;
        bool linear = false;
        //check degenerate details
        if(segment.p2()==trap.getTop().p2())
            triangleOver = true;
        if(segment.p2()==trap.getBottom().p2())
            triangleOver = false;
        if(segment.p2()!=trap.getTop().p2() && segment.p2()!=trap.getBottom().p2())
            linear = true;
        std::vector<cg3::Point2d> ints = {gas::intersection(trap.getTop(), segment.p1()),gas::intersection(trap.getBottom(), segment.p1())}; //compute upper and lower left endpoint intersections


        std::vector<std::tuple<cg3::Segment2d, cg3::Segment2d, cg3::Point2d, cg3::Point2d>> traps = { //making trapezoids templates
            std::make_tuple(cg3::Segment2d(trap.getTop().p1(),ints[0]), cg3::Segment2d(trap.getBottom().p1(), ints[1]), trap.getLeftp(), segment.p1()),
            std::make_tuple(cg3::Segment2d(ints[0],trap.getTop().p2()),cg3::Segment2d(segment.p1(),segment.p2()),segment.p1(), segment.p2()),
            std::make_tuple(cg3::Segment2d(segment.p1(),segment.p2()), cg3::Segment2d(ints[1],trap.getBottom().p2()), segment.p1(), segment.p2())
        };
        gas::bind(std::get<0>(traps[0]), std::get<1>(traps[0]), std::get<2>(traps[0]), std::get<3>(traps[0]), *this);
        gas::bind(std::get<0>(traps[1]), std::get<1>(traps[1]), std::get<2>(traps[1]), std::get<3>(traps[1]), *this);
        gas::bind(std::get<0>(traps[2]), std::get<1>(traps[2]), std::get<2>(traps[2]), std::get<3>(traps[2]), *this);
        itr = ref.end();
        iterators[2] = &*(--itr);
        iterators[1] = &*(--itr);
        iterators[0] = &*(--itr);

        iterators[0]->setNeighbors(trap.getLeftUp(),trap.getLeftDown(),*iterators[1], *iterators[2]); //A
        iterators[1]->setLeftUp(*iterators[0]);
        iterators[1]->setLeftDown(*iterators[0]);
        iterators[2]->setLeftUp(*iterators[0]);
        iterators[2]->setLeftDown(*iterators[0]);

        rightSingle(trap, iterators, triangleOver, linear); //neighborhood update
        removeTrapezoid(trap.getItr());
        return ref.end();
}

/**
 * @brief insert a new portion of the trapezoidal map, substituing the one founded by point location (both left and right degenerate cases)
 * @param reference to trapezoid founded by point location
 * @param reference to new segment
 * @return iterator of trapezoidal map
 */
std::list<Trapezoid>::iterator TrapezoidalMap::totallyDegenerateSingleInsertion(Trapezoid& trap, const cg3::Segment2d& segment)
{
    std::list<Trapezoid>& ref = traps();
    std::list<Trapezoid>::iterator itr;
    std::vector<Trapezoid*> iterators(2);

    bool triangleOverLeft;
    bool triangleOverRight;
    bool linearRight = false;
    bool linearLeft = false;
    std::vector<std::tuple<cg3::Segment2d, cg3::Segment2d, cg3::Point2d, cg3::Point2d>> traps = { //making trapezoid templates
        std::make_tuple(cg3::Segment2d(trap.getTop().p1(),trap.getTop().p2()),cg3::Segment2d(segment.p1(),segment.p2()),segment.p1(), segment.p2()),
        std::make_tuple(cg3::Segment2d(segment.p1(),segment.p2()), cg3::Segment2d(trap.getBottom().p1(),trap.getBottom().p2()), segment.p1(), segment.p2())
    };
    //check both left and right degenerate details
    if(segment.p1()==trap.getTop().p1())
        triangleOverLeft = true;
    if(segment.p2()==trap.getTop().p2())
        triangleOverRight = true;
    if(segment.p1()==trap.getBottom().p1())
        triangleOverLeft = false;
    if(segment.p2()==trap.getBottom().p2())
        triangleOverRight = false;
    if(segment.p1()!=trap.getTop().p1() && segment.p1()!=trap.getBottom().p1())
        linearLeft = true;
    if(segment.p2()!=trap.getTop().p2() && segment.p2()!=trap.getBottom().p2())
        linearRight = true;
    gas::bind(std::get<0>(traps[0]), std::get<1>(traps[0]), std::get<2>(traps[0]), std::get<3>(traps[0]), *this);
    gas::bind(std::get<0>(traps[1]), std::get<1>(traps[1]), std::get<2>(traps[1]), std::get<3>(traps[1]), *this);
    itr = ref.end();
    iterators[1] = &*(--itr);
    iterators[0] = &*(--itr);
    bothDegenerate(trap,iterators,triangleOverLeft,triangleOverRight,linearRight,linearLeft); //neighborhood updates
    removeTrapezoid(trap.getItr());
    return ref.end();
}


/**
 * @brief insert a new portion of the trapezoidal map, substituing the one founded by point location
 * @param reference to trapezoid founded by point location
 * @param reference to new segment
 * @param last deleted trapezoid
 * @return vector of pointers to inserted trapezoids
 */
std::vector<Trapezoid*>  TrapezoidalMap::createLeftMost(Trapezoid &trap, const cg3::Segment2d &segment, Trapezoid *&lastDeleted)
{
    std::list<Trapezoid>& ref = TrapezoidalMap::traps();
    std::list<Trapezoid>::iterator itr;
    std::vector<Trapezoid*> iterators(3);
    std::vector<cg3::Point2d> ints = {gas::intersection(trap.getTop(), segment.p1()), //compute intersections
                                      gas::intersection(trap.getBottom(), segment.p1()),
                                      gas::intersection(segment, trap.getTop().p2())};
    std::vector<std::tuple<cg3::Segment2d, cg3::Segment2d, cg3::Point2d, cg3::Point2d>> traps = { //making trapezoids templates
        std::make_tuple(cg3::Segment2d(trap.getTop().p1(),ints[0]),cg3::Segment2d(trap.getBottom().p1(),ints[1]), trap.getLeftp() , segment.p1()),
        std::make_tuple(cg3::Segment2d(ints[0],trap.getTop().p2()),cg3::Segment2d(segment.p1(), ints[2]),segment.p1(), ((trap.getRightp().y()>ints[2].y())?trap.getRightp():ints[2])),
        std::make_tuple(cg3::Segment2d(segment.p1(),ints[2]),cg3::Segment2d(ints[1], trap.getBottom().p2()),segment.p1(), ((trap.getRightp().y()>ints[2].y())?ints[2]:trap.getRightp())),

    };
    gas::bind(std::get<0>(traps[0]), std::get<1>(traps[0]), std::get<2>(traps[0]), std::get<3>(traps[0]), *this);
    gas::bind(std::get<0>(traps[1]), std::get<1>(traps[1]), std::get<2>(traps[1]), std::get<3>(traps[1]), *this);
    gas::bind(std::get<0>(traps[2]), std::get<1>(traps[2]), std::get<2>(traps[2]), std::get<3>(traps[2]), *this);
    itr = ref.end();
    for(int i=2; i>-1; i--)
        iterators[i] = &*(--itr);
    updateNeighborsMultiTrapezoid(trap, iterators, 0, nullptr, lastDeleted); //neighborhood updates
    lastDeleted = &trap;
    removeTrapezoid(trap.getItr());
    return iterators;
}

/**
 * @brief insert a new portion of the trapezoidal map, substituing the one founded by point location (degenerate case)
 * @param reference to trapezoid founded by point location
 * @param reference to new segment
 * @param last deleted trapezoid
 * @return vector of pointers to inserted trapezoids
 */
std::vector<Trapezoid*>  TrapezoidalMap::createLeftMostDegenerate(Trapezoid &trap, const cg3::Segment2d &segment, Trapezoid *&lastDeleted)
{
    bool aboverightp;
    bool triangleOver = false;
    bool linear = false;
    std::list<Trapezoid>& ref = traps();
    std::list<Trapezoid>::iterator itr;
    std::vector<Trapezoid*> iterators(2);
    std::vector<cg3::Point2d> ints = {gas::intersection(trap.getTop(), segment.p1()),  //compute left endpoint intersections
                                      gas::intersection(trap.getBottom(), segment.p1()),
                                      gas::intersection(segment, trap.getTop().p2())};
    (ints[2].y()>trap.getRightp().y())?aboverightp=true:aboverightp=false;
    std::vector<std::tuple<cg3::Segment2d, cg3::Segment2d, cg3::Point2d, cg3::Point2d>> traps = { //making trapezoids templates
        std::make_tuple(trap.getTop(),cg3::Segment2d(segment.p1(),ints[2]), segment.p1() , (trap.getRightp().y()<ints[2].y())?ints[2]:trap.getRightp()),
        std::make_tuple(cg3::Segment2d(segment.p1(),ints[2]),trap.getBottom(),segment.p1(), (trap.getRightp().y()>ints[2].y())?ints[2]:trap.getRightp()),

    };
    //check degenerate details
    if(segment.p1()==trap.getTop().p1())
        triangleOver = true;
    if(segment.p1()==trap.getBottom().p1())
        triangleOver = false;
    if(segment.p1()!=trap.getTop().p1() && segment.p1()!=trap.getBottom().p1())
        linear = true;
   gas::bind(std::get<0>(traps[0]), std::get<1>(traps[0]), std::get<2>(traps[0]), std::get<3>(traps[0]), *this);
   gas::bind(std::get<0>(traps[1]), std::get<1>(traps[1]), std::get<2>(traps[1]), std::get<3>(traps[1]), *this);
    itr = ref.end();
    for(int i=1; i>-1; i--)
        iterators[i] = &*(--itr);

    leftMulti(trap, iterators, aboverightp,triangleOver,linear); //neighborhood update

    lastDeleted = &trap;
    removeTrapezoid(trap.getItr());
    return iterators;
}

/**
 * @brief insert a new portion of the trapezoidal map, substituing the one obtained by following the segment
 * @param reference to trapezoid founded by point location
 * @param reference to new segment
 * @param previus created type Y node
 * @param last deleted trapezoid
 * @return vector of pointers to inserted trapezoids
 */
std::vector<Trapezoid*> TrapezoidalMap::createRightMost(Trapezoid &trap, const cg3::Segment2d &segment, DagNodeSegment &prevSeg, Trapezoid *&lastDeleted)
{
    std::list<Trapezoid>& ref = traps();
    std::list<Trapezoid>::iterator itr;
    bool upper;
    std::vector<Trapezoid*> iterators(3);
    std::vector<cg3::Point2d> ints = {gas::intersection(trap.getTop(), segment.p2()), //compute right endpoint intersections
                                      gas::intersection(trap.getBottom(), segment.p2()),
                                      gas::intersection(segment, trap.getTop().p1())};

    std::vector<std::tuple<cg3::Segment2d, cg3::Segment2d, cg3::Point2d, cg3::Point2d>> traps = { //making trapezoids templates
        std::make_tuple(cg3::Segment2d(trap.getTop().p1(),ints[0]),cg3::Segment2d(ints[2], segment.p2()),(trap.getLeftp().y()<ints[2].y())?ints[2]:trap.getLeftp() , segment.p2()),
        std::make_tuple(cg3::Segment2d(ints[2],segment.p2()),cg3::Segment2d(trap.getBottom().p1(), ints[1]),(trap.getLeftp().y()>ints[2].y())?ints[2]:trap.getLeftp(), segment.p2()),
        std::make_tuple(cg3::Segment2d(ints[0],trap.getTop().p2()),cg3::Segment2d(ints[1], trap.getBottom().p2()),segment.p2(), trap.getRightp()),
    };

    gas::bind(std::get<0>(traps[2]), std::get<1>(traps[2]), std::get<2>(traps[2]), std::get<3>(traps[2]), *this);
    itr = ref.end();
    iterators[2] = &*(--itr);
    itr = ref.end();
    if(ints[2].y()>trap.getLeftp().y()){ //above rightp case
        TrapezoidalMap::merge(((DagNodeArea*)prevSeg.getLeftChild())->getT(), traps[0]);
        gas::bind(std::get<0>(traps[1]), std::get<1>(traps[1]), std::get<2>(traps[1]), std::get<3>(traps[1]), *this);
        upper = true;
        iterators[1] = &*(--itr);
        iterators[0] = &((DagNodeArea*)prevSeg.getLeftChild())->getT();
    }
    else{ //under rightp case
        merge(((DagNodeArea*)prevSeg.getRightChild())->getT(),traps[1]);
        gas::bind(std::get<0>(traps[0]), std::get<1>(traps[0]), std::get<2>(traps[0]), std::get<3>(traps[0]), *this);
        upper = false;
        iterators[0] = &*(--itr);
        iterators[1] = &((DagNodeArea*)prevSeg.getRightChild())->getT();

    }
    TrapezoidalMap::updateNeighborsMultiTrapezoid(trap, iterators, 2, &prevSeg, lastDeleted); //newighborhood update
    if(upper){
        iterators[0]->setNeighbors(iterators[0]->getLeftUp(),iterators[0]->getLeftDown(),*iterators[2], *iterators[2]);//
        iterators[1]->setNeighbors((trap.getLeftUp().getRightp().x()<iterators[1]->getRightp().x())?trap.getLeftUp():((DagNodeArea*)prevSeg.getRightChild())->getT(),(trap.getLeftDown()==*lastDeleted)?((DagNodeArea*)prevSeg.getRightChild())->getT():trap.getLeftDown(),*iterators[2], *iterators[2]);
    }
    else{
        iterators[0]->setNeighbors((trap.getLeftUp()==*lastDeleted)?((DagNodeArea*)prevSeg.getLeftChild())->getT():trap.getLeftUp(),(trap.getLeftDown().getRightp().x()<iterators[0]->getRightp().x())?trap.getLeftDown():((DagNodeArea*)prevSeg.getLeftChild())->getT(),*iterators[2], *iterators[2]);//
        iterators[1]->setNeighbors(iterators[1]->getLeftUp(),iterators[1]->getLeftDown(),*iterators[2], *iterators[2]);
    }
    iterators[2]->setNeighbors(*iterators[0],*iterators[1], trap.getRightUp(), trap.getRightDown());//
    lastDeleted = &trap;
    removeTrapezoid(trap.getItr());
    return iterators;
}

/**
 * @brief insert a new portion of the trapezoidal map, substituing the one obtained by following the segment (degenerate case)
 * @param reference to trapezoid founded by point location
 * @param reference to new segment
 * @param previus created type Y node
 * @param last deleted trapezoid
 * @return vector of pointers to inserted trapezoids
 */
std::vector<Trapezoid*> TrapezoidalMap::createRightMostDegenerate(Trapezoid &trap, const cg3::Segment2d &segment, DagNodeSegment &prevSeg, Trapezoid *&lastDeleted)
{
    std::list<Trapezoid>& ref = traps();
    std::list<Trapezoid>::iterator itr;

    bool upper;
    bool triangleOver = false;
    bool linear = false;
    std::vector<Trapezoid*> iterators(2);

    std::vector<cg3::Point2d> ints = {gas::intersection(segment, trap.getTop().p1()), gas::intersection(segment, trap.getTop().p2())}; //compute intersections
    std::vector<std::tuple<cg3::Segment2d, cg3::Segment2d, cg3::Point2d, cg3::Point2d>> traps = { //making trapezoids template
        std::make_tuple(cg3::Segment2d(trap.getTop().p1(),trap.getTop().p2()),cg3::Segment2d(ints[0], segment.p2()),(trap.getLeftp().y()<ints[0].y())?ints[0]:trap.getLeftp(), segment.p2()),
        std::make_tuple(cg3::Segment2d(ints[0], segment.p2()),cg3::Segment2d(trap.getBottom().p1(), trap.getBottom().p2()),(trap.getLeftp().y()>ints[0].y())?ints[0]:trap.getLeftp(), segment.p2()),
    };

    //check degenerate details
    if(segment.p2()==trap.getTop().p2())
        triangleOver = true;
    if(segment.p2()==trap.getBottom().p2())
        triangleOver = false;
    if(segment.p2()!=trap.getTop().p2() && segment.p2()!=trap.getBottom().p2())
        linear = true;
    itr = ref.end();
    if(ints[0].y()>trap.getLeftp().y()){ //above rightp case
        TrapezoidalMap::merge(((DagNodeArea*)prevSeg.getLeftChild())->getT(), traps[0]);
        gas::bind(std::get<0>(traps[1]), std::get<1>(traps[1]), std::get<2>(traps[1]), std::get<3>(traps[1]), *this);
        upper = true;
        iterators[1] = &*(--itr);
        iterators[0] = &((DagNodeArea*)prevSeg.getLeftChild())->getT();
        if(((DagNodeArea*)prevSeg.getRightChild())->getT().getRightUp()==trap)
            ((DagNodeArea*)prevSeg.getRightChild())->getT().setRightUp(*iterators[1]);
        if(((DagNodeArea*)prevSeg.getRightChild())->getT().getRightDown()==trap)
            ((DagNodeArea*)prevSeg.getRightChild())->getT().setRightDown(*iterators[1]);
        if(trap.getLeftDown()!= *lastDeleted && trap.getLeftDown().getRightUp()==trap)
            trap.getLeftDown().setRightUp(*iterators[1]);
        if(trap.getLeftDown()!= *lastDeleted && trap.getLeftDown().getRightDown()==trap)
            trap.getLeftDown().setRightDown(*iterators[1]);
    }
    else{ //under rightp case
        TrapezoidalMap::merge(((DagNodeArea*)prevSeg.getRightChild())->getT(),traps[1]);
        gas::bind(std::get<0>(traps[0]), std::get<1>(traps[0]), std::get<2>(traps[0]), std::get<3>(traps[0]), *this);
        upper = false;
        iterators[0] = &*(--itr);
        iterators[1] = &((DagNodeArea*)prevSeg.getRightChild())->getT();
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
        iterators[1]->setLeftUp((trap.getLeftUp().getRightp().x()<iterators[1]->getRightp().x())?trap.getLeftUp():((DagNodeArea*)prevSeg.getRightChild())->getT());
        iterators[1]->setLeftDown(trap.getLeftDown());
    }
    else{
        iterators[0]->setLeftUp(trap.getLeftUp());
        iterators[0]->setLeftDown((trap.getLeftDown().getRightp().x()<iterators[0]->getRightp().x())?trap.getLeftDown():((DagNodeArea*)prevSeg.getLeftChild())->getT());
    }
    rightMulti(trap,iterators,triangleOver,linear); //neighborhood update


    lastDeleted = &trap;

    TrapezoidalMap::removeTrapezoid(trap.getItr());
    return iterators;
}

/**
 * @brief insert a new portion of the trapezoidal map, substituing the one obtained by following the segment
 * @param reference to trapezoid founded by point location
 * @param reference to new segment
 * @param previus created type Y node
 * @param last deleted trapezoid
 * @return vector of pointers to inserted trapezoids
 */
std::vector<Trapezoid*> TrapezoidalMap::createIntermediate(Trapezoid &trap, const cg3::Segment2d &segment, DagNodeSegment &prevSeg, Trapezoid *&lastDeleted)
{
    std::list<Trapezoid>& ref = traps();
    std::list<Trapezoid>::iterator itr;
    bool upper;
    std::vector<Trapezoid*> iterators(2);
    std::vector<cg3::Point2d> ints = {gas::intersection(segment, trap.getTop().p1()), gas::intersection(segment, trap.getTop().p2())}; //compute intersections
    std::vector<std::tuple<cg3::Segment2d, cg3::Segment2d, cg3::Point2d, cg3::Point2d>> traps = { //making trapezoids templates
        std::make_tuple(cg3::Segment2d(trap.getTop().p1(),trap.getTop().p2()),cg3::Segment2d(ints[0], ints[1]),(trap.getLeftp().y()<ints[0].y())?ints[0]:trap.getLeftp(), (trap.getRightp().y()<ints[1].y())?ints[1]:trap.getRightp()),
        std::make_tuple(cg3::Segment2d(ints[0], ints[1]),cg3::Segment2d(trap.getBottom().p1(), trap.getBottom().p2()),(trap.getLeftp().y()>ints[0].y())?ints[0]:trap.getLeftp(), (trap.getRightp().y()>ints[1].y())?ints[1]:trap.getRightp()),
    };
    itr = ref.end();
    if(ints[0].y()>trap.getLeftp().y()){ //above rightp case
        TrapezoidalMap::merge(((DagNodeArea*)prevSeg.getLeftChild())->getT(), traps[0]);
        gas::bind(std::get<0>(traps[1]), std::get<1>(traps[1]), std::get<2>(traps[1]), std::get<3>(traps[1]),*this);
        upper = true;
        iterators[1] = &*(--itr);
        iterators[0] = &((DagNodeArea*)prevSeg.getLeftChild())->getT();
    }
    else{ //under rightp case
        TrapezoidalMap::merge(((DagNodeArea*)prevSeg.getRightChild())->getT(),traps[1]);
        gas::bind(std::get<0>(traps[0]), std::get<1>(traps[0]), std::get<2>(traps[0]), std::get<3>(traps[0]), *this);
        upper = false;
        iterators[0] = &*(--itr);
        iterators[1] = &((DagNodeArea*)prevSeg.getRightChild())->getT();
    }
    TrapezoidalMap::updateNeighborsMultiTrapezoid(trap, iterators, 1, &prevSeg, lastDeleted); //neighborhood updates
    if(upper){
        iterators[0]->setRightUp(trap.getRightUp());
        iterators[0]->setRightDown((iterators[0]->getBottom().p2().y()<trap.getRightp().y())?trap.getRightDown():trap.getRightUp());
        iterators[1]->setNeighbors((trap.getLeftUp().getRightp().x()<iterators[1]->getRightp().x())?trap.getLeftUp():((DagNodeArea*)prevSeg.getRightChild())->getT(), trap.getLeftDown(),
                                   (iterators[0]->getBottom().p2().y()<trap.getRightp().y())?trap.getRightDown():trap.getRightUp(),
                                    trap.getRightDown());//

    }
    else{
        iterators[0]->setNeighbors(trap.getLeftUp(), (trap.getLeftDown().getRightp().x()<iterators[0]->getRightp().x())?trap.getLeftDown():((DagNodeArea*)prevSeg.getLeftChild())->getT(),trap.getRightUp(),
                                   (iterators[1]->getTop().p2().y()>trap.getRightp().y())?trap.getRightUp():trap.getRightDown());
        iterators[1]->setRightUp((iterators[1]->getTop().p2().y()>trap.getRightp().y())?trap.getRightUp():trap.getRightDown());
        iterators[1]->setRightDown(trap.getRightDown());
    }

    lastDeleted = &trap;
    removeTrapezoid(trap.getItr());
    return iterators;
}

/**
 * @return trapezoid selected by point location
 */
std::list<Trapezoid>::iterator TrapezoidalMap::getQuery() const
{
    return query;
}

/**
 * @brief sets the selected trapezoid obtained by query point
 * @param iterator of queryPoint result trapezoid
 */
void TrapezoidalMap::setQuery(std::list<Trapezoid>::iterator value)
{
    query = value;
}

/**
 * @brief clear the whole trapezoidal map
 */
void TrapezoidalMap::clear()
{
    trapezoids.clear();
    query = trapezoids.end();
}


/**
 * @brief obtain current trapezoidal map
 * @return reference to list of trapezoids, composing the trapezoidal map
 */
const std::list<Trapezoid>& TrapezoidalMap::getTrapezoids() const
{
    return trapezoids ;
}


/**
 * @brief obtain current trapezoidal map
 * @return reference to list of trapezoids, composing the trapezoidal map
 */
std::list<Trapezoid> &TrapezoidalMap::traps()
{
    return trapezoids;
}


/**
 * @brief add a trapezoid in trapezoidal map
 * @param reference to trapezoid
 */
void TrapezoidalMap::addTrapezoid(Trapezoid& t)
{
    trapezoids.push_back(t);
    trapezoids.back().setItr(prev(trapezoids.end()));
}


/**
 * @brief remove a trapezoid from trapezoidal map
 * @param iterator of trapezoid that should be removed
 */
void TrapezoidalMap::removeTrapezoid(std::list<Trapezoid>::iterator itr) //O(1) Delete a list element
{
    std::list<Trapezoid>& ref = TrapezoidalMap::traps();
    ref.erase(itr);

}




/**
 * @brief update all neighborhood of neighbors of old trapezoid, making them point to new ones (single insertion case)
 * @param reference to old trapezoid
 * @param list of pointers to new trapezoids
 */
void TrapezoidalMap::updateNeighbors(const Trapezoid &t, std::vector<Trapezoid*>& heirs)
{
    Trapezoid* leftUp = &t.getLeftUp();
    Trapezoid* leftDown = &t.getLeftDown();
    Trapezoid* rightUp = &t.getRightUp();
    Trapezoid* rightDown = &t.getRightDown();
    if(leftUp!=nullptr && leftDown!=nullptr){
        if(leftUp->getRightUp()==t)
            leftUp->setRightUp(*heirs[0]);
        if(leftUp->getRightDown()==t)
            leftUp->setRightDown(*heirs[0]);
        if(leftDown->getRightUp()==t)
            leftDown->setRightUp(*heirs[0]);
        if(leftDown->getRightDown()==t)
            leftDown->setRightDown(*heirs[0]);
    }
    if(rightUp !=nullptr && rightDown !=nullptr){
        if(rightDown->getLeftUp()==t)
            rightDown->setLeftUp(*heirs[3]);
        if(rightDown->getLeftDown()==t)
            rightDown->setLeftDown(*heirs[3]);
        if(rightUp->getLeftUp()==t)
            rightUp->setLeftUp(*heirs[3]);
        if(rightUp->getLeftDown()==t)
            rightUp->setLeftDown(*heirs[3]);
    }
}

/**
 * @brief update all neighborhood of neighbors of old trapezoid, making them point to new ones (handles all cases of multiple insertion)
 * @param reference to old trapezoid
 * @param list of pointers to new trapezoids
 * @param type of neighborhood update
 * @param previous type Y node created
 * @param last deleted trapezoid
 */
void TrapezoidalMap::updateNeighborsMultiTrapezoid(Trapezoid &t, std::vector<Trapezoid *> &heirs, int type, DagNodeSegment* prevSeg, Trapezoid*& lastDeleted)
{
    enum insertionType {leftmost = 0, intermediate = 1, rightmost = 2};
    Trapezoid* leftUp = &t.getLeftUp();
    Trapezoid* leftDown = &t.getLeftDown();
    Trapezoid* rightUp = &t.getRightUp();
    Trapezoid* rightDown = &t.getRightDown();
    bool aboverightp;
    if(type == leftmost){
        //from new to old neighbors
        (heirs[1]->getBottom().p2().y()>t.getRightp().y())?aboverightp=true:aboverightp=false; //check if the segment will pass under or above the current rightp

        //from old to new
        if(leftUp!=nullptr && leftDown!=nullptr){
            if(leftUp->getRightUp()==t)
                leftUp->setRightUp(*heirs[0]);
            if(leftUp->getRightDown()==t)
                leftUp->setRightDown(*heirs[0]);
            if(leftDown->getRightUp()==t)
                leftDown->setRightUp(*heirs[0]);
            if(leftDown->getRightDown()==t)
                leftDown->setRightDown(*heirs[0]);
        }
        if(aboverightp){
            if(rightDown->getLeftUp()==t)
                rightDown->setLeftUp(*heirs[2]);
            if(rightDown->getLeftDown()==t)
                rightDown->setLeftDown(*heirs[2]);
            if(rightUp->getLeftUp()==t)
                rightUp->setLeftUp(*heirs[1]);
            if(rightUp->getLeftDown()==t)
                rightUp->setLeftDown(*heirs[2]);
        }
        else{
            if(rightDown->getLeftUp()==t)
                rightDown->setLeftUp(*heirs[1]);
            if(rightDown->getLeftDown()==t)
                rightDown->setLeftDown(*heirs[2]);
            if(rightUp->getLeftUp()==t)
                rightUp->setLeftUp(*heirs[1]);
            if(rightUp->getLeftDown()==t)
                rightUp->setLeftDown(*heirs[1]);
        }
        heirs[0]->setNeighbors(t.getLeftUp(),t.getLeftDown(),*heirs[1], *heirs[2]);
        heirs[1]->setNeighbors(*heirs[0],*heirs[0], t.getRightUp(), (aboverightp)?t.getRightUp():t.getRightDown());
        heirs[2]->setNeighbors(*heirs[0],*heirs[0], (aboverightp)?t.getRightUp():t.getRightDown(), t.getRightDown());

    }
    if(type == intermediate){
        if(heirs[0]->getTop().p1()==t.getTop().p1()){ //merged trapezoid is the lower one
            if(t.getLeftUp()==*lastDeleted)
                t.setLeftUp(((DagNodeArea*)prevSeg->getLeftChild())->getT());
            if(((DagNodeArea*)prevSeg->getLeftChild())->getT().getRightUp()==t)
                ((DagNodeArea*)prevSeg->getLeftChild())->getT().setRightUp(*heirs[0]);
            if(((DagNodeArea*)prevSeg->getLeftChild())->getT().getRightDown()==t)
                ((DagNodeArea*)prevSeg->getLeftChild())->getT().setRightDown(*heirs[0]);
            if(t.getLeftUp()!= *lastDeleted &&  t.getLeftUp().getRightUp()==t)
                t.getLeftUp().setRightUp(*heirs[0]);
            if(t.getLeftUp()!= *lastDeleted && t.getLeftUp().getRightDown()==t)
                t.getLeftUp().setRightDown(*heirs[0]);
            if(rightUp!=nullptr && rightDown!=nullptr){
                if(heirs[0]->getBottom().p2().y()>t.getRightp().y()){
                    if(rightUp->getLeftUp()==t)
                        rightUp->setLeftUp(*heirs[0]);
                    if(rightUp->getLeftDown()==t)
                        rightUp->setLeftDown(*heirs[1]);
                    if(rightDown->getLeftUp()==t)
                        rightDown->setLeftUp(*heirs[1]);
                    if(rightDown->getLeftDown()==t)
                        rightDown->setLeftDown(*heirs[1]);
                }
                else{
                    if(rightUp->getLeftUp()==t)
                        rightUp->setLeftUp(*heirs[0]);
                    if(rightUp->getLeftDown()==t)
                        rightUp->setLeftDown(*heirs[0]);
                    if(rightDown->getLeftUp()==t)
                        rightDown->setLeftUp(*heirs[0]);
                    if(rightDown->getLeftDown()==t)
                        rightDown->setLeftDown(*heirs[1]);
                }

            }

        }
        else{ //merged trapezoid is the upper one
            if(t.getLeftDown()==*lastDeleted)
                t.setLeftDown(((DagNodeArea*)prevSeg->getRightChild())->getT());
            if(((DagNodeArea*)prevSeg->getRightChild())->getT().getRightUp()==t)
                ((DagNodeArea*)prevSeg->getRightChild())->getT().setRightUp(*heirs[1]);
            if(((DagNodeArea*)prevSeg->getRightChild())->getT().getRightDown()==t)
                ((DagNodeArea*)prevSeg->getRightChild())->getT().setRightDown(*heirs[1]);
            if(t.getLeftDown()!= *lastDeleted && t.getLeftDown().getRightUp()==t)
                t.getLeftDown().setRightUp(*heirs[1]);
            if(t.getLeftDown()!= *lastDeleted && t.getLeftDown().getRightDown()==t)
                t.getLeftDown().setRightDown(*heirs[1]);
            if(rightUp!=nullptr && rightDown!=nullptr){
                if(heirs[0]->getBottom().p2().y()>t.getRightp().y()){
                    if(rightUp->getLeftUp()==t)
                        rightUp->setLeftUp(*heirs[0]);
                    if(rightUp->getLeftDown()==t)
                        rightUp->setLeftDown(*heirs[1]);
                    if(rightDown->getLeftUp()==t)
                        rightDown->setLeftUp(*heirs[1]);
                    if(rightDown->getLeftDown()==t)
                        rightDown->setLeftDown(*heirs[1]);
                }
                else{
                    if(rightUp->getLeftUp()==t)
                        rightUp->setLeftUp(*heirs[0]);
                    if(rightUp->getLeftDown()==t)
                        rightUp->setLeftDown(*heirs[0]);
                    if(rightDown->getLeftUp()==t)
                        rightDown->setLeftUp(*heirs[0]);
                    if(rightDown->getLeftDown()==t)
                        rightDown->setLeftDown(*heirs[1]);
                }
            }


        }


    }
    if(type == rightmost){
        if(heirs[0]->getTop().p1()==t.getTop().p1()){ //merged trapezoid is the lower one
            if(((DagNodeArea*)prevSeg->getLeftChild())->getT().getRightUp()==t)
                ((DagNodeArea*)prevSeg->getLeftChild())->getT().setRightUp(*heirs[0]);
            if(((DagNodeArea*)prevSeg->getLeftChild())->getT().getRightDown()==t)
                ((DagNodeArea*)prevSeg->getLeftChild())->getT().setRightDown(*heirs[0]);
            if(t.getLeftUp()!= *lastDeleted &&  t.getLeftUp().getRightUp()==t)
                t.getLeftUp().setRightUp(*heirs[0]);
            if(t.getLeftUp()!= *lastDeleted && t.getLeftUp().getRightDown()==t)
                t.getLeftUp().setRightDown(*heirs[0]);
        }
        else{ //merged trapezoid is the upper one
            if(((DagNodeArea*)prevSeg->getRightChild())->getT().getRightUp()==t)
                ((DagNodeArea*)prevSeg->getRightChild())->getT().setRightUp(*heirs[1]);
            if(((DagNodeArea*)prevSeg->getRightChild())->getT().getRightDown()==t)
                ((DagNodeArea*)prevSeg->getRightChild())->getT().setRightDown(*heirs[1]);
            if(t.getLeftDown()!= *lastDeleted && t.getLeftDown().getRightUp()==t)
                t.getLeftDown().setRightUp(*heirs[1]);
            if(t.getLeftDown()!= *lastDeleted && t.getLeftDown().getRightDown()==t)
                t.getLeftDown().setRightDown(*heirs[1]);
        }
        if(rightUp!=nullptr && rightDown!=nullptr){
            if(rightUp->getLeftUp()==t)
                rightUp->setLeftUp(*heirs[2]);
            if(rightUp->getLeftDown()==t)
                rightUp->setLeftDown(*heirs[2]);
            if(rightDown->getLeftUp()==t)
                rightDown->setLeftUp(*heirs[2]);
            if(rightDown->getLeftDown()==t)
                rightDown->setLeftDown(*heirs[2]);
        }
    }

}

/**
 * @brief neighborhood update in left degenerate single insertion
 * @param reference to old trapezoid
 * @param new trapezoids
 * @param check which trapezoid has no left neighbors (it is closed in an angle)
 * @param check if the segment is collinear with the one that has common right point
 */
void TrapezoidalMap::leftSingle(Trapezoid & trap, std::vector<Trapezoid *> & iterators, bool triangleOver, bool linear)
{
    if(!trap.rightUpNull() && trap.getRightUp().getLeftUp()==trap)
        trap.getRightUp().setLeftUp(*iterators[2]);
    if(!trap.rightUpNull() && trap.getRightUp().getLeftDown()==trap)
        trap.getRightUp().setLeftDown(*iterators[2]);
    if(!trap.rightDownNull() && trap.getRightDown().getLeftUp()==trap)
        trap.getRightDown().setLeftUp(*iterators[2]);
    if(!trap.rightDownNull() && trap.getRightDown().getLeftDown()==trap)
        trap.getRightDown().setLeftDown(*iterators[2]);
    if(trap.getTop().p1()!=trap.getBottom().p1()){
        if(!linear){
            if(!trap.leftUpNull() && trap.getLeftUp().getRightUp()==trap && !triangleOver)
                trap.getLeftUp().setRightUp(*iterators[0]);
            if(!trap.leftUpNull() && trap.getLeftUp().getRightDown()==trap && !triangleOver)
                trap.getLeftUp().setRightDown(*iterators[0]);

            if(!trap.leftDownNull() && trap.getLeftDown().getRightDown()==trap && triangleOver)
                trap.getLeftDown().setRightDown(*iterators[1]);
            if(!trap.leftDownNull() && trap.getLeftDown().getRightUp()==trap && triangleOver)
                trap.getLeftDown().setRightUp(*iterators[1]);

            if(!triangleOver){
                iterators[0]->setNeighbors(trap.getLeftUp(),trap.getLeftUp(), *iterators[2], *iterators[2]);
                iterators[1]->setLeftUpNull();
                iterators[1]->setLeftDownNull();


            }else{
                iterators[0]->setLeftUpNull();
                iterators[0]->setLeftDownNull();
                iterators[1]->setNeighbors(trap.getLeftDown(),trap.getLeftDown(), *iterators[2],*iterators[2]);
            }
        }else{
            if(!trap.leftUpNull() && trap.getLeftUp().getRightUp()==trap )
                trap.getLeftUp().setRightUp(*iterators[0]);
            if(!trap.leftUpNull() && trap.getLeftUp().getRightDown()==trap)
                trap.getLeftUp().setRightDown(*iterators[0]);

            if(!trap.leftDownNull() && trap.getLeftDown().getRightDown()==trap )
                trap.getLeftDown().setRightDown(*iterators[1]);
            if(!trap.leftDownNull()  && trap.getLeftDown().getRightUp()==trap)
                trap.getLeftDown().setRightUp(*iterators[1]);

            iterators[0]->setNeighbors(trap.getLeftUp(),trap.getLeftUp(),*iterators[2], *iterators[2]); //A
            iterators[1]->setNeighbors(trap.getLeftDown(),trap.getLeftDown(),*iterators[2], *iterators[2]); //B
        }

    }
    else{
        iterators[0]->setRightUp(*iterators[2]);
        iterators[0]->setRightDown(*iterators[2]);
        iterators[1]->setRightUp(*iterators[2]);
        iterators[1]->setRightDown(*iterators[2]);
        iterators[0]->setLeftUpNull();
        iterators[0]->setLeftDownNull();
        iterators[1]->setLeftUpNull();
        iterators[1]->setLeftDownNull();
    }
}

/**
 * @brief neighborhood update in right degenerate single insertion
 * @param reference to old trapezoid
 * @param new trapezoids
 * @param check which trapezoid has no right neighbors (it is closed in an angle)
 * @param check if the segment is collinear with the one that has common left point
 */
void TrapezoidalMap::rightSingle(Trapezoid & trap, std::vector<Trapezoid *> & iterators, bool triangleOver, bool linear)
{
    if(!trap.leftUpNull() && trap.getLeftUp().getRightUp()==trap)
        trap.getLeftUp().setRightUp(*iterators[0]);
    if(!trap.leftUpNull() && trap.getLeftUp().getRightDown()==trap)
        trap.getLeftUp().setRightDown(*iterators[0]);
    if(!trap.leftDownNull() && trap.getLeftDown().getRightUp()==trap)
        trap.getLeftDown().setRightUp(*iterators[0]);
    if(!trap.leftDownNull() && trap.getLeftDown().getRightDown()==trap)
        trap.getLeftDown().setRightDown(*iterators[0]);
    if(trap.getTop().p2()!=trap.getBottom().p2()){
        if(!linear){
            if(!trap.rightUpNull() && trap.getRightUp().getLeftUp()==trap && !triangleOver)
                trap.getRightUp().setLeftUp(*iterators[1]);
            if(!trap.rightUpNull() && trap.getRightUp().getLeftDown()==trap && !triangleOver)
                trap.getRightUp().setLeftDown(*iterators[1]);
            if(!trap.rightDownNull() && trap.getRightDown().getLeftDown()==trap && triangleOver)
                trap.getRightDown().setLeftDown(*iterators[2]);
            if(!trap.rightDownNull() && trap.getRightDown().getLeftUp()==trap && triangleOver)
                trap.getRightDown().setLeftUp(*iterators[2]);
            if(!triangleOver){
                iterators[1]->setRightUp(trap.getRightUp());
                iterators[1]->setRightDown(trap.getRightUp());
                iterators[2]->setRightUpNull();
                iterators[2]->setRightDownNull();
            }else{
                iterators[1]->setRightUpNull();
                iterators[1]->setRightDownNull();
                iterators[2]->setRightUp(trap.getRightDown());
                iterators[2]->setRightDown(trap.getRightDown());
            }
        }else{
            if(!trap.rightUpNull() && trap.getRightUp().getLeftUp()==trap )
                trap.getRightUp().setLeftUp(*iterators[1]);
            if(!trap.rightUpNull() && trap.getRightUp().getLeftDown()==trap)
                trap.getRightUp().setLeftDown(*iterators[1]);

            if(!trap.rightDownNull() && trap.getRightDown().getLeftDown()==trap )
                trap.getRightDown().setLeftDown(*iterators[2]);
            if(!trap.rightDownNull() && trap.getRightDown().getLeftUp()==trap )
                trap.getRightDown().setLeftUp(*iterators[2]);

            iterators[1]->setRightUp(trap.getRightUp());
            iterators[1]->setRightDown(trap.getRightUp());
            iterators[2]->setRightUp(trap.getRightDown());
            iterators[2]->setRightDown(trap.getRightDown());

        }
    }
    else{
        iterators[1]->setRightUpNull();
        iterators[1]->setRightDownNull();
        iterators[2]->setRightUpNull();
        iterators[2]->setRightDownNull();
    }
}

/**
 * @brief neighborhood update in right degenerate single insertion
 * @param reference to old trapezoid
 * @param new trapezoids
 * @param check which trapezoid has no left neighbors (it is closed in an angle)
 * @param check which trapezoid has no right neighbors (it is closed in an angle)
 * @param check if the segment is collinear with the one that has common right point
 * @param check if the segment is collinear with the one that has common left point
 */
void TrapezoidalMap::bothDegenerate(Trapezoid & trap, std::vector<Trapezoid *> & iterators, bool triangleOverLeft, bool triangleOverRight, bool linearRight, bool linearLeft)
{
    if(trap.getTop().p1()!=trap.getBottom().p1()){
        if(!linearLeft){
            if(!trap.leftUpNull() && trap.getLeftUp().getRightUp()==trap && !triangleOverLeft)
                trap.getLeftUp().setRightUp(*iterators[0]);
            if(!trap.leftUpNull() && trap.getLeftUp().getRightDown()==trap && !triangleOverLeft)
                trap.getLeftUp().setRightDown(*iterators[0]);
            if(!trap.leftDownNull() && trap.getLeftDown().getRightDown()==trap && triangleOverLeft)
                trap.getLeftDown().setRightDown(*iterators[1]);
            if(!trap.leftDownNull() && trap.getLeftDown().getRightUp()==trap && triangleOverLeft)
                trap.getLeftDown().setRightUp(*iterators[1]);
            if(!triangleOverLeft){
                iterators[0]->setLeftUp(trap.getLeftUp());
                iterators[0]->setLeftDown(trap.getLeftUp());
                iterators[1]->setLeftUpNull();
                iterators[1]->setLeftDownNull();
            }else{
                iterators[0]->setLeftUpNull();
                iterators[0]->setLeftDownNull();
                iterators[1]->setLeftUp(trap.getLeftUp());
                iterators[1]->setLeftDown(trap.getLeftUp());
            }
        }else{
            if(!trap.leftUpNull() && trap.getLeftUp().getRightUp()==trap )
                trap.getLeftUp().setRightUp(*iterators[0]);
            if(!trap.leftUpNull() && trap.getLeftUp().getRightDown()==trap)
                trap.getLeftUp().setRightDown(*iterators[0]);

            if(!trap.leftDownNull() && trap.getLeftDown().getRightDown()==trap )
                trap.getLeftDown().setRightDown(*iterators[1]);
            if(!trap.leftDownNull() && trap.getLeftDown().getRightUp()==trap )
                trap.getLeftDown().setRightUp(*iterators[1]);

            iterators[0]->setLeftUp(trap.getLeftUp());
            iterators[0]->setLeftDown(trap.getLeftUp());
            iterators[1]->setLeftUp(trap.getLeftDown());
            iterators[1]->setLeftDown(trap.getLeftDown());
        }
    }
    else{
        iterators[0]->setLeftUpNull();
        iterators[0]->setLeftDownNull();
        iterators[1]->setLeftUpNull();
        iterators[1]->setLeftDownNull();
    }
    if(trap.getTop().p2()!=trap.getBottom().p2()){
        if(!linearRight){
            if(!trap.rightUpNull() && trap.getRightUp().getLeftUp()==trap && !triangleOverRight)
                trap.getRightUp().setLeftUp(*iterators[0]);
            if(!trap.rightUpNull() && trap.getRightUp().getLeftDown()==trap && !triangleOverRight)
                trap.getRightUp().setLeftDown(*iterators[0]);
            if(!trap.rightDownNull() && trap.getRightDown().getLeftDown()==trap && triangleOverRight)
                trap.getRightDown().setLeftDown(*iterators[1]);
            if(!trap.rightDownNull() && trap.getRightDown().getLeftUp()==trap && triangleOverRight)
                trap.getRightDown().setLeftUp(*iterators[1]);
            if(!triangleOverRight){
                iterators[0]->setRightUp(trap.getRightUp());
                iterators[0]->setRightDown(trap.getRightUp());
                iterators[1]->setRightUpNull();
                iterators[1]->setRightDownNull();
            }else{
                iterators[0]->setRightUpNull();
                iterators[0]->setRightDownNull();
                iterators[1]->setRightUp(trap.getRightDown());
                iterators[1]->setRightDown(trap.getRightDown());
            }
        }else{
            if(!trap.rightUpNull() && trap.getRightUp().getLeftUp()==trap )
                trap.getRightUp().setLeftUp(*iterators[0]);
            if(!trap.rightUpNull() && trap.getRightUp().getLeftDown()==trap)
                trap.getRightUp().setLeftDown(*iterators[0]);

            if(!trap.rightDownNull() && trap.getRightDown().getLeftDown()==trap )
                trap.getRightDown().setLeftDown(*iterators[1]);
            if(!trap.rightDownNull() && trap.getRightDown().getLeftUp()==trap )
                trap.getRightDown().setLeftUp(*iterators[1]);

            iterators[0]->setRightUp(trap.getRightUp());
            iterators[0]->setRightDown(trap.getRightUp());
            iterators[1]->setRightUp(trap.getRightDown());
            iterators[1]->setRightDown(trap.getRightDown());
        }
    }
    else{
        iterators[0]->setRightUpNull();
        iterators[0]->setRightDownNull();
        iterators[1]->setRightUpNull();
        iterators[1]->setRightDownNull();
    }
}

/**
 * @brief neighborhood update in right degenerate single insertion
 * @param reference to old trapezoid
 * @param new trapezoids
 * @param check if the segment passes above or under old trapezoid's rightp
 * @param check which trapezoid has no right neighbors (it is closed in an angle)
 * @param check if the segment is collinear with the one that has common left point
 */
void TrapezoidalMap::leftMulti(Trapezoid & trap, std::vector<Trapezoid *> & iterators, bool aboverightp, bool triangleOver, bool linear)
{
    Trapezoid* rightUp = &trap.getRightUp();
    Trapezoid* rightDown = &trap.getRightDown();
    if(trap.getTop().p1()!=trap.getBottom().p1()){
        if(!linear){
            if(!trap.leftUpNull() && trap.getLeftUp().getRightUp()==trap && !triangleOver)
                trap.getLeftUp().setRightUp(*iterators[0]);
            if(!trap.leftUpNull() && trap.getLeftUp().getRightDown()==trap &&!triangleOver)
                trap.getLeftUp().setRightDown(*iterators[0]);

            if(!trap.leftDownNull() && trap.getLeftDown().getRightUp()==trap && triangleOver)
                trap.getLeftDown().setRightUp(*iterators[1]);
            if(!trap.leftDownNull() && trap.getLeftDown().getRightDown()==trap && triangleOver)
                trap.getLeftDown().setRightDown(*iterators[1]);
            if(!triangleOver){
                iterators[0]->setNeighbors(trap.getLeftUp(),trap.getLeftUp(), trap.getRightUp(), (aboverightp)?trap.getRightUp():trap.getRightDown());// be careful on right side neighbors
                iterators[1]->setLeftUpNull();
                iterators[1]->setLeftDownNull();
                iterators[1]->setRightUp((aboverightp)?trap.getRightUp():trap.getRightDown());
                iterators[1]->setRightDown(trap.getRightDown());
            }else{
                iterators[0]->setLeftUpNull();
                iterators[0]->setLeftDownNull();
                iterators[0]->setRightUp(trap.getRightUp());
                iterators[0]->setRightDown((aboverightp)?trap.getRightUp():trap.getRightDown());
                iterators[1]->setNeighbors(trap.getLeftDown(),trap.getLeftDown(), (aboverightp)?trap.getRightUp():trap.getRightDown(), trap.getRightDown());//also there
            }
        }
        else{
            if(!trap.leftUpNull() && trap.getLeftUp().getRightUp()==trap )
                trap.getLeftUp().setRightUp(*iterators[0]);
            if(!trap.leftUpNull() && trap.getLeftUp().getRightDown()==trap )
                trap.getLeftUp().setRightDown(*iterators[0]);
            if(!trap.leftDownNull() && trap.getLeftDown().getRightUp()==trap )
                trap.getLeftDown().setRightUp(*iterators[1]);
            if(!trap.leftDownNull() && trap.getLeftDown().getRightDown()==trap )
                trap.getLeftDown().setRightDown(*iterators[1]);

            iterators[0]->setNeighbors(trap.getLeftUp(),trap.getLeftUp(), trap.getRightUp(), (aboverightp)?trap.getRightUp():trap.getRightDown());// be careful on right side neighbors
            iterators[1]->setNeighbors(trap.getLeftDown(),trap.getLeftDown(), (aboverightp)?trap.getRightUp():trap.getRightDown(), trap.getRightDown());//also there
        }
    }
    else{
        iterators[0]->setRightUp(trap.getRightUp());
        iterators[0]->setRightDown((aboverightp)?trap.getRightUp():trap.getRightDown());
        iterators[1]->setRightUp((aboverightp)?trap.getRightUp():trap.getRightDown());
        iterators[1]->setRightDown(trap.getRightDown());
        iterators[0]->setLeftUpNull();
        iterators[0]->setLeftDownNull();
        iterators[1]->setLeftUpNull();
        iterators[1]->setLeftDownNull();
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
}

/**
 * @brief neighborhood update in right degenerate single insertion
 * @param reference to old trapezoid
 * @param new trapezoids
 * @param check which trapezoid has no right neighbors (it is closed in an angle)
 * @param check if the segment is collinear with the one that has common left point
 */
void TrapezoidalMap::rightMulti(Trapezoid & trap, std::vector<Trapezoid *> & iterators, bool triangleOver, bool linear)
{
    if(trap.getTop().p2()!=trap.getBottom().p2()){
        if(!linear){
            if(!trap.rightUpNull() && trap.getRightUp().getLeftUp()==trap && !triangleOver)
                trap.getRightUp().setLeftUp(*iterators[0]);
            if(!trap.rightUpNull() && trap.getRightUp().getLeftDown()==trap && !triangleOver)
                trap.getRightUp().setLeftDown(*iterators[0]);
            if(!trap.rightDownNull() && trap.getRightDown().getLeftDown()==trap && triangleOver)
                trap.getRightDown().setLeftDown(*iterators[1]);
            if(!trap.rightDownNull() && trap.getRightDown().getLeftUp()==trap && triangleOver)
                trap.getRightDown().setLeftUp(*iterators[1]);
            if(!triangleOver){
                iterators[0]->setRightUp(trap.getRightUp());
                iterators[0]->setRightDown(trap.getRightUp());
                iterators[1]->setRightUpNull();
                iterators[1]->setRightDownNull();
            }else{
                iterators[0]->setRightUpNull();
                iterators[0]->setRightDownNull();
                iterators[1]->setRightUp(trap.getRightDown());
                iterators[1]->setRightDown(trap.getRightDown());
            }
        }else{
            if(!trap.rightUpNull() && trap.getRightUp().getLeftUp()==trap )
                trap.getRightUp().setLeftUp(*iterators[0]);
            if(!trap.rightUpNull() && trap.getRightUp().getLeftDown()==trap)
                trap.getRightUp().setLeftDown(*iterators[0]);

            if(!trap.rightDownNull() && trap.getRightDown().getLeftDown()==trap )
                trap.getRightDown().setLeftDown(*iterators[1]);
            if(!trap.rightDownNull() && trap.getRightDown().getLeftUp()==trap )
                trap.getRightDown().setLeftUp(*iterators[1]);

            iterators[0]->setRightUp(trap.getRightUp());
            iterators[0]->setRightDown(trap.getRightUp());
            iterators[1]->setRightUp(trap.getRightDown());
            iterators[1]->setRightDown(trap.getRightDown());
        }
    }else{
        iterators[0]->setRightUpNull();
        iterators[0]->setRightDownNull();
        iterators[1]->setRightUpNull();
        iterators[1]->setRightDownNull();

    }

}
/**
 * @brief merge two trapezoid
 * @param trapezoid that have to be enlarged
 * @param new ipothetical trapezoid, containing infos to enlarge left trapezoid
 */
void TrapezoidalMap::merge(Trapezoid &tLeft, std::tuple<cg3::Segment2d, cg3::Segment2d, cg3::Point2d, cg3::Point2d>& tRight)
{
    cg3::Point2d rightp = std::get<3>(tRight);
    tLeft.setRightp(rightp);
    tLeft.setTop(cg3::Segment2d(tLeft.getTop().p1(), std::get<0>(tRight).p2()));
    tLeft.setBottom(cg3::Segment2d(tLeft.getBottom().p1(), std::get<1>(tRight).p2()));
}




