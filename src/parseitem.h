#ifndef PARSEITEM_H
#define PARSEITEM_H

#include <QGraphicsItem>
#include <QObject>

class ParseItem : public QGraphicsTextItem
{
public:
    ParseItem();
    ParseItem(const QString &text,QGraphicsItem *parent=Q_NULLPTR);

    // QGraphicsItem interface
public:
    QGraphicsItem *myparent;
    int cengshu;

    // QGraphicsItem interface
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;

    // QGraphicsItem interface
public:
    QPainterPath shape() const override;

    // QGraphicsItem interface
public:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
};

#endif // PARSEITEM_H
