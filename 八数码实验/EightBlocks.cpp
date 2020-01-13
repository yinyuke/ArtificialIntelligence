#include <iostream>
#include<fstream>
#include <string>
#include <graphics.h>  //ͼ�λ���

using namespace std;

//���峣��
#define SIZE 300 //����������С
#define Num 9  

int step = 0; //����

//��ͼ��������
void draw_struct(int left, int right, int top, int bottom); //�����߽ṹ
void draw_number(int num, int x, int y, int size);  //������� �������� ��ʼλ�� ����ߴ�
void draw_move(int x, int y, int step); //�����ƶ�

//��������������༰��Ա���� 
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

//��ȡ��ʼ���ļ�
TEight::TEight(const char *fname)
{
	ifstream fin;
	fin.open(fname, ios::in | ios::_Nocreate);
	if (!fin)
	{
		cout << "���ܴ������ļ�!" << endl;
		return;
	}
	int i;
	//��ȡ��ʼ״̬
	for (i = 0; i < Num;)
		fin >> p[i++];
	//��ȡ�ո�
	fin >> spac;
	//��ȡĿ��״̬
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
	step = total; //ȡ������
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

template<class Type> class TList;            //���Ա�ǰ�Ӷ���   

template<class Type> class TNode            //���Ա�����ģ��   
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
	TList() { Last = First = 0; Length = 0; }   //���캯��   
	int Getlen()const { return Length; }   //��Ա�������������Ա���   
	int Append(const Type& T);           //��Ա�������ӱ�β������   
	int Insert(const Type& T, int k);       //��Ա������������   
	Type GetData(int i);                         //��Ա���������ؽ�����ݳ�Ա   
	void SetData(const Type& T, int k); //��Ա���������ý�����ݳ�Ա      
private:
	TNode<Type> *First, *Last;             //���ݳ�Ա�����Ա��ס�βָ��     
	int Length;                                         //���ݳ�Ա�����Ա���   
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
	AStar() {}                           //���캯��   
	AStar(const char *fname1, const char *fname2);   //���������캯��    
	virtual void Search();               //A*������   
private:
	int f, g, h;                               //���ۺ���     
	int r[Num];                              //�洢״̬�и�������λ�õĸ�������   
	static int s[Num];                       //�洢Ŀ��״̬�и�������λ�õĸ�������   
	static int e[];                            //�洢����������Ծ���ĸ�������      
	void Printl(TList<AStar> L);       //��Ա�������������·��    
	int Expend(int i);                        //��Ա������A*�㷨��״̬��չ����    
	int Calcuf();                             //��Ա������������ۺ���   
	void Sort(TList<AStar>& L, int k);  //��Ա������������չ��㰴f��С����˳��������չ������   
	int Repeat(TList<AStar> &L);       //��Ա������������Ƿ��ظ�   
};

int AStar::s[Num], AStar::e[Num*Num];

AStar::AStar(const char *fname1, const char *fname2) :TEight(fname1)
{
	for (int i = 0; i < Num;)
	{
		r[p[i]] = i;   //�洢��ʼ״̬���ֵ�λ��   
		s[q[i]] = i++; //�洢Ŀ��״̬���ֵ�λ��   
	}
	ifstream fin;
	fin.open(fname2, ios::in );//�������ļ�   
	if (!fin)
	{
		cout << "���ܴ������ļ�!" << endl;
		return;
	}
	for (int i = 0; i < Num*Num; i++)   //�������������Ծ���ֵ     
		fin >> e[i];
	fin.close();
	f = g = h = 0;       //���ۺ�����ʼֵ     
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
	if (Extend(i))   //������չ   
	{
		int temp = r[p[r[0]]];   //�ı�״̬������λ�ñ仯���洢�ı���λ��    
		r[p[r[0]]] = r[0];
		r[0] = temp;
		return 1;
	}
	return 0;
}

int AStar::Calcuf()
{
	h = 0;
	for (int i = 0; i < Num; i++)  //������ۺ�����h   
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
	AStar T = *this;              //��ʼ���   
	T.f = T.Calcuf();               //��ʼ���Ĺ��ۺ���   
	TList<AStar> L;           //��������   
	L.Append(T);                 //��ʼ������   
	int head = 0, tail = 0;           //����ͷ��βָ��   
	while (head <= tail)           //���в�����ѭ��   
	{
		for (int i = 0; i < 4; i++)            //�ո�����ƶ�����   
		{
			T = L.GetData(head);   //ȥ����ͷ���   
			if (T.h == 0)                   //��Ŀ����   
			{
				T.Printl(L);//�������·��   
				T.Printf();    //���Ŀ��״̬   
				return;        //����   
			}
			if (T.Expend(i))          //��������չ   
			{
				int k = T.Repeat(L); //����������չ����ظ������        
				if (k < head)               //����ǲ�����չ�Ľ��   
					continue; //����   
				T.last = head;      //���ǲ�����չ�Ľ�㣬��¼�����   
				T.f = T.Calcuf(); //����f   
				if (k <= tail)         //�½�������չ����ظ�   
				{
					AStar Temp = L.GetData(k);
					if (Temp.g > T.g)         //�Ƚ������gֵ   
						L.SetData(T, k); //����gֵС��   
					continue;
				}
				T.Sort(L, head);            //�½��������չ������        
				tail++;        //����βָ�����    
			}
		}
		head++; //һ����㲻������չ������ͷָ��ָ����һ���   
	}
}

/*ͼ�λ�������غ���*/
void GraphMain(int width, int length, int x, int y, int step) { //ͼ�λ����������� ��ʼ���� ����

	initgraph(width, length);   // ������ͼ���ڣ���СΪ w*l ����

	setbkcolor(WHITE); //���ñ���ɫ
	cleardevice(); //������Ļ,������ʾ����ɫ

	setlinecolor(BLUE);
	draw_struct(x, x + SIZE, y, y + SIZE); //�����߽ṹ

	settextcolor(RED);
	settextstyle(16, 0, _T("����"));
	RECT r = { 0, 0, width, 200 };
	drawtext(_T("�������������"), &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);//�������

	draw_move(x, y, step); //�����ƶ�

	settextcolor(RED);
	settextstyle(16, 0, _T("����"));
	RECT re = { 0, 0, width, 200 };
	drawtext(_T("�ƶ��������밴ENTER����!"), &re, DT_CENTER | DT_VCENTER | DT_SINGLELINE);//�������

	getchar();
	closegraph();//�ر�ͼ�ν���

}

void draw_struct(int left, int right, int top, int bottom) { //�����߽ṹ

	setlinestyle(PS_DASH);//�趨����Ϊ����

	rectangle(left, top, right, bottom);//���װ�

	setlinestyle(PS_SOLID); //�ָ�ʵ��
	setfillcolor(YELLOW); //���������ɫ
	setfillstyle(BS_SOLID);//��ʵ���

	for (int i = 1; i <= 3; i++) {
		for (int j = 1; j <= 3; j++) {
			fillrectangle(left + 100 * (j - 1), top + 100 * (i - 1), left + 100 * j, top + 100 * i);//���Ÿ���䷽��
		}
	}

}

void draw_number(int num, int x, int y, int size) {//������� �������� ��ʼλ�� ����ߴ�

	setlinecolor(BLACK);
	setlinestyle(PS_SOLID); //ʵ��

	switch (num) {
		case 1:
			line(x + size, y, x + size, y + 2 * size);//����
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

void draw_move(int x, int y, int step) { //�����ƶ�

	ifstream input("result.txt", ios::in | ios::_Nocreate);//���ļ�

	string a;
	int order[9]; //��¼����˳��
	int orderBefore[9] = { 0 }; //��¼ǰһ���˳��
	int count;//����

	for (int i = 1; i <= step; i++) {
		input >> a;//�������
		count = 0;
		for (int j = 1; j <= 9; j++) {//����˳��
			if (count == 3)
				count = 1;
			else
				count = count + 1;//����

			input >> order[j - 1];

			if (orderBefore[j - 1] != order[j - 1]) { //�����ƶ���
				orderBefore[j - 1] = order[j - 1]; //��Ϊ��ֵ

				if (order[j - 1] == 0) {//��λ
					setlinestyle(PS_SOLID); //�ָ�ʵ��

					setfillcolor(YELLOW); //���������ɫ
					setfillstyle(BS_SOLID);//��ʵ���

					fillrectangle(x + 100 * (count - 1), y + 100 * ((j - 1) / 3), x + 100 * count, y + 100 * ((j - 1) / 3 + 1));//�����λ
				}
				else {//0λ��Ϊ��0λ
					setlinestyle(PS_SOLID); //�ָ�ʵ��
					setfillcolor(GREEN); //���������ɫ
					setfillstyle(BS_SOLID);//��ʵ���

					fillrectangle(x + 100 * (count - 1), y + 100 * ((j - 1) / 3), x + 100 * count, y + 100 * ((j - 1) / 3 + 1));//�����ɫ


					draw_number(order[j - 1], x + 100 * (count - 1) + 37, y + 100 * ((j - 1) / 3) + 25, 25);//������ֱ��

				}


			}

		}

		getchar();
	}

	input.close(); //�ر��ļ�

}

int main()
{
	AStar aStar("init.txt", "dis.txt");
	aStar.Search();

	GraphMain(550, 550, 125, 125, step);//ͼ�λ�

	return 0;
}
