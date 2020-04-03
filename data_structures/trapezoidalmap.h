#ifndef TRAPEZOIDALMAP_H
#define TRAPEZOIDALMAP_H
#include "trapezoid.h"
#include "dag_dataset.h"

class TrapezoidalMap
{
public:
    TrapezoidalMap();

    static std::vector<Trapezoid>& getTrapezoids();
    static void addTrapezoids(std::vector<Trapezoid> vec);
    static void addTrapezoid(Trapezoid t);
    static void removeTrapezoid(Trapezoid& t);
    static Trapezoid &getBoundingBox();
    static void updateNeighbors(const Trapezoid& t, std::vector<Trapezoid>& heirs);
private:
    static std::vector<Trapezoid> trapezoids;
    static Trapezoid boundingBox;
};

#endif // TRAPEZOIDALMAP_H
