#ifndef LL1_PARSE_H
#define LL1_PARSE_H

#include "globals.h"

#include <QObject>
#include <QSharedPointer>
#include <QStack>
#include <QThread>



extern QSet<LexType> NTSet;
extern QSet<LexType> TTSet;

extern int lineno;
extern QMap<LexType, QString> lexName;
class LL1_parse :public QThread
{
	Q_OBJECT
public:

	static LL1_parse *getInstance(const Token *head) {
		auto instance = new LL1_parse(head);
		return instance;
	}
	TreeNode *get_parsetree_head();
	// QThread interface
protected:
	void run() override;

signals:
	void parse_success(QSharedPointer<TreeNode> p, QString title);


private:
	LL1_parse(const Token *root);
	void createLL1Table();
	void process(int id);
private:
	//指向Token的行号line
	const Token *head;
	//预测分析表
	QMap<QPair<LexType, LexType>, int> table;
	//符号栈 存的是Token的种别码
	QStack<LexType> symbal_stack;
	//语法分析树节点栈
	QStack<TreeNode **> syntaxtree_stack;
	//操作符栈
	QStack<TreeNode *> op_stack;
	//操作数栈
	QStack<TreeNode *> num_stack;
	//语法分析树的根节点
	TreeNode *root;
};

#endif // LL1_PARSE_H
