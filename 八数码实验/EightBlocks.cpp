#include <iostream>
#include<fstream>
#include <string>
#include <graphics.h>  //图形化库

using namespace std;

//定义常量
#define SIZE 300 //八数码外框大小
#define Num 9  

int step = 0; //步数

//绘图函数声明
void draw_struct(int left, int right, int top, int bottom); //画框线结构
void draw_number(int num, int x, int y, int size);  //输出数字 数字内容 起始位置 横向尺寸
void draw_move(int x, int y, int step); //方块移动

//八数码问题相关类及成员函数 
class TEight
{
public:
	TEight() {}
	TEight(const char *fname);
	virtual void Search() = 0;
protected:
	int p[Num];
	int last, spac;
	static int q[Num], d[], total;
	void Printf();
	bool operator==(const TEight &T);
	bool Extend(int i);
};

int TEight::q[Num];
int TEight::d[] = { 1,3,-1,-3 };
int TEight::total = 0;

//读取初始化文件
TEight::TEight(const char *fname)
{
	ifstream fin;
	fin.open(fname, ios::in | ios::_Nocreate);
	if (!fin)
	{
		cout << "不能打开数据文件!" << endl;
		return;
	}
	int i;
	//读取初始状态
	for (i = 0; i < Num;)
		fin >> p[i++];
	//读取空格
	fin >> spac;
	//读取目标状态
	for (i = 0; i < Num;)
		fin >> q[i++];
	fin.close();
	last = -1;
	total = 0;
}

void TEight::Printf()
{
	ofstream fout;
	fout.open("result.txt", ios::ate | ios::app);
	fout << total++ << "step:";
	step = total; //取到步数
	for (int i = 0; i < Num;)
		fout << "  " << p[i++];
	fout << endl;
	fout.close();
}

bool TEight::operator==(const TEight &T)
{
	for (int i = 0; i < Num;)
		if (T.p[i] != p[i++])
			return 0;
	return 1;
}

bool TEight::Extend(int i)
{
	if (i == 0 && spac % 3 == 2 || i == 1 && spac > 5
		|| i == 2 && spac % 3 == 0 || i == 3 && spac < 3)
		return 0;
	int temp = spac;
	spac += d[i];
	p[temp] = p[spac];
	p[spac] = 0;
	return 1;
}

template<class Type> class TList;            //线性表前视定义   

template<class Type> class TNode            //线性表结点类模板   
{
	friend class TList<Type>;
public:
	TNode() {}
	//TNode(const Type& dat);
private:
	TNode<Type>* Next;
	Type Data;
};

template<class Type> class TList
{
public:
	TList() { Last = First = 0; Length = 0; }   //构造函数   
	int Getlen()const { return Length; }   //成员函数，返回线性表长度   
	int Append(const Type& T);           //成员函数，从表尾加入结点   
	int Insert(const Type& T, int k);       //成员函数，插入结点   
	Type GetData(int i);                         //成员函数，返回结点数据成员   
	void SetData(const Type& T, int k); //成员函数，设置结点数据成员      
private:
	TNode<Type> *First, *Last;             //数据成员，线性表首、尾指针     
	int Length;                                         //数据成员，线性表长度   
};

template<class Type> int TList<Type>::Append(const Type& T)
{
	Insert(T, Length);
	return 1;
}

template<class Type> int TList<Type>::Insert(const Type& T, int k)
{
	TNode<Type> *p = new TNode<Type>;
	p->Data = T;
	if (First)
	{
		if (k <= 0)
		{
			p->Next = First;
			First = p;
		}
		if (k > Length - 1)
		{
			Last->Next = p;
			Last = Last->Next;
			Last->Next = 0;
		}
		if (k > 0 && k < Length)
		{
			k--;
			TNode<Type> *q = First;
			while (k-- > 0)
				q = q->Next;
			p->Next = q->Next;
			q->Next = p;
		}
	}
	else
	{
		First = Last = p;
		First->Next = Last->Next = 0;
	}
	Length++;
	return 1;
}

template<class Type> Type TList<Type>::GetData(int k)
{
	TNode<Type> *p = First;
	while (k-- > 0)
		p = p->Next;
	return p->Data;
}

template<class Type> void TList<Type>::SetData(const Type& T, int k)
{
	TNode<Type> *p = First;
	while (k-- > 0)
		p = p->Next;
	p->Data = T;
}

class AStar :public TEight
{
public:
	AStar() {}                           //构造函数   
	AStar(const char *fname1, const char *fname2);   //带参数构造函数    
	virtual void Search();               //A*搜索法   
private:
	int f, g, h;                               //估价函数     
	int r[Num];                              //存储状态中各个数字位置的辅助数组   
	static int s[Num];                       //存储目标状态中各个数字位置的辅助数组   
	static int e[];                            //存储各个数字相对距离的辅助数组      
	void Printl(TList<AStar> L);       //成员函数，输出搜索路径    
	int Expend(int i);                        //成员函数，A*算法的状态扩展函数    
	int Calcuf();                             //成员函数，计算估价函数   
	void Sort(TList<AStar>& L, int k);  //成员函数，将新扩展结点按f从小到大顺序插入待扩展结点队列   
	int Repeat(TList<AStar> &L);       //成员函数，检查结点是否重复   
};

int AStar::s[Num], AStar::e[Num*Num];

AStar::AStar(const char *fname1, const char *fname2) :TEight(fname1)
{
	for (int i = 0; i < Num;)
	{
		r[p[i]] = i;   //存储初始状态数字的位置   
		s[q[i]] = i++; //存储目标状态数字的位置   
	}
	ifstream fin;
	fin.open(fname2, ios::in );//打开数据文件   
	if (!fin)
	{
		cout << "不能打开数据文件!" << endl;
		return;
	}
	for (int i = 0; i < Num*Num; i++)   //读入各个数字相对距离值     
		fin >> e[i];
	fin.close();
	f = g = h = 0;       //估价函数初始值     
}

void AStar::Printl(TList<AStar> L)
{
	AStar T = *this;
	if (T.last == -1)
		return;
	else
	{
		T = L.GetData(T.last);
		T.Printl(L);
		T.Printf();
	}
}

int AStar::Expend(int i)
{
	if (Extend(i))   //结点可扩展   
	{
		int temp = r[p[r[0]]];   //改变状态后数字位置变化，存储改变后的位置    
		r[p[r[0]]] = r[0];
		r[0] = temp;
		return 1;
	}
	return 0;
}

int AStar::Calcuf()
{
	h = 0;
	for (int i = 0; i < Num; i++)  //计算估价函数的h   
		h += e[Num*r[i] + s[i]];
	return ++g + h;
}

void AStar::Sort(TList<AStar>& L, int k)
{
	int n = L.Getlen();
	int i;
	for (i = k + 1; i < n; i++)
	{
		AStar T = L.GetData(i);
		if (this->f <= T.f)
			break;
	}
	L.Insert(*this, i);
}

int AStar::Repeat(TList<AStar> &L)
{
	int i;
	int n = L.Getlen();
	for (i = 0; i < n; i++)
		if (L.GetData(i) == *this)
			break;
	return i;
}

void AStar::Search()
{
	AStar T = *this;              //初始结点   
	T.f = T.Calcuf();               //初始结点的估价函数   
	TList<AStar> L;           //建立队列   
	L.Append(T);                 //初始结点入队   
	int head = 0, tail = 0;           //队列头和尾指针   
	while (head <= tail)           //队列不空则循环   
	{
		for (int i = 0; i < 4; i++)            //空格可能移动方向   
		{
			T = L.GetData(head);   //去队列头结点   
			if (T.h == 0)                   //是目标结点   
			{
				T.Printl(L);//输出搜索路径   
				T.Printf();    //输出目标状态   
				return;        //结束   
			}
			if (T.Expend(i))          //若结点可扩展   
			{
				int k = T.Repeat(L); //返回与已扩展结点重复的序号        
				if (k < head)               //如果是不能扩展的结点   
					continue; //丢弃   
				T.last = head;      //不是不能扩展的结点，记录父结点   
				T.f = T.Calcuf(); //计算f   
				if (k <= tail)         //新结点与可扩展结点重复   
				{
					AStar Temp = L.GetData(k);
					if (Temp.g > T.g)         //比较两结点g值   
						L.SetData(T, k); //保留g值小的   
					continue;
				}
				T.Sort(L, head);            //新结点插入可扩展结点队列        
				tail++;        //队列尾指针后移    
			}
		}
		head++; //一个结点不能再扩展，队列头指针指向下一结点   
	}
}

/*图形化界面相关函数*/
void GraphMain(int width, int length, int x, int y, int step) { //图形化界面主函数 起始坐标 步数

	initgraph(width, length);   // 创建绘图窗口，大小为 w*l 像素

	setbkcolor(WHITE); //设置背景色
	cleardevice(); //清理屏幕,才能显示背景色

	setlinecolor(BLUE);
	draw_struct(x, x + SIZE, y, y + SIZE); //画框线结构

	settextcolor(RED);
	settextstyle(16, 0, _T("宋体"));
	RECT r = { 0, 0, width, 200 };
	drawtext(_T("八数码问题求解"), &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);//输出文字

	draw_move(x, y, step); //方块移动

	settextcolor(RED);
	settextstyle(16, 0, _T("宋体"));
	RECT re = { 0, 0, width, 200 };
	drawtext(_T("移动结束，请按ENTER结束!"), &re, DT_CENTER | DT_VCENTER | DT_SINGLELINE);//输出文字

	getchar();
	closegraph();//关闭图形界面

}

void draw_struct(int left, int right, int top, int bottom) { //画框线结构

	setlinestyle(PS_DASH);//设定线性为虚线

	rectangle(left, top, right, bottom);//画底板

	setlinestyle(PS_SOLID); //恢复实线
	setfillcolor(YELLOW); //设置填充颜色
	setfillstyle(BS_SOLID);//固实填充

	for (int i = 1; i <= 3; i++) {
		for (int j = 1; j <= 3; j++) {
			fillrectangle(left + 100 * (j - 1), top + 100 * (i - 1), left + 100 * j, top + 100 * i);//画九个填充方块
		}
	}

}

void draw_number(int num, int x, int y, int size) {//输出数字 数字内容 起始位置 横向尺寸

	setlinecolor(BLACK);
	setlinestyle(PS_SOLID); //实线

	switch (num) {
		case 1:
			line(x + size, y, x + size, y + 2 * size);//画线
			break;
		case 2:
			line(x, y, x + size, y);
			line(x + size, y, x + size, y + size);
			line(x + size, y + size, x, y + size);
			line(x, y + size, x, y + 2 * size);
			line(x, y + 2 * size, x + size, y + 2 * size);
			break;
		case 3:
			line(x, y, x + size, y);
			line(x + size, y, x + size, y + size);
			line(x, y + size, x + size, y + size);
			line(x + size, y + size, x + size, y + 2 * size);
			line(x + size, y + 2 * size, x, y + 2 * size);
			break;
		case 4:
			line(x, y, x, y + size);
			line(x, y + size, x + size, y + size);
			line(x + size, y, x + size, y + 2 * size);
			break;
		case 5:
			line(x + size, y, x, y);
			line(x, y, x, y + size);
			line(x, y + size, x + size, y + size);
			line(x + size, y + size, x + size, y + 2 * size);
			line(x + size, y + 2 * size, x, y + 2 * size);
			break;
		case 6:
			line(x + size, y, x, y);
			line(x, y, x, y + 2 * size);
			line(x, y + size, x + size, y + size);
			line(x + size, y + size, x + size, y + 2 * size);
			line(x, y + 2 * size, x + size, y + 2 * size);
			break;
		case 7:
			line(x, y, x + size, y);
			line(x + size, y, x + size, y + 2 * size);
			break;
		case 8:
			line(x, y, x + size, y);
			line(x, y, x, y + 2 * size);
			line(x + size, y, x + size, y + 2 * size);
			line(x, y + size, x + size, y + size);
			line(x, y + 2 * size, x + size, y + 2 * size);
			break;
		default:
			break;

	}

}

void draw_move(int x, int y, int step) { //方块移动

	ifstream input("result.txt", ios::in | ios::_Nocreate);//读文件

	string a;
	int order[9]; //记录排列顺序
	int orderBefore[9] = { 0 }; //记录前一组的顺序
	int count;//计数

	for (int i = 1; i <= step; i++) {
		input >> a;//读过序号
		count = 0;
		for (int j = 1; j <= 9; j++) {//读入顺序
			if (count == 3)
				count = 1;
			else
				count = count + 1;//计数

			input >> order[j - 1];

			if (orderBefore[j - 1] != order[j - 1]) { //方块移动处
				orderBefore[j - 1] = order[j - 1]; //赋为新值

				if (order[j - 1] == 0) {//空位
					setlinestyle(PS_SOLID); //恢复实线

					setfillcolor(YELLOW); //设置填充颜色
					setfillstyle(BS_SOLID);//固实填充

					fillrectangle(x + 100 * (count - 1), y + 100 * ((j - 1) / 3), x + 100 * count, y + 100 * ((j - 1) / 3 + 1));//输出空位
				}
				else {//0位置为非0位
					setlinestyle(PS_SOLID); //恢复实线
					setfillcolor(GREEN); //设置填充颜色
					setfillstyle(BS_SOLID);//固实填充

					fillrectangle(x + 100 * (count - 1), y + 100 * ((j - 1) / 3), x + 100 * count, y + 100 * ((j - 1) / 3 + 1));//输出底色


					draw_number(order[j - 1], x + 100 * (count - 1) + 37, y + 100 * ((j - 1) / 3) + 25, 25);//输出数字标记

				}


			}

		}

		getchar();
	}

	input.close(); //关闭文件

}

int main()
{
	AStar aStar("init.txt", "dis.txt");
	aStar.Search();

	GraphMain(550, 550, 125, 125, step);//图形化

	return 0;
}
