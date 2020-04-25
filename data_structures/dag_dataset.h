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
    static DagNodePoint* addSegmentInSingleTrap(DagNode* node, cg3::Segment2d& segment);
    static DagNodePoint* leftDegenerateSingleInsertion(DagNode* node, cg3::Segment2d& segment);
    static DagNodePoint* rightDegenerateSingleInsertion(DagNode* node, cg3::Segment2d& segment);
    static DagNodeSegment* totallyDegenerateSingleInsertion(DagNode* node, cg3::Segment2d& segment);
    static void addSegment(cg3::Segment2d& segment);
    static DagNode* createLeftMost(cg3::Segment2d& segment, DagNodeArea& leaf);
    static DagNode* createRightMost(cg3::Segment2d& segment, DagNodeArea& leaf, DagNodeSegment& prevSeg);
    static DagNode* createIntermediate(cg3::Segment2d& segment, DagNodeArea& leaf, DagNodeSegment& prevSeg);
    static void addMultiTrapezoidalSegment(cg3::Segment2d& segment);
    static DagNode* searchPoint(const cg3::Point2d& point);
    static DagNode*& searchAndAppend(const cg3::Segment2d& point);
    static void followSegment(cg3::Segment2d& segment, DagNodeArea* leaf);
    static DagNode *getDag();
    static DagNodeArea* bindWithParentRef(cg3::Segment2d& top, cg3::Segment2d& bottom, cg3::Point2d& leftp, cg3::Point2d& rightp, DagNode*& ref);
    friend class DagNodeArea;
    static void addNode(DagNode& node);
    static DagNode*& dagRef();


private:
    static DagNode* dag;

};

//DagNode* Dag::dag = nullptr;
#endif // DAG_DATASET_H
