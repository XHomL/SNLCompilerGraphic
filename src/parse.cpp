#include "parse.h"
#include "globals.h"
#include "utils.h"
#include <QDebug>


//该函数调用总程序处理分析函数，创建语法分析树，同时处理文件的提前结束错误
void Parse::run() {
    root = program();
    if (head->getLex() != ENDFILE) {
        syntaxError("end earily in line:"+QString::number(head->getLine()));
    }
        emit parse_success(QSharedPointer<TreeNode>(root), QString::fromLocal8Bit("递归下降"));
}

/**
 * program ::= programHead declarePart programBody
 * @return
 */
Parse::Parse(const Token *root):head(root)
{

}

/*该函数根据文法产生式生成语法树的根节点root，调用程序头部分析函数programHead()、声明部分分析函数declarePart()、程序体部分分析函数programBody()，分别为语法树的3个儿子节点。
匹配程序结束标志“.”(DOT)。如果处理成功，函数返回程序根节点语法树节点root；否则返回NULL。
*/
TreeNode *Parse::program() {
    TreeNode *ph = programHead();
    TreeNode *dp = declarePart();
    TreeNode *pb = programBody();

    root = newRootNode();
    if (nullptr != root) {
        root->lineno = 0;
        if (nullptr != ph) root->child[0] = ph;
        else syntaxError("need a program head");
        if (nullptr != dp) root->child[1] = dp;
        if (nullptr != pb) root->child[2] = pb;
        else syntaxError("need a program body");
    }
    match(DOT);

    return root;
}

void Parse::syntaxError(QString msg) {

    qDebug()<<msg;
    //TODO show the msg
}

//该函数将当前单词与函数参数给定单词相比较，如果一致，则取下一单词；否则，退出语法分析程序。
bool Parse::match(LexType expected) {
    if (nullptr!=head&&head->getLex() == expected) {
        head = head->next;
        if(head!=nullptr){
        line0 = head->getLine();
        lineno=line0;

        return true;
        }
    } else {
        syntaxError("ERROR not match,except:"+lexName[expected]+" get:"+lexName[head->getLex()]+" in line "+head->getLine());
        return false;
    }

}

/**
 * programHead ::=PROGRAM ProgramName
 * @return
 */
/*该函数根据读入的单词，匹配保留字PROGRAM，然后记录程序名于程序头节点，匹配ID。如果处理成功，函数返回生成的程序头节点类型语法树节点；否则返回NULL。
*/
TreeNode *Parse::programHead() {
    TreeNode *t = newPheadNode();
	//若当前程序不是以program开头，则返回NULL
    if(!match(PROGRAM))
        goto e;
	//新语法树节点t创建成功，且当前单词为ID，则将当前单词的语义信息赋给新语法树节点t的成员attr.name[0]，作为程序的名字。
    if ((nullptr != t) && (head->getLex() == ID)) {
        t->lineno = head->getLine();
        strcpy(t->name[0], head->getSem().toStdString().c_str());
	//将当前单词和ID匹配，若失败则返回NULL
    if(!match(ID))
        goto e;
    }
	//若当前单词(程序名)为空或为保留字(非ID)则报错
	else{
        syntaxError("need a program name in line:"+lineno);
    }
    return t;
    e:
    delete t;
    t=nullptr;
    return t;
}


/*
 * declarePart ::=typeDec varDec procDec
 */
/*该函数根据文法产生式，创建新的类型声明标志节点、变量声明标志节点。
  调用类型声明部分处理分析函数TypeDec()(其返回指针tp1为类型声明标志节点的第一个子节点)、变量声明部分处理分析函数VarDec()(其返回指针tp2为变量声明标志节点的第一个子节点)、
  过程声明部分处理分析函数ProcDec()。
  使变量声明标志节点为类型声明标志节点的兄弟节点，过程声明节点为变量声明标志节点的兄弟节点。
  如果处理成功，函数返回指向类型声明标志节点(如果没有类型声明，指向变量声明标志节点；如果没有变量声明，则指向过程声明节点)；否则返回NULL。
*/
TreeNode *Parse::declarePart() {
	//创建新的类型声明标志类型语法树节点
    TreeNode *tp = newDecANode(TypeK);
    TreeNode *pp = tp;
	//若新语法树节点typeP创建成功
    if (nullptr != tp) {
        tp->lineno = 0;//TODO lineno=lineno??
		//调用类型声明处理函数，返回值tp1
        TreeNode *tp1 = typeDec();
		//若tp1创建成功，则tp1赋给typeP的成员child[0]，作为类型声明部分
        if (nullptr != tp1)
            tp->child[0] = tp1;
		//若tp1创建失败则说明没有类型声明，将tp节点设为NULL
        else {
            free(tp);
            tp = nullptr;
        }
    }

	//创建新的变量声明标志类型语法树节点
    TreeNode *varp = newDecANode(VarK);
	//若新语法树节点varp创建成功
    if (nullptr != varp) {
        varp->lineno = 0;//TODO lineno??
		//调用变量声明处理函数，返回值tp2
        TreeNode *tp2 = varDec();
		//若tp2创建成功，则将tp2赋给varp的成员child[0]，作为变量声明部分
        if (nullptr != tp2)
            varp->child[0] = tp2;
        else {
            free(varp);
            varp = nullptr;
        }
    }

	//调用过程声明处理函数
    TreeNode *proc = procDec();
    if (nullptr == proc) {}
    if (nullptr == varp) { varp = proc; };
    if (nullptr == tp) { pp = tp = varp; };
    if (tp != varp) {
        tp->sibling = varp;
        //tp = varp;//??此句的作用，是否可以删除?
    }
    if (varp != proc) {
        varp->sibling = proc;
        //varp = proc;
    };
    return pp;
}


/**
 * typeDec ::=0|TypeDeclaration
 * @return
 */
//该函数根据读入的下个单词，如当前单词为TYPE，调用函数typeDeclaration()并赋值给t，则函数返回t；否则返回NULL。
TreeNode *Parse::typeDec() {
    TreeNode *t = nullptr;
    switch (head->getLex()) {
        case TYPE:
            t = typeDeclaration();
            break;
        case VAR:
        case PROCEDURE:
        case BEGIN:
            break;

		//若单词不是TYPE\VAR\PROCEDURE\BEGIN则显示提示信息并跳过此单词，读取下一个单词
        default :
            syntaxError("unexpected token:" + head->getSem()+" in line :"+QString::number(lineno));
            head = head->next;
            break;
    }
    return t;
}

/**
 * TypeDeclaration ::=TYPE TypeDecList
 * @return
 */
//该函数根据文法产生式，匹配保留字TYPE，调用函数typeDecList()并赋值给t。如果t为NULL，则显示提示信息。函数返回t。
TreeNode *Parse::typeDeclaration() {
    if(!match(TYPE))
        syntaxError("Warning :need a declaration");
    auto t = typeDecList();
    if (nullptr == t) {
        syntaxError("Warning :need a declaration");
    }
    return t;

}

/**
 * TypeDecList ::= TypeId=TypeDef;TypeDecMore
 * @return
 */
//该函数根据文法产生式，创建新的声明类型节点t。如果申请成功，调用函数typeId()，匹配保留字EQ，调用函数typeName()，匹配保留字SEMI，调用函数typeDecMore()，返回值赋值给t的成员sibling。
//函数返回t
TreeNode *Parse::typeDecList() {
    auto t = newDecNode();
    if (nullptr != t) {
        t->lineno = line0;
        typeId(t);
        if(!match(EQ))
            goto e;
        typeName(t);
        if(!match(SEMI))
            goto e;
        auto *more = typeDecMore();
        if (nullptr != more) {
            t->sibling = more;
        }
    }

    return t;

    e:
    delete t;
    t=nullptr;
    return t;
}

/**
 * typeId ::=id
 * @param pNode
 */
//参数为t，无返回值。该函数根据读入的单词，判断TOKEN.Lex=ID的值，如果为真，则将TOKEN.Sem字符串拷贝到参数t的成员attr.name[tnum]中
//其中，tnum是用来记录name个数的临时变量。然后再将tnum加1，送回参数t的成员attr.idnum中。匹配单词ID。
void Parse::typeId(TreeNode *pNode) {

    int tnum = pNode->idnum;
    if (head->getLex() == ID) {
        strcpy(pNode->name[tnum], head->getSem().toStdString().c_str());
        tnum += 1;
    }else{
        syntaxError("Type Define need a ID");
    }
    pNode->idnum = tnum;
    match(ID);

}

/**
 * typeName ::=baseType|structureType|id
 * @param pNode
 */
//参数为pNode,无返回值。该函数根据读入的单词，或调用BaseType()，或调用structureType()，或者复制标识符名称、匹配标识符，
//								      或者跳过错误单词、读入下一个单词。
void Parse::typeName(TreeNode *pNode) {
    if (nullptr != pNode) {
        switch (head->getLex()) {
			//若当前单词为INTERGER\CHAR，则调用baseType()
            case INTEGER_T:
            case CHAR_T:
                baseType(pNode);
                break;

			//若当前单词为ARRAY\RECORD，则调用structureType()
            case ARRAY:
            case RECORD:
                structureType(pNode);
                break;

			//若当前单词为ID，则将pNode的成员kind.dec赋值为IdK,将该标识符赋值给pNode的成员type_name[0]，匹配ID
            case ID:
                pNode->kind.dec = IdK;
                strcpy(pNode->type_name, head->getSem().toStdString().c_str());
                match(ID);
                break;

			//若单词不为以上类型，则直接读取下一个单词
            default :
                head = head->next;
                syntaxError("unexpected Type");
                break;
        }
    }

}

/**
 * baseType ::= INTEGER|CHAR
 * @param pNode
 */
//参数为pNode,无返回值。该函数根据读入的单词判断执行哪个分支。
void Parse::baseType(TreeNode *pNode) {
    switch (head->getLex()) {
		//匹配单词INTEGER，参数pNode的成员kind.dec赋值为IntegerK
        case INTEGER_T:
            match(INTEGER_T);
            pNode->kind.dec = IntegerK;
            break;
		//匹配单词CHAR，参数pNode的成员kind.dec赋值为CharK
        case CHAR_T:
            match(CHAR_T);
            pNode->kind.dec = CharK;
            break;

		//若单词不为以上类型，则直接读取下一个单词
        default:
            head = head->next;
            syntaxError("unexpected BaseType");
            break;
    }

}

/**
 * structureType :==arrayType|recType
 * @param pNode
 */
//参数为pNode,无返回值。该函数根据读入的单词判断选择调用函数ArrayType,或者调用函数RecType()。
void Parse::structureType(TreeNode *pNode) {
    switch (head->getLex()) {
        case ARRAY:
            arrayType(pNode);
            break;
        case RECORD:
            pNode->kind.dec = RecordK;
            recType(pNode);
            break;
        default:
            head = head->next;
            syntaxError("unexpected StructureType");
            break;
    }

}

/**
 * arrayTyoe ::=ARRAY [low..top] OF BaseType
 * @param pNode
 */
//参数为pNode，无返回值。该函数对读入的单词进行匹配，并记录数组的上界和下界数值，再匹配保留字，最后调用基本类型函数baseType()，记录数组的子类型。
void Parse::arrayType(TreeNode *pNode) {
    match(ARRAY);
    match(LMIDPAREN);
	//当前单词为INTC，则将当前的数字字符转换成有意义的数字，并赋值给参数pNode的成员attr.ArrayAttr.low，作为数组下界。
    if (head->getLex() == INTC_VAL) {
        pNode->attr.ArrayAttr.low = head->getSem().toInt();
    }
    match(INTC_VAL);
    match(UNDERRANGE);
	//当前单词为INTC，则将当前的数字字符转换成有意义的数字，并赋值给参数pNode的成员attr.ArrayAttr.up，作为数组上界。
    if (head->getLex() == INTC_VAL) {
        pNode->attr.ArrayAttr.up = head->getSem().toInt();
    }
    match(INTC_VAL);
    match(RMIDPAREN);
    match(OF);
    baseType(pNode);
	//将参数pNode的成员kind.dec赋值给pNode的另一个成员attr.ArrayAttr.childtype作为子类型，kind.dec赋值为ArrayK。
    pNode->attr.ArrayAttr.childtype = pNode->kind.dec;
    pNode->kind.dec = ArrayK;
}

/**
 * recType ::= RECORD fieldDecLIst END
 * @param pNode
 */
//参数为pNode，无返回值。该函数对读入的单词进行匹配，调用记录中的域函数fieldDecList(),最后再对读入的单词进行匹配。
void Parse::recType(TreeNode *pNode) {
    match(RECORD);
    TreeNode *p = nullptr;
    p = fieldDecList();
    if (nullptr != p) {
        pNode->child[0] = p;
    } else syntaxError("need a record body");
    match(END);


}

/**
 * fieldDecList ::=baseType idList;FieldDecMore|ArrayType idList;FieldDecMore
 * @return
 */
//该函数根据读入的单词判断记录域中的变量属于哪种类型，根据产生式的select集合判断执行哪个分支程序。
//其中，相同类型的变量id用“,”分隔开，其名称记录在语法树的同一个节点中；不同类型变量节点互为兄弟节点。
TreeNode *Parse::fieldDecList() {
    auto *t = newDecNode();
    TreeNode *p = nullptr;
    t->lineno = line0;
    switch (head->getLex()) {
		//若当前单词为INTEGER\CHAR
        case INTEGER_T:
        case CHAR_T:
            baseType(t);
            idList(t);
            match(SEMI);
            p = fieldDecMore();
            break;

		//若当前单词为ARRAY
        case ARRAY:
            arrayType(t);
            idList(t);
            match(SEMI);
            p = fieldDecMore();
            break;

		//若单词不为以上类型，则直接读取下一个单词
        default :
            head = head->next;
            syntaxError("unexpected Type . Only accept INTEGER CHAR ARRAY");
            break;
    }
    t->sibling = p;
    return t;
}

/**
 * idList ::= ID IdMore
 * @param pNode
 */
 //参数为pNode,无返回值。该函数根据文法产生式，匹配标识符ID，记录标识符名称，调用递归处理函数IdMore()。
void Parse::idList(TreeNode *pNode) {
    if (head->getLex() == ID) {
		//记录标识符名称于参数pNode的成员name[pNode->idnum]中，并且把pNode->idnum加1.
        strcpy(pNode->name[pNode->idnum], head->getSem().toStdString().c_str());
        match(ID);
        pNode->idnum += 1;
    }else{
        syntaxError("IDList need an ID but "+lexName[head->getLex()]+" get");
    }
    idMore(pNode);

}

/**
 * idMore ::=0|,IdList
 * @param pNode
 */
//参数为pNode,无返回值。该函数根据文法产生式判断读入的单词，处理调用相应的函数。
void Parse::idMore(TreeNode *pNode) {
    switch (head->getLex()) {
        case SEMI:
            break;
        case COMMA:
            match(COMMA);
            idList(pNode);
            break;
        default :
            head = head->next;
            syntaxError("unexpected token in idMore");
            break;
    }
}

/**
 * fieldDeMore ::=0|FieldDecList
 * @return
 */
//该函数根据文法产生式判断读入的单词，若为END，则不做任何处理；若为INTEGER、CHAR或ARRAY，则调用FieldDecList()递归处理函数；
//否则读入下一个TOKEN，函数返回p。
TreeNode *Parse::fieldDecMore() {
    TreeNode *p = nullptr;
    switch (head->getLex()) {
        case END:
            break;
        case INTEGER_T:
        case CHAR_T:
        case ARRAY:
            p = fieldDecList();
            break;
        default :
            head = head->next;
            syntaxError("unexpected token "+lexName[head->getLex()]);
            break;
    }
    return p;
}

/**
 * typeDecMore ::=0|TypeDecList
 * @return
 */
//该函数根据读入的单词，或者调用函数typeDecList()并赋值给p，则函数返回p；或者什么都不做，或者跳过此错误单词，读入下一单词，返回NULL.
TreeNode *Parse::typeDecMore() {
    TreeNode *p = nullptr;
    switch (head->getLex()) {
		//若读入单词为VAR\PROCEDURE\BEGIN，则直接返回p=NULL.
        case VAR:
        case PROCEDURE:
        case BEGIN:
            break;

		//若读入单词为ID，则调用函数typeDecList()
        case ID:
            p = typeDecList();
            break;

        default:
		//若读入单词均不为上述类型，则读入下一个单词
            head = head->next;
            //syntaxError("unexpected token when declare more Type");
            break;
    }
    return p;
}


/********************************************************************/
/* 产生式 < varDec > ::=  ε |  varDeclaration                      */
/********************************************************************/
//该函数根据文法产生式判断读入的单词，处理调用相应的函数。如果处理成功，则返回t,否则返回NULL.
TreeNode *Parse::varDec() {
    TreeNode *t = nullptr;
    switch (head->getLex()) {
        case PROCEDURE:
        case BEGIN:
            break;
        case VAR:
            t = varDeclaration();
            break;
        default:
            //head = head->next;
            syntaxError("unexpected token is here! "+lexName[head->getLex()]+head->getLine());
            break;
    }
    return t;
}


/********************************************************************/
/* 产生式 < varDeclaration > ::=  VAR  varDecList                   */
/********************************************************************/
//该函数根据文法产生式，匹配保留字VAR，调用函数varDecList()。如果处理成功，则返回t;否则返回NULL.
TreeNode *Parse::varDeclaration() {
    match(VAR);
    TreeNode *t = varDecList();
    if (t == nullptr)
        syntaxError("a var declaration is expected!");
    return t;
}


/**
* 产生式 < varDecList > ::=  typeName varIdList; varDecMore
**/
//该函数根据文法产生式调用相应的变量声明部分的处理函数typeName(),varIdList(),varDecMore()。
//如果处理成功，则返回t；否则返回NULL.
TreeNode *Parse::varDecList() {
    TreeNode *t = newDecNode();
    TreeNode *p = nullptr;

    if (t != nullptr) {
        t->lineno = line0;
        typeName(t);
        varIdList(t);
        match(SEMI);
        p = varDecMore();
        t->sibling = p;
    }
    return t;
}

/********************************************************************/
/* 产生式 < varDecMore > ::=  ε |  varDecList                      */
/********************************************************************/
//该函数根据文法产生式，由读入的单词判断执行哪个分支，最后返回t。
TreeNode *Parse::varDecMore() {
    TreeNode *t = nullptr;

    switch (head->getLex()) {
        case PROCEDURE:
        case BEGIN:
            break;
        case INTEGER_T:
        case CHAR_T:
        case ARRAY:
        case RECORD:
        case ID:
            t = varDecList();
            break;
        default:
            syntaxError("unexpected token"+head->getLexName()+" is here in line:"+QString::number(lineno));
            break;
    }
    return t;
}

/**
/* 产生式 < varIdList > ::=  id  varIdMore
*/
//参数为t，无返回值。该函数根据文法产生式，匹配标识符名ID，记录标识符名称，调用函数varIdMore().
void Parse::varIdList(TreeNode *t) {
    if (head->getLex() == ID) {
        strcpy(t->name[(t->idnum)], head->getSem().toStdString().c_str());
        match(ID);
        t->idnum = (t->idnum) + 1;
    } else {
		head = head->next;
        syntaxError("a varid is expected here! in line:"+QString::number(lineno));
    }
    varIdMore(t);
}


/********************************************************************/
/* 产生式 < varIdMore > ::=  ε |  , varIdList                      */
/********************************************************************/
//参数为t，无返回值。该函数根据文法产生式，由读入的单词判断执行哪个分支(判断是否还有其他变量)。
void Parse::varIdMore(TreeNode *t) {
    switch (head->getLex()) {
        case SEMI:
            break;
        case COMMA:
            match(COMMA);
            varIdList(t);
            break;
        default:
			head = head->next;
            syntaxError("unexpected token "+head->getLexName()+ "is here! in line:"+QString::number(lineno));
            break;
    }
}
/********************************************************************/
/* 产生式 < programBody > ::=  BEGIN  stmList   END                 */
/********************************************************************/
//该函数根据文法产生式，创建新的语句标志类型语法树节点，并匹配保留字，调用语句序列函数stmList()。
//如果成功处理，则返回t；否则返回NULL.
TreeNode *Parse::programBody() {
    TreeNode *t = newStmlNode();
    match(BEGIN);
    if (t != nullptr) {
        t->lineno = 0;
        t->child[0] = stmList();
    }
    match(END);
    return t;
}


/********************************************************************/
/* 产生式 < stmList > ::=  stm    stmMore                           */
/********************************************************************/
//该函数根据文法产生式，调用语句处理函数stm()，返回指针t；
//调用更多语句处理函数stmMore()，返回指针p,并使p成为t的兄弟节点。
//如果处理成功，则返回指针t，否则返回NULL.
TreeNode *Parse::stmList() {
    TreeNode *t = stm();
    TreeNode *p = stmMore();
    if (t != nullptr)
        if (p != nullptr)
            t->sibling = p;
    return t;
}

/********************************************************************/
/* 产生式 < stmMore > ::=   ε |  ; stmList                         */
/********************************************************************/
//该函数根据文法产生式，由读入单词判断执行哪个分支结构，即是否有更多的语句。
//如果处理成功，则返回t；否则返回NULL.
TreeNode *Parse::stmMore() {
    TreeNode *t = nullptr;
    switch (head->getLex()) {
        case ELSE:
        case FI:
        case END:
        case ENDWH:
            break;
        case SEMI:
            match(SEMI);
            t = stmList();
            break;
        default:
			head = head->next;
            syntaxError("unexpected token "+head->getLexName()+"is here! in line:"+QString::number(lineno));
            break;
    }
    return t;
}

/********************************************************************/
/* 产生式 < stm > ::=   conditionalStm   {IF}                       */
/*                    | loopStm          {WHILE}                    */
/*                    | inputStm         {READ}                     */
/*                    | outputStm        {WRITE}                    */
/*                    | returnStm        {RETURN}                   */
/*                    | id  assCall      {id}                       */
/********************************************************************/
//该函数根据读入单词和每条产生式所对应的predict集合元素，选择调用相应的语句处理函数。
//例如，如果当前单词为IF，则调用条件语句处理函数conditionalStm()。
//如果处理成功，函数返回生成的语句类型语法树节点t；否则返回NULL.
TreeNode *Parse::stm() {

    TreeNode *t = nullptr;
    switch (head->getLex()) {
        case IF:
            t = conditionalStm();
            break;
        case WHILE:
            t = loopStm();
            break;
        case READ:
            t = inputStm();
            break;
        case WRITE:
            t = outputStm();
            break;
        case RETURN:
            t = returnStm();
            break;
        case ID:
            temp_name = head->getSem();
            match(ID); //此处和课本有出入
            t = assCall();
            break;

		//当前单词为其他单词，非期望单词语法错误，跳过当前单词，读入下一单词
        default:
			head = head->next;
            syntaxError("unexpected token "+head->getLexName()+"is here! in  line:"+QString::number(lineno));
            break;
    }
    return t;
}

/********************************************************************/
/* 产生式 < assCall > ::=   assignmentRest   {:=,LMIDPAREN,DOT}     */
/*                        | callStmRest      {(}                    */
/********************************************************************/
//由于赋值语句和过程调用语句的开始部分都是标识符，所以该函数根据读入的单词选择调用相应的处理程序(赋值语句处理和过程调用语句处理程序)。
//如果处理成功，函数返回生成的语句类型语法树节点t;否则返回NULL.
TreeNode *Parse::assCall() {
    TreeNode *t = nullptr;
    switch (head->getLex()) {
        case ASSIGN:
        case LMIDPAREN:
        case DOT:
            t = assignmentRest();
            break;
        case LPAREN:
            t = callStmRest();
            break;
        default:
            head = head->next;
            syntaxError("unexpected token "+head->getLexName()+"is here! in line:"+QString::number(lineno));
            break;
    }
    return t;
}

/********************************************************************/
/* 产生式 < assignmentRest > ::=  variMore : = exp                  */
/********************************************************************/
TreeNode *Parse::assignmentRest() {
    TreeNode *t = newStmtNode(AssignK);

    /* 赋值语句节点的第一个儿子节点记录赋值语句的左侧变量名，*
     * 第二个儿子结点记录赋值语句的右侧表达式*/
    if (t != nullptr) {
        t->lineno = line0;


        TreeNode *child1 = newExpNode(VariK);
        if (child1 != nullptr) {
            child1->lineno = line0;
            strcpy(child1->name[0], temp_name.toStdString().c_str());
            (child1->idnum)++;
            variMore(child1);
            t->child[0] = child1;
        }


        match(ASSIGN);


        t->child[1] = mexp();

    }
    return t;
}

/********************************************************************/
/* 产生式 < conditionalStm > ::= IF exp THEN stmList ELSE stmList FI*/
/********************************************************************/
//该函数根据文法产生式，匹配保留字IF，调用表达式函数Exp()；
//匹配保留字THEN，调用语句序列函数StmL()(此处的语句序列函数不同于前处的StmList()，如果只有一条语句，则语句后面不用加分号；
//									   如果有两条或者多条语句，则要在语句序列开始前加上BEGIN，在结束后加上END。其中的最后一条语句末尾不用加分号)。
//如果处理成功，函数返回生成的条件语句类型语法树节点t；否则返回NULL.
TreeNode *Parse::conditionalStm() {
    TreeNode *t = newStmtNode(IfK);
    match(IF);
    if (t != nullptr) {
        t->lineno = line0;
        t->child[0] = mexp();
    }
    match(THEN);
    if (t != nullptr) 
		t->child[1] = stmList();
    if (head->getLex() == ELSE) {
        match(ELSE);
        if (t != nullptr)
            t->child[2] = stmList();
    }
    match(FI);
    return t;
}


/********************************************************************/
/* 产生式 < loopStm > ::=      WHILE exp DO stmList ENDWH           */
/********************************************************************/
//该函数根据文法的产生式，匹配保留字WHILE，调用表达式处理函数mexp(),再匹配保留字DO，调用语句序列处理函数(不同于条件语句中的语句序列函数部分)，最后匹配保留字ENDWH。
//如果处理成功，函数返回新生成的循环语句类型的语法树节点t，否则返回NULL.
TreeNode *Parse::loopStm() {
    TreeNode *t = newStmtNode(WhileK);
    match(WHILE);
    if (t != nullptr) {
        t->lineno = line0;
        t->child[0] = mexp();
        match(DO);
        t->child[1] = stmList();
        match(ENDWH);
    }
    return t;
}

/********************************************************************/
/* 产生式 < inputStm > ::=    READ(id)                              */
/********************************************************************/
//该函数根据文法产生式，创建新的输入语句类型语法树节点t，匹配保留字READ，LPAREN，记录标识符名称，匹配ID,RPAREN.
//如果处理成功，函数返回新建的语法树节点指针t；否则返回NULL.
TreeNode *Parse::inputStm() {
    TreeNode *t = newStmtNode(ReadK);
    match(READ);
    match(LPAREN);
    if ((t != nullptr) && (head->getLex() == ID)) {
        t->lineno = line0;
        strcpy(t->name[0], head->getSem().toStdString().c_str());
        (t->idnum)++;
    }
    match(ID);
    match(RPAREN);
    return t;
}

/********************************************************************/
/* 产生式 < outputStm > ::=   WRITE(exp)                            */
/********************************************************************/
//该函数根据文法产生式，创建新的输出语句类型语法树节点t，匹配保留字WRITE,LPAREN,调用函数mexp()，匹配保留字RPAREN.
//如果处理成功，函数返回新创建的输出类型节点t，否则NULL。
TreeNode *Parse::outputStm() {
    TreeNode *t = newStmtNode(WriteK);
    match(WRITE);
    match(LPAREN);
    if (t != nullptr) {
        t->lineno = line0;
        t->child[0] = mexp();
    }
    match(RPAREN);
    return t;
}

/********************************************************************/
/* 产生式 < returnStm > ::=   RETURN                                */
/********************************************************************/
//该函数根据文法产生式，创建新的过程返回类型的语法树节点t,匹配保留字RETURN。
//如果处理成功，函数返回新的过程返回类型的节点t；否则返回NULL.
TreeNode *Parse::returnStm() {
    TreeNode *t = newStmtNode(ReturnK);
    match(RETURN);
    if (t != nullptr)
        t->lineno = line0;
    return t;
}

/********************************************************************/
/* 产生式 < callStmRest > ::=  (actParamList)                       */
/********************************************************************/
//该函数根据文法产生式，创建新的过程调用类型语法树节点t，匹配保留字LPAREN，调用实参处理分析函数actParamList(),匹配RPAREN。如果处理成功，函数返回新的过程调用类型节点t；否则返回NULL.
TreeNode *Parse::callStmRest() {
    TreeNode *t = newStmtNode(CallK);
    match(LPAREN);
    /*函数调用时，其子节点指向实参*/
    if (t != nullptr) {
        t->lineno = line0;


        TreeNode *child0 = newExpNode(VariK);
        if (child0 != nullptr) {
            child0->lineno = line0;
            strcpy(child0->name[0], temp_name.toStdString().c_str());
            (child0->idnum)++;
            t->child[0] = child0;
        }
        t->child[1] = actParamList();
    }
    match(RPAREN);
    return t;
}

/**
/* 函数名 actParamList
/* 产生式 < actParamList > ::=     ε |  exp actParamMore
**/
//该函数根据读入的单词选择执行哪段程序(有参数或者无参数)。
//如果处理成功，则返回t；否则返回NULL.
TreeNode *Parse::actParamList() {
    TreeNode *t = nullptr;

    switch (head->getLex()) {
        case RPAREN:
            break;
        case ID:
        case INTC_VAL:
            t = mexp();
            if (t != nullptr)
                t->sibling = actParamMore();
            break;
        default:
            head = head->next;
            syntaxError("unexpected token "+head->getLexName()+" is here! in line:"+QString::number(lineno));
            break;
    }
    return t;
}

/**
 * 功  能 函数调用实参部分的处理函数
 * 产生式 < actParamMore > ::=     ε |  , actParamList
 */
TreeNode *Parse::actParamMore() {
    TreeNode *t = nullptr;
    switch (head->getLex()) {
        case RPAREN:
            break;
        case COMMA:
            match(COMMA);
            t = actParamList();
            break;
        default:
      //      head = head->next;
            syntaxError("unexpected token "+head->getLexName()+" is here! in line:"+QString::number(lineno));
            break;
    }
    return t;
}



/*************************表达式部分********************************/
/****************************************************************************/
/* 产生式 < 表达式 > ::= < 简单表达式 > [< 关系运算符 > < 简单表达式 > ]	*/
/****************************************************************************/
TreeNode *Parse::mexp() {

    TreeNode *t = simple_exp();


    if ((head->getLex() == LT) || (head->getLex() == EQ)) {

        TreeNode *p = newExpNode(OpK);


        if (p != nullptr) {
            p->lineno = line0;
            p->child[0] = t;
            p->attr.ExpAttr.op = head->getLex();


            t = p;
        }


        match(head->getLex());


        if (t != nullptr)
            t->child[1] = simple_exp();
    }


    return t;
}


/************************************************************************/
/* 产生式 < 简单表达式 >::=	< 项 > { < 加法运算符 > < 项 > }			*/
/************************************************************************/
TreeNode *Parse::simple_exp() {

    TreeNode *t = term();


    while ((head->getLex() == PLUS) || (head->getLex() == MINUS)) {

        TreeNode *p = newExpNode(OpK);


        if (p != nullptr) {
            p->lineno = line0;
            p->child[0] = t;
            p->attr.ExpAttr.op = head->getLex();


            t = p;


            match(head->getLex());


            t->child[1] = term();
        }
    }

    return t;
}


/********************************************************************/
/* 产生式 < procDec > ::=  ε |  procDeclaration                    */
/********************************************************************/
//该函数根据文法产生式判断当前单词，若为BEGIN，则不作任何动作；若为PROCEDURE，则调用过程声明函数procDeclaration()；否则，读入下一个单词。
//函数返回t。
TreeNode *Parse::procDec() {
    TreeNode *t = nullptr;
    switch (head->getLex()) {
        case BEGIN:
            break;
        case PROCEDURE:
            t = procDeclaration();
            break;
        default:
            //head = head->next;
            syntaxError("unexpected token "+head->getLexName()+" is here! in line:"+QString::number(lineno));
            break;
    }
    return t;
}


/********************************************************************/
/* 产生式 < formList > ::=  id  fidMore                             */
/********************************************************************/
//参数为t，无返回值。
//该函数根据文法产生式，记录参数声明中的参数名称，匹配保留字ID，调用fidMore()
void Parse::formList(TreeNode *t) {
    if (head->getLex() == ID) {
        strcpy(t->name[(t->idnum)], head->getSem().toStdString().c_str());
        t->idnum = (t->idnum) + 1;
        match(ID);
    }
    fidMore(t);
}


/********************************************************************/
/* 产生式 < fidMore > ::=   ε |  , formList                        */
/********************************************************************/
//参数为t，无返回值。
//该函数根据文法产生式，由读入的单词判断执行哪个分支程序。
void Parse::fidMore(TreeNode *t) {
    switch (head->getLex()) {
        case SEMI:
        case RPAREN:
            break;
        case COMMA:
            match(COMMA);
            formList(t);
            break;
        default:
            head = head->next;
            syntaxError("unexpected token "+head->getLexName()+" is here! in line:"+QString::number(lineno));
            break;
    }
}
/********************************************************************/
/* 产生式 < procDeclaration > ::=  PROCEDURE                        */
/*                                 ProcName(paramList);             */
/*                                 procDecPart                      */
/*                                 procBody                         */
/*                                 procDec                          */
/********************************************************************/
//该函数根据文法产生式，匹配保留字PROCEDURE，调用过程名函数procName()，参数函数ParamList()，过程体内部声明部分函数procDecPart(),过程体函数procBody()。
//如果处理成功，则返回t；否则返回NULL。
TreeNode *Parse::procDeclaration() {
    TreeNode *t = newProcNode();
    match(PROCEDURE);
    if (t != nullptr) {
        t->lineno = line0;
        if (head->getLex() == ID) {
            strcpy(t->name[0], head->getSem().toStdString().c_str());
            (t->idnum)++;
            match(ID);
        }
        match(LPAREN);
        paramList(t);
        match(RPAREN);
        match(COLON);
        t->child[1] = procDecPart();
        t->child[2] = procBody();
        //t->sibling = procDec();
    }
    return t;
}


/********************************************************************/
/* 产生式 < procBody > ::=  programBody                             */
/********************************************************************/
//该函数根据文法产生式，调用程序体部分函数programBody()。如果处理成功，则返回t；否则返回NULL。
TreeNode *Parse::procBody() {
    TreeNode *t = programBody();
    if (t == nullptr)
        syntaxError("a program body is requested!");
    return t;
}

/********************************************************************/
/* 产生式 < procDecPart > ::=  declarePart                          */
/********************************************************************/
//该函数根据文法产生式，调用声明部分函数declarePart()，如果处理成功，则返回t；否则返回NULL.
TreeNode *Parse::procDecPart() {
    TreeNode *t = declarePart();
    return t;
}



/********************************************************************/
/* 产生式 < paramList > ::=  ε |  paramDecList                     */
/********************************************************************/
//参数为t，无返回值。该函数根据文法产生式判断读入的单词，若为RPAREN，则不做任何动作；
//若为INTEGER,CHAR,ARRAY,RECORD,ID,VAR,则调用参数声明序列处理函数paramDecList()；否则，读入下一个TOKEN.
void Parse::paramList(TreeNode *t) {
    TreeNode *p = nullptr;

    switch (head->getLex()) {
        case RPAREN:
            break;
        case INTEGER_T:
        case CHAR_T:
        case ARRAY:
        case RECORD:
        case ID:
        case VAR:
            p = paramDecList();
            t->child[0] = p;
            break;
        default:
            head = head->next;
            syntaxError("unexpected token "+head->getLexName()+" is here! in line:"+QString::number(lineno));
            break;
    }
}


/********************************************************************/
/* 产生式 < paramDecList > ::=  param  paramMore                    */
/********************************************************************/
//该函数根据文法产生式，调用函数mparam()，返回指针t；调用函数paramMore()，返回指针p。
//如果p不为NULL，p赋值给t的成员变量sibling。如果处理成功，则函数返回指针t；否则返回NULL.
TreeNode *Parse::paramDecList() {
    TreeNode *t = mparam();
    TreeNode *p = paramMore();
    if (p != nullptr) {
        t->sibling = p;
    }
    return t;
}

/********************************************************************/
/* 产生式 < paramMore > ::=  ε | ; paramDecList                     */
/********************************************************************/
//该函数根据文法产生式，由读入的单词判断执行哪个分支(判断是否还有其他参数声明)。
TreeNode *Parse::paramMore() {
    TreeNode *t = nullptr;
    switch (head->getLex()) {
        case RPAREN:
            break;
        case SEMI:
            match(SEMI);
            t = paramDecList();
            if (t == nullptr)
                syntaxError("a param declaration is request!");
            break;
        default:
            head = head->next;
            syntaxError("unexpected token "+head->getLexName()+" is here! in line:"+QString::number(lineno));
            break;
    }
    return t;
}

/********************************************************************/
/* 产生式 < param > ::=  typeName formList | VAR typeName formList  */
/********************************************************************/
//该函数根据文法产生式，由读入的单词判断执行哪个分支程序，即判断是值参还是变参。
//如果处理成功，则返回t；否则返回NULL.
TreeNode *Parse::mparam() {
    TreeNode *t = newDecNode();
    if (t != nullptr) {
        t->lineno = line0;
        switch (head->getLex()) {
            case INTEGER_T:
            case CHAR_T:
            case ARRAY:
            case RECORD:
            case ID:
                t->attr.ProcAttr.paramt = valparamType;
                typeName(t);
                formList(t);
                break;
            case VAR:
                match(VAR);
                t->attr.ProcAttr.paramt = varparamType;
                typeName(t);
                formList(t);
                break;
            default:
                head = head->next;

            syntaxError("unexpected token "+head->getLexName()+" is here! in line:"+QString::number(lineno));
                break;
        }
    }
    return t;
}



/****************************************************************************/
/* 产生式 < 项 > ::= < 因子 > { < 乘法运算符 > < 因子 > }					*/
/****************************************************************************/
TreeNode *Parse::term() {

    TreeNode *t = factor();


    while ((head->getLex() == TIMES) || (head->getLex() == DIVIDE)) {

        treeNode *p = newExpNode(OpK);


        if (p != nullptr) {
            p->lineno = line0;
            p->child[0] = t;
            p->attr.ExpAttr.op = head->getLex();
            t = p;
        }


        match(head->getLex());


        p->child[1] = factor();

    }

    return t;
}


/****************************************************************************/
/* 产生式 factor ::= ( exp ) | INTC | variable                  			*/
/****************************************************************************/
TreeNode *Parse::factor() {

    TreeNode *t = nullptr;

    switch (head->getLex()) {
        case INTC_VAL :


            t = newExpNode(ConstK);


            if ((t != nullptr) && (head->getLex() == INTC_VAL)) {
                t->lineno = line0;
                t->attr.ExpAttr.val = head->getSem().toInt();
            }


            match(INTC_VAL);
            break;


        case ID :


            t = variable();
            break;


        case LPAREN :


            match(LPAREN);


            t = mexp();


            match(RPAREN);

            break;


        default:
            head = head->next;
            syntaxError("unexpected token "+head->getLexName()+" is here! in line:"+QString::number(lineno));
            break;
    }

    return t;
}

/********************************************************************/		
/* 产生式 variable   ::=   id variMore                   			*/
/********************************************************************/
TreeNode *Parse::variable() {
    TreeNode *t = newExpNode(VariK);

    if ((t != nullptr) && (head->getLex() == ID)) {
        t->lineno = line0;
        strcpy(t->name[0], head->getSem().toStdString().c_str());
        (t->idnum)++;
    }

    match(ID);
    variMore(t);
    return t;
}


/********************************************************************/

/* 产生式 variMore   ::=  ε                             			*/
/*                       | [exp]            {[}                     */
/*                       | . fieldvar       {DOT}                   */
/********************************************************************/
void Parse::variMore(TreeNode *t) {
    switch (head->getLex()
            ) {
        case ASSIGN:
        case TIMES:
        case EQ:
        case LT:
        case PLUS:
        case MINUS:
        case DIVIDE:
        case RPAREN:
        case RMIDPAREN:
        case SEMI:
        case COMMA:
        case THEN:
        case ELSE:
        case FI:
        case DO:
        case ENDWH:
        case END:
            break;
        case LMIDPAREN:
            match(LMIDPAREN);


            t->child[0] = mexp();

            t->attr.ExpAttr.varkind = ArrayMembV;


            t->child[0]->attr.ExpAttr.varkind = IdV;
            match(RMIDPAREN);
            break;
        case DOT:
            match(DOT);

            t->child[0] = fieldvar();

            t->attr.ExpAttr.varkind = FieldMembV;

            t->child[0]->attr.ExpAttr.varkind = IdV;
            break;
        default:
            head = head->next;
            syntaxError("unexpected token "+head->getLexName()+" is here! in line:"+QString::number(lineno));
            break;
    }
}
/********************************************************************/
/* 产生式 fieldvar   ::=  id  fieldvarMore                          *
/********************************************************************/
TreeNode *Parse::fieldvar() {

    TreeNode *t = newExpNode(VariK);

    if ((t != nullptr) && (head->getLex() == ID)) {
        t->lineno = line0;
        strcpy(t->name[0], head->getSem().toStdString().c_str());
        (t->idnum)++;
    }
    match(ID);

    fieldvarMore(t);

    return t;
}

/********************************************************************/
/* 产生式 fieldvarMore   ::=  ε                             		*/
/*                           | [exp]            {[}                 */
/********************************************************************/
void Parse::fieldvarMore(TreeNode *t) {
    switch (head->getLex()) {
        case ASSIGN:
        case TIMES:
        case EQ:
        case LT:
        case PLUS:
        case MINUS:
        case DIVIDE:
        case RPAREN:
        case SEMI:
        case COMMA:
        case THEN:
        case ELSE:
        case FI:
        case DO:
        case ENDWH:
        case END:
            break;
        case LMIDPAREN:
            match(LMIDPAREN);

            /*用来以后求出其表达式的值，送入用于数组下标计算*/
            t->child[0] = mexp();
            t->child[0]->attr.ExpAttr.varkind = ArrayMembV;
            match(RMIDPAREN);
            break;
        default:
            head = head->next;
            syntaxError("unexpected token "+head->getLexName()+" is here! in line:"+QString::number(lineno));
            break;
    }
}
