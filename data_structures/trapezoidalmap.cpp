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
std::vector<Trapezoid*> TrapezoidalMap::garbage = {};
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
    std::list<Trapezoid>& ref = TrapezoidalMap::getTrapezoids();
    TrapezoidalMap::garbage.push_back(&*itr);
    if(ref.begin() == itr){
        ref.erase(itr);
        return;
    }


//    std::list<Trapezoid>::iterator before;
//    std::list<Trapezoid>::iterator after;
//    before = prev(itr);
//    after = next(itr);
//    before._M_node->_M_next=after._M_node;
//    after._M_node->_M_prev=before._M_node;
//    itr._M_node->_M_unhook();
//    itr._M_node->~_List_node_base();
    (*itr).~Trapezoid();
    ref.erase(itr);

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

void TrapezoidalMap::updateNeighborsMultiTrapezoid(const Trapezoid &t, std::vector<Trapezoid *> &heirs, int type, DagNodeSegment& prevSeg, Trapezoid*& lastDeleted)
{
    enum insertionType {leftmost = 0, intermediate = 1, rightmost = 2};
    Trapezoid* leftUp = &t.getLeftUp();
    Trapezoid* leftDown = &t.getLeftDown();
    Trapezoid* rightUp = &t.getRightUp();
    Trapezoid* rightDown = &t.getRightDown();
    bool aboveleftp;
    bool aboverightp;
    bool degenerate;
    if(type == leftmost){
        //from new to old neighbors
        (heirs[1]->getBottom().p2().y()>t.getRightp().y())?aboverightp=true:aboverightp=false; //check if the segment will pass under or above the current rightp
        (heirs[0]->getTop().p1().y()>=t.getLeftp().y())?aboveleftp=true:aboveleftp=false; // check where the new trapezoids are inserted with respect to previous trapezoid's segment
        (heirs[0]->getRightp()==t.getLeftp())?degenerate=true:degenerate=false; //check if the new segment has as a leftpoint a point of a previous segment
        heirs[0]->setNeighbors(t.getLeftUp(),t.getLeftDown(),*heirs[1], *heirs[2]); //
        heirs[1]->setNeighbors(*heirs[0],*heirs[0], t.getRightUp(), (aboverightp)?t.getRightUp():t.getRightDown());// be careful on right side neighbors
        heirs[2]->setNeighbors(*heirs[0],*heirs[0], (aboverightp)?t.getRightUp():t.getRightDown(), t.getRightDown());//also there
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

    }
    if(type == intermediate){

        if(heirs[0]->getTop().p1()==t.getTop().p1()){ //Il trapeziode unito è quello in basso
            if(((DagNodeArea*)prevSeg.getLeftChild())->getT().getRightUp()==t)
                ((DagNodeArea*)prevSeg.getLeftChild())->getT().setRightUp(*heirs[0]);
            if(((DagNodeArea*)prevSeg.getLeftChild())->getT().getRightDown()==t)
                ((DagNodeArea*)prevSeg.getLeftChild())->getT().setRightDown(*heirs[0]);
            if(t.getLeftUp()!= *lastDeleted &&  t.getLeftUp().getRightUp()==t)
                t.getLeftUp().setRightUp(*heirs[0]);
            if(t.getLeftUp()!= *lastDeleted && t.getLeftUp().getRightDown()==t)
                t.getLeftUp().setRightDown(*heirs[0]);
//            if(t.getLeftDown()!= *lastDeleted && t.getLeftDown().getRightUp()==t)
//                t.getLeftDown().setRightUp(*heirs[0]);
            if(rightUp!=nullptr && rightDown!=nullptr){
                if(rightUp == rightDown){
                    rightUp->setLeftDown(*heirs[0]);
                }
                else{
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
//            if(t.getLeftDown()!= *lastDeleted && t.getLeftDown().getRightDown()==t)
//                t.getLeftDown().setRightDown(*heirs[0]);
        }
        else{ //Il trapezoide unito è quello in alto
            if(((DagNodeArea*)prevSeg.getRightChild())->getT().getRightUp()==t)
                ((DagNodeArea*)prevSeg.getRightChild())->getT().setRightUp(*heirs[1]);
            if(((DagNodeArea*)prevSeg.getRightChild())->getT().getRightDown()==t)
                ((DagNodeArea*)prevSeg.getRightChild())->getT().setRightDown(*heirs[1]);
            if(t.getLeftDown()!= *lastDeleted && t.getLeftDown().getRightUp()==t)
                t.getLeftDown().setRightUp(*heirs[1]);
            if(t.getLeftDown()!= *lastDeleted && t.getLeftDown().getRightDown()==t)
                t.getLeftDown().setRightDown(*heirs[1]);
//            if(t.getLeftUp()!= *lastDeleted && t.getLeftUp().getRightDown()==t)
//                t.getLeftUp().setRightDown(*heirs[1]);
//            if(t.getLeftUp()!= *lastDeleted && t.getLeftUp().getRightUp()==t)
//                t.getLeftUp().setRightUp(*heirs[1]);
            if(rightUp!=nullptr && rightDown!=nullptr){
                if(rightUp == rightDown){
                    rightDown->setLeftUp(*heirs[1]);
                }
                else{
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


    }
    if(type == rightmost){
       //updateNeighborsMultiTrapezoid(t, heirs, 1, prevSeg, lastDeleted);
        if(heirs[0]->getTop().p1()==t.getTop().p1()){ //Il trapeziode unito è quello in basso
            if(((DagNodeArea*)prevSeg.getLeftChild())->getT().getRightUp()==t)
                ((DagNodeArea*)prevSeg.getLeftChild())->getT().setRightUp(*heirs[0]);
            if(((DagNodeArea*)prevSeg.getLeftChild())->getT().getRightDown()==t)
                ((DagNodeArea*)prevSeg.getLeftChild())->getT().setRightDown(*heirs[0]);
            if(t.getLeftUp()!= *lastDeleted &&  t.getLeftUp().getRightUp()==t)
                t.getLeftUp().setRightUp(*heirs[0]);
            if(t.getLeftUp()!= *lastDeleted && t.getLeftUp().getRightDown()==t)
                t.getLeftUp().setRightDown(*heirs[0]);
        }
        else{ //Il trapezoide unito è quello in alto
            if(((DagNodeArea*)prevSeg.getRightChild())->getT().getRightUp()==t)
                ((DagNodeArea*)prevSeg.getRightChild())->getT().setRightUp(*heirs[1]);
            if(((DagNodeArea*)prevSeg.getRightChild())->getT().getRightDown()==t)
                ((DagNodeArea*)prevSeg.getRightChild())->getT().setRightDown(*heirs[1]);
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

DagNodeArea*& TrapezoidalMap::merge(Trapezoid &tLeft, std::tuple<cg3::Segment2d, cg3::Segment2d, cg3::Point2d, cg3::Point2d>& tRight)
{
    DagNodeArea *&  tLeftLeaf = tLeft.getNodeRef();
    cg3::Point2d rightp = std::get<3>(tRight);
    tLeft.setRightp(rightp);
    tLeft.setTop(cg3::Segment2d(tLeft.getTop().p1(), std::get<0>(tRight).p2()));
    tLeft.setBottom(cg3::Segment2d(tLeft.getBottom().p1(), std::get<1>(tRight).p2()));
    return tLeftLeaf;
}

void TrapezoidalMap::deleteGarbage(std::vector<std::list<Trapezoid>::iterator> &garbage)
{
    std::list<Trapezoid>::iterator before;
    std::list<Trapezoid>::iterator after;
    for(std::list<Trapezoid>::iterator it: garbage) {
//        before = prev(it);
//        after = next(it);
//        before._M_node->_M_next=after._M_node;
//        after._M_node->_M_prev=before._M_node;
//        it._M_node->_M_unhook();
//        it._M_node->~_List_node_base();
        if(it==(trapezoids.end()))
            trapezoids.pop_back();
        else
            trapezoids.erase(it);
    }


}
