#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "lex.h"
#include "lexscene.h"
#include "parsescene.h"

#include <QLabel>
#include <QListWidget>
#include <QMainWindow>
#include <QPlainTextEdit>
#include <QSplitter>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextStream>
#include <QThread>
#include <QGraphicsView>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


private:
    void init_ui();

private:
    Ui::MainWindow *ui;

    QSplitter *rightsplitter;
    QPlainTextEdit *editer_left;
    QTabWidget *tab_center_widget;
    QGraphicsView *pview;
    LexScene *lexscene;
    ParseScene *parseScene;
    QVBoxLayout *layout_right;
    QHBoxLayout *layout_left;
    QHBoxLayout *layout;
    QLabel *label_current_char;
    QLabel *label_current_line;
	//用于显示token列表
    QListWidget *listwidget_token;

    QString filename;
    QTextStream *ins;
    Lex *lex;

    QThread *worker_thread;

public slots:
    void char_changed(char c);
    void idbuff_changed(QString str);
    void token_get(Token *token);

    void re_parse();
    void ll1_parse();

    bool eventFilter(QObject *watched, QEvent *event) override;
};

#endif // MAINWINDOW_H
