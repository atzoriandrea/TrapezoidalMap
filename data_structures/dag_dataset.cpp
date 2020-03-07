#include <data_structures/dag_dataset.h>
#include <data_structures/dag_node.h>


void Dag::addSegment(cg3::Segment2d* segment){
    DagNode * ep1 = searchPoint(segment->p1());
    DagNode * ep2 = searchPoint(segment->p2());
    if(ep1==ep2){
        cg3::Point2d p = segment->p1();
        DagNode* dg = new DagNodePoint(p);
        cg3::Point2d p2 = segment->p2();
        DagNode* dg2 = new DagNodePoint(p);
        dg->setRightChild(dg2);
        DagNode* dgs = new DagNodeSegment(segment);
        dg2->setLeftChild(dgs);
        ep1 = dg;
    }
};
DagNode* Dag::searchPoint(cg3::Point2d point){
    DagNode *const* parentPointer = nullptr;
    DagNode* tmpdag=getInstance();
    while(tmpdag!=nullptr){
        parentPointer = tmpdag->compareTo(point);
        tmpdag = *(parentPointer);
    }
    return tmpdag;

}

DagNode* Dag::getInstance(){
    static DagNode * dag;
    return dag;
}









