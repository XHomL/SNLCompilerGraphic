#include "parseitem.h"
#include "parsescene.h"

#include <QGraphicsLinearLayout>
#include <QGraphicsSceneDragDropEvent>
#include <QQueue>



QMap<NodeKind,QString> nodekind_map ={
{ProK,"ProK"},
{PheadK,"PheadK"},
{DecK,"DecK"},
{TypeK,"TypeK"},
{VarK,"VarK"},
{ProcDecK,"ProcDecK"},
{StmLK,"StmLK"},
{StmtK,"StmtK"},
{ExpK,"ExpK"},
};
QMap<DecKind,QString> deckind_map={
{ArrayK,"ArrayK"},
{CharK,"CharK"},
{IntegerK,"IntegerK"},
{RecordK,"RecordK"},
{IdK,"IdK"},
};
QMap<StmtKind,QString> stmtkind_map={
{IfK,"IfK"},
{WhileK,"WhileK"},
{AssignK,"AssignK"},
{ReadK,"ReadK"},
{WriteK,"WriteK"},
{CallK,"CallK"},
{ReturnK,"ReturnK"},
};
QMap<ExpKind,QString> expkind_map={
{OpK,"OpK"},
{ConstK,"ConstK"},
{VariK,"VariK"},
};
QMap<VarKind,QString> varkind_map={
{IdV,"IdV"},
{ArrayMembV,"ArrayMembV"},
{FieldMembV,"FieldMembV"},
};
QMap<ExpType,QString> exptype_map={
{Void,"Void"},
{Integer,"Integer"},
{Boolean,"Boolean"},
};
QMap<ParamType,QString> paramtype_map={
{valparamType,"valparamType"},
{varparamType,"varparamType"},
};





ParseScene::ParseScene()
{

}

void ParseScene::show_parsetree(QSharedPointer<TreeNode> root, QString text)
{
//    for(auto i:this->items()){
//        this->removeItem(i);
//    }
    this->clear();
    auto tt=addText(text);
    tt->moveBy(400,0);
    tt->setTextWidth(12);

    if(root->nodekind!=ProK){
        //TODO ERROR
    }
    QQueue<TreeNode  *> queue;
    QQueue<ParseItem*> iqueue;

    auto item=new ParseItem(getName(root.data()));
    this->addItem(item);

    bool deep=false;
    auto parent=item;
    QGraphicsLineItem *line;
    for(auto i=0;i!=3;++i){
        if(root->child[i]!=nullptr){
			//将root作为root->child的parent
            item=new ParseItem(getName(root->child[i]),parent);
            item->cengshu=0;
            this->addItem(item);
			//设置节点坐标
            item->setPos(QPoint(-100+i*150+(qrand()%100),100+(qrand()%100)));

            line=new QGraphicsLineItem(QLine(item->myparent->scenePos().toPoint(),item->scenePos().toPoint()));
            this->addItem(line);
            line->setPos(item->myparent->scenePos());
            queue.enqueue(root->child[i]);
            iqueue.enqueue(item);
        }
    }
    while (!queue.empty()) {
        auto node=queue.dequeue();
        auto item=iqueue.dequeue();
        deep=!deep;

    auto parent=item;

    if(node->sibling!=nullptr){


        item=new ParseItem(getName(node->sibling),parent);
        this->addItem(item);
        item->setPos(QPoint(500+(qrand()%500),0)/2);

               // auto coll=this->collidingItems(item,Qt::IntersectsItemBoundingRect);
               // while(!coll.empty()){
               //     item->moveBy(0,qrand()%100);
               // }

		//用于绘制节点间的连线
        line=new QGraphicsLineItem(QLine(QPoint(0,0),item->pos().toPoint()));
        this->addItem(line);
        line->setPos(item->myparent->scenePos());

        queue.enqueue(node->sibling);
        iqueue.enqueue(item);

    }

        for(auto i=0;i!=3;++i){
            if(node->child[i]!=nullptr){
                item=new ParseItem(getName(node->child[i]),parent);
				//子节点的层数为parent节点层数+1
                item->cengshu=parent->cengshu+1;
                this->addItem(item);
                item->setPos(QPoint(-300+(qrand()%400)+i*500,180+(qrand()%400))/1.5);

               // while(!item->collidingItems(Qt::IntersectsItemBoundingRect).isEmpty()){
               //     item->moveBy(0,qrand()%50);
               // }
                line=new QGraphicsLineItem(QLine(QPoint(0,0), item->pos().toPoint()));
                this->addItem(line);
                line->setPos(item->myparent->scenePos());

                queue.enqueue(node->child[i]);
                iqueue.enqueue(item);

            }
        }


    }

}

//获得一个节点中多个变量的名字
QString ParseScene::get_names(const TreeNode *node)
{
    QString name;
    for(auto i=0;i!=node->idnum;++i){
        name+=node->name[i];
        name.append(",");
    };

    return name;
}

//获得节点名
QString ParseScene::getName(const TreeNode *node)
{
    QString name;
    switch (node->nodekind) {
    case ProK:
        return "Prok";
    case PheadK:
        return "PheadK:"+QString(node->name[0]);
    case TypeK:
        return "TypeK";
    case VarK:
        return "VarK";
    case ProcDecK:
        return "ProcDeck:"+QString(node->name[0])+" of Type "+paramtype_map[node->attr.ProcAttr.paramt];
    case StmLK:
        return "StmLK: ";
    case DecK:
    {
        switch (node->kind.dec) {
        case ArrayK:
            return "DecK Array:"+QString(node->name[0])+" of "+deckind_map[node->attr.ArrayAttr.childtype]+" f "+QString::number(node->attr.ArrayAttr.low)+" t "+QString::number(node->attr.ArrayAttr.up);
        case CharK:
            return "DecK Char";
        case IntegerK:{
            name = get_names(node);
            return "DecK IntegerK:"+name;
        }
        case RecordK:
            name=get_names(node);
            return "DecK RecordK:"+name;
        case IdK:
            name=get_names(node);
            return "DecK IdK:"+name+" of "+node->type_name;
        }
        break;
    }
    case StmtK:
    {
        switch (node->kind.stmt) {
        case IfK:
            return "IfK";
        case WhileK:
            return "WHileK";
        case AssignK:
            return "AssignK";
        case ReadK:
            return "ReadK";
        case WriteK:
            return "WriteK";
        case CallK:
            return "CallK";
        case ReturnK:
            return "ReturnK";
        }
        break;
    case ExpK:
        {
            switch (node->kind.exp) {
            case OpK:
                return "ExpK OpK:"+lexName[node->attr.ExpAttr.op];
            case ConstK:
                return "ExpK ConstK:"+QString::number(node->attr.ExpAttr.val);
            case VariK:
                return "ExpK "+varkind_map[node->attr.ExpAttr.varkind]+": " +QString(node->name[0]);
            }
            break;
        }
    default:
            return "ERROR";
    }





    }

        return "ERROR";
}

void ParseScene::dragMoveEvent(QGraphicsSceneDragDropEvent *event)
{
    event->accept();

}
