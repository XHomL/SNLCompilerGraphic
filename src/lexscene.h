#ifndef LEXVIEW_H
#define LEXVIEW_H

#include <QGraphicsView>
#include <QVector>
#include <QGraphicsItem>
#include <QtCore/QSet>
#include <QPair>


class LexScene : public  QGraphicsScene
{
//    Q_OBJECT
public:
    LexScene();



public slots:
    void show_path(QVector<QPair<int,int> > v);

private:
    QVector<QGraphicsItem *> items;
    QVector<QPoint> pos;
    QVector<QPoint> line;
    int r;
    QSet<int> round;
    QSet<int> nameid;
    QMap<int,QString> nameid_string;
	QMap<int, QString> round_string;

    QVector<QGraphicsItem *>path_item;

    // QGraphicsScene interface
protected:
    void drawBackground(QPainter *painter, const QRectF &rect) override;
    void drawForeground(QPainter *painter, const QRectF &rect) override;
};

#endif // LEXVIEW_H
