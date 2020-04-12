#ifndef TRAPEZOIDALMAP_H
#define TRAPEZOIDALMAP_H
#include "trapezoid.h"
#include "dag_dataset.h"

class TrapezoidalMap
{
public:
    TrapezoidalMap();

    static std::list<Trapezoid>& getTrapezoids();
    static void addTrapezoids(std::vector<Trapezoid> vec);
    static void addTrapezoid(Trapezoid& t);
    static void removeTrapezoid(const Trapezoid& t);
    //static void replaceTrapezoid(Trapezoid& old, Trapezoid& substitute);
    static Trapezoid &getBoundingBox();
    static void updateNeighbors(const Trapezoid& t, std::vector<Trapezoid*>& heirs);
    static void updateNeighborsMultiTrapezoid(const Trapezoid& t, std::vector<Trapezoid*>& heirs, int type);
    static void merge(Trapezoid& tLeft, Trapezoid& tRight, std::vector<std::list<Trapezoid>::iterator>&);
    static void deleteGarbage(std::vector<std::list<Trapezoid>::iterator>& garbage);
private:
    static std::list<Trapezoid> trapezoids;
    static Trapezoid boundingBox;
};

#endif // TRAPEZOIDALMAP_H
