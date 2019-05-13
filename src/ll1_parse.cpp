#include "ll1_parse.h"
#include "utils.h"

#include <QDebug>

TreeNode *current_treenode;
//Token * head;
DecKind *temp;
TreeNode *saveP;

bool getExpResult = true;
int  expflag = 0;
int  getExpResult2 =false;

QSet<LexType> TTSet={
    /* 簿记单词符号 */
    ENDFILE,	ERROR,
    /* 保留字 */
    PROGRAM,	PROCEDURE,	TYPE,	VAR,		IF,
    THEN,		ELSE,		FI,		WHILE,		DO,
    ENDWH,		BEGIN,		END,	READ,		WRITE,
    ARRAY,		OF,			RECORD,	RETURN,

    INTEGER_T,	CHAR_T,
    /* 多字符单词符号 */
    ID,			INTC_VAL,		CHARC_VAL,
    /*特殊符号 */
    ASSIGN,		EQ,			LT,		PLUS,		MINUS,
    TIMES,DIVIDE,		LPAREN,	RPAREN,		DOT,
    COLON,		SEMI,		COMMA,	LMIDPAREN,	RMIDPAREN,
    UNDERRANGE,
};

QSet<LexType> NTSet={
    //非终结符
    Program,	      ProgramHead,	    ProgramName,	DeclarePart,
    TypeDec,        TypeDeclaration,	TypeDecList,	TypeDecMore,
    TypeId,	      TypeName,			BaseType,	    StructureType,
    ArrayType,      Low,	            Top,            RecType,
    FieldDecList,   FieldDecMore,	    IdList,	        IdMore,
    VarDec,	      VarDeclaration,	VarDecList,		VarDecMore,
    VarIdList,	  VarIdMore,		ProcDec,		ProcDeclaration,
    ProcDecMore,    ProcName,		    ParamList,		ParamDecList,
    ParamMore,      Param,		    FormList,		FidMore,
    ProcDecPart,    ProcBody,	    	ProgramBody,	StmList,
    StmMore,        Stm,				AssCall,		AssignmentRest,
    ConditionalStm, StmL,			    LoopStm,		InputStm,
    InVar,          OutputStm,		ReturnStm,		CallStmRest,
    ActParamList,   ActParamMore,		RelExp,			OtherRelE,
    Exp,			  OtherTerm,		Term,           OtherFactor,
    Factor,         Variable,			VariMore,		FieldVar,
    FieldVarMore,   CmpOp,			AddOp,          MultOp,

};

LL1_parse::LL1_parse(const Token *root):head(root) {

    table={
        {{Program,PROGRAM},1},
        {{ProgramHead,PROGRAM},2},
        {{ProgramName,ID},3},
        {{DeclarePart,TYPE},4},
        {{DeclarePart,VAR},4},
        {{DeclarePart,PROCEDURE},4},
        {{DeclarePart,BEGIN},4},
        {{TypeDec,TYPE},6},
        {{TypeDec,VAR},5},
        {{TypeDec,PROCEDURE},5},
        {{TypeDec,BEGIN},5},
        {{TypeDeclaration,TYPE},7},
        {{TypeDecList,ID},8},
        {{TypeDecMore,VAR},9},
        {{TypeDecMore,PROCEDURE},9},
        {{TypeDecMore,BEGIN},9},
        {{TypeDecMore,ID},10},
        {{TypeId,ID},11},
        {{TypeName,INTEGER_T},12},
        {{TypeName,CHAR_T},12},
        {{TypeName,ARRAY},13},
        {{TypeName,RECORD},13},
        {{TypeName,ID},14},


        {{BaseType,INTEGER_T},15},
        {{BaseType,CHAR_T},16},
        {{StructureType,ARRAY},17},
        {{StructureType,RECORD},18},
        {{ArrayType,ARRAY},19},
        {{Low,INTC_VAL},20},
        {{Top,INTC_VAL},21},
        {{RecType,RECORD},22},
        {{FieldDecList,INTEGER_T},23},
        {{FieldDecList,CHAR_T},23},
        {{FieldDecList,ARRAY},24},
        {{FieldDecMore,INTEGER_T},26},
        {{FieldDecMore,CHAR_T},26},
        {{FieldDecMore,ARRAY},26},
        {{FieldDecMore,END},25},
        {{IdList,ID},27},
        {{IdMore,SEMI},28},
        {{IdMore,COMMA},29},
        {{VarDec,VAR},31},
        {{VarDec,PROCEDURE},30},
        {{VarDec,BEGIN},30},
        {{VarDeclaration,VAR},32},
        {{VarDecList,INTEGER_T},33},
        {{VarDecList,CHAR_T},33},
        {{VarDecList,ARRAY},33},
        {{VarDecList,RECORD},33},
        {{VarDecList,ID},33},
        {{VarDecMore,INTEGER_T},35},
        {{VarDecMore,CHAR_T},35},
        {{VarDecMore,ARRAY},35},
        {{VarDecMore,RECORD},35},
        {{VarDecMore,PROCEDURE},34},
        {{VarDecMore,BEGIN},34},
        {{VarDecMore,ID},35},
        {{VarIdList,ID},36},
        {{VarIdMore,SEMI},37},
        {{VarIdMore,COMMA},38},
        {{ProcDec,PROCEDURE},40},
        {{ProcDec,BEGIN},39},
        {{ProcDeclaration,PROCEDURE},41},
        {{ProcDecMore,PROCEDURE},41},
        {{ProcDecMore,BEGIN},42},
        {{ProcName,ID},44},






        {{ParamList,INTEGER_T},46},
        {{ParamList,CHAR_T},46},
        {{ParamList,ARRAY},46},
        {{ParamList,RECORD},46},
        {{ParamList,VAR},46},
        {{ParamList,ID},46},
        {{ParamList,RMIDPAREN},45},
        {{ParamDecList,INTEGER_T},47},
        {{ParamDecList,CHAR_T},47},
        {{ParamDecList,ARRAY},47},
        {{ParamDecList,RECORD},47},
        {{ParamDecList,VAR},47},
        {{ParamDecList,ID},47},
        {{ParamMore,SEMI},49},
        {{ParamMore,RPAREN},48},
        {{Param,INTEGER_T},50},
        {{Param,CHAR_T},50},
        {{Param,ARRAY},50},
        {{Param,RECORD},50},
        {{Param,END},51},
        {{Param,ID},50},

        {{Param,VAR},51},
        {{FormList,ID},52},
        {{FidMore,SEMI},53},

        {{FidMore,COMMA},54},
        {{FidMore,RPAREN},53},

        {{ProcDecPart,TYPE},55},
        {{ProcDecPart,VAR},55},
        {{ProcDecPart,PROCEDURE},55},

        {{ProcDecPart,BEGIN} , 55},

        {{ProcBody,BEGIN},56},

        {{ProgramBody,BEGIN},57},
        {{StmList,IF},58},
        {{StmList,WHILE},58},

        {{StmList,READ},58},
        {{StmList,WRITE},58},
        {{StmList,RETURN},58},
        {{StmList,ID},58},
        {{StmMore,END},59},

        {{StmMore,ELSE},59},
        {{StmMore,FI},59},
        {{StmMore,ENDWH},59},
        {{StmMore,SEMI},60},

        {{Stm,IF},61},
        {{Stm,WHILE},62},
        {{Stm,READ},63},
        {{Stm,WRITE},64},
        {{Stm,RETURN},65},
        {{Stm,ID},66},






        //TODO shushang cuole
        {{AssCall,ASSIGN},67},
        {{AssCall,DOT},67},
        {{AssCall,LMIDPAREN},67},

        {{AssCall,LPAREN},68},
        {{AssignmentRest,DOT},69},
        {{AssignmentRest,LMIDPAREN},69},
        {{AssignmentRest,ASSIGN},69},
        {{ConditionalStm,IF},70},
        {{LoopStm,WHILE},71},
        {{InputStm,READ},72},
        {{InVar,ID},73},
        {{OutputStm,WRITE},74},
        {{ReturnStm,RETURN},75},
        {{CallStmRest,LPAREN},76},
        {{ActParamList,INTC_VAL},78},
        {{ActParamList,ID},78},
        {{ActParamList,LPAREN},78},
        {{ActParamList,RPAREN},77},
        {{ActParamMore,COMMA},80},
        {{ActParamMore,RPAREN},79},
        {{RelExp,INTC_VAL},81},
        {{RelExp,ID},81},
        {{RelExp,LMIDPAREN},81},
        {{OtherRelE,LT},82},
        {{OtherRelE,EQ},82},

        {{Exp,INTC_VAL},83},

        {{OtherFactor,END},87},
        {{OtherFactor,THEN},87},
        {{OtherFactor,ELSE},87},
        {{OtherFactor,FI},87},
        {{OtherFactor,DO},87},
        {{OtherFactor,ENDWH},87},
        {{OtherFactor,SEMI},87},
        {{OtherFactor,COMMA},87},
        {{OtherFactor,RPAREN},87},
        {{OtherFactor,RMIDPAREN},87},
        {{OtherFactor,LT},87},
        {{OtherFactor,EQ},87},
        {{OtherFactor,PLUS},87},
        {{OtherFactor,MINUS},87},


        {{OtherFactor,TIMES},88},
        {{OtherFactor,DIVIDE},88},







        {{Term,INTC_VAL},86},
        {{Factor,INTC_VAL},90},

        {{OtherTerm,END},84},
        {{Term,END},87},
        {{VariMore,END},93},
        {{FieldVarMore,END},97},

        {{OtherTerm,THEN},84},
        {{Term,THEN},87},
        {{VariMore,THEN},93},
        {{FieldVarMore,THEN},97},

        {{OtherTerm,ELSE},84},
        {{Term,ELSE},87},
        {{VariMore,ELSE},93},
        {{FieldVarMore,ELSE},97},

        {{OtherTerm,FI},84},
        {{Term,FI},87},
        {{VariMore,FI},93},
        {{FieldVarMore,FI},97},

        {{OtherTerm,DO},84},
        {{Term,DO},87},
        {{VariMore,DO},93},
        {{FieldVarMore,DO},97},

        {{OtherTerm,ENDWH},84},
        {{Term,ENDWH},87},
        {{VariMore,ENDWH},93},
        {{FieldVarMore,ENDWH},97},

        {{Exp,ID},83},
        {{Term,ID},86},
        {{Factor,ID},91},
        {{Variable,ID},92},
        {{FieldVar,ID},96},

        {{VariMore,DOT},95},

        {{OtherTerm,SEMI},84},
        {{Term,SEMI},87},
        {{VariMore,SEMI},93},
        {{FieldVarMore,SEMI},97},

        {{OtherTerm,COMMA},84},
        {{Term,COMMA},87},
        {{VariMore,COMMA},93},
        {{FieldVarMore,COMMA},97},

        {{Exp,LPAREN},83},
        {{Term,LPAREN},86},
        {{Factor,LPAREN},89},

        {{OtherTerm,RPAREN},84},
        {{Term,RPAREN},87},
        {{VariMore,RPAREN},93},
        {{FieldVarMore,RPAREN},97},

        {{VariMore,LMIDPAREN},94},
        {{FieldVarMore,LMIDPAREN},98},

        {{OtherTerm,RMIDPAREN},84},
        {{Term,RMIDPAREN},87},
        {{VariMore,RMIDPAREN},93},
        {{FieldVarMore,RMIDPAREN},97},

        {{OtherTerm,LT},84},
        {{Term,LT},87},
        {{VariMore,LT},93},
        {{FieldVarMore,LT},97},
        {{CmpOp,LT},99},

        {{OtherTerm,EQ},84},
        {{Term,EQ},87},
        {{VariMore,EQ},93},
        {{FieldVarMore,EQ},97},
        {{CmpOp,EQ},100},

        {{OtherTerm,PLUS},85},
        {{Term,PLUS},87},
        {{VariMore,PLUS},93},
        {{FieldVarMore,PLUS},97},
        {{AddOp,PLUS},101},

        {{OtherTerm,MINUS},85},
        {{Term,MINUS},87},
        {{VariMore,MINUS},93},
        {{FieldVarMore,MINUS},97},
        {{AddOp,MINUS},102},

        {{Term,TIMES},88},
        {{VariMore,TIMES},93},
        {{FieldVarMore,TIMES},97},
        {{MultOp,TIMES},103},

        {{Term,DIVIDE},88},
        {{VariMore,DIVIDE},93},
        {{FieldVarMore,DIVIDE},97},
        {{MultOp,DIVIDE},104},

        {{VariMore,ASSIGN},93},
        {{FieldVarMore,ASSIGN},97},


        {{CmpOp,LT},99},
        {{CmpOp,EQ},100},
        {{AddOp,PLUS},101},
        {{AddOp,MINUS},102},
        {{MultOp,TIMES},103},
        {{MultOp,DIVIDE},104},

    };

}
int  Priosity( LexType  op)
{
    int  pri=0;
  switch(op)
  { case END:
            pri = -1; break;//栈底标识，优先级最低
    case LPAREN:
            pri = 0;
    case LT:
    case EQ:
            pri = 1; break;
    case PLUS:
    case MINUS:
        pri = 2; break;
    case TIMES:
    case DIVIDE:
            pri = 3; break;
    default:  {
      qDebug()<<QString::fromLocal8Bit("没有这个操作符");
               pri= -1;
              }
  }
  return  pri;
}
TreeNode *LL1_parse::get_parsetree_head()
{
    return root;

}

void LL1_parse::run() {
  symbal_stack.push(Program);

  root = newSpecNode(ProK);

  syntaxtree_stack.push(&root->child[2]);
  syntaxtree_stack.push(&root->child[1]);
  syntaxtree_stack.push(&root->child[0]);

  lineno=head->getLine();
  while (!symbal_stack.empty()) {
      if(nullptr==head){
          //TODO end early
          exit(0);
      }
      if(NTSet.contains(symbal_stack.top())) {
          auto ss=symbal_stack.pop();
         auto iter=table.find(QPair<LexType,LexType>(ss,head->getLex())) ;
         if(table.cend()!=iter){
             process(iter.value());
         }else{
          qDebug()<<"i don't understand the :"<<head->getLexName()<< "in this line:"<<lineno;   //auto func=iter.value();
             //TODO not in table
         }
      };
      if(TTSet.contains(symbal_stack.top())){
          if(symbal_stack.top()==head->getLex()){
              symbal_stack.pop();
              head=head->next;
              lineno=head->getLine();
          }else{
              qDebug()<<"unexpected token:"<<head->getLexName()<<" in line "<<lineno;//TODO ERROR not match
              head=head->next;
              //TODO error handing : just jump over
          }

      }
  }

  emit parse_success(QSharedPointer<TreeNode>(root),"LL1");
}

void LL1_parse::process(int id)
{
    switch (id) {
    case 1:
    {
        symbal_stack.push(DOT);
        symbal_stack.push(ProgramBody);
        symbal_stack.push(DeclarePart);
        symbal_stack.push(ProgramHead);
        break;
    }
    case 2:
    {
        symbal_stack.push(ProgramName);
        symbal_stack.push(PROGRAM);
        current_treenode=newSpecNode(PheadK);
        auto item=syntaxtree_stack.pop();
        (*item)=current_treenode;
        break;
    }
    case 3:
    {
        symbal_stack.push(ID);
        strcpy(current_treenode->name[0],head->getSem().toStdString().c_str());
        ++current_treenode->idnum;
        break;
    }
    case 4:
    {
        symbal_stack.push(ProcDec);
        symbal_stack.push(VarDec);
        symbal_stack.push(TypeDec);

        break;
    }
    case 5:
    {
        break;
    }
    case 6:
    {
        symbal_stack.push(TypeDeclaration);
        break;
    }
    case 7:
    {
        symbal_stack.push(TypeDecList);
        symbal_stack.push(TYPE);

        auto t=syntaxtree_stack.pop();
        current_treenode=newSpecNode(TypeK);

        (*t)=current_treenode;
        syntaxtree_stack.push(&(current_treenode->sibling));
        syntaxtree_stack.push(&(current_treenode->child[0]));

        break;
    }
    case 8:
    {

        symbal_stack.push(TypeDecMore);
        symbal_stack.push(SEMI);
        symbal_stack.push(TypeName);
        symbal_stack.push(EQ);
        symbal_stack.push(TypeId);

        auto t=syntaxtree_stack.pop();

        current_treenode=newSpecNode(DecK);
        (*t)=current_treenode;

        syntaxtree_stack.push(&(current_treenode->sibling));
        break;
    }
    case 9:
    {
        syntaxtree_stack.pop();
        break;
    }
    case 10:
    {
        symbal_stack.push(TypeDecList);
        break;
    }
    case 11:
    {
        symbal_stack.push(ID);
        strcpy(current_treenode->name[0],head->getSem().toStdString().c_str());
        current_treenode->idnum++;
        break;
    }
    case 12:
    {
        symbal_stack.push(BaseType);
        temp=&(current_treenode->kind.dec);
        break;
    }
    case 13:
    {

        symbal_stack.push(StructureType);
        break;
    }
    case 14:{

        symbal_stack.push(ID);
        current_treenode->kind.dec=IdK;
        strcpy(current_treenode->type_name,head->getSem().toStdString().c_str());
        break;
    }
    case 15:{
        symbal_stack.push(INTEGER_T);
        (*temp)=IntegerK;
        break;
    }
    case 16:{
        symbal_stack.push(CHAR_T);
        (*temp)=CharK;
        break;
    }
    case 17:{
        symbal_stack.push(ArrayType);

        break;
    }
    case 18:{
        symbal_stack.push(RecType);
        break;
    }
    case 19:{
        symbal_stack.push(BaseType);
        symbal_stack.push(OF);
        symbal_stack.push(RMIDPAREN);
        symbal_stack.push(Top);
        symbal_stack.push(UNDERRANGE);
        symbal_stack.push(Low);
        symbal_stack.push(LMIDPAREN);
        symbal_stack.push(ARRAY);

        current_treenode->kind.dec=ArrayK;
        temp=&current_treenode->attr.ArrayAttr.childtype;
        break;
    }
    case 20:{
        symbal_stack.push(INTC_VAL);
        current_treenode->attr.ArrayAttr.low=head->getSem().toInt();

        break;
    }
    case 21:{

        symbal_stack.push(INTC_VAL);

        current_treenode->attr.ArrayAttr.up=head->getSem().toInt();
        break;
    }
    case 22:{
        symbal_stack.push(END);
        symbal_stack.push(FieldDecList);
        symbal_stack.push(RECORD);

        current_treenode->kind.dec=RecordK;

        saveP=current_treenode;

        syntaxtree_stack.push(&(current_treenode->child[0]));
        break;
    }
    case 23:{
        symbal_stack.push(FieldDecMore);
        symbal_stack.push(SEMI);
        symbal_stack.push(IdList);
        symbal_stack.push(BaseType);


        auto t=syntaxtree_stack.pop();
        current_treenode=newSpecNode(DecK);
        temp=&current_treenode->kind.dec;
        (*t)=current_treenode;

        syntaxtree_stack.push(&current_treenode->sibling);
        break;

    }
    case 24:{
        symbal_stack.push(FieldDecMore);
        symbal_stack.push(SEMI);
        symbal_stack.push(IdList);
        symbal_stack.push(ArrayType);

        TreeNode  **t=syntaxtree_stack.pop();
        current_treenode = newDecNode(); /*生成记录类型的下一个域，不添任何信息*/
        (*t) = current_treenode; /*若是第一个，则是record类型的子结点指向当前结点，
                           否则，是上一个纪录域声明的兄弟结点*/
        syntaxtree_stack.push(&((*current_treenode).sibling));

        break;
    }

    case 25 :
    {
        /*后面没有记录类型的下一个域了，恢复当前纪录类型节点的指针*/
        syntaxtree_stack.pop();
        current_treenode = saveP;
        break;
    }

    case 26 :
    {
        symbal_stack.push(FieldDecList);

        break;
    }
    case 27 :
    {
        symbal_stack.push(IdMore);
        symbal_stack.push(ID);

        /*纪录一个域中各个变量的语义信息*/
        strcpy(current_treenode->name[current_treenode->idnum], head->getSem().toStdString().c_str());
        current_treenode->idnum++;
        break;
    }
    case 28 :
    {
        break;
    }

    case 29 :
    {
        symbal_stack.push(IdList);
        symbal_stack.push(COMMA);

        break;
    }
    case 30 :
    {
        break;
    }

    case 31 :
    {
        symbal_stack.push(VarDeclaration);

        break;
    }
    case 32 :
    {
       symbal_stack.push(VarDecList);
       symbal_stack.push(VAR);

       current_treenode=newVarNode();   /*生成一个标志变量声明的节点*/
       TreeNode **t=syntaxtree_stack.pop();
       (*t)=current_treenode;
       syntaxtree_stack.push(&((*current_treenode).sibling));  /*压入指向函数声明的指针*/
       syntaxtree_stack.push(&((*current_treenode).child[0])); /*压入指向第一个变量声明节点的指针*/

        break;
    }
    case 33 :
    {
        symbal_stack.push(VarDecMore);
        symbal_stack.push(SEMI);
        symbal_stack.push(VarIdList);
        symbal_stack.push(TypeName);

        TreeNode  **t=syntaxtree_stack.pop();
        current_treenode = newDecNode();/*建立一个新的声明节点，这里表示变量声明*/
        (*t) = current_treenode;       /*若是第一个节点，则变量声明的头指针指向它，
                                 否则它是前一个变量声明的后继*/
        syntaxtree_stack.push(&((*current_treenode).sibling));

        break;
    }
    case 34 :
    {
        syntaxtree_stack.pop();
        break;
    }

    case 35 :
    {
        symbal_stack.push(VarDecList);
        break;

    }
    case 36 :
    {
       symbal_stack.push(VarIdMore);
       symbal_stack.push(ID);

       strcpy(current_treenode->name[current_treenode->idnum], head->getSem().toStdString().c_str());
       current_treenode->idnum++;
        break;

    }
    case 37 :
    {
        break;
    }

    case 38 :
    {
        symbal_stack.push(VarIdList);
        symbal_stack.push(COMMA);

        break;
    }
    case 39 :
    {
        break;
    }

    case 40 :
    {
        symbal_stack.push(ProcDeclaration);

        break;
    }
    case 41 :
    {
       symbal_stack.push(ProcDecMore);
       symbal_stack.push(ProcBody);
       symbal_stack.push(ProcDecPart);
       symbal_stack.push(SEMI);
       symbal_stack.push(RPAREN);
       symbal_stack.push(ParamList);
       symbal_stack.push(LPAREN);
       symbal_stack.push(ProcName);
       symbal_stack.push(PROCEDURE);


       current_treenode=newProcNode();
       TreeNode **t=syntaxtree_stack.pop();
       (*t)=current_treenode;

       syntaxtree_stack.push(&(current_treenode->sibling));

       syntaxtree_stack.push(&(current_treenode->child[2])); /*指向语句序列*/

       syntaxtree_stack.push(&(current_treenode->child[1]));  /*指向函数的声明部分*/

       syntaxtree_stack.push(&(current_treenode->child[0]));  /*指向参数声明部分*/
        break;
    }

    case 42 :
    {   /*弹出过程节点的兄弟节点指针*/
        //syntaxtree_stack.pop();  /*为了统一处理，不能现在弹出*/
        break;
    }

    case 43 :
    {
        symbal_stack.push(ProcDeclaration);
        break;
    }

    case 44 :
    {
        symbal_stack.push(ID);

        strcpy( current_treenode->name[0] , head->getSem().toStdString().c_str() );
        current_treenode->idnum++;
        break;
    }

    case 45 :
    {
        /*形参部分为空，弹出指向形参的指针*/
        syntaxtree_stack.pop();
        break;
    }

    case 46 :
    {
        symbal_stack.push(ParamDecList);
        break;
    }
    case 47 :
    {
        symbal_stack.push(ParamMore);
        symbal_stack.push(Param);
        break;
    }
    case 48 :
    {
        syntaxtree_stack.pop();
        break;
    }

    case 49 :
    {
        symbal_stack.push(ParamDecList);
        symbal_stack.push(SEMI);
        break;

    }
    case 50 :
    {
        symbal_stack.push(FormList);
        symbal_stack.push(TypeName);

        TreeNode **t=syntaxtree_stack.pop();
        current_treenode=newDecNode();
        /*函数的参数类型是值类型*/
        current_treenode->attr.ProcAttr.paramt=valparamType;
        (*t)=current_treenode;
        syntaxtree_stack.push(&(current_treenode->sibling));

        break;

    }
    case 51 :
    {
        symbal_stack.push(FormList);
        symbal_stack.push(TypeName);
        symbal_stack.push(VAR);

        TreeNode **t=syntaxtree_stack.pop();
        current_treenode=newDecNode();
        /*函数的参数类型是变量类型*/
        current_treenode->attr.ProcAttr.paramt=varparamType;
        (*t)=current_treenode;
        syntaxtree_stack.push(&(current_treenode->sibling));

        break;
    }
    case 52 :
    {
       symbal_stack.push(FidMore);
       symbal_stack.push(ID);

       strcpy( current_treenode->name[current_treenode->idnum], head->getSem().toStdString().c_str());
       current_treenode->idnum++;

        break;
    }
    case 53 :
    {
        break;
    }

    case 54 :
    {
        symbal_stack.push(FormList);
        symbal_stack.push(COMMA);
        break;
    }

    case 55 :
    {
        symbal_stack.push(DeclarePart);
        break;
    }
    case 56 :
    {
        symbal_stack.push(ProgramBody);
        break;
    }

    case 57 :
    {
        symbal_stack.push(END);
        symbal_stack.push(StmList);
        symbal_stack.push(BEGIN);

        /*注意，若没有声明部分，则弹出的是程序或过程根节点中指向
          声明部分的指针child[1];若有声明部分，则弹出的是语句序列前
          的最后一个声明标识节点的兄弟指针；不管是哪种情况，都正好
          需要弹出语法树栈中的一个指针*/
        syntaxtree_stack.pop();

        /*建立语句序列标识节点*/
        TreeNode **t=syntaxtree_stack.pop();
        current_treenode=newStmlNode();
        (*t)=current_treenode;
        syntaxtree_stack.push(&(current_treenode->child[0]));
        break;

    }
    case 58 :
    {
        symbal_stack.push(StmMore);
        symbal_stack.push(Stm);
        break;
    }
    case 59 :
    {
        syntaxtree_stack.pop();
        break;
    }

    case 60 :
    {
        symbal_stack.push(StmList);
        symbal_stack.push(SEMI);

        break;
    }
    case 61 :
    {
        symbal_stack.push(ConditionalStm);

        current_treenode=newStmtNode(IfK);
        //current_treenode->kind.stmt=;

        TreeNode **t=syntaxtree_stack.pop();
        (*t)=current_treenode;
        syntaxtree_stack.push(&current_treenode->sibling);

        break;

    }
    case 62 :
    {
        symbal_stack.push(LoopStm);

        current_treenode=newStmtNode(WhileK);
        //current_treenode->kind.stmt=;

        TreeNode **t=syntaxtree_stack.pop();
        (*t)=current_treenode;
        syntaxtree_stack.push(&current_treenode->sibling);

        break;
    }

    case 63 :
    {
        symbal_stack.push(InputStm);

        TreeNode **t=syntaxtree_stack.pop();
        current_treenode=newStmtNode(ReadK);
        //current_treenode->kind.stmt=;
        (*t)=current_treenode;
        syntaxtree_stack.push(&current_treenode->sibling);

        break;
    }
    case 64 :
    {
        symbal_stack.push(OutputStm);

        TreeNode **t=syntaxtree_stack.pop();
        current_treenode=newStmtNode(WriteK);
        //current_treenode->kind.stmt=;
        (*t)=current_treenode;
        syntaxtree_stack.push(&current_treenode->sibling);

        break;
    }
    case 65 :
    {
        symbal_stack.push(ReturnStm);

        TreeNode **t=syntaxtree_stack.pop();
        current_treenode=newStmtNode(ReturnK);
        //current_treenode->kind.stmt=;
        (*t)=current_treenode;
        syntaxtree_stack.push(&current_treenode->sibling);

        break;
    }

    case 66 :
    {
        symbal_stack.push(AssCall);
        symbal_stack.push(ID);

        current_treenode=newStmtNode(AssignK);

        /*赋值语句左部变量节点*/
        TreeNode  *t = newExpNode(VariK);
        strcpy( t->name[0], head->getSem().toStdString().c_str() );
        t->idnum++;

        /*赋值语句的child[0]指向左部的变量节点*/
        current_treenode->child[0]=t;

        TreeNode **t1=syntaxtree_stack.pop();
        (*t1)=current_treenode;
        syntaxtree_stack.push(&current_treenode->sibling);

        break;
    }
    case 67 :
    {
        symbal_stack.push(AssignmentRest);

        current_treenode->kind.stmt=AssignK;
        break;
    }
    case 68 :
    {
        symbal_stack.push(CallStmRest);
        /*过程调用语句左部是标识符变量，表示过程名*/
        current_treenode->child[0]->attr.ExpAttr.varkind=IdV;

        current_treenode->kind.stmt=CallK;
        break;
    }
    case 69 :
    {
       symbal_stack.push(Exp);
       symbal_stack.push(ASSIGN);
       symbal_stack.push(VariMore);
       /*压入指向赋值右部的指针*/
       syntaxtree_stack.push(&(current_treenode->child[1]));
       /*当前指针指向赋值左部*/
       current_treenode=current_treenode->child[0];

       TreeNode *t=newExpNode(OpK);
       t->attr.ExpAttr.op = END ;
       op_stack.push(t);  //操作符栈的栈底存入一个特殊的操作符作为标志
        break;
    }
    case 70 :
    {
       symbal_stack.push(FI);
       symbal_stack.push(StmList);
       symbal_stack.push(ELSE);
       symbal_stack.push(StmList);
       symbal_stack.push(THEN);
       symbal_stack.push(RelExp);
       symbal_stack.push(IF);

       syntaxtree_stack.push(&(current_treenode->child[2]));
       syntaxtree_stack.push(&(current_treenode->child[1]));
       syntaxtree_stack.push(&(current_treenode->child[0]));

        break;
    }

    case 71 :
    {
       symbal_stack.push(ENDWH);
       symbal_stack.push(StmList);
       symbal_stack.push(DO);
       symbal_stack.push(RelExp);
       symbal_stack.push(WHILE);

       syntaxtree_stack.push(&(current_treenode->child[1]));
       syntaxtree_stack.push(&(current_treenode->child[0]));

        break;
    }

    case 72 :
    {
       symbal_stack.push(RPAREN);
       symbal_stack.push(InVar);
       symbal_stack.push(LPAREN);
       symbal_stack.push(READ);
        break;
    }
    case 73 :
    {
       symbal_stack.push(ID);

       strcpy( current_treenode->name[0], head->getSem().toStdString().c_str());
       current_treenode->idnum++;
        break;
    }
    case 74 :
    {
       symbal_stack.push(RPAREN);
       symbal_stack.push(Exp);
       symbal_stack.push(LPAREN);
       symbal_stack.push(WRITE);

       syntaxtree_stack.push(&(current_treenode->child[0]));

       TreeNode *t=newExpNode(OpK);
       t->attr.ExpAttr.op = END ;
       op_stack.push(t);  //操作符栈的栈底存入一个特殊的操作符作为标志
        break;
    }
    case 75 :
    {
       symbal_stack.push(RETURN);
        break;
    }

    case 76 :
    {
       symbal_stack.push(RPAREN);
       symbal_stack.push(ActParamList);
       symbal_stack.push(LPAREN);

       syntaxtree_stack.push(&(current_treenode->child[1]));
        break;

    }
    case 77 :
    {
       syntaxtree_stack.pop();
        break;
    }
    case 78 :
    {
       symbal_stack.push(ActParamMore);
       symbal_stack.push(Exp);

       TreeNode *t=newExpNode(OpK);
       t->attr.ExpAttr.op = END ;
       op_stack.push(t);  //操作符栈的栈底存入一个特殊的操作符作为标志
        break;
    }

    case 79 :
    {
        break;
    }
    case 80 :
    {
       symbal_stack.push(ActParamList);
       symbal_stack.push(COMMA);

       syntaxtree_stack.push(&(current_treenode->sibling));
        break;
    }




    /*********************表达式部分************************/

    case 81 :
    {
       symbal_stack.push(OtherRelE);
       symbal_stack.push(Exp);

       TreeNode *t=newExpNode(OpK);
       t->attr.ExpAttr.op = END ;
       op_stack.push(t);  //操作符栈的栈底存入一个特殊的操作符作为标志

       getExpResult=false;
        break;
    }

    case 82 :
    {
       symbal_stack.push(Exp);
       symbal_stack.push(CmpOp);

       TreeNode  *current_treenode=newExpNode(OpK);
       current_treenode->attr.ExpAttr.op = head->getLex();

       LexType  sTop=op_stack.top()->attr.ExpAttr.op;
       while ( Priosity(sTop) >= Priosity(head->getLex()) )
       /*如果操作符栈顶运算符的优先级高于或等于当前读到的操作符*/
       {   TreeNode *t=op_stack.pop();
           TreeNode *Rnum=num_stack.pop();
           TreeNode *Lnum=num_stack.pop();
           t->child[1]=Rnum;
           t->child[0]=Lnum;
           num_stack.push(t);

           sTop=op_stack.top()->attr.ExpAttr.op;
       }

       op_stack.push(current_treenode);
       /*处理完关系操作符右部的表达式时，要弹语法树栈，故
         设置getExpResult为真*/
       getExpResult	=true;
        break;
    }

    case 83 :
    {
       symbal_stack.push(OtherTerm);
       symbal_stack.push(Term);
        break;
    }

    case 84 :
    {
      if ((head->getLex()==RPAREN)&&(expflag!=0))
         //说明当前右括号是表达式中的一部分
        { while (op_stack.top()->attr.ExpAttr.op!=LPAREN)
              {TreeNode   *t=op_stack.pop();
                TreeNode  *Rnum=num_stack.pop();
                TreeNode  *Lnum=num_stack.pop();

                t->child[1] = Rnum;
                t->child[0] = Lnum;
                num_stack.push(t);
             }
            op_stack.pop(); //弹出左括号
            expflag--;
         }
      else
      { if ((getExpResult)||(getExpResult2))
        {  while  (op_stack.top()->attr.ExpAttr.op!=END)
          {TreeNode  *t=op_stack.pop();
           TreeNode  *Rnum=num_stack.pop();
           TreeNode  *Lnum=num_stack.pop();

           t->child[1] = Rnum;
           t->child[0] = Lnum;
           num_stack.push(t);
          }
          op_stack.pop();//弹出栈底标志
          current_treenode=num_stack.pop();

          TreeNode  **t=syntaxtree_stack.pop();
          (*t)=current_treenode;

          /*处理完数组变量，标志恢复初始值假，
            遇到下一个数组下标表达式时，再将其设置为真值*/
          if (getExpResult2==true)
              getExpResult2 = false;
        }
     }
        break;
    }

    case 85 :
    {
       symbal_stack.push(Exp);
       symbal_stack.push(AddOp);

       TreeNode  *current_treenode=newExpNode(OpK);
       current_treenode->attr.ExpAttr.op = head->getLex();
       LexType  sTop=op_stack.top()->attr.ExpAttr.op;
       while ( Priosity(sTop) >= Priosity(head->getLex()) )
       {   TreeNode *t=op_stack.pop();
           TreeNode *Rnum=num_stack.pop();
           TreeNode *Lnum=num_stack.pop();
           t->child[1]=Rnum;
           t->child[0]=Lnum;
           num_stack.push(t);
           sTop=op_stack.top()->attr.ExpAttr.op;
       }
       op_stack.push(current_treenode);
        break;
    }

    case 86:
    {
       symbal_stack.push(OtherFactor);
       symbal_stack.push(Factor);
        break;
    }
    case 87 :
    {
        break;
    }
    case 88 :
    {
       symbal_stack.push(Term);
       symbal_stack.push(MultOp);

       TreeNode  *current_treenode=newExpNode(OpK);
       current_treenode->attr.ExpAttr.op = head->getLex();

       LexType  sTop=op_stack.top()->attr.ExpAttr.op ;
       while ( Priosity(sTop) >= Priosity(head->getLex()) )
       /*如果操作符栈顶运算符的优先级高于或等于当前读到的操作符*/
       {   TreeNode *t=op_stack.pop();
           TreeNode *Rnum=num_stack.pop();
           TreeNode *Lnum=num_stack.pop();
           t->child[1]=Rnum;
           t->child[0]=Lnum;
           num_stack.push(t);

           sTop=op_stack.top()->attr.ExpAttr.op;
       }
       op_stack.push(current_treenode);
        break;
    }

    case 89 :
    {
       symbal_stack.push(RPAREN);
       symbal_stack.push(Exp);
       symbal_stack.push(LPAREN);

       TreeNode *t=newExpNode(OpK);
       t->attr.ExpAttr.op = head->getLex(); /*把左括号也压入栈中*/
       op_stack.push(t);
       expflag++;
        break;
    }

    case 90 :
    {
      symbal_stack.push(INTC_VAL);

      TreeNode *t=newExpNode(ConstK);
      t->attr.ExpAttr.val = head->getSem().toInt();
      /*常数节点入操作数栈*/
      num_stack.push(t);

        break;
    }

    case 91 :
    {
     symbal_stack.push(Variable);
        break;
    }

    case 92 :
    {
        symbal_stack.push(VariMore);
        symbal_stack.push(ID);

        current_treenode=newExpNode(VariK);
        strcpy( current_treenode->name[0] , head->getSem().toStdString().c_str());
        current_treenode->idnum++;
        /*变量节点入操作数栈*/
        num_stack.push(current_treenode);

        break;
    }

    case 93 :
    {
     /*标识符变量*/
     current_treenode->attr.ExpAttr.varkind=IdV;
        break;
    }

    case 94 :
    {
        symbal_stack.push( RMIDPAREN);
        symbal_stack.push( Exp);
        symbal_stack.push( LMIDPAREN);
        /*数组成员变量*/
        current_treenode->attr.ExpAttr.varkind=ArrayMembV;
        syntaxtree_stack.push(&current_treenode->child[0]);

        /*要进入表达式处理，初始化操作符栈*/
        //操作符栈的栈底存入一个特殊的操作符作为标志
        TreeNode *t=newExpNode(OpK);
        t->attr.ExpAttr.op = END ;
        op_stack.push(t);

        /*要进入数组下标表达式处理，在函数process84处理中，要
          操作语法树栈，故将标志getExpResult2设置为真值*/
        getExpResult2 = true;

        break;

    }

    case 95 :
    {
        symbal_stack.push( FieldVar);
        symbal_stack.push( DOT);
        /*域成员变量*/
        current_treenode->attr.ExpAttr.varkind=FieldMembV;
        syntaxtree_stack.push(&current_treenode->child[0]);
        break;
    }

    case 96 :
    {
        symbal_stack.push(FieldVarMore);
        symbal_stack.push(ID);

        /*纪录域的成员*/
        current_treenode=newExpNode(VariK);
        strcpy( current_treenode->name[0] , head->getSem().toStdString().c_str());
        current_treenode->idnum++;

        TreeNode **t=syntaxtree_stack.pop();
        (*t)=current_treenode;

        break;

    }

    case 97 :
    {
      /*域成员是标识符变量*/
      current_treenode->attr.ExpAttr.varkind=IdV;
        break;
    }

    case 98 :
    {
        symbal_stack.push(RMIDPAREN);
        symbal_stack.push(Exp);
        symbal_stack.push(LMIDPAREN);
        /*域成员是数组变量*/
        current_treenode->attr.ExpAttr.varkind=ArrayMembV;
        /*指向数组成员表达式*/
        syntaxtree_stack.push(&current_treenode->child[0]);

        //操作符栈的栈底存入一个特殊的操作符作为标志
        TreeNode *t=newExpNode(OpK);
        t->attr.ExpAttr.op = END ;
        op_stack.push(t);

        /*要进入数组下标表达式处理，在函数process84处理中，要
          操作语法树栈，故将标志getExpResult2设置为真值*/
        getExpResult2 = true;
        break;
    }
    case 99 :
    {
        symbal_stack.push(LT);
        break;
    }

    case 100 :
    {
        symbal_stack.push(EQ);
        break;
    }

    case 101 :
    {
        symbal_stack.push(PLUS);
        break;
    }

    case 102 :
    {
        symbal_stack.push(MINUS);
        break;
    }

    case 103 :
    {
        symbal_stack.push(TIMES);
        break;
    }

    case 104 :
    {
        symbal_stack.push(DIVIDE);
        break;
    }


    }


}
