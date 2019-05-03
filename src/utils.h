//
// Created by tender on 18-5-11.
//

#ifndef SNLCOMPILERGRAPHIC_UTILS_H
#define SNLCOMPILERGRAPHIC_UTILS_H


#include "globals.h"

class utils {

};
TreeNode *newSpecNode(NodeKind kind);

TreeNode *newTypeNode();

TreeNode *newVarNode();


TreeNode *newRootNode();

TreeNode *newPheadNode();

TreeNode *newDecANode(NodeKind kind);

TreeNode *newDecNode();

TreeNode *newProcNode();

TreeNode *newStmlNode();

TreeNode *newStmtNode(StmtKind kind);


TreeNode *newExpNode(ExpKind kind);

#endif //SNLCOMPILERGRAPHIC_UTILS_H
