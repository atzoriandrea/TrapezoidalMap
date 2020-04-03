#include "dag_dataset.h"
#include "trapezoidalmap.h"

std::vector<Trapezoid> TrapezoidalMap::trapezoids = {};
cg3::Segment2d topbb = cg3::Segment2d(cg3::Point2d(-1000000.0, 1000000.0),cg3::Point2d(1000000.0, 1000000.0));
cg3::Segment2d botbb = cg3::Segment2d(cg3::Point2d(-1000000.0, -1000000.0),cg3::Point2d(1000000.0, -1000000.0));
cg3::Point2d leftp = cg3::Point2d(-1000000.0, 1000000.0);
cg3::Point2d rightp = cg3::Point2d(1000000.0, 1000000.0);
Trapezoid t = Trapezoid(topbb, botbb, leftp, rightp,nullptr);
Trapezoid TrapezoidalMap::boundingBox = t;
DagNode * Dag::dag = bind(TrapezoidalMap::getBoundingBox());
TrapezoidalMap::TrapezoidalMap(){

}
std::vector<Trapezoid>& TrapezoidalMap::getTrapezoids()
{
    return  trapezoids;
}


void TrapezoidalMap::addTrapezoids(std::vector<Trapezoid> traps)
{
    //std::copy( vec.begin(), vec.end(), std::inserter( trapezoids, trapezoids.end() ) );
    for (unsigned int i = 0; i<traps.size();i++) {
        trapezoids.push_back(traps[i]);
    }

}

void TrapezoidalMap::addTrapezoid(Trapezoid t)
{
    trapezoids.push_back(t);
}

void TrapezoidalMap::removeTrapezoid(Trapezoid &tr)
{
//    std::vector<Trapezoid> copy = trapezoids;
//    trapezoids.clear();
//    for (auto t : copy){
//        if(t!=tr)
//            trapezoids.push_back(t);
//    }
    std::vector<Trapezoid>::iterator it = std::find(trapezoids.begin(), trapezoids.end(), tr);
    if(it!=trapezoids.end())
        trapezoids.erase(it);

}

Trapezoid &TrapezoidalMap::getBoundingBox()
{
    return boundingBox;
}

void TrapezoidalMap::updateNeighbors(const Trapezoid &t, std::vector<Trapezoid>& heirs)
{
    if(t==TrapezoidalMap::boundingBox)
        return;
    Trapezoid* leftUp = &t.getLeftUp();
    Trapezoid* leftDown = &t.getLeftDown();
    Trapezoid* rightUp = &t.getRightUp();
    Trapezoid* rightDown = &t.getRightDown();
    if(leftUp !=nullptr && leftDown !=nullptr){
        if(leftUp==leftDown){
            if(t.getTop().p1()==heirs[0].getTop().p1())
                leftUp->setRightUp(heirs[0]);
            else
                leftUp->setRightDown(heirs[0]);
        }
        else{
            leftUp->setRightUp(heirs[0]);
            leftUp->setRightDown(heirs[0]);
            leftDown->setRightUp(heirs[0]);
            leftDown->setRightDown(heirs[0]);
        }
    }
    if(rightUp !=nullptr && rightDown !=nullptr){
        if (rightUp==rightDown){
            if(t.getTop().p2()==heirs[3].getTop().p2())
                rightUp->setLeftUp(heirs[3]);
            else
                rightUp->setLeftDown(heirs[3]);
        }
        else{
            rightUp->setLeftUp(heirs[3]);
            rightUp->setLeftDown(heirs[3]);
            rightDown->setLeftUp(heirs[3]);
            rightDown->setLeftDown(heirs[3]);
        }
    }
}
