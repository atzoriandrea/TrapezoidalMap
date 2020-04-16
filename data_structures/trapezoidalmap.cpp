#include "dag_dataset.h"
#include "trapezoidalmap.h"

//std::list<Trapezoid> TrapezoidalMap::trapezoids = {};
cg3::Segment2d topbb = cg3::Segment2d(cg3::Point2d(-1000000.0, 1000000.0),cg3::Point2d(1000000.0, 1000000.0));
cg3::Segment2d botbb = cg3::Segment2d(cg3::Point2d(-1000000.0, -1000000.0),cg3::Point2d(1000000.0, -1000000.0));
cg3::Point2d leftp = cg3::Point2d(-1000000.0, 1000000.0);
cg3::Point2d rightp = cg3::Point2d(1000000.0, 1000000.0);
Trapezoid TrapezoidalMap::boundingBox = Trapezoid(topbb, botbb, leftp, rightp,(DagNodeArea*&)Dag::dagRef());
std::list<Trapezoid> TrapezoidalMap::trapezoids = {boundingBox};
DagNode* Dag::dag = new DagNodeArea(TrapezoidalMap::getBoundingBox().setItr(TrapezoidalMap::getTrapezoids().begin()));
TrapezoidalMap::TrapezoidalMap(){

}
std::list<Trapezoid>& TrapezoidalMap::getTrapezoids()
{
    return trapezoids;
}

Trapezoid &TrapezoidalMap::getTrapezoid()
{
    return *trapezoids.begin();
}


void TrapezoidalMap::addTrapezoids(std::vector<Trapezoid> traps)
{
    //std::copy( vec.begin(), vec.end(), std::inserter( trapezoids, trapezoids.end() ) );
    for (unsigned int i = 0; i<traps.size();i++) {
        TrapezoidalMap::addTrapezoid(traps[i]);
        TrapezoidalMap::trapezoids.back().setItr(prev(TrapezoidalMap::trapezoids.end()));
    }

}

void TrapezoidalMap::addTrapezoid(Trapezoid& t)
{
    trapezoids.push_back(t);
    trapezoids.back().setItr(prev(trapezoids.end()));
}

void TrapezoidalMap::removeTrapezoid(std::list<Trapezoid>::iterator itr) //O(1) Delete a list element
{
    if(TrapezoidalMap::getTrapezoids().begin() == itr){
        trapezoids.erase(itr);
        return;
    }

    std::list<Trapezoid>::iterator before;
    std::list<Trapezoid>::iterator after;
    before = prev(itr);
    after = next(itr);
    before._M_node->_M_next=after._M_node;
    after._M_node->_M_prev=before._M_node;
    itr._M_node->_M_unhook();
    itr._M_node->~_List_node_base();
    trapezoids.erase(itr);
}

std::list<Trapezoid>& TrapezoidalMap::init(Trapezoid &ref)
{
    trapezoids.push_back(ref);
    trapezoids.back().setItr(--trapezoids.end());
    return trapezoids;
}

Trapezoid &TrapezoidalMap::getBoundingBox()
{
    return boundingBox;
}

void TrapezoidalMap::updateNeighbors(const Trapezoid &t, std::vector<Trapezoid*>& heirs)
{
    if(t==TrapezoidalMap::boundingBox)
        return;
    Trapezoid* leftUp = &t.getLeftUp();
    Trapezoid* leftDown = &t.getLeftDown();
    Trapezoid* rightUp = &t.getRightUp();
    Trapezoid* rightDown = &t.getRightDown();
    if(leftUp !=nullptr && leftDown !=nullptr){
        if(leftUp==leftDown){
            if(t.getTop().p1()==leftUp->getRightp())
                leftUp->setRightDown(*heirs[0]);
            else
                leftUp->setRightUp(*heirs[0]);
        }
        else{
            leftUp->setRightUp(*heirs[0]);
            leftUp->setRightDown(*heirs[0]);
            leftDown->setRightUp(*heirs[0]);
            leftDown->setRightDown(*heirs[0]);
        }
    }
    if(rightUp !=nullptr && rightDown !=nullptr){
        if (rightUp==rightDown){
            if(t.getTop().p2()==rightUp->getLeftp())
                rightUp->setLeftDown(*heirs[3]);
            else
                rightUp->setLeftUp(*heirs[3]);
        }
        else{
            rightUp->setLeftUp(*heirs[3]);
            rightUp->setLeftDown(*heirs[3]);
            rightDown->setLeftUp(*heirs[3]);
            rightDown->setLeftDown(*heirs[3]);
        }
    }
}

void TrapezoidalMap::updateNeighborsMultiTrapezoid(const Trapezoid &t, std::vector<Trapezoid *> &heirs, int type)
{
    enum insertionType {leftmost = 0, intermediate = 1, rightmost = 2};
    Trapezoid* leftUp = &t.getLeftUp();
    Trapezoid* leftDown = &t.getLeftDown();
    Trapezoid* rightUp = &t.getRightUp();
    Trapezoid* rightDown = &t.getRightDown();
    if(type==leftmost){
        if(leftUp !=nullptr && leftDown !=nullptr){
            if(leftUp==leftDown){
                if(t.getTop().p1()==leftUp->getRightp())
                    leftUp->setRightDown(*heirs[0]);
                else
                    leftUp->setRightUp(*heirs[0]);
            }
            else{
                leftUp->setRightUp(*heirs[0]);
                leftUp->setRightDown(*heirs[0]);
                leftDown->setRightUp(*heirs[0]);
                leftDown->setRightDown(*heirs[0]);
            }
        }
        if(rightUp !=nullptr && rightDown !=nullptr){
            if (rightUp==rightDown){
                if(t.getTop().p2()==rightUp->getLeftp()){
                    rightUp->setLeftDown(*heirs[1]);

                }else
                    rightUp->setLeftUp(*heirs[2]);
            }
            else{
                if(heirs[1]->getBottom().p2().y()>rightUp->getBottom().p1().y()){
                    rightUp->setLeftUp(*heirs[1]);
                    rightUp->setLeftDown(*heirs[2]);
                    rightDown->setLeftUp(*heirs[2]);
                    rightDown->setLeftDown(*heirs[2]);
                }
                else{
                    rightUp->setLeftUp(*heirs[1]);
                    rightUp->setLeftDown(*heirs[1]);
                    rightDown->setLeftUp(*heirs[1]);
                    rightDown->setLeftDown(*heirs[2]);
                }
            }
        }
    }
    if(type==intermediate){

    }
    if(type==rightmost){

    }
    
}

//void TrapezoidalMap::merge(Trapezoid &tLeft, Trapezoid &tRight, std::vector<std::list<Trapezoid>::iterator>& garbageCollector)
//{
//    DagNodeArea const * tLeftLeaf = tLeft.getNode();
//    tRight.setLeftp(tLeft.getLeftp());
//    tRight.setLeftUp(tLeft.getLeftUp());
//    tRight.setLeftDown(tLeft.getLeftDown());
//    tRight.setTop(cg3::Segment2d(tLeft.getTop().p1(), tRight.getTop().p2()));
//    tRight.setBottom(cg3::Segment2d(tLeft.getBottom().p1(), tRight.getBottom().p2()));
//    if(tLeft.getLeftUp().getRightUp()==tLeft)
//        tLeft.getLeftUp().setRightUp(tRight);

//    if(tLeft.getLeftUp().getRightDown()==tLeft)
//        tLeft.getLeftUp().setRightDown(tRight);

//    if(tLeft.getLeftDown().getRightUp()==tLeft)
//        tLeft.getLeftDown().setRightUp(tRight);

//    if(tLeft.getLeftDown().getRightDown()==tLeft)
//        tLeft.getLeftDown().setRightDown(tRight);

//    tLeftLeaf->setTrap(tRight);
//    garbageCollector.push_back(tLeft.getItr());
//    //TrapezoidalMap::removeTrapezoid(tLeft);
//}

void TrapezoidalMap::deleteGarbage(std::vector<std::list<Trapezoid>::iterator> &garbage)
{
//    std::list<Trapezoid>::iterator before;
//    std::list<Trapezoid>::iterator after;
//    for(std::list<Trapezoid>::iterator it: garbage) {
//        before = prev(it);
//        after = next(it);
//        before._M_node->_M_next=after._M_node;
//        after._M_node->_M_prev=before._M_node;
//        it._M_node->_M_unhook();
//        it._M_node->~_List_node_base();
//        trapezoids.erase(it);
//    }


}
