#ifndef HELPIR
#define HELPIR
#include "semanticAnalyze.cpp"

//函数声明
void addAssign(Gnode AssignTree);  //赋值语句
void addState(Gnode StateTree);    //变量声明
int addCompute(Gnode AssignTree);  //算术运算语句



const int maxQuaternion = 1024;
int IRnum = 0;         //四元式的数量
int Ti = 0;
struct Quaternion
{
    int id;        //语句序号
    string op;    //操作
    int arg1Index;   //参数1 指向符号表的位置
    int arg2Index;   //参数2指向符号表的位置
    bool isArg1Num;  //参数1是否是立即数
    bool isArg2Num; //参数2是否是立即数
    int resultIndex;   //结果 指向符号表的位置
}IRtable[maxQuaternion];

int findArgIndex(string arg)     //寻找参数在符号表中位置
{
    for(int i=0;i<notationNum;i++)
        if(notationTable[i].name == arg)
        {
        
            return i;
        }
    cout<<"参数不在符号表中！"<<endl;
    exit(0);
    
}

string getNodeType(Gnode testNode)
{
    string name = testNode->name;
    if(name == "+" || name == "-")
        return "Arithmetic";
    if(name == "&&" || name == "||" || name == ">" || name == ">="|| name == "<"|| name == "<="|| name == "==")
        return "logic";
    return "wrong";
}

void addState(Gnode StateTree)    //变量声明
{
    string type = StateTree->type;
    for(int i=0;i<StateTree->child.size();i++)
    {
        int index = findArgIndex(StateTree->child[i]->name);
        IRtable[IRnum].id = IRnum;
        IRtable[IRnum].op = type;
        IRtable[IRnum].arg1Index = index;
        IRnum++;
    }
}

void addAssign(Gnode AssignTree)  //赋值语句
{
    Gnode resultNode = AssignTree->child[0];  //左边是结果
    string result = resultNode->name;
    int resultIndex = findArgIndex(result);

    Gnode computeNode = AssignTree->child[1];
    string rightType = getNodeType(computeNode);
    string rightName = computeNode->name;

    int arg1Index;
    bool isArg1Num = false;
    if(rightType == "Arithmetic" || rightType == "logic") //右边是运算子树
        arg1Index = addCompute(computeNode);
    else if(rightName[0]>'a' && rightName[0]>'z' || rightName[0]>'A' || rightName[0]<'Z') //如果右边是变量
        arg1Index = findArgIndex(rightName);
    else  //右边是数字
    {
        arg1Index = atoi(rightName.c_str());
        isArg1Num = true;
    }
    
    IRtable[IRnum].id = IRnum;
    IRtable[IRnum].op = "=";
    IRtable[IRnum].arg1Index = arg1Index;
    IRtable[IRnum].isArg1Num = isArg1Num;
    IRtable[IRnum].resultIndex = resultIndex;
    IRnum++;
}

int addCompute(Gnode ComputeTree)  //算术和逻辑运算语句 返回临时变量的地址
{
    //算数运算子树一定是二叉树
    Gnode left = ComputeTree->child[0];
    Gnode right = ComputeTree->child[1];
    int leftIndex,rightIndex;
    bool leftFuct = false,rightFuct = false,leftIsNum = false,rightIsNum = false;
    string leftType = getNodeType(left),rightType = getNodeType(right);
    if(leftType == "Arithmetic" || leftType == "logic")   //左边是子树
    {
        leftFuct = true;
        leftIndex = addCompute(left);
    }
    if(rightType == "Arithmetic" || rightType == "logic")  //右边是子树
    {
        rightFuct = true;
        rightIndex = addCompute(right);
    }
    if(!leftFuct)  //左边是叶节点
    {
        //如果是变量
        if(left->name[0]>'a' && left->name[0]<'z' || left->name[0]>'A' && left->name[0]<'Z')
            leftIndex = findArgIndex(left->name);
        else   //如果是个数字
        {
            leftIndex = atoi(left->name.c_str());
            leftIsNum = true;
        }
    }
    if(!rightFuct)  //右边是叶节点
    {
        //如果是变量
        if(right->name[0]>'a' && right->name[0]<'z' || right->name[0]>'A' && right->name[0]<'Z')
            rightIndex = findArgIndex(right->name);
        else   //如果是个数字
        {
            rightIndex = atoi(right->name.c_str());
            rightIsNum = true;
        }
            
    }

    //生成一个中间变量存结果
    string result = "t";
    char idT = Ti + '0';
    result += idT;
    Ti++;
    string opType = getNodeType(ComputeTree);
    bool exist;
    if(opType == "Arithmetic")
        exist = checkVariable("int",result);
    else if(opType == "logic")
        exist = checkVariable("bool",result);
    if(exist) {
        cout<<"生成中间结果错误"<<endl;
        exit(0);
    }

    int resultIndex = findArgIndex(result);

    IRtable[IRnum].id = IRnum;
    IRtable[IRnum].op = ComputeTree->name;
    IRtable[IRnum].arg1Index = leftIndex;
    IRtable[IRnum].arg2Index = rightIndex;
    if(leftIsNum)
        IRtable[IRnum].isArg1Num = true;
    else
        IRtable[IRnum].isArg1Num = false;
    if(rightIsNum)
        IRtable[IRnum].isArg2Num = true;
    else
        IRtable[IRnum].isArg2Num = false;
    IRtable[IRnum].resultIndex = resultIndex;
    IRnum++;
    return resultIndex;
    
}













#endif // 
