#ifndef LEX_H
#define LEX_H

#include <QObject>
#include <QTextStream>
#include <QThread>
#include <utility>
#include "globals.h"
#include <QPair>
#include <QTextDocument>
#include <QVector>

/*enum LexType{
    PROGRAM=1,TYPE,   VAR,    PROCEDURE,
    BEGIN,    END,    ARRAY,  OF,
    RECORD,   IF,     THEN,   ELSE,
    FI,       WHILE,  DO,     ENDWH,
    READ,     WRITE,  RETURN,INTEGER, CHAR,
    //types, just a symbol of type **name**, no actual sematic infomation
    INTEGER_T, CHAR_T,
    //
    ID,
    //not like INTEGER_T and CHAR_T, these two do have certain meanings
    INTC_VAL, CHAR_VAL,
    //
    ASSIGN, EQ,     LT,     PLUS, MINUS,
    TIMES,  DIVIDE, LPAREN, RPAREN,
    DOT,    COLON,  SEMI,   COMMA,
    LMIDPAREN, RMIDPAREN, UNDERRANGE,
    //
    ENDFILE = 0, ERROR = -1,

};
*/
extern QMap<LexType,QString> lexName;
extern QMap<QString,LexType> reservedWords;

class Lex : public QThread
{
    Q_OBJECT
public:
    static Lex * getInstance(QTextDocument *doc){
        static auto *lex=new Lex();
        lex->doc=doc;
		//设置速度，其值越小程序读取速度越快
        lex->set_speed(1);
        return lex;

    }
    const Token * getTokenList();
    void set_speed(int speed);



private:
//    explicit Lex(QObject *parent = nullptr);

	//通过sinals实现线程间通信，以完成将文本框中的内容传递到显示框中的内容的目的
signals:
	//用于传递更改current_char面板中的字符
    void charget(char c);
	//用于传递更改current_line面板中的单词
    void idbuff_changed(QString buff);
	//用于传递增加token面板中的token
    void token_get(Token *token);
	//用于传递判断走图中的哪一条路径
    void go_path(QVector<QPair<int,int>> v);


public slots:


private:
    QTextDocument *doc;
    QTextStream ins;
    double intBuff;
    QString idBuff;
    Token *current;
    Token *head;
    int line_number;
    int sleep_time;
private:
    bool ischar(char c);
    bool isnum(char c);
    bool issinglesep(char c);
    Token *getsinglesep(char c);
	//声明保留字查找函数
    Token *lookup(QString str);


    // QThread interface
protected:
    void run() override;
};

#endif // LEX_H
