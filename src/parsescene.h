#ifndef PARSESCENE_H
#define PARSESCENE_H

#include <QGraphicsScene>
#include "globals.h"



class ParseScene : public QGraphicsScene
{
public:
    ParseScene();


    QString get_names(const TreeNode *node);

public slots:
    void show_parsetree(QSharedPointer<TreeNode> root, QString text);


private:
    QString getName(const TreeNode *node);

    // QGraphicsScene interface
protected:
    void dragMoveEvent(QGraphicsSceneDragDropEvent *event) override;
};

#endif // PARSESCENE_H
