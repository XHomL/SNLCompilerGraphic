#include "lexscene.h"

#include <QDebug>
#include <QThread>

LexScene::LexScene() {
	//圆的半径
    r=20;

	//设置qt中圆和直线的位置
    pos = {
            {-100, -200 },  {-50,  -200 },{-50,-230},   {0,-230},{0,-200},              {50,-200},
            {-100,-150},    {-50,  -150 },{-50,-180},   {0,-180},{0,-150},              {50,-150},
            {-100,-100},                                {0,   -100 },                   {50,-100},
            {-100,-50},     {-50,  -50 },{-50,-80},     {0,-50},                        {50,-80},{50,-50},
            {-100,0},       {-50,  0},                                                  {50,  0   },
            {-100,50},      {-50,50},{-50,  20  },      {0,50},                  {50,20},{50,50},
            {-100,100},     {-50,100},{-50,70},         {0,100},{0,70},     {30,100},   {50,100},{50,70},
            {-100,150},     {-50,150},{50,150},
            {50,200},
    };


    line={
            {0,1}, {1,2}, {2,3}, {3,4}, {1,4}, {4,5},
            {0,6},{6,7},{7,8},{8,9},{9,10},{7,10},{10,11},{5,11},
            {6,12},{12,13},{13,14},{11,14},
        {12,15},{15,16},{16,17},{17,19},{16,18},{19,20},{18,20},{14,19},
            {15,21},{21,22},{22,23},{20,23},
            {21,24},{24,25},{25,26},{26,28},{23,28},{28,29},{25,27},{27,29},
        {24,30},{30,31},{31,32},{32,34},{31,33},{34,33},{34,37},{33,35},{35,36},{37,36},{29,37},
            {30,38},{38,39},{39,40},{36,40},
            {40,41}
    };

	//对qt中的圆进行编号
    round=QSet<int>::fromList({0,1,7,13,16,18,22,25,27,31,33,35,39,41});

	//对qt中的直线进行编号
    nameid=QSet<int>::fromList({0,3,5,6,9,11,12,14,15,16,17,19,20,21,22,23,24,25,26,28,29,30,31,32,33,36,37,38,40});

	//qt中直线的remark
    nameid_string={
        {0,QString::fromLocal8Bit("字母")},
        {3,QString::fromLocal8Bit("字母，数字")},
        {5,QString::fromLocal8Bit("标识符")},
        {6,QString::fromLocal8Bit("数字")},
        {9,QString::fromLocal8Bit("数字")},
        {11,QString::fromLocal8Bit("无符号整数")},
        {12,QString::fromLocal8Bit("+,-,*,/,(,),;,[,],=,<,EOF,空白")},
        {14,QString::fromLocal8Bit("单分界符")},
        {15,QString::fromLocal8Bit(":")},
        {16,QString::fromLocal8Bit("=")},
        {17,QString::fromLocal8Bit("其他符号")},
        {19,QString::fromLocal8Bit("出错")},
        {20,QString::fromLocal8Bit("双分界符")},
        {21,QString::fromLocal8Bit("{")},
        {23,QString::fromLocal8Bit("注释头符号")},
        {24,QString::fromLocal8Bit(".")},
        {25,QString::fromLocal8Bit(".")},
        {26,QString::fromLocal8Bit("其他符号")},
        {28,QString::fromLocal8Bit("程序结束标志")},
        {29,QString::fromLocal8Bit("数组下标")},
        {30,QString::fromLocal8Bit("'")},
        {31,QString::fromLocal8Bit("数字或字母")},
        {32,QString::fromLocal8Bit("其他符号")},
        {33,QString::fromLocal8Bit("'")},
        {36,QString::fromLocal8Bit("字符状态")},
        {37,QString::fromLocal8Bit("出错")},
        {38,QString::fromLocal8Bit("其他")},
        {40,QString::fromLocal8Bit("出错")},
    };

	round_string = {
		{1,QString::fromLocal8Bit("INID")},
		{7,QString::fromLocal8Bit("INNUM")},
		{13,QString::fromLocal8Bit("DONE")},
		{18,QString::fromLocal8Bit("INASSIGN")},
		{22,QString::fromLocal8Bit("INCOMMENT")},
		{27,QString::fromLocal8Bit("INRANGE")},
		{31,QString::fromLocal8Bit("INCHAR")},
		{33,QString::fromLocal8Bit("DONE")},
	};

	//通过圆和直线的编号，使用qt进行绘制
    for(int i=0;i!=pos.length();++i){
        if(round.contains(i)){
            auto el=addEllipse(-r/2,-r/2,r,r);
            el->setPos(pos.at(i)*2);
            items.append(el);
        }
		else{
            auto p=addLine(0,0,0,0);
            p->setPos(pos.at(i) * 2);
            items.append(p);
        }
		if (nameid.contains(i)) {
			//给直线加上remark
			auto tt = addText(nameid_string[i]);
			//qt设置文字的大小比例
			tt->setScale(1);
			tt->setPos(pos.at(i) * 2+QPoint(10,-20));
		}
		if (round.contains(i)) {
			//给圆加上remark
			auto tt = addText(round_string[i]);
			//qt设置文字的大小比例
			tt->setScale(1);
			tt->setPos(pos.at(i) * 2);
		}
    }

	//通过line中的vector，获取序号，并通过序号获取items中对应项的坐标
    for(auto item:line){
        auto x=items.at(item.x());
        auto y=items.at(item.y());
        auto l=addLine(0,0,y->x()-x->x(),y->y()-x->y());
        l->setPos(x->scenePos());
    }
}

//将DFA的当前path用蓝线标明
void LexScene::show_path(QVector<QPair<int, int> > v)
{
    for(auto item:path_item){
        removeItem(item);
    }
	//清除之前path的蓝线
    path_item.clear();
    QPen pen;
    pen.setColor(Qt::blue);
    pen.setWidth(3);
    for(auto item:v){
        auto x=items.at(item.first);
        auto y=items.at(item.second);
        auto l=addLine(0,0,y->x()-x->x(),y->y()-x->y(),pen);
        l->setPos(x->scenePos());
        path_item.append(l);

    }

}

void LexScene::drawBackground(QPainter *painter, const QRectF &rect)
{

}

void LexScene::drawForeground(QPainter *painter, const QRectF &rect)
{

}


