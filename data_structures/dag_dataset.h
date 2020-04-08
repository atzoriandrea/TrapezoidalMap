#ifndef DAG_DATASET_H
#define DAG_DATASET_H

#include "data_structures/dag_node.h"
#include "trapezoidalmap.h"
#include <cg3/geometry/point2.h>
#include <cg3/geometry/segment2.h>
#include "trapezoidalmap_dataset.h"
#include "algorithms/algorithms.h"


class Dag{
public:
    Dag();
    static void innerNodes(DagNode *split, cg3::Segment2d &segment, DagNode* meaningful);
    static DagNodePoint* addSegmentInSingleTrap(DagNodeArea * node, cg3::Segment2d& segment);
    static void addSegment(cg3::Segment2d& segment);
    static DagNode* createLeftMost(cg3::Segment2d& segment, DagNodeArea& leaf);
    static DagNode* createRightMost(cg3::Segment2d& segment, DagNodeArea& leaf);
    static DagNode* createIntermediate(cg3::Segment2d& segment, DagNodeArea& leaf);

    static void addMultiTrapezoidalSegment(cg3::Segment2d& segment);
    static DagNode * searchPoint(const cg3::Point2d& point);
    static DagNode*& searchAndAppend(const cg3::Point2d& point);
    static DagNode * splitNode(cg3::Segment2d& segment);
    static void followSegment(cg3::Segment2d& segment, DagNodeArea& leaf);
    static DagNode *getDag();
    static std::vector<std::pair<DagNode **, DagNode*>> tempmap;
    static DagNode * bind(Trapezoid&);
    friend class DagNodeArea;
private:
    static DagNode * dag;

};

//DagNode* Dag::dag = nullptr;
#endif // DAG_DATASET_H
