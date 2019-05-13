#include "lexscene.h"
#include "ll1_parse.h"
#include "mainwindow.h"
#include "parse.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QGraphicsWidget>
#include <QGroupBox>
#include <QtWidgets/QWidgetAction>
#include <QtGui/QSyntaxHighlighter>
#include <QGraphicsView>
#include <QApplication>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    init_ui();
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event) {
    if (watched == pview) {
        QPoint p;

        if (event->type() == QEvent::Wheel) {
            auto e = static_cast<QWheelEvent *>(event);
            if (QApplication::keyboardModifiers() == Qt::ControlModifier) {
                auto delt = e->angleDelta();

                double m_zoom_delta = 0;
                if (delt.y() > 0) {
                    m_zoom_delta = 0.1;
                } else {
                    m_zoom_delta = -0.1;
                }
                pview->scale(1 + m_zoom_delta, 1 + m_zoom_delta);
            }

            return QObject::eventFilter(watched, event);

        } else {
            return QObject::eventFilter(watched, event);
        }

    } else
        return QObject::eventFilter(watched, event);
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::init_ui() {

	//设置窗口默认大小1440*900
    this->setMinimumWidth(1440);
    this->setMinimumHeight(900);
    auto leftsplitter = new QSplitter(Qt::Horizontal);

	//将左侧设置为文本框
    editer_left = new QPlainTextEdit(leftsplitter);

	//将中间设置为SNL词法分析的DFA图
    tab_center_widget = new QTabWidget(leftsplitter);
    // TODO Add tabview
    lexscene = new LexScene();
    parseScene = new ParseScene();
    auto view = new QGraphicsView(tab_center_widget);
    view->setScene(lexscene);
    pview = new QGraphicsView(tab_center_widget);
    pview->installEventFilter(this);
    pview->setScene(parseScene);
    pview->setDragMode(QGraphicsView::ScrollHandDrag);
    tab_center_widget->addTab(view, QString(tr("LEX")));
    tab_center_widget->addTab(pview, QString(tr("Parse")));


    rightsplitter = new QSplitter(Qt::Vertical, leftsplitter);

    auto cclabel = new QLabel(rightsplitter);
    cclabel->setText(tr("CURRENT CHAR:"));

    label_current_char = new QLabel(rightsplitter);
    label_current_char->setText("EMPTY");

    auto cllabel = new QLabel(rightsplitter);
    cllabel->setText(tr("CURRENT LINE:"));

    label_current_line = new QLabel(rightsplitter);
    label_current_line->setText("EMPTY");

    listwidget_token = new QListWidget(rightsplitter);
    listwidget_token->setMinimumWidth(230);

    this->setCentralWidget(leftsplitter);

    /**
     * FileMenu
     */

    auto openfile = new QAction(QString::fromLocal8Bit("打开源代码"), this->menuBar());
    auto filemenu = this->menuBar()->addMenu(tr("&File"));
    filemenu->addAction(openfile);
    connect(openfile, &QAction::triggered, [=]() {
        filename = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("请选择源代码文件"));
        auto file = new QFile(filename, this);
        file->open(QIODevice::ReadWrite);
        ins->setDevice(file);
        editer_left->document()->setPlainText(ins->readAll());

    });
    auto exit = filemenu->addAction(tr("Exit"));
    connect(exit, &QAction::triggered, []() { qApp->exit(); });

    /**
     * LexMenu
     */
    auto menu_lex = this->menuBar()->addMenu("LEX");

    menu_lex->addSeparator();

    auto speed_slider = new QSlider(Qt::Horizontal, menu_lex);
    speed_slider->setMaximum(100);
    speed_slider->setMinimum(1);
    speed_slider->setValue(100);
    connect(speed_slider, &QSlider::valueChanged, [this](int value) {
        lex->set_speed(value);
    });

    auto speed_action = new QWidgetAction(menu_lex);
    speed_action->setDefaultWidget(speed_slider);

    menu_lex->addAction(speed_action);

	//设置当用户点击开始菜单的"Lex->Lex"时开始运行进行词法分析线程
    auto action_lex = menu_lex->addAction("Lex");
    connect(action_lex, &QAction::triggered, [this]() {
        listwidget_token->clear();
        lex = Lex::getInstance(editer_left->document());

		/*函数原型：
		bool QObject::connect(const QObject* sender,const char * signal,const QObject * receiver,const char * member)[static]
		功能：
		将信号发送者sender对象中的信号signal与接受者receiver中的member槽函数联系起来。当指定信号signal时必须使用宏SIGNAL（），当指定槽函数时必须使用宏SLOT()，
		如果发送者与连接者属于同一个对象时，那么在connect调用中接受者参数可以忽略。
		*/
        connect(lex, &Lex::charget, this, &MainWindow::char_changed, Qt::ConnectionType::UniqueConnection);
        connect(lex, &Lex::idbuff_changed, this, &MainWindow::idbuff_changed, Qt::ConnectionType::UniqueConnection);
        connect(lex, &Lex::token_get, this, &MainWindow::token_get, Qt::ConnectionType::UniqueConnection);
        connect(lex, &Lex::go_path, lexscene, &LexScene::show_path, Qt::ConnectionType::UniqueConnection);
        lex->start();

    });

    /**
     * SyntaxMenu
     */

    auto action_parse = this->menuBar()->addMenu(tr("Parse"));
    auto re = action_parse->addAction(tr("re"));
    connect(re, &QAction::triggered, this, &MainWindow::re_parse);
    auto LL1 = action_parse->addAction(tr("LL1"));
    connect(LL1, &QAction::triggered, this, &MainWindow::ll1_parse);
    ins = new QTextStream();
}

//更新current_char面板的当前字符
void MainWindow::char_changed(char c) {
    label_current_char->setText("\'" + QString(c) + "\'");
}

//更新current_line面板的当前单词
void MainWindow::idbuff_changed(QString str) {
    label_current_line->setText("\"" + str + "\"");
}

void MainWindow::token_get(Token *token) {
    QString str("Line:%1\tLex:%2\t\tSem:%3");
    QString item = str.arg(token->getLine()).arg(token->getLexName()).arg(token->getSem());
    listwidget_token->addItem(item);

}

void MainWindow::re_parse() {
    auto re = Parse::getInstance(lex->getTokenList());
    connect(re, &Parse::parse_success, parseScene, &ParseScene::show_parsetree, Qt::ConnectionType::UniqueConnection);
    re->start();

}

void MainWindow::ll1_parse() {
    auto ll1 = LL1_parse::getInstance(lex->getTokenList());
    connect(ll1, &LL1_parse::parse_success, parseScene, &ParseScene::show_parsetree,
            Qt::ConnectionType::UniqueConnection);
    ll1->start();

}
