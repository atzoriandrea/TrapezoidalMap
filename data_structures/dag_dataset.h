#ifndef DAG_DATASET_H
#define DAG_DATASET_H

#include "data_structures/dag_node.h"
#include <cg3/geometry/point2.h>
#include <cg3/geometry/segment2.h>
#include "trapezoidalmap_dataset.h"
class Dag{
public:
    Dag();
    static void innerNodes(DagNode *split, cg3::Segment2d &segment, DagNode* meaningful);
    static DagNode* addSegmentInSingleTrap(DagNode * node, cg3::Segment2d& segment);
    static void addSegment(cg3::Segment2d& segment);
    static void addMultiTrapezoidalSegment(cg3::Segment2d& segment);
    static DagNode * searchPoint(const cg3::Point2d& point);
    static DagNode * splitNode(cg3::Segment2d& segment);
    static DagNode *getDag();
    static std::vector<std::pair<DagNode **, DagNode*>> tempmap;
    static std::set<Trapezoid>& getTrapezoids();
    static void addTrapezoids(std::vector<Trapezoid> vec);
    static void removeTrapezoid(const Trapezoid& t);
    static std::map<Trapezoid*, std::vector<DagNode **>>& getMultiPointed();


private:
    static DagNode * dag;
    static std::set<Trapezoid> trapezoids;
    static std::map<Trapezoid*, std::vector<DagNode **>> multiPointed;
};

//DagNode* Dag::dag = nullptr;
#endif // DAG_DATASET_H
