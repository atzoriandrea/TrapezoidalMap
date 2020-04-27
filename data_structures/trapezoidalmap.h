#ifndef TRAPEZOIDALMAP_H
#define TRAPEZOIDALMAP_H
#include "trapezoid.h"
#include "dag_dataset.h"

class TrapezoidalMap
{
public:
    TrapezoidalMap();

    static std::list<Trapezoid>& getTrapezoids();
    static Trapezoid& getTrapezoid();
    static void addTrapezoids(std::vector<Trapezoid> vec);
    static void addTrapezoid(Trapezoid& t);
    static void removeTrapezoid(std::list<Trapezoid>::iterator);
    static std::list<Trapezoid>& init(Trapezoid& ref);
    static Trapezoid &getBoundingBox();
    static void updateNeighbors(const Trapezoid& t, std::vector<Trapezoid*>& heirs);
    static void updateNeighborsMultiTrapezoid(const Trapezoid& t, std::vector<Trapezoid*>& heirs, int type, DagNodeSegment& prevSeg, Trapezoid*& lastDeleted);
    static DagNodeArea *& merge(Trapezoid& tLeft, std::tuple<cg3::Segment2d, cg3::Segment2d, cg3::Point2d, cg3::Point2d>&);
    static void deleteGarbage(std::vector<std::list<Trapezoid>::iterator>& garbage);
private:
    static std::list<Trapezoid> trapezoids;
    static Trapezoid boundingBox;
    static std::vector<Trapezoid*> garbage;
};

#endif // TRAPEZOIDALMAP_H
