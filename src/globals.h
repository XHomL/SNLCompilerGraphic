
#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include "stdio.h"
#include "stdlib.h"

#include <QMap>
#include <QSet>
#include <QString>





typedef enum 
{
    //簿记单词符号
    ENDFILE,	ERROR,

    //保留字
    PROGRAM,	PROCEDURE,	TYPE,	VAR,		IF,
	THEN,		ELSE,		FI,		WHILE,		DO,
	ENDWH,		BEGIN,		END,	READ,		WRITE,
	ARRAY,		OF,			RECORD,	RETURN, 

    //类型
    INTEGER_T,	CHAR_T,

    //多字符单词符号
    ID,			INTC_VAL,		CHARC_VAL,

    //特殊符号
	ASSIGN,		EQ,			LT,		PLUS,		MINUS,
    TIMES,DIVIDE,		LPAREN,	RPAREN,		DOT,
    COLON,		SEMI,		COMMA,	LMIDPAREN,	RMIDPAREN,
    UNDERRANGE,




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
    FieldVarMore,   CmpOp,			AddOp,          MultOp
} LexType;

extern QMap<LexType, QString> lexName;


class Token{
public:
    Token(int line ,LexType lex, QString sem){
        this->line=line;
        this->lex=lex;
        this->sem=sem;
    }
    int getLine() const{return line;}
    QString getLexName() const{return lexName[lex];}
    LexType getLex() const{return lex;}
    QString getSem() const{return sem;}

    Token *next;

private:
    int line;
    LexType lex;
    QString sem;
};


//nodekind:记录语法树节点类型，枚举值为语法树节点类型
typedef enum {ProK,PheadK,DecK,TypeK,VarK,ProcDecK,StmLK,StmtK,ExpK}
              NodeKind;
extern QMap<NodeKind,QString> nodekind_map;

//kind成员dec：记录语法树节点的声明类型，当nodekind=DecK时有效，枚举值为语法树节点声明类型
typedef enum {ArrayK,CharK,IntegerK,RecordK,IdK}  DecKind;
extern QMap<DecKind,QString> deckind_map;

//kind成员stmt：记录语法树节点的语句类型，当nodekind=StmtK时有效，枚举值为语法树节点语句类型
typedef enum {IfK,WhileK,AssignK,ReadK,WriteK,CallK,ReturnK} StmtKind;
extern QMap<StmtKind,QString> stmtkind_map;

//kind成员exp：记录语法树节点的表达式类型，当nodekind=ExpK时有效，枚举值为语法树节点表达式类型
typedef enum {OpK,ConstK,VariK} ExpKind;
extern QMap<ExpKind,QString> expkind_map;



//成员attr:记录语法树节点其他属性，为结构体类型
//attr成员ExpAttr:记录表达式的属性

//ExpAttr成员varkind:记录变量的类别，枚举值分别表示变量是标识符变量，数组成员变量还是域成员变量
typedef enum {IdV,ArrayMembV,FieldMembV} VarKind; 
extern QMap<VarKind,QString> varkind_map;

//ExpAttr成员type:记录语法树节点的检查类型，枚举值为类型检查ExpType类型
typedef enum {Void,Integer,Boolean} ExpType;
extern QMap<ExpType,QString> exptype_map;

typedef enum {valparamType,varparamType} ParamType; 
extern QMap<ParamType,QString> paramtype_map;

#define MAXCHILDREN 3 


struct symbtable;

typedef struct treeNode
//SNL语法树的节点结构设计参见实验书P66表5.2
   { 
	//指向子语法树节点指针，为语法树节点指针类型
     struct treeNode * child[MAXCHILDREN];
	 //指向兄弟语法树节点指针，为语法树节点指针类型
     struct treeNode * sibling;
	 //记录源程序行号，为整数类型
     int lineno;
     NodeKind nodekind;
	 //成员kind:记录语法树节点的具体类型，为共用体结构
     union 
	 {
		 DecKind  dec;
	     StmtKind stmt; 
		 ExpKind  exp; 
     } kind;

	 //记录一个节点中的标识符的个数
     int idnum;
	 
	 //字符串数组，数组成员是节点中的标识符的名字
     char name[10][10];

	 //指针数组，数组成员是节点中的各个标识符在符号表中的入口
     struct symbtable * table[10];

	 //记录类型名，当节点为声明类型，且类型是由类型标识符表示时有效
      char type_name[10];

	  //成员attr:记录语法树节点的其他属性，为结构体类型
     struct
	 {
		 //attr成员ArrayAttr:记录数组类型的属性
		struct
			{	
				//Arrayttr成员low:整数类型变量，记录数组的下界
                int low;
				//Arrayttr成员up:整数类型变量，记录数组的上界
                int up;
                DecKind   childtype;
            }ArrayAttr;
	         
		//attr成员procAttr:记录过程的属性
		struct
			{	
				//procAttr成员paramt:记录过程的参数类型，值为枚举类型valparamtype或者varparamtype，表示过程的参数是值惨还是变参
                ParamType  paramt;
            }ProcAttr;
		
		//attr成员ExpAttr:记录表达式的属性
		struct
			{
				/*ExpAttr成员op：记录语法树节点的运算符单词，为单词类型。
				当语法树节点为“关系运算表达式”对应节点时，取值LT,EQ；
				当语法树节点为“加法运算简单表达式”对应节点时，取值为PLUS,MINUS;
				当语法树节点为“乘法运算项”对应节点时，取值TIMES,OVER；
				其他情况下无效
				*/
                LexType op;
				//记录语法树节点的数值，当语法树节点为“数字因子”对应的语法树节点时有效，为整数类型
                int val;
				//以下两个类型见声明处
                VarKind  varkind;
                ExpType type;
            }ExpAttr;

             
     } attr;
}TreeNode;




extern QSet<LexType> NTSet;
extern QSet<LexType> TTSet;



/*清单的行号*/
extern int lineno;



#endif



















