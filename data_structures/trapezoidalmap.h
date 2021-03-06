#ifndef TRAPEZOIDALMAP_H
#define TRAPEZOIDALMAP_H
#include "dag_dataset.h"
class TrapezoidalMap
{
public:
     TrapezoidalMap();

     const std::list<Trapezoid>& getTrapezoids() const;
     std::list<Trapezoid>& traps();
     void addTrapezoid(Trapezoid& t);
     void removeTrapezoid(std::list<Trapezoid>::iterator);
     void merge(Trapezoid &, std::tuple<cg3::Segment2d, cg3::Segment2d, cg3::Point2d, cg3::Point2d>&);
     void updateNeighbors(const Trapezoid& t, std::vector<Trapezoid*>& heirs);
     void updateNeighborsMultiTrapezoid(Trapezoid& t, std::vector<Trapezoid*>& heirs, int type, DagNodeSegment* prevSeg, Trapezoid*& lastDeleted);
     void leftSingle(Trapezoid&, std::vector<Trapezoid*>&, bool triangleOver, bool linear);
     void leftMulti(Trapezoid&, std::vector<Trapezoid*>&, bool aboverightp, bool triangleOver, bool linear);
     void rightMulti(Trapezoid&, std::vector<Trapezoid*>&, bool triangleOver, bool linear );
     void rightSingle(Trapezoid&, std::vector<Trapezoid*>&, bool triangleOver, bool linear);
     void bothDegenerate(Trapezoid&, std::vector<Trapezoid*>&, bool triangleOverLeft, bool triangleOverRight, bool linearRight, bool linearLeft);

     std::list<Trapezoid>::iterator addSegmentInSingleTrap(Trapezoid& trap, const cg3::Segment2d& segment);
     std::list<Trapezoid>::iterator leftDegenerateSingleInsertion(Trapezoid& trap, const cg3::Segment2d& segment);
     std::list<Trapezoid>::iterator rightDegenerateSingleInsertion(Trapezoid& trap, const cg3::Segment2d& segment);
     std::list<Trapezoid>::iterator totallyDegenerateSingleInsertion(Trapezoid& trap, const cg3::Segment2d& segment);
     std::vector<Trapezoid*> createLeftMost(Trapezoid& trap, const cg3::Segment2d& segment, Trapezoid*& lastDeleted);
     std::vector<Trapezoid*> createLeftMostDegenerate(Trapezoid& trap, const cg3::Segment2d& segment, Trapezoid*& lastDeleted);
     std::vector<Trapezoid*> createRightMost(Trapezoid& trap, const cg3::Segment2d& segment, DagNodeSegment& prevSeg, Trapezoid*& lastDeleted);
     std::vector<Trapezoid*> createRightMostDegenerate(Trapezoid& trap, const cg3::Segment2d& segment, DagNodeSegment& prevSeg, Trapezoid*& lastDeleted);
     std::vector<Trapezoid*> createIntermediate(Trapezoid& trap, const cg3::Segment2d& segment, DagNodeSegment& prevSeg, Trapezoid*& lastDeleted);

     void queryPoint(const cg3::Point2d);
     std::list<Trapezoid>::iterator getQuery() const;
     void setQuery(std::list<Trapezoid>::iterator value);

     void clear();

private:
     std::list<Trapezoid>::iterator query;
     std::list<Trapezoid> trapezoids;
};

#endif // TRAPEZOIDALMAP_H
