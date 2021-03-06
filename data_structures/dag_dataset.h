#ifndef DAG_DATASET_H
#define DAG_DATASET_H

#include "data_structures/dag_node.h"
//#include "trapezoidalmap.h"
#include <cg3/geometry/point2.h>
#include <cg3/geometry/segment2.h>
//#include "trapezoidalmap_dataset.h"
//


class Dag{
public:
    Dag(Trapezoid& link);
    DagNodePoint* addSegmentInSingleTrap(const cg3::Segment2d&, Trapezoid&, Trapezoid&, Trapezoid&, Trapezoid&);
    DagNodePoint* leftDegenerateSingleInsertion(const cg3::Segment2d& segment, Trapezoid&, Trapezoid&, Trapezoid&);
    DagNodePoint* rightDegenerateSingleInsertion(const cg3::Segment2d& segment, Trapezoid&, Trapezoid&, Trapezoid&);
    DagNodeSegment* totallyDegenerateSingleInsertion(const cg3::Segment2d& segment, Trapezoid&, Trapezoid&);
    DagNode* createLeftMost(const cg3::Segment2d &segment, std::vector<Trapezoid*>&);
    DagNode* createLeftMostDegenerate(const cg3::Segment2d &segment, std::vector<Trapezoid*>&);
    DagNode* createRightMost(const cg3::Segment2d &segment, DagNodeSegment&, std::vector<Trapezoid*>&);
    DagNode* createRightMostDegenerate(const cg3::Segment2d &segment, DagNodeSegment&, std::vector<Trapezoid*>&);
    DagNode* createIntermediate(const cg3::Segment2d &segment, DagNodeSegment&, std::vector<Trapezoid*>&);

    void addSegment(cg3::Segment2d& segment);
    void addMultiTrapezoidalSegment(cg3::Segment2d& segment);
    DagNode* searchPoint(const cg3::Point2d& point);
    DagNode*& searchAndAppend(const cg3::Segment2d& point);
    void followSegment(cg3::Segment2d& segment, DagNodeArea* leaf);
    DagNode *getDag();
    DagNodeArea* bindWithParentRef(cg3::Segment2d& top, cg3::Segment2d& bottom, cg3::Point2d& leftp, cg3::Point2d& rightp, DagNode*& ref);
    friend class DagNodeArea;
    void addNode(DagNode& node);
    DagNode*& dagRef();


    void setDag(DagNode *value);
    void clear(DagNode *);

private:
    DagNode* dag;

};

//DagNode* Dag::dag = nullptr;
#endif // DAG_DATASET_H
