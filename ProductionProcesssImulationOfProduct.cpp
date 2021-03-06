#include "stdafx.h"
#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <string>
#include <list>
#include <algorithm>
#include <memory>
#include <fstream>
#include <time.h>
#include <cstdlib>
using namespace std;

struct comparatorless
{
	bool operator()(const string &s1, const string &s2) const;    //重载函数调用运算符比较编号和类别编号或编号和编号,判断s1是否小于s2
};
bool comparatorless::operator()(const string &s1, const string &s2) const
{
	if (s2.find_first_of('+') == string::npos)   //类别编号s2和编号s1比较
	{
		return s1.substr(0, s1.find_first_of('+')) < s2;
	}
	else             //编号s1和编号s2比较
	{
		string::size_type index = s2.find_first_of('+', 0);
		string prefixP = s2.substr(0, index);   //提取s2以+分隔的前后缀
		string suffixP = s2.substr(index + 1);

		index = s1.find_first_of('+', 0);
		string prefix = s1.substr(0, index);   //提取s1以+分隔的前后缀
		string suffix = s1.substr(index + 1);

		if (prefix < prefixP)
		{
			return true;
		}
		else
		{
			if (prefix == prefixP)
			{
				if (suffix < suffixP)
				{
					return true;
				}
				else
				{
					return false;
				}
			}
			else
			{
				return false;
			}
		}
	}
}

//初始化过程即构造函数实现未定义,第一次运行前再定义,初始GCDTime的计算,类定义实现放入头文件
struct CodePriorityNAT  //设备等待队列中或设备上正在生产的产品的编号，在该设备上的生产优先级以及在上一个设备上生产完毕后已流逝的自然时效时间
{
	bool operator<(const CodePriorityNAT &P) const { return ProductionPriority < P.ProductionPriority; }  //重载关系运算符用优先级比较实现CodePriorityNAT对象比较
	bool operator>(const CodePriorityNAT &P) const { return ProductionPriority > P.ProductionPriority; }
	bool operator>=(const CodePriorityNAT &P) const { return !(operator<(P)); }
	bool operator<=(const CodePriorityNAT &P) const { return !(operator>(P)); }
	bool operator==(const CodePriorityNAT &P) const { return !(operator<(P)) && !(operator>(P)); }
	bool operator!=(const CodePriorityNAT &P) const { return !(operator==(P)); }
	string ProductCode; //产品编号
	int ProductionPriority;  //生产优先级
	int NaturalAgingTime;  //已流逝的自然时效时间
	CodePriorityNAT(string P1, int P2, int N) :ProductCode(P1), ProductionPriority(P2), NaturalAgingTime(N) {}
	CodePriorityNAT() = default;
};

class Priority_Queue    //设备等待队列类(优先级队列),队列数据元素为CodePriorityNAT
{
public:
	typedef list<CodePriorityNAT>::iterator iterator;
	Priority_Queue() = default;
	~Priority_Queue() = default;
	pair<bool, Priority_Queue::iterator> Insert(const CodePriorityNAT &x);  //插入操作,返回的pair的first指示插入是否成功,second为指向插入元素的迭代器
	bool RemoveTop(CodePriorityNAT &x);     //删除最高优先级元素并用x将其返回
	bool GetTop(CodePriorityNAT &x) const;  //获取最高优先级元素并用x将其返回 
	void MakeEmpty() { Queue.clear(); }    //清空队列
	bool isEmpty() const { return Queue.empty(); }   //判断队列是否为空
	bool isFull() const {return Queue.size() == Queue.max_size(); }   //判断队列是否已满
	Priority_Queue::iterator erase(const Priority_Queue::iterator &p) { return Queue.erase(p); }  //删除队列中p所指元素返回被删元素的下一元素
	Priority_Queue::iterator insert(const Priority_Queue::iterator &p, const CodePriorityNAT &c) { return Queue.insert(p, c); }  //将c插入至p所指位置,返回指向插入元素的迭代器
	list<CodePriorityNAT>::size_type  GetSize() const { return Queue.size(); }   //获取队列实际大小
	iterator begin() { return Queue.begin(); }   //获取指向队列最高优先级元素的迭代器
	iterator end() { return Queue.end(); }   //获取队列尾后迭代器

private:
	list<CodePriorityNAT>::iterator adjust();   //新元素加入队列后调整元素位置,使队列中各元素保持优先级关系
	list<CodePriorityNAT> Queue;   
};

list<CodePriorityNAT>::iterator Priority_Queue::adjust()
{
	CodePriorityNAT temp = Queue.back();
	auto p = Queue.end();
	--p;
	p=Queue.erase(p);

	if (Queue.begin() != p)
		--p;
	else
	{
		return Queue.insert(p, temp);
	}

	while (true)
	{
		if ((*p) > temp)
		{
			if (p != Queue.begin())
			{
				--p;
				if (p == Queue.begin())
					continue;
			}
		}
		else
		{
			++p;
			return Queue.insert(p, temp);
		}

		if (p == Queue.begin())
			break;
	}
	return Queue.insert(p, temp);
}

pair<bool, Priority_Queue::iterator> Priority_Queue::Insert(const CodePriorityNAT &x)
{
	if (isFull())
		return {false, end()};
	else
	{
		Queue.push_back(x);
		return { true, adjust() };
	}
}

bool Priority_Queue::RemoveTop(CodePriorityNAT &x)
{
	if (isEmpty())
		return false;
	else
	{
		x = Queue.front();
		Queue.pop_front();
		return true;
	}
}

bool Priority_Queue::GetTop(CodePriorityNAT &x) const
{
	if (isEmpty())
		return false;
	else
	{
		x = Queue.front();
		return true;
	}
}

struct PrepareTimeProduceTime
{
	int PrepareTime;   //生产准备时间
	int ProduceTime;   //实际生产时间
	PrepareTimeProduceTime(int Pre, int Pro) :PrepareTime(Pre), ProduceTime(Pro) {}
	PrepareTimeProduceTime() = default;
};

struct ProductionTimeAndPriority
{

	map<int, PrepareTimeProduceTime> ProductionTime;   //当前类别的设备在在ProductClassTimePriorityMappingTable中的给定的产品类别对应的生产工序vector中的下标(map关键字)及对应的准备及生产时间(映射值)
	int Priority;  //给定类别的产品在当前设备的上的生产优先级
};

struct DeviceCategoryAndNAT
{
	string DeviceCategoryCode;    //设备类别编号
	int NAT;    //当前类别的产品在该设备上生产完毕后的自然时效时间 
	DeviceCategoryAndNAT(string De, int N) :DeviceCategoryCode(De), NAT(N) {}
};

class Product
{
public:
	string DeviceID;  //产品正在占用或等待的设备编号
	int index; //上述DeviceID标识的设备的设备类别在当前产品的类别对应的产品工艺流程vector中的下标
	int time;  //产品正在等待DeviceID标识的设备为0,否则为产品已占用DeviceID标识的设备的时间
	int flag;  //产品正在占用设备为1,产品正在等待设备为0
	static map<string, vector<DeviceCategoryAndNAT>> ProductCategoryWorkingProcedure; //各产品类别及对应的工艺流程的集合
	static void initProductCategoryWorkingProcedure();   //初始化ProductCategoryWorkingProcedure
	Product(string D, int i, int t, int f) :DeviceID(D), index(i), time(t), flag(f) {}
};

void Product::initProductCategoryWorkingProcedure()   //解析当前目录下ProductCategoryWorkingProcedure.txt从中提取初始化数据初始化ProductCategoryWorkingProcedure
{
	ifstream input("ProductCategoryWorkingProcedure.txt");
	string temp;
	int level;
	int flag;
	map<string, vector<DeviceCategoryAndNAT>>::iterator p1;
	while (input >> temp)
	{
		if (temp == "#b1" || temp == "#e1")
		{
			if (temp == "#b1")
			{
				level = 1;
			}
			else
			{
				continue;
			}
		}
		else
		{
			if (temp == "#b2" || temp == "#e2")
			{
				if (temp == "#b2")
				{
					level = 2;
					flag = 0;
				}
				else
				{
					continue;
				}
			}
			else
			{
				if (level == 1)
				{
					p1 = ProductCategoryWorkingProcedure.insert(make_pair(temp, vector<DeviceCategoryAndNAT>())).first;
				}
				else 
				{
					if (flag == 0)
					{
						p1->second.push_back(DeviceCategoryAndNAT(temp, 0));
						flag = 1;
					}
					else
					{
						p1->second.back().NAT = stoi(temp);
						flag = 0;
					}
				}
			}
		}
	}
}

map<string, vector<DeviceCategoryAndNAT>> Product::ProductCategoryWorkingProcedure;

struct Mouldstate
{
	string MouldCode;   //与产品类别对应的模具编号
	bool isOn;  //标志模具状态为上线下线的bool变量(T上线F下线)
	Mouldstate(string Mo, bool isOn) :MouldCode(Mo), isOn(isOn) {}
	Mouldstate() = default;
};

class Device
{
public:
	static map<string, map<string, ProductionTimeAndPriority>> DeviceCategorySet; //设备类别和其能够生产的产品类别及对应的生产时间优先级构成的集合,外层关键字映射值为映射表，关键字为产品类别编号，映射值为该类别的设备能够生产的产品类别,及该类别的设备在该产品类别的工艺流程表中对应各下标及生产时间
	static void initDeviceCategorySet();  //初始化DeviceCategorySet
	Priority_Queue WaitingQueue; //当前设备的等待队列
	set<CodePriorityNAT> ProductisProducing;  //设备当前正在生产的零件对应的三元组,set中只能有一个元素 
	bool isTakenUp;  //标志当前设备是否被占用,T占用(有产品生产或处于调整状态),F闲置
	string LatestProductCategory; //该设备最近一次生产完的产品的类别编号
	map<string, Mouldstate> MouldOnDevice; //映射表,保存模具能够生产的产品类别(关键字)和模具及模具状态的对应关系
};

void Device::initDeviceCategorySet()   ////解析当前目录下PriorityProducePerpareTime.txt从中提取初始化数据初始化ProductCategoryWorkingProcedure
{
	ifstream input("PriorityProducePerpareTime.txt");
	string temp;
	int level;
	int rowinlevel;
	map<string, map<string, ProductionTimeAndPriority>>::iterator p1;
	map<string, ProductionTimeAndPriority>::iterator p2;
	map<int, PrepareTimeProduceTime>::iterator p3;
	while (input >> temp)
	{
		if (temp == "#b1" || temp == "#e1")
		{
			if (temp == "#b1")
			{
				level = 1;
				rowinlevel = 1;
			}
			else
			{
				continue;
			}
		}
		else
		{
			if (temp == "#b2" || temp == "#e2")
			{
				if (temp == "#b2")
				{
					level = 2;
					rowinlevel = 1;
				}
				else
				{
					continue;
				}
			}
			else
			{
				if (temp == "#b3" || temp == "#e3")
				{
					if (temp == "#b3")
					{
						level = 3;
						rowinlevel = 1;
					}
					else
					{
						continue;
					}
				}
				else
				{
					if (level == 1)
					{
						p1 = DeviceCategorySet.insert(make_pair(temp, map<string, ProductionTimeAndPriority>())).first;
					}
					else if (level == 2)
					{
						p2 = p1->second.insert(make_pair(temp, ProductionTimeAndPriority())).first;
					}
					else if (level == 3)
					{
						if (rowinlevel == 1)
						{
							p2->second.Priority = stoi(temp);
						}
						else
						{
							if (rowinlevel % 3 == 2)
							{
								p3 = p2->second.ProductionTime.insert(make_pair(stoi(temp), PrepareTimeProduceTime())).first;
							}
							else if (rowinlevel % 3 == 0)
							{
								p3->second.PrepareTime = stoi(temp);

							}
							else
							{
								p3->second.ProduceTime = stoi(temp);
							}

						}
						++rowinlevel;
					}
				}
			}
		}
	}
}

map<string, map<string, ProductionTimeAndPriority>> Device::DeviceCategorySet;

class mould   //模具类 添加关键字对象比较
{
public:
	map<string, bool> AsPartComponent;  //映射表,内容为组成模具的零件编号和标志零件是否被占用的标志变量之间的映射关系
	string ProductIsProducing; //正在使用该模具生产的产品编号
	string DeviceBelongTo;  //模具所属设备编号
	bool MouldState; //模具状态,T上线,F下线
};

class Component  //零件类添加关键字对象比较
{
public:
	set<string> MouldBelongTo;  //零件所属模具编号的集合
	string ComponentCorrespondingMouldClass; //该零件占用的模具能够生产的产品类别编号
	string MouldSerialNumber;  //该零件占用的模具序号
};

struct ProductInsertion
{
	int InsertTime;   //产品插入时间
	set<string> ProductToInsert;   //时间InsertTime下要插入的产品的编号构成的集合
	ProductInsertion(int I) :InsertTime(I) {}
};

struct IncompleteOptimization
{
	string ProductCode;   //目前已生成的不完全优化方案中对应队列的已加入产品中排在最末端的产品编号
	Priority_Queue::iterator It;    //在队列中指向该产品的迭代器
	int WaitTime;    //该产品的等待时间
	IncompleteOptimization(string P, Priority_Queue::iterator I, int W) :ProductCode(P), It(I), WaitTime(W) {}
	IncompleteOptimization() = default;

};

class ProductionCycleComputing
{
public:
	int ProductionCycleCalculation();  //计算产品生产周期,返回生产完成时间
	ProductionCycleComputing();

private:
	static int PositionRelation(const Priority_Queue::iterator &it1, const Priority_Queue::iterator &it2, Priority_Queue &m);  //判断优先级队列中it1是在it2之后还是在之前,在之前返回0,在之后返回1,调用者保证it1,it2不等
	int WaitingTimeComputing(Priority_Queue::iterator it, map<string, Device, comparatorless>::iterator m, int InsertScheme);  //计算it所指元素在队列中的等待时间
	int TimeIncrementComputing(const Priority_Queue::iterator &BeCalculated, const Priority_Queue::iterator &CauseCalculating, map<string, Device, comparatorless>::iterator m, int InsertScheme); //计算CauseCalculating加入队列后导致修正BeCalculated等待时间所需时间增量
	bool NewProductInsert(int &t, const bool &TF);  //处理t时刻新产品插入,TF为true表示在第二轮循环处理否则表示在第一轮循环处理
	shared_ptr<map<string, map<string, map<string, Device, comparatorless>::iterator>>> ProductInsertHandle(map<string, multiset<CodePriorityNAT>> &ProductInsert, const int &tstart, const int &t);  //tstart时刻将ProductInsert中的产品加入对应设备集的等待队列
	void ProductAddToProduction(map<string, map<string, Device, comparatorless>::iterator> &SetofDevice, const int tstart, const int tend, const int &t, const size_t &end, map<string, map<string, map<string, Device, comparatorless>::iterator>> &DynamicList, map<string, pair<map<string, Device, comparatorless>::iterator, map<string, Device, comparatorless>::iterator>> &NoStartInsert);  //根据SetofDevice中的加入关系执行产品加入
	void GCDTimeUpdate();  //用于插入产品后更新最大公约数时间GCDTime
	static int GCDComputing(const int n, const int m); //计算并返回n,m的最大公约数
	void initInsert();
	void initMould();
	void initComponent();
	void initDevice();
	void initStateQuery();
	void initRepairTimeOfDevice();
	map<string, Device, comparatorless> SetofDevice;  //所有同类或不同类的设备组成的集合,关键字设备编号(设备类别编号+设备序号)
	map<string, Product, comparatorless> SetofProduct; //所有进入生产流程的产品组成的集合,关键字产品编号(产品类别编号+产品序号)
	map<string, mould, comparatorless> SetofMould;  //所有模具的集合,模具编号(模具能够生产的产品类别编号+模具序号)
	map<string, Component> SetofComponent; //所有零件的集合,关键字零件编号
	map<string, vector<ProductInsertion>> Insert;  //设备类别和各时间点下要插入的产品的对应关系
	vector<ProductInsertion>::size_type SearchPoint = 0;
	int GCDTime;  //最大公约数时间(各类设备上生产各类产品的时间,各类产品在各类设备上生产完毕后的自然时效时间,各类产品在各类设备上的生产准备时间，各类设备小修大修时间及时间间隔，以及在运行时要计入的设备产品的剩余生产时间)
	struct DeviceState
	{
		int T2;  //距设备最近一次调整已流逝的时间
		int T1;  //设备已调整时间
		int sign; //最近一次进行的是大修还是小修,0小修,1大修
		bool flag;  //设备是否正在调整,T正在调整,F未在调整
	};
	map<string, DeviceState> StateQuery;  //设备编号(关键字)和设备状态之间的映射关系
	struct RepairTime
	{
		int T2; //大修时长
		int T1; //小修时长
		int DeltaT1;  //小修至大修的时间间隔
		int DeltaT2;  //大修至小修的时间间隔
	};
	map<string, RepairTime> RepairTimeOfDevice; //设备类别和维修时间之间的映射关系
};

void ProductionCycleComputing::initRepairTimeOfDevice()
{
	ifstream input("RepairTimeOfDevice.txt");
	string temp;
	int level;
	map<string, RepairTime>::iterator p1;
	while (input >> temp)
	{
		if (temp == "#b1" || temp == "#e1")
		{
			if (temp == "#b1")
			{
				level = 1;
			}
			else
			{
				continue;
			}
		}
		else
		{
			if (temp == "#b2" || temp == "#e2")
			{
				if (temp == "#b2")
				{
					level = 2;
				}
				else
				{
					continue;
				}
			}
			else
			{
				if (level == 1)
				{
					p1 = RepairTimeOfDevice.insert(make_pair(temp, RepairTime())).first;
				}
				else
				{
					int flag = 1;
					while (temp != "#e2")
					{
						if (flag == 1)
						{
							p1->second.DeltaT1 = stoi(temp);
							flag = 2;
						}
						else if (flag == 2)
						{
							p1->second.DeltaT2 = stoi(temp);
							flag = 3;
						}
						else if (flag == 3)
						{
							p1->second.T1 = stoi(temp);
							flag = 4;
						}
						else
						{
							p1->second.T2 = stoi(temp);
						}
						input >> temp;
					}
				}
			}
		}
	}
}
void ProductionCycleComputing::initStateQuery()
{
	ifstream input("StateQuery.txt");
	string temp;
	int level;
	map<string, DeviceState>::iterator p1;
	while (input >> temp)
	{
		if (temp == "#b1" || temp == "#e1")
		{
			if (temp == "#b1")
			{
				level = 1;
			}
			else
			{
				continue;
			}
		}
		else
		{
			if (temp == "#b2" || temp == "#e2")
			{
				if (temp == "#b2")
				{
					level = 2;
				}
				else
				{
					continue;
				}
			}
			else
			{
				if (level == 1)
				{
					p1 = StateQuery.insert(make_pair(temp, DeviceState())).first;
				}
				else
				{

					p1->second.flag = false;
					p1->second.T1 = 0;
					p1->second.T2 = 0;
					p1->second.sign =stoi(temp);
					input >> temp;
				}
			}
		}
	}
}
void ProductionCycleComputing::initDevice()
{
	ifstream input("Device.txt");
	string temp;
	int level;
	map<string, Device, comparatorless>::iterator p1;
	while (input >> temp)
	{
		if (temp == "#b1" || temp == "#e1")
		{
			if (temp == "#b1")
			{
				level = 1;
			}
			else
			{
				continue;
			}
		}
		else
		{
			if (temp == "#b2" || temp == "#e2")
			{
				if (temp == "#b2")
				{
					level = 2;
				}
				else
				{
					continue;
				}
			}
			else
			{
				if (level == 1)
				{
					p1 = SetofDevice.insert(make_pair(temp, Device())).first;
					p1->second.isTakenUp = false;
					p1->second.LatestProductCategory = "";
				}
				else
				{
					int flag = 1;
					map<string, Mouldstate>::iterator p2;
					while (temp != "#e2")
					{
						if (flag == 1)
						{
							p2 = p1->second.MouldOnDevice.insert(make_pair(temp, Mouldstate())).first;
							flag = 2;
						}
						else if (flag == 2)
						{
							p2->second.MouldCode = temp;
							flag = 3;
						}
						else
						{
							if (temp == "T")
							{
								p2->second.isOn = true;
							}
							else
							{
								p2->second.isOn = false;
							}
							flag = 1;
						}
						input >> temp;
					}
				}
			}
		}
	}
}
void ProductionCycleComputing::initComponent()
{
	ifstream input("Component.txt");
	string temp;
	int level;
	int flag;
	map<string, Component>::iterator p1;
	while (input >> temp)
	{
		if (temp == "#b1" || temp == "#e1")
		{
			if (temp == "#b1")
			{
				level = 1;
			}
			else
			{
				continue;
			}
		}
		else
		{
			if (temp == "#b2" || temp == "#e2")
			{
				if (temp == "#b2")
				{
					level = 2;
					flag = 1;
				}
				else
				{
					continue;
				}
			}
			else
			{
				if (temp == "#b3" || temp == "#e3")
				{
					if (temp == "#b3")
					{
						level = 3;
					}
					else
						continue;
				}
				else
				{
					if (level == 1)
					{
						p1 = SetofComponent.insert(make_pair(temp, Component())).first;
					}
					else
					{
						if (level == 2)
						{
							if (flag == 1)
							{
								p1->second.ComponentCorrespondingMouldClass = temp;
								flag = 2;
							}
							else
							{
								p1->second.MouldSerialNumber = temp;
							}
						}
						else
						{
							while (temp != "#e3")
							{
								p1->second.MouldBelongTo.insert(temp);
								input >> temp;
							}
						}
					}
				}
			}
		}
	}
}
void ProductionCycleComputing::initMould()
{
	ifstream input("Mould.txt");
	string temp;
	int level;
	int flag;
	map<string, mould, comparatorless>::iterator p1;
	while (input >> temp)
	{
		if (temp == "#b1" || temp == "#e1")
		{
			if (temp == "#b1")
			{
				level = 1;
			}
			else
			{
				continue;
			}
		}
		else
		{
			if (temp == "#b2" || temp == "#e2")
			{
				if (temp == "#b2")
				{
					level = 2;
					flag = 1;
				}
				else
				{
					continue;
				}
			}
			else
			{
				if (temp == "#b3" || temp == "#e3")
				{
					if (temp == "#b3")
					{
						level = 3;
						flag = 1;
					}
					else
						continue;
				}
				else
				{
					if (level == 1)
					{
						p1 = SetofMould.insert(make_pair(temp, mould())).first;
					}
					else
					{
						if (level == 2)
						{
							if (flag == 1)
							{
								p1->second.DeviceBelongTo = temp;
								flag = 2;
							}
							else
							{
								if (temp == "T")
									p1->second.MouldState = true;
								else
									p1->second.MouldState = false;
								p1->second.ProductIsProducing = "";
							}
						}
						else
						{
							map<string, bool>::iterator p2;
							while (temp != "#e3")
							{
								if (flag == 1)
								{
									p2 = p1->second.AsPartComponent.insert(make_pair(temp, false)).first;
									flag = 2;
								}
								else
								{
									if (temp == "T")
										p2->second = true;
									else
										p2->second = false;
									flag = 1;
								}
								input >> temp;
							}
						}
					}
				}
			}
		}
	}
}

void ProductionCycleComputing::initInsert()
{
	ifstream input("Insert.txt");
	vector<int> templist;
	string temp;
	while (true)
	{
		input >> temp;
		if (temp == "#b")
		{
			continue;
		}

		if (temp == "#e")
			break;

		templist.push_back(stoi(temp));
	}

	int rowinlevel;
	map<string, vector<ProductInsertion>>::iterator p1;
	while (input >> temp)
	{
		if (temp == "#b1" || temp == "#e1")
		{
			if (temp == "#b1")
			{
				rowinlevel = 1;
			}
			else
			{
				continue;
			}
		}
		else
		{
			if (rowinlevel == 1)
			{
				p1 = Insert.insert(make_pair(temp, vector<ProductInsertion>())).first;
				++rowinlevel;
			}
			else
			{
				p1->second.push_back(ProductInsertion(templist[rowinlevel-2]));
				while (temp != "#e")
				{
					if (temp == "#b")
					{
						input >> temp;
						continue;
					}
					p1->second.back().ProductToInsert.insert(temp);
					input >> temp;
				}
				++rowinlevel;
			}
		}
	}
}

ProductionCycleComputing::ProductionCycleComputing()
{
	initInsert();
	initMould();
	initComponent();
	initDevice();
	initStateQuery();
	initRepairTimeOfDevice();

	//求各类设备上生产各类产品的时间, 各类产品在各类设备上的生产准备时间
	//各类产品在各类设备上生产完毕后的自然时效时间
	//各类设备小修大修时间及时间间隔的最大公约数更新GCDTime
	set<int> temp;
	for (map<string, map<string, ProductionTimeAndPriority>>::iterator p = Device::DeviceCategorySet.begin(); p != Device::DeviceCategorySet.end(); ++p)
	{
		for (map<string, ProductionTimeAndPriority>::iterator q = p->second.begin(); q != p->second.end(); ++q)
		{
			for (map<int, PrepareTimeProduceTime>::iterator s = q->second.ProductionTime.begin(); s != q->second.ProductionTime.end(); ++s)
			{
				temp.insert(s->second.PrepareTime);
				temp.insert(s->second.ProduceTime);
			}
		}
	}

	for (map<string, vector<DeviceCategoryAndNAT>>::iterator p = Product::ProductCategoryWorkingProcedure.begin(); p != Product::ProductCategoryWorkingProcedure.end(); ++p)
	{
		for (vector<DeviceCategoryAndNAT>::iterator q = p->second.begin(); q != p->second.end(); ++q)
		{
			temp.insert(q->NAT);
		}
	}

	for (map<string, RepairTime>::iterator p = RepairTimeOfDevice.begin(); p != RepairTimeOfDevice.end(); ++p)
	{
		temp.insert(p->second.DeltaT1);
		temp.insert(p->second.DeltaT2);
		temp.insert(p->second.T1);
		temp.insert(p->second.T2);
	}

	set<int>::iterator p = temp.begin();
	set<int>::iterator q = p;
	++q;
	if (q != temp.end())
	{
		GCDTime = GCDComputing(*p, *q);
	}
	else
	{
		GCDTime = *p;
	}
	for (set<int>::iterator s = q; s != temp.end(); ++s)
	{
		if (s != q)
		{
			GCDTime = GCDComputing(GCDTime, *s);
		}
	}

}

int ProductionCycleComputing::GCDComputing(int n, int m)
{
	while (m != 0)
	{
		int temp = m;
		m = n % m;
		n = temp;
	}

	return n;
}

void ProductionCycleComputing::GCDTimeUpdate()
{
	{
		map<string, RepairTime>::iterator p = RepairTimeOfDevice.begin();
		for (map<string, DeviceState>::iterator q = StateQuery.begin(); q != StateQuery.end(); )    //用各设备的剩余调整时间和未调整的时间更新最大公约数时间
		{
			if (q->first.substr(0, q->first.find_first_of('+')) == p->first)
			{
				if (q->second.flag == false)
				{
					if ((q->second.sign == 0 && p->second.DeltaT1 != q->second.T2) || (q->second.sign == 1 && p->second.DeltaT2 != q->second.T2))
					{
						if (q->second.sign == 0)
						    GCDTime = GCDComputing(GCDTime, p->second.DeltaT1- q->second.T2);
						else
							GCDTime = GCDComputing(GCDTime, p->second.DeltaT2 - q->second.T2);
					}
				}
				else
				{
					if (q->second.sign == 0)
					{
						GCDTime = GCDComputing(GCDTime, p->second.T1 - q->second.T1);
					}
					else
					{
						GCDTime = GCDComputing(GCDTime, p->second.T2 - q->second.T1);
					}
				}
				++q;
			}
			else
			{
				++p;
			}
		}
	}

	{
		for (map<string, Device, comparatorless>::iterator p = SetofDevice.begin(); p != SetofDevice.end(); ++p)   //用各设备剩余自然时效时间更新最大公约数
		{
			if (p->second.WaitingQueue.isEmpty() == false)
			{
				for (Priority_Queue::iterator q = p->second.WaitingQueue.begin(); q != p->second.WaitingQueue.end(); ++q)
				{
					if (q->NaturalAgingTime != -1 && q->NaturalAgingTime != Product::ProductCategoryWorkingProcedure.find(q->ProductCode.substr(0, q->ProductCode.find_first_of('+')))->second[SetofProduct.find(q->ProductCode)->second.index - 1].NAT)
						GCDTime = GCDComputing(GCDTime, Product::ProductCategoryWorkingProcedure.find(q->ProductCode.substr(0, q->ProductCode.find_first_of('+')))->second[SetofProduct.find(q->ProductCode)->second.index - 1].NAT - q->NaturalAgingTime);
				}
			}

			if (p->second.ProductisProducing.empty() == false)   //用设备正在生产的产品的剩余生产时间更新最大公约数
			{
				int t = 0;
				const string &temp = (*p->second.ProductisProducing.begin()).ProductCode;
				PrepareTimeProduceTime &temp1 = p->second.DeviceCategorySet.find(p->first.substr(0, p->first.find_first_of('+')))->second[temp.substr(0, temp.find_first_of('+'))].ProductionTime[SetofProduct.find(temp)->second.index];
				t += temp1.ProduceTime;
				if (temp.substr(0, temp.find_first_of('+')) != p->second.LatestProductCategory)
					t += temp1.PrepareTime;
				t -= SetofProduct.find(temp)->second.time;
				GCDTime = GCDComputing(GCDTime, t);
			}
		}
	}
}

bool ProductionCycleComputing::NewProductInsert(int &t, const bool &TF)
{
	if (Insert.empty() == false)
	{
		vector<ProductInsertion>::size_type start = SearchPoint;
		for ( ; start != Insert.begin()->second.size(); ++start)      //找出[t, GCDTime]时间区间内有产品插入的时间点集合
		{
			if (t < Insert.begin()->second[start].InsertTime)
				break;
		}

		vector<ProductInsertion>::size_type end;
		if (start == Insert.begin()->second.size())
		{
			if (Insert.begin()->second[start - 1].InsertTime == t)
			{
				if (TF == false)  //不是第二轮循环
				{
					--start;
					end = start;
					SearchPoint = Insert.begin()->second.size();
				}
				else
				{
					return false;
				}
			}
			else
			{
				return false;
			}
		}
		else
		{
			vector<ProductInsertion>::size_type i = start;
			for ( ; i != Insert.begin()->second.size(); ++i)
			{
				if (Insert.begin()->second[i].InsertTime >= t + GCDTime)
					break;
			}

			if (start == 0)
			{
				if (i == 0)
				{
					return false;
				}
				else
				{
					SearchPoint = i;
					start = 0;
					end = i - 1;
				}
			}
			else
			{
				if (i == start)
				{
					if (t == Insert.begin()->second[start - 1].InsertTime)
					{
						if (TF == false)
						{
							--start;
							end = start;
							SearchPoint = i;
						}
						else
						{
							SearchPoint = i;  //  SearchPoint = i;可有可无
							return false;
						}
					}
					else
					{
						SearchPoint = i;
						return false;
					}
				}
				else
				{
					if (t == Insert.begin()->second[start - 1].InsertTime)
					{
						if (TF == false)
						{
							--start;
						}
						end = i - 1;
						SearchPoint = i;
					}
					else
					{
						end = i - 1;
						SearchPoint = i;
					}
				}
			}
		}

		if (Insert.begin()->second[start].InsertTime != t || (Insert.begin()->second[start].InsertTime == t && start != end))
		{
			map<string, map<string, map<string, Device, comparatorless>::iterator>> DynamicList; //外层map关键字设备类别,内层设备编号,映射值为指向设备迭代器

			vector<ProductInsertion>::size_type first;
			if (Insert.begin()->second[start].InsertTime == t)
			{
				 first = start + 1;
				 map<string, multiset<CodePriorityNAT>> temp;
				 for (map<string, vector<ProductInsertion>>::iterator q = Insert.begin(); q != Insert.end(); ++q)
				 {
					 if (q->second[start].ProductToInsert.empty() == false)
					 {
						 map<string, multiset<CodePriorityNAT>>::iterator temp1 = temp.insert(make_pair(q->first, multiset<CodePriorityNAT>())).first;
						 for (set<string>::iterator p = q->second[start].ProductToInsert.begin(); p != q->second[start].ProductToInsert.end(); ++p)
						 {
							 int Priority = Device::DeviceCategorySet.find(q->first)->second[(*p).substr(0, (*p).find_first_of('+'))].Priority;
							 temp1->second.insert(CodePriorityNAT(*p, Priority, -1));
					     }
					 }
				 }
				 ProductInsertHandle(temp, t, t);
			}
			else
			{
				 first = start;
			}

			map<string, Device, comparatorless>::iterator before = SetofDevice.begin();
			map<string, pair<map<string, Device, comparatorless>::iterator, map<string, Device, comparatorless>::iterator>> NoStartInsert;  //关键字设备类别,值该类别设备的迭代器范围
			for (map<string, vector<ProductInsertion>>::iterator p = Insert.begin(); p != Insert.end(); ++p)
			{
				vector<ProductInsertion>::size_type q = first;
				for ( ; q != end + 1; ++q)
				{
					if (p->second[q].ProductToInsert.empty() == false)
						break;
				}

				if (q != end + 1)
				{
					DynamicList.insert(make_pair(p->first, map<string, map<string, Device, comparatorless>::iterator>()));

					while (p->first != before->first.substr(0, before->first.find_first_of('+')))
					{
						++before;
					}

					map<string, Device, comparatorless>::iterator after = before;
					while (after != SetofDevice.end() && p->first == after->first.substr(0, after->first.find_first_of('+')))
					{
						++after;
					}

					NoStartInsert.insert(make_pair(p->first, make_pair(before, after)));
					before = after;
				}
			}

			//此处执行加入设备活动集和设备状态更新
			{
				map<string, RepairTime>::iterator p = RepairTimeOfDevice.begin();
				for (map<string, DeviceState>::iterator q = StateQuery.begin(); q != StateQuery.end(); )
				{
					if (q->first.substr(0, q->first.find_first_of('+')) == p->first)
					{
						if (q->second.flag == false)
						{
							if ((q->second.sign == 0 && p->second.DeltaT1 != q->second.T2) || (q->second.sign == 1 && p->second.DeltaT2 != q->second.T2))
							{
								q->second.T2 += Insert.begin()->second[end].InsertTime - t;
							}
						}
						++q;
					}
					else
					{
						++p;
					}
				}
			}

			map<string, map<string, Device, comparatorless>::iterator> EquipmentToBeAdded;
			for (map<string, Device, comparatorless>::iterator p = SetofDevice.begin(); p != SetofDevice.end(); ++p)
			{
				EquipmentToBeAdded.insert(make_pair(p->first, p));
				if (p->second.WaitingQueue.isEmpty() == false)
				{
					for (Priority_Queue::iterator q = p->second.WaitingQueue.begin(); q != p->second.WaitingQueue.end(); ++q)
					{
						if (q->NaturalAgingTime != -1 && q->NaturalAgingTime != Product::ProductCategoryWorkingProcedure.find(q->ProductCode.substr(0, q->ProductCode.find_first_of('+')))->second[SetofProduct.find(q->ProductCode)->second.index - 1].NAT)
							q->NaturalAgingTime += Insert.begin()->second[end].InsertTime - t;
					}
				}
			}

			ProductAddToProduction(EquipmentToBeAdded, t, Insert.begin()->second[first].InsertTime, t, end, DynamicList, NoStartInsert);
			for (vector<ProductInsertion>::size_type i = first; i <= end; ++i)
			{
				map<string, multiset<CodePriorityNAT>> temp;
				for (map<string, vector<ProductInsertion>>::iterator q = Insert.begin(); q != Insert.end(); ++q)
				{
					if (q->second[i].ProductToInsert.empty() == false)
					{
						map<string, multiset<CodePriorityNAT>>::iterator temp1 = temp.insert(make_pair(q->first, multiset<CodePriorityNAT>())).first;
						for (set<string>::iterator p = q->second[i].ProductToInsert.begin(); p != q->second[i].ProductToInsert.end(); ++p)
						{
							int Priority = Device::DeviceCategorySet.find(q->first)->second[(*p).substr(0, (*p).find_first_of('+'))].Priority;
							temp1->second.insert(CodePriorityNAT(*p, Priority, -1));
						}
					}
				}

				if (i != end)
				{
					map<string, map<string, Device, comparatorless>::iterator> SD;
					shared_ptr<map<string, map<string, map<string, Device, comparatorless>::iterator>>> EquipmentToBeAdded = ProductInsertHandle(temp, Insert.begin()->second[i].InsertTime, t);
					map<string, map<string, map<string, Device, comparatorless>::iterator>>::iterator pa = DynamicList.begin();
					map<string, map<string, map<string, Device, comparatorless>::iterator>>::iterator pb = EquipmentToBeAdded->begin();
					while (pa != DynamicList.end() && pb != EquipmentToBeAdded->end())   //通过动态表和有产品加入等待队列的设备集的交集确定可执行产品加入的设备集
					{
						if (pa->first == pb->first)
						{
							map<string, map<string, Device, comparatorless>::iterator>::iterator pc = pa->second.begin();
							map<string, map<string, Device, comparatorless>::iterator>::iterator pd = pb->second.begin();
							while (pc != pa->second.end() && pd != pb->second.end())
							{
								if (pc->first == pd->first)
								{
									SD.insert(*pc);
									++pc;
									++pd;
								}
								else if (pc->first < pd->first)
								{
									++pc;
								}
								else
								{
									++pd;
								}
							}
							++pa;
							++pb;
						}
						else if (pa->first < pb->first)
						{
							++pa;
						}
						else
						{
							++pb;
						}
					}
					ProductAddToProduction(SD, Insert.begin()->second[i].InsertTime, Insert.begin()->second[i + 1].InsertTime, t, end, DynamicList, NoStartInsert);
				}
				else
				{
					ProductInsertHandle(temp, Insert.begin()->second[i].InsertTime, Insert.begin()->second[i].InsertTime);
				}
			}
			//这里更新t和GCDTime
			t = Insert.begin()->second[end].InsertTime;
			GCDTimeUpdate();
		}
		else
		{
			map<string, multiset<CodePriorityNAT>> temp;
			for (map<string, vector<ProductInsertion>>::iterator q = Insert.begin(); q != Insert.end(); ++q)
			{
				if (q->second[start].ProductToInsert.empty() == false)
				{
					map<string, multiset<CodePriorityNAT>>::iterator temp1 = temp.insert(make_pair(q->first, multiset<CodePriorityNAT>())).first;
					for (set<string>::iterator p = q->second[start].ProductToInsert.begin(); p != q->second[start].ProductToInsert.end(); ++p)
					{
						int Priority = Device::DeviceCategorySet.find(q->first)->second[(*p).substr(0, (*p).find_first_of('+'))].Priority;
						temp1->second.insert(CodePriorityNAT(*p, Priority, -1));
					}
				}
			}
			ProductInsertHandle(temp, t, t);
		}
		return true;
	}
}

void ProductionCycleComputing::ProductAddToProduction(map<string, map<string, Device, comparatorless>::iterator> &SetofDevice, const int tstart, const int tend, const int &t, const size_t &end, map<string, map<string, map<string, Device, comparatorless>::iterator>> &DynamicList, map<string, pair<map<string, Device, comparatorless>::iterator, map<string, Device, comparatorless>::iterator>> &NoStartInsert)
{
	if (SetofDevice.empty() == false)
	{
		map<string, multiset<CodePriorityNAT>> TempMap; //设备编号和temp集的映射关系
		Priority_Queue TempQueue;  //存放进入设备生产的候选产品的优先级队列
		for (map<string, map<string, Device, comparatorless>::iterator>::iterator p = SetofDevice.begin(); p != SetofDevice.end(); ++p)
		{
			if (!(*p->second).second.ProductisProducing.empty())
			{
				if (tstart == t)
				{
					set<CodePriorityNAT>::iterator q = (*p->second).second.ProductisProducing.begin();
					map<string, Product, comparatorless>::iterator m = SetofProduct.find((*q).ProductCode);
					if (NoStartInsert.find((*p->second).first.substr(0, (*p->second).first.find_first_of('+'))) != NoStartInsert.end())
					{
						(*m).second.time += tend - tstart;
					}
					else
					{
						(*m).second.time += Insert.begin()->second[end].InsertTime - t;
					}
				}
			}
			else
			{
				if (t == tstart)
				{
					if ((*p->second).second.isTakenUp == true)
					{
						if (NoStartInsert.find((*p->second).first.substr(0, (*p->second).first.find_first_of('+'))) != NoStartInsert.end())
						{
							StateQuery[(*p->second).first].T1 += tend - tstart;
						}
						else
						{
							StateQuery[(*p->second).first].T1 += Insert.begin()->second[end].InsertTime - t;
						}
					}
				}

				if (!(*p->second).second.WaitingQueue.isEmpty())
				{
					if ((*p->second).second.isTakenUp == false)
					{
						multiset<CodePriorityNAT> temp;
						CodePriorityNAT temp1;
						bool TF = false;
						while ((*p->second).second.WaitingQueue.isEmpty() == false)
						{
							(*p->second).second.WaitingQueue.RemoveTop(temp1);
							map<string, Product, comparatorless>::iterator q = SetofProduct.find(temp1.ProductCode);
							map<string, vector<DeviceCategoryAndNAT>>::iterator t = Product::ProductCategoryWorkingProcedure.find(temp1.ProductCode.substr(0, temp1.ProductCode.find_first_of('+')));
							if (temp1.NaturalAgingTime != -1 && (*t).second[(*q).second.index - 1].NAT != temp1.NaturalAgingTime)
							{
								temp.insert(temp1);
							}
							else
							{
								TF = true;
								break;
							}
						}

						if ((*p->second).second.WaitingQueue.isEmpty() && TF == false)
						{
							if (tstart == t)
							{
								map<string, map<string, map<string, Device, comparatorless>::iterator>>::iterator temp2;
								if ((temp2 = DynamicList.find((*p->second).first.substr(0, (*p->second).first.find_first_of('+')))) != DynamicList.end())
								{
									temp2->second.insert(make_pair((*p->second).first, p->second));
								}
							}
							for (auto t = temp.begin(); t != temp.end(); t = temp.erase(t))
							{
								(*p->second).second.WaitingQueue.Insert(*t);
							}
						}
						else
						{
							TempQueue.Insert(temp1);
							TempMap.insert(make_pair(p->second->first, temp));

						}
						continue;
					}
				}
				else
				{
					if (tstart == t)
					{
						if ((*p->second).second.isTakenUp == false)
						{
							if ((*p->second).second.ProductisProducing.empty() == true)
							{
								map<string, map<string, map<string, Device, comparatorless>::iterator>>::iterator temp2;
								if ((temp2 = DynamicList.find((*p->second).first.substr(0, (*p->second).first.find_first_of('+')))) != DynamicList.end())
								{
									temp2->second.insert(make_pair((*p->second).first, p->second));
								}
							}
						}
					}
				}
			}
		}
		while (TempQueue.isEmpty() == false)
		{
			CodePriorityNAT temp;
			TempQueue.RemoveTop(temp);
			map<string, Product, comparatorless>::iterator q = SetofProduct.find(temp.ProductCode);
			map<string, map<string, Device, comparatorless>::iterator>::iterator p = SetofDevice.find(q->second.DeviceID);
			map<string, Mouldstate>::iterator m = p->second->second.MouldOnDevice.find(temp.ProductCode.substr(0, temp.ProductCode.find_first_of('+')));

			if (m->second.isOn)// 从这里开始
			{
				q->second.flag = 1;
				p->second->second.ProductisProducing.insert(temp);
				SetofMould.find(m->second.MouldCode)->second.ProductIsProducing = temp.ProductCode;   ////
				p->second->second.isTakenUp = true;
				if (tstart == t)
				{
					if (NoStartInsert.find(p->second->first.substr(0, p->second->first.find_first_of('+'))) != NoStartInsert.end())
					{
						q->second.time = tend - tstart;
					}
					else
					{
						q->second.time = Insert.begin()->second[end].InsertTime - t;
					}
				}
				else
				{
					DynamicList[p->second->first.substr(0, p->second->first.find_first_of('+'))].erase(p->second->first);;
				}
				map<string, multiset<CodePriorityNAT>>::iterator temp1;
				temp1 = TempMap.find(q->second.DeviceID);
				if (temp1->second.empty() == false)
				{
					for (multiset<CodePriorityNAT>::iterator t = temp1->second.begin(); t != temp1->second.end(); t = temp1->second.erase(t))
						p->second->second.WaitingQueue.Insert(*t);
				}
				TempMap.erase(temp1);
			}
			else
			{
				map<string, mould, comparatorless>::iterator v = SetofMould.find(m->second.MouldCode);
				bool ismeet = true;
				for (map<string, bool>::iterator s = v->second.AsPartComponent.begin(); s != v->second.AsPartComponent.end(); ++s)
				{
					if (s->second == false)
					{
						map<string, Component>::iterator f = SetofComponent.find(s->first);
						map<string, mould, comparatorless>::iterator w = SetofMould.find(f->second.ComponentCorrespondingMouldClass + '+' + f->second.MouldSerialNumber);
						if (w->second.MouldState == true && w->second.ProductIsProducing != "")
						{
							ismeet = false;
							break;
						}
					}
				}
				if (ismeet == true)
				{
					v->second.MouldState = true;
					v->second.ProductIsProducing = temp.ProductCode;
					m->second.isOn = true;
					for (map<string, bool>::iterator s = v->second.AsPartComponent.begin(); s != v->second.AsPartComponent.end(); ++s)
					{
						if (s->second == false)
						{
							map<string, Component>::iterator f = SetofComponent.find(s->first);
							map<string, mould, comparatorless>::iterator w = SetofMould.find(f->second.ComponentCorrespondingMouldClass + '+' + f->second.MouldSerialNumber);
							if (w->second.MouldState == true)
							{
								w->second.ProductIsProducing = "";
								w->second.MouldState = false;
								SetofDevice.find(w->second.DeviceBelongTo)->second->second.MouldOnDevice[w->first.substr(0, w->first.find_first_of('+'))].isOn = false;
							}

							w->second.AsPartComponent[f->first] = false;
							f->second.ComponentCorrespondingMouldClass = v->first.substr(0, v->first.find_first_of('+'));
							f->second.MouldSerialNumber = v->first.substr(v->first.find_first_of('+') + 1);
							v->second.AsPartComponent[f->first] = true;
						}
					}
					q->second.flag = 1;
					p->second->second.ProductisProducing.insert(temp);
					p->second->second.isTakenUp = true;

					if (tstart == t)
					{
						if (NoStartInsert.find(p->first.substr(0, p->first.find_first_of('+'))) != NoStartInsert.end())
						{
							q->second.time = tend - tstart;
						}
						else
						{
							q->second.time = Insert.begin()->second[end].InsertTime - t;
						}
					}
					else
					{
						DynamicList[p->first.substr(0, p->first.find_first_of('+'))].erase(p->first);
					}

					map<string, multiset<CodePriorityNAT>>::iterator temp1;

					temp1 = TempMap.find(q->second.DeviceID);
					if (temp1->second.empty() == false)
					{
						for (multiset<CodePriorityNAT>::iterator t = temp1->second.begin(); t != temp1->second.end(); t = temp1->second.erase(t))
							p->second->second.WaitingQueue.Insert(*t);
					}
					TempMap.erase(temp1);
				}
				else
				{
					TempMap[q->second.DeviceID].insert(temp);
					if (!p->second->second.WaitingQueue.isEmpty())
					{
						CodePriorityNAT temp1;
						bool TF = false;
						while ((*p).second->second.WaitingQueue.isEmpty() == false)
						{
							(*p).second->second.WaitingQueue.RemoveTop(temp1);
							map<string, Product, comparatorless>::iterator s = SetofProduct.find(temp1.ProductCode);
							map<string, vector<DeviceCategoryAndNAT>>::iterator v = Product::ProductCategoryWorkingProcedure.find(temp1.ProductCode.substr(0, temp1.ProductCode.find_first_of('+')));
							if (temp1.NaturalAgingTime != -1 && (*v).second[(*s).second.index - 1].NAT != temp1.NaturalAgingTime)
							{
								TempMap[q->second.DeviceID].insert(temp1);
							}
							else
							{
								TF = true;
								break;
							}
						}

						if (!(*p).second->second.WaitingQueue.isEmpty() || TF)
						{
							TempQueue.Insert(temp1);
							continue;
						}
					}

					if (tstart == t)
					{
						map<string, map<string, map<string, Device, comparatorless>::iterator>>::iterator temp2;
						if ((temp2 = DynamicList.find((*p).first.substr(0, (*p).first.find_first_of('+')))) != DynamicList.end())
						{
							temp2->second.insert(make_pair((*p).first, p->second));
						}
					}

					for (auto x = TempMap[q->second.DeviceID].begin(); x != TempMap[q->second.DeviceID].end(); x = TempMap[q->second.DeviceID].erase(x))
					{
						(*p).second->second.WaitingQueue.Insert(*x);
					}
					TempMap.erase(q->second.DeviceID);
				}
			}
		}
		for (map<string, map<string, map<string, Device, comparatorless>::iterator>>::iterator p = DynamicList.begin(); p != DynamicList.end(); )
		{
			if (p->second.empty() == true)
				p = DynamicList.erase(p);
			else
				++p;
		}
	}

	if (t != tstart)
	{
		map<string, DeviceState>::iterator before = StateQuery.begin();
		for (map<string, pair<map<string, Device, comparatorless>::iterator, map<string, Device, comparatorless>::iterator>>::iterator p = NoStartInsert.begin(); p != NoStartInsert.end(); ++p)
		{
			while (p->first != before->first.substr(0, before->first.find_first_of('+')))
			{
				++before;
			}

			map<string, DeviceState>::iterator after = before;
			while (after != StateQuery.end() && p->first == after->first.substr(0, after->first.find_first_of('+')))
			{
				++after;
			}

			map<string, DeviceState>::iterator q = before;
			for (map<string, Device, comparatorless>::iterator m = p->second.first; m != p->second.second; ++m, ++q)
			{
				if (q->second.flag == true)
				{
					q->second.T1 += tend - tstart;
				}

				if (m->second.ProductisProducing.empty() == false)
				{
					map<string, Product, comparatorless>::iterator temp = SetofProduct.find(m->second.ProductisProducing.begin()->ProductCode);
					temp->second.time += tend - tstart;
				}
			}
			before = after;
		}
	}
}

shared_ptr<map<string, map<string, map<string, Device, comparatorless>::iterator>>> ProductionCycleComputing::ProductInsertHandle(map<string, multiset<CodePriorityNAT>> &ProductInsert, const int &tstart, const int &t)
{
	if (ProductInsert.empty() == false)
	{
		shared_ptr<map<string, map<string, map<string, Device, comparatorless>::iterator>>> insertinfo;
		if (tstart != t)
		{
			insertinfo = shared_ptr<map<string, map<string, map<string, Device, comparatorless>::iterator>>>(new map<string, map<string, map<string, Device, comparatorless>::iterator>>);
		}

		for (map<string, multiset<CodePriorityNAT>>::iterator p = ProductInsert.begin(); p != ProductInsert.end(); ++p)
		{
			pair<map<string, Device, comparatorless>::iterator, map<string, Device, comparatorless>::iterator> tempIterator = SetofDevice.equal_range(p->first);
			struct IteratorAndScheme
			{
				int InsertScheme;   //设备等待时间计算方案
				map<string, Device, comparatorless>::iterator DeviceIterator;  //指向设备的迭代器
				IteratorAndScheme(map<string, Device, comparatorless>::iterator It, int I) :DeviceIterator(It), InsertScheme(I) {}
			};
			vector<IteratorAndScheme> QueueIndex;
			for (map<string, Device, comparatorless>::iterator q = tempIterator.first; q != tempIterator.second; ++q)
			{
				if (StateQuery[q->first].flag == true)
				{
					if (StateQuery[q->first].sign == 0)
					{
						if (StateQuery[q->first].T1 + GCDTime == RepairTimeOfDevice[p->first].T1)
						{
							QueueIndex.push_back(IteratorAndScheme(q, 1));
						}
						else
						{
							QueueIndex.push_back(IteratorAndScheme(q, 2));
						}
					}
					else
					{
						if (StateQuery[q->first].T1 + GCDTime == RepairTimeOfDevice[p->first].T2)
						{
							QueueIndex.push_back(IteratorAndScheme(q, 1));
						}
						else
						{
							QueueIndex.push_back(IteratorAndScheme(q, 2));
						}
					}
				}
				else
				{
					QueueIndex.push_back(IteratorAndScheme(q, 1));
				}
			}

			map<int, IncompleteOptimization> CurrentOptimization;  //关键字队列编号
			struct PriorityInsertionTime
			{
				vector<int> ProductTime;  //存放给定产品按优先级插入各队列后的等待时间
				vector<IteratorAndScheme>::size_type index; //产品所在的等待时间最小的队列序号
				int tmin; //最小等待时间
			};
			map<string, PriorityInsertionTime> ProductQueueWaitingTime;//关键字产品编号
			int tmax;  //映射表CurrentOptimization中各队列排在末端产品等待时间最大值
			vector<IteratorAndScheme>::size_type index;  //每一轮循环开始时为上一轮循环最优化插入的产品插入的队列编号
			Priority_Queue::iterator it; //每一轮循环开始时为上一轮循环最优化插入产品插入队列中指向该产品的迭代器
			vector<IteratorAndScheme>::size_type indexmax; //每一轮循环开始时为上一轮循环CurrentOptimization中各队列排在末端产品中等待时间最长的所在队列编号
			Priority_Queue::iterator itmax;  //每一轮循环开始时为指向上一轮循环CurrentOptimization中各队列排在末端产品中等待时间最长的产品所在位置的后一位置的迭代器
			multiset<CodePriorityNAT>::size_type count = p->second.size();

			map<string, map<string, map<string, Device, comparatorless>::iterator>>::iterator currentinsert;
			while (p->second.empty() == false)
			{
				if (p->second.size() == count)
				{
					vector<IteratorAndScheme>::size_type indextemp;
					Priority_Queue::iterator iteratortemp;
					string codetemp;
					int tmin;
					multiset<CodePriorityNAT>::iterator qmin;
					for (multiset<CodePriorityNAT>::iterator q = p->second.begin(); q != p->second.end(); ++q)
					{
						int Tmint;
						vector<IteratorAndScheme>::size_type Indext;
						Priority_Queue::iterator Iteratort;
						map<string, PriorityInsertionTime>::iterator temp1 = ProductQueueWaitingTime.insert(make_pair(q->ProductCode, PriorityInsertionTime())).first;
						for (vector<IteratorAndScheme>::size_type i = 0; i < QueueIndex.size(); ++i)
						{
							Priority_Queue::iterator temp = QueueIndex[i].DeviceIterator->second.WaitingQueue.Insert(*q).second;
							int waittime = WaitingTimeComputing(temp, QueueIndex[i].DeviceIterator, QueueIndex[i].InsertScheme);
							if (i == 0)
							{
								Tmint = waittime;
								Indext = i;
								Iteratort = temp;
								++Iteratort;
							}
							else
							{
								if (waittime < Tmint)
								{
									Tmint = waittime;
									Indext = i;
									Iteratort = temp;
									++Iteratort;
								}
							}
							QueueIndex[i].DeviceIterator->second.WaitingQueue.erase(temp);
							temp1->second.ProductTime.push_back(waittime);
						}
						temp1->second.index = Indext;
						temp1->second.tmin = Tmint;
						if (q == p->second.begin())
						{
							qmin = q;
							tmin = Tmint;
							codetemp = q->ProductCode;
							indextemp = Indext;
							iteratortemp = Iteratort;
						}
						else
						{
							if (Tmint < tmin)
							{
								qmin = q;
								tmin = Tmint;
								codetemp = q->ProductCode;
								indextemp = Indext;
								iteratortemp = Iteratort;
							}
						}
					}

					tmax = tmin;
					it = QueueIndex[indextemp].DeviceIterator->second.WaitingQueue.insert(iteratortemp, *qmin);
					if (t != tstart)
					{
						currentinsert = insertinfo->insert(make_pair(p->first, map<string, map<string, Device, comparatorless>::iterator>())).first;
						currentinsert->second.insert(make_pair(QueueIndex[indextemp].DeviceIterator->first, QueueIndex[indextemp].DeviceIterator));
					}
					SetofProduct.insert(make_pair(qmin->ProductCode, Product(QueueIndex[indextemp].DeviceIterator->first, 0, 0, 0)));
					index = indextemp;
					indexmax = indextemp;
					itmax = iteratortemp;
					CurrentOptimization.insert(make_pair(indextemp, IncompleteOptimization(codetemp, --iteratortemp, tmin)));
					ProductQueueWaitingTime.erase(codetemp);
					p->second.erase(qmin);
				}
				else
				{
					if (p->second.size() != 1)
					{
						Priority_Queue::iterator iteratorop;
						Priority_Queue::iterator iteratormaxop;
						vector<IteratorAndScheme>::size_type indexmaxop;
						vector<IteratorAndScheme>::size_type indexop;
						multiset<CodePriorityNAT>::iterator qop;
						int tmaxop;
						for (multiset<CodePriorityNAT>::iterator q = p->second.begin(); q != p->second.end(); ++q)
						{
							Priority_Queue::iterator iteratort;
							Priority_Queue::iterator iteratormaxt;
							vector<IteratorAndScheme>::size_type indexmaxt;
							int tmaxt;

							iteratort = QueueIndex[index].DeviceIterator->second.WaitingQueue.Insert(*q).second;
							if (PositionRelation(iteratort, it, QueueIndex[index].DeviceIterator->second.WaitingQueue) == 1)
							{
								int temp = TimeIncrementComputing(iteratort, it, QueueIndex[index].DeviceIterator, QueueIndex[index].InsertScheme);
								ProductQueueWaitingTime[q->ProductCode].ProductTime[index] += temp;

								if (ProductQueueWaitingTime[q->ProductCode].index == index)
								{
									int mintime;
									vector<int>::size_type minindex;
									for (vector<int>::size_type i = 0; i != ProductQueueWaitingTime[q->ProductCode].ProductTime.size(); ++i)
									{
										if (i == 0)
										{
											mintime = ProductQueueWaitingTime[q->ProductCode].ProductTime[i];
											minindex = i;
										}
										else
										{
											if (ProductQueueWaitingTime[q->ProductCode].ProductTime[i] < mintime)
											{
												mintime = ProductQueueWaitingTime[q->ProductCode].ProductTime[i];
												minindex = i;
											}
										}
									}
									ProductQueueWaitingTime[q->ProductCode].index = minindex;         //这里无符号类型不匹配,注意,可以采用使用迭代器的改进方案
									ProductQueueWaitingTime[q->ProductCode].tmin = mintime;
								}
							}
							QueueIndex[index].DeviceIterator->second.WaitingQueue.erase(iteratort);

							Priority_Queue::iterator tempinsert = QueueIndex[ProductQueueWaitingTime[q->ProductCode].index].DeviceIterator->second.WaitingQueue.Insert(*q).second;
							if (CurrentOptimization.find(ProductQueueWaitingTime[q->ProductCode].index) == CurrentOptimization.end())
							{
								Priority_Queue::iterator after = tempinsert;
								++after;
								if (ProductQueueWaitingTime[q->ProductCode].tmin > tmax)
								{
									tmaxt = ProductQueueWaitingTime[q->ProductCode].tmin;
									iteratort = after;
									iteratormaxt = after;
									indexmaxt = ProductQueueWaitingTime[q->ProductCode].index;
								}
								else
								{
									tmaxt = tmax;
									iteratort = after;
									iteratormaxt = itmax;
									indexmaxt = indexmax;
								}
							}
							else
							{
								Priority_Queue::iterator after = tempinsert;
								++after;
								Priority_Queue::iterator indexit = CurrentOptimization[ProductQueueWaitingTime[q->ProductCode].index].It;
								if (PositionRelation(tempinsert, indexit, QueueIndex[ProductQueueWaitingTime[q->ProductCode].index].DeviceIterator->second.WaitingQueue) == 1)
								{
									if (ProductQueueWaitingTime[q->ProductCode].tmin > tmax)
									{
										tmaxt = ProductQueueWaitingTime[q->ProductCode].tmin;
										iteratort = after;
										iteratormaxt = after;
										indexmaxt = ProductQueueWaitingTime[q->ProductCode].index;
									}
									else
									{
										tmaxt = tmax;
										iteratort = after;
										iteratormaxt = itmax;
										indexmaxt = indexmax;
									}
								}
								else
								{
									int DeltaTime = TimeIncrementComputing(indexit, tempinsert, QueueIndex[ProductQueueWaitingTime[q->ProductCode].index].DeviceIterator, QueueIndex[ProductQueueWaitingTime[q->ProductCode].index].InsertScheme);
									int WaitTime = CurrentOptimization[ProductQueueWaitingTime[q->ProductCode].index].WaitTime;
									if (WaitTime + DeltaTime > tmax)
									{
										tmaxt = WaitTime + DeltaTime;
										iteratort = after;
										iteratormaxt = CurrentOptimization[ProductQueueWaitingTime[q->ProductCode].index].It;
										++iteratormaxt;
										indexmaxt = ProductQueueWaitingTime[q->ProductCode].index;
									}
									else
									{
										tmaxt = tmax;
										iteratort = after;
										iteratormaxt = itmax;
										indexmaxt = indexmax;
									}
								}
							}

							if (q == p->second.begin())
							{
								tmaxop = tmaxt;
								iteratormaxop = iteratormaxt;
								iteratorop = iteratort;
								indexmaxop = indexmaxt;
								indexop = ProductQueueWaitingTime[q->ProductCode].index;
								qop = q;
							}
							else
							{
								if (tmaxt < tmaxop)
								{
									tmaxop = tmaxt;
									iteratormaxop = iteratormaxt;
									iteratorop = iteratort;
									indexmaxop = indexmaxt;
									indexop = ProductQueueWaitingTime[q->ProductCode].index;
									qop = q;
								}
							}
							QueueIndex[ProductQueueWaitingTime[q->ProductCode].index].DeviceIterator->second.WaitingQueue.erase(tempinsert);
						}

						iteratorop = QueueIndex[indexop].DeviceIterator->second.WaitingQueue.insert(iteratorop, *qop);
						if (t != tstart)
						{
							currentinsert->second.insert(make_pair(QueueIndex[indexop].DeviceIterator->first, QueueIndex[indexop].DeviceIterator));
						}
						SetofProduct.insert(make_pair(qop->ProductCode, Product(QueueIndex[indexop].DeviceIterator->first, 0, 0, 0)));
						if (CurrentOptimization.find(indexop) != CurrentOptimization.end())
						{
							if (PositionRelation(iteratorop, CurrentOptimization[indexop].It, QueueIndex[indexop].DeviceIterator->second.WaitingQueue) == 0)
							{
								int TimeCorrrect = TimeIncrementComputing(CurrentOptimization[indexop].It, iteratorop, QueueIndex[indexop].DeviceIterator, QueueIndex[indexop].InsertScheme);
								CurrentOptimization[indexop].WaitTime += TimeCorrrect;
							}
							else
							{
								CurrentOptimization[indexop].ProductCode = qop->ProductCode;
								CurrentOptimization[indexop].It = iteratorop;
								CurrentOptimization[indexop].WaitTime = ProductQueueWaitingTime[qop->ProductCode].tmin;
							}
						}
						else
						{
							CurrentOptimization.insert(make_pair(indexop, IncompleteOptimization(qop->ProductCode, iteratorop, ProductQueueWaitingTime[qop->ProductCode].tmin)));
						}
						tmax = tmaxop;
						index = indexop;
						it = iteratorop;
						indexmax = indexmaxop;
						itmax = iteratormaxop;

						ProductQueueWaitingTime.erase(qop->ProductCode);
						p->second.erase(qop);
					}
					else
					{
						multiset<CodePriorityNAT>::iterator q = p->second.begin();
							Priority_Queue::iterator iteratort = QueueIndex[index].DeviceIterator->second.WaitingQueue.Insert(*q).second;
						if (PositionRelation(iteratort, it, QueueIndex[index].DeviceIterator->second.WaitingQueue) == 1)
						{
							int temp = TimeIncrementComputing(iteratort, it, QueueIndex[index].DeviceIterator, QueueIndex[index].InsertScheme);
							ProductQueueWaitingTime[q->ProductCode].ProductTime[index] += temp;

							if (ProductQueueWaitingTime[q->ProductCode].index == index)
							{
								int mintime;
								vector<int>::size_type minindex;
								for (vector<int>::size_type i = 0; i != ProductQueueWaitingTime[q->ProductCode].ProductTime.size(); ++i)
								{
									if (i == 0)
									{
										mintime = ProductQueueWaitingTime[q->ProductCode].ProductTime[i];
										minindex = i;
									}
									else
									{
										if (ProductQueueWaitingTime[q->ProductCode].ProductTime[i] < mintime)
										{
											mintime = ProductQueueWaitingTime[q->ProductCode].ProductTime[i];
											minindex = i;
										}
									}
								}
								ProductQueueWaitingTime[q->ProductCode].index = minindex;         //这里无符号类型不匹配,注意,可以采用使用迭代器的改进方案
								ProductQueueWaitingTime[q->ProductCode].tmin = mintime;
							}
						}
						QueueIndex[index].DeviceIterator->second.WaitingQueue.erase(iteratort);
						QueueIndex[ProductQueueWaitingTime[q->ProductCode].index].DeviceIterator->second.WaitingQueue.Insert(*q);
						if (t != tstart)
						{
							currentinsert->second.insert(make_pair(QueueIndex[ProductQueueWaitingTime[q->ProductCode].index].DeviceIterator->first, QueueIndex[ProductQueueWaitingTime[q->ProductCode].index].DeviceIterator));
						}
						SetofProduct.insert(make_pair(q->ProductCode, Product(QueueIndex[ProductQueueWaitingTime[q->ProductCode].index].DeviceIterator->first, 0, 0, 0)));
						p->second.clear();
					}
				}
			}
		}
		return insertinfo;
	}
	return shared_ptr<map<string, map<string, map<string, Device, comparatorless>::iterator>>>(nullptr);
}

int ProductionCycleComputing::ProductionCycleCalculation()
{
	clock_t starttime = clock();
	int t = 0;
	while (true)
	{
		//此处检测并处理新产品插入
		bool TF = false; //第一轮循环
		while (NewProductInsert(t, TF) != false)
		{
			if (TF == false)
				TF = true;
		}

		map<string, multiset<CodePriorityNAT>> TempMap; //设备编号和temp集的映射关系
		Priority_Queue TempQueue;  //存放进入设备生产的候选产品的优先级队列
		for (map<string, Device, comparatorless>::iterator p = SetofDevice.begin(); p != SetofDevice.end(); ++p)
		{
			if (!(*p).second.ProductisProducing.empty())
			{
				set<CodePriorityNAT>::iterator q=(*p).second.ProductisProducing.begin();
				map<string, Product, comparatorless>::iterator m = SetofProduct.find((*q).ProductCode);
				(*m).second.time += GCDTime;
			}
			else 
			{
				if (!(*p).second.WaitingQueue.isEmpty())
				{
					if ((*p).second.isTakenUp == false)
					{
						multiset<CodePriorityNAT> temp;
						CodePriorityNAT temp1;
						TF = false;
						while ((*p).second.WaitingQueue.isEmpty() == false)
						{
							(*p).second.WaitingQueue.RemoveTop(temp1);
							map<string, Product, comparatorless>::iterator q = SetofProduct.find(temp1.ProductCode);
							map<string, vector<DeviceCategoryAndNAT>>::iterator t = Product::ProductCategoryWorkingProcedure.find(temp1.ProductCode.substr(0, temp1.ProductCode.find_first_of('+')));
							if (temp1.NaturalAgingTime != -1 && (*t).second[(*q).second.index-1].NAT != temp1.NaturalAgingTime)
							{
								temp1.NaturalAgingTime += GCDTime;
								temp.insert(temp1);
							}
							else
							{
								TF = true;
								break;
							}
						}

						if ((*p).second.WaitingQueue.isEmpty() && TF == false)
						{
							for (auto t = temp.begin(); t != temp.end(); t=temp.erase(t))
							{
								(*p).second.WaitingQueue.Insert(*t);
							}
						}
						else
						{
							TempQueue.Insert(temp1);
							TempMap.insert(make_pair(p->first, temp));

						}
						continue;
					}
				}
			}
			if ((*p).second.WaitingQueue.isEmpty() == false)
			{
				for (Priority_Queue::iterator q = p->second.WaitingQueue.begin(); q != p->second.WaitingQueue.end(); ++q)
				{
					if (q->NaturalAgingTime != -1 && q->NaturalAgingTime != Product::ProductCategoryWorkingProcedure.find(q->ProductCode.substr(0, q->ProductCode.find_first_of('+')))->second[SetofProduct.find(q->ProductCode)->second.index - 1].NAT)
						q->NaturalAgingTime += GCDTime;
				}
			}
			
		}
		while (TempQueue.isEmpty() == false)
		{
			CodePriorityNAT temp;
			TempQueue.RemoveTop(temp);
			map<string, Product, comparatorless>::iterator q = SetofProduct.find(temp.ProductCode);
			map<string, Device, comparatorless>::iterator p = SetofDevice.find(q->second.DeviceID);
			map<string, Mouldstate>::iterator m = p->second.MouldOnDevice.find(temp.ProductCode.substr(0, temp.ProductCode.find_first_of('+')));

			if (m->second.isOn)
			{
				q->second.time = GCDTime;
				q->second.flag = 1;
				SetofMould.find(m->second.MouldCode)->second.ProductIsProducing = temp.ProductCode; ////
				p->second.ProductisProducing.insert(temp);
				map<string, multiset<CodePriorityNAT>>::iterator temp1;

				if (p->second.WaitingQueue.isEmpty() == false)
				{
					for (Priority_Queue::iterator s = p->second.WaitingQueue.begin(); s != p->second.WaitingQueue.end(); ++s)
					{
						if (s->NaturalAgingTime != -1 && s->NaturalAgingTime != Product::ProductCategoryWorkingProcedure.find(s->ProductCode.substr(0, s->ProductCode.find_first_of('+')))->second[SetofProduct.find(s->ProductCode)->second.index - 1].NAT)
							s->NaturalAgingTime += GCDTime;
					}
				}

				temp1 = TempMap.find(q->second.DeviceID);
				if (temp1->second.empty() == false)
				{
					for (multiset<CodePriorityNAT>::iterator t = temp1->second.begin(); t != temp1->second.end(); t= temp1->second.erase(t))
						p->second.WaitingQueue.Insert(*t);
				}
				TempMap.erase(temp1);
			}
			else
			{
				map<string, mould, comparatorless>::iterator t1 = SetofMould.find(m->second.MouldCode);
				bool ismeet = true;
				for (map<string, bool>::iterator s = t1->second.AsPartComponent.begin(); s != t1->second.AsPartComponent.end(); ++s)
				{
					if (s->second == false)
					{
						map<string, Component>::iterator f = SetofComponent.find(s->first);
						map<string, mould, comparatorless>::iterator w = SetofMould.find(f->second.ComponentCorrespondingMouldClass +'+'+ f->second.MouldSerialNumber);
						if (w->second.MouldState == true && w->second.ProductIsProducing != "")
						{
							ismeet = false;
							break;
						}
					}
				}
				if (ismeet == true)
				{
					t1->second.MouldState = true;
					t1->second.ProductIsProducing = temp.ProductCode;
					m->second.isOn = true;
					for (map<string, bool>::iterator s = t1->second.AsPartComponent.begin(); s != t1->second.AsPartComponent.end(); ++s)
					{
						if (s->second == false)
						{
							map<string, Component>::iterator f = SetofComponent.find(s->first);
							map<string, mould, comparatorless>::iterator w = SetofMould.find(f->second.ComponentCorrespondingMouldClass + '+' + f->second.MouldSerialNumber);
							if (w->second.MouldState == true)
							{
								w->second.ProductIsProducing = "";
								w->second.MouldState = false;
								SetofDevice.find(w->second.DeviceBelongTo)->second.MouldOnDevice[w->first.substr(0, w->first.find_first_of('+'))].isOn = false;
							}

							w->second.AsPartComponent[f->first] = false;
							f->second.ComponentCorrespondingMouldClass = t1->first.substr(0, t1->first.find_first_of('+'));
							f->second.MouldSerialNumber = t1->first.substr(t1->first.find_first_of('+') + 1);
							t1->second.AsPartComponent[f->first] = true;   
						}
					}
					q->second.time = GCDTime;
					q->second.flag = 1;
					p->second.ProductisProducing.insert(temp);
					map<string, multiset<CodePriorityNAT>>::iterator temp1;

					if (p->second.WaitingQueue.isEmpty() == false)
					{
						for (Priority_Queue::iterator s = p->second.WaitingQueue.begin(); s != p->second.WaitingQueue.end(); ++s)
						{
							if (s->NaturalAgingTime != -1 && s->NaturalAgingTime != Product::ProductCategoryWorkingProcedure.find(s->ProductCode.substr(0, s->ProductCode.find_first_of('+')))->second[SetofProduct.find(s->ProductCode)->second.index - 1].NAT)
								s->NaturalAgingTime += GCDTime;
						}
					}

					temp1 = TempMap.find(q->second.DeviceID);
					if (temp1->second.empty() == false)
					{
						for (multiset<CodePriorityNAT>::iterator t = temp1->second.begin(); t != temp1->second.end(); t = temp1->second.erase(t))
							p->second.WaitingQueue.Insert(*t);
					}
					TempMap.erase(temp1);
				}
				else
				{
					TempMap[q->second.DeviceID].insert(temp);
					if (!p->second.WaitingQueue.isEmpty())
					{
						CodePriorityNAT temp1;
						bool TF = false;
						while ((*p).second.WaitingQueue.isEmpty() == false)
						{
							(*p).second.WaitingQueue.RemoveTop(temp1);
							map<string, Product, comparatorless>::iterator s = SetofProduct.find(temp1.ProductCode);
							map<string, vector<DeviceCategoryAndNAT>>::iterator v = Product::ProductCategoryWorkingProcedure.find(temp1.ProductCode.substr(0, temp1.ProductCode.find_first_of('+')));
							if (temp1.NaturalAgingTime != -1 && (*v).second[(*s).second.index - 1].NAT != temp1.NaturalAgingTime)
							{
								temp1.NaturalAgingTime += GCDTime;
								TempMap[q->second.DeviceID].insert(temp1);
							}
							else
							{
								TF = true;
								break;
							}
						}

						if (!(*p).second.WaitingQueue.isEmpty() || TF)
						{
							TempQueue.Insert(temp1);
							continue;
						}
					}

					for (auto x = TempMap[q->second.DeviceID].begin(); x != TempMap[q->second.DeviceID].end(); x = TempMap[q->second.DeviceID].erase(x))
					{
						(*p).second.WaitingQueue.Insert(*x);
					}
					TempMap.erase(q->second.DeviceID);
				}
			}
		}

		map<string, multiset<CodePriorityNAT>> ProductInsert;  //设备类别和待加入所有该类别设备的等待队列的所有产品的集合的映射关系
		for (map<string, Product, comparatorless>::iterator p = SetofProduct.begin(); p != SetofProduct.end(); )
		{
			if (p->second.flag == 1)
			{
				map<string, Device, comparatorless>::iterator q = SetofDevice.find(p->second.DeviceID);
				map<string, map<string, ProductionTimeAndPriority>>::iterator t1 = q->second.DeviceCategorySet.find(p->second.DeviceID.substr(0, p->second.DeviceID.find_first_of('+')));
				map<string, ProductionTimeAndPriority>::iterator s = t1->second.find(p->first.substr(0, p->first.find_first_of('+')));
				map<int, PrepareTimeProduceTime>::iterator m = s->second.ProductionTime.find(p->second.index);

				int temp;
				if (q->second.LatestProductCategory != p->first.substr(0, p->first.find_first_of('+')))
				{
					temp = m->second.PrepareTime;
				}
				else
				{
					temp = 0;
				}

				if (m->second.ProduceTime + temp == p->second.time)
				{
					map<string, vector<DeviceCategoryAndNAT>>::iterator v = p->second.ProductCategoryWorkingProcedure.find(p->first.substr(0, p->first.find_first_of('+')));
					map<string, Mouldstate>::iterator m = q->second.MouldOnDevice.find(p->first.substr(0, p->first.find_first_of('+')));   ////
					map<string, mould, comparatorless>::iterator t1 = SetofMould.find(m->second.MouldCode);
					t1->second.ProductIsProducing = "";     ////

					if (p->second.index == v->second.size() - 1)
					{
						q->second.ProductisProducing.clear();
						q->second.LatestProductCategory = p->first.substr(0, p->first.find_first_of('+'));
						if (v->second.back().NAT == 0)  //p所指产品生产完成,生产完成时间为t+GCDTime
						{
							p = SetofProduct.erase(p);
						}       
						else
						{
							p->second.flag = 0;     //p所指产品在最后一台设备上生产完毕后有自然时效时间,把产品状态更新为特殊值表示产品处于自然时效阶段
							p->second.time = 0;
							p->second.DeviceID = "";
							++p;
						}
						continue;
					}
					else
					{
						string Cat = v->second[p->second.index + 1].DeviceCategoryCode;
						map<string, multiset<CodePriorityNAT>>::iterator c = ProductInsert.find(Cat);
						CodePriorityNAT temp(p->first, Device::DeviceCategorySet.find(Cat)->second[q->second.ProductisProducing.begin()->ProductCode.substr(0, q->second.ProductisProducing.begin()->ProductCode.find_first_of('+'))].Priority, 0);
						if (c == ProductInsert.end())
						{
							multiset<CodePriorityNAT> temp1{ temp };
							ProductInsert.insert(make_pair(Cat, temp1));
						}
						else
						{
							c->second.insert(temp);
						}

						p->second.flag = 0;
						p->second.time = 0;
						++(p->second.index);
						q->second.ProductisProducing.clear();    //该产品在当前设备上生产完成时间为t+GCDTime
					}
					q->second.LatestProductCategory = p->first.substr(0, p->first.find_first_of('+'));
					++p;
				}
				else
				{
					++p;
				}

			}
			else
			{
				if (p->second.DeviceID == "")
				{
					p->second.time += GCDTime;
					if (p->second.ProductCategoryWorkingProcedure.find(p->first.substr(0, p->first.find_first_of('+')))->second.back().NAT == p->second.time)
					{
						p = SetofProduct.erase(p);
						continue;
					}
				}
				++p;
			}
		}

		if (SetofProduct.empty())
		{
			//所有产品生产完成,时间为t+GCDTime,结束
			clock_t endtime = clock();
			cout << "生产时间计算耗费时间为" << (endtime - starttime) << "毫秒" << endl;
			return t+GCDTime;
		}

		//此处利用ProductInsert执行多重优化插入,先判映射表是否为空
		if (ProductInsert.empty() == false)
		{
			for (map<string, multiset<CodePriorityNAT>>::iterator p = ProductInsert.begin(); p != ProductInsert.end(); ++p)
			{
				pair<map<string, Device, comparatorless>::iterator, map<string, Device, comparatorless>::iterator> tempIterator = SetofDevice.equal_range(p->first);
				struct IteratorAndScheme
				{
					int InsertScheme;   //设备等待时间计算方案
					map<string, Device, comparatorless>::iterator DeviceIterator;  //指向设备的迭代器
					IteratorAndScheme(map<string, Device, comparatorless>::iterator It, int I) :DeviceIterator(It), InsertScheme(I) {}
				};
				vector<IteratorAndScheme> QueueIndex;
				for (map<string, Device, comparatorless>::iterator q = tempIterator.first; q != tempIterator.second; ++q)
				{
					if (StateQuery[q->first].flag == true)
					{
						if (StateQuery[q->first].sign == 0)
						{
							if (StateQuery[q->first].T1 + GCDTime == RepairTimeOfDevice[p->first].T1)
							{
								QueueIndex.push_back(IteratorAndScheme(q, 1));
							}
							else
							{
								QueueIndex.push_back(IteratorAndScheme(q, 2));
							}
						}
						else
						{
							if (StateQuery[q->first].T1 + GCDTime == RepairTimeOfDevice[p->first].T2)
							{
								QueueIndex.push_back(IteratorAndScheme(q, 1));
							}
							else
							{
								QueueIndex.push_back(IteratorAndScheme(q, 2));
							}
						}
					}
					else
					{
						QueueIndex.push_back(IteratorAndScheme(q, 1));
					}
				}

				map<int, IncompleteOptimization> CurrentOptimization;  //关键字队列编号
				struct PriorityInsertionTime
				{
					vector<int> ProductTime;  //存放给定产品按优先级插入各队列后的等待时间
					vector<IteratorAndScheme>::size_type index; //产品所在的等待时间最小的队列序号
					int tmin; //最小等待时间
				};
				map<string, PriorityInsertionTime> ProductQueueWaitingTime;//关键字产品编号
				int tmax;  //映射表CurrentOptimization中各队列排在末端产品等待时间最大值
				vector<IteratorAndScheme>::size_type index;  //每一轮循环开始时为上一轮循环最优化插入的产品插入的队列编号
				Priority_Queue::iterator it; //每一轮循环开始时为上一轮循环最优化插入产品插入队列中指向该产品的迭代器
				vector<IteratorAndScheme>::size_type indexmax; //每一轮循环开始时为上一轮循环CurrentOptimization中各队列排在末端产品中等待时间最长的所在队列编号
				Priority_Queue::iterator itmax;  //每一轮循环开始时为指向上一轮循环CurrentOptimization中各队列排在末端产品中等待时间最长的产品所在位置的后一位置的迭代器
				multiset<CodePriorityNAT>::size_type count = p->second.size();
				while (p->second.empty() == false)
				{
					if (p->second.size() == count)
					{
						vector<IteratorAndScheme>::size_type indextemp;
						Priority_Queue::iterator iteratortemp;
						string codetemp;
						int tmin;
						multiset<CodePriorityNAT>::iterator qmin;
						for (multiset<CodePriorityNAT>::iterator q = p->second.begin(); q != p->second.end(); ++q)
						{
							int Tmint;
							vector<IteratorAndScheme>::size_type Indext;
							Priority_Queue::iterator Iteratort;
							map<string, PriorityInsertionTime>::iterator temp1 = ProductQueueWaitingTime.insert(make_pair(q->ProductCode, PriorityInsertionTime())).first;
							for (vector<IteratorAndScheme>::size_type i = 0; i < QueueIndex.size(); ++i)
							{
								Priority_Queue::iterator temp = QueueIndex[i].DeviceIterator->second.WaitingQueue.Insert(*q).second;
								int waittime = WaitingTimeComputing(temp, QueueIndex[i].DeviceIterator, QueueIndex[i].InsertScheme);
								if (i == 0)
								{
									Tmint = waittime;
									Indext = i;
									Iteratort = temp;
									++Iteratort;
								}
								else
								{
									if (waittime < Tmint)
									{
										Tmint = waittime;
										Indext = i;
										Iteratort = temp;
										++Iteratort;
									}
								}
								QueueIndex[i].DeviceIterator->second.WaitingQueue.erase(temp);
								temp1->second.ProductTime.push_back(waittime);
							}
							temp1->second.index = Indext;
							temp1->second.tmin = Tmint;
							if (q == p->second.begin())
							{
								qmin = q;
								tmin = Tmint;
								codetemp = q->ProductCode;
								indextemp = Indext;
								iteratortemp = Iteratort;
							}
							else
							{
								if (Tmint < tmin)
								{
									qmin = q;
									tmin = Tmint;
									codetemp = q->ProductCode;
									indextemp = Indext;
									iteratortemp = Iteratort;
								}
							}
						}
						tmax = tmin;
						it = QueueIndex[indextemp].DeviceIterator->second.WaitingQueue.insert(iteratortemp, *qmin);
						map<string, Product, comparatorless>::iterator s = SetofProduct.find(qmin->ProductCode);
						s->second.DeviceID = QueueIndex[indextemp].DeviceIterator->first;
						index = indextemp;
						indexmax = indextemp;
						itmax = iteratortemp;
						CurrentOptimization.insert(make_pair(indextemp, IncompleteOptimization(codetemp, --iteratortemp, tmin)));
						ProductQueueWaitingTime.erase(codetemp);
						p->second.erase(qmin);
					}
					else
					{
						if (p->second.size() != 1)
						{
							Priority_Queue::iterator iteratorop;
							Priority_Queue::iterator iteratormaxop;
							vector<IteratorAndScheme>::size_type indexmaxop;
							vector<IteratorAndScheme>::size_type indexop;
							multiset<CodePriorityNAT>::iterator qop;
							int tmaxop;
							for (multiset<CodePriorityNAT>::iterator q = p->second.begin(); q != p->second.end(); ++q)
							{
								Priority_Queue::iterator iteratort;
								Priority_Queue::iterator iteratormaxt;
								vector<IteratorAndScheme>::size_type indexmaxt;
								int tmaxt;

								iteratort = QueueIndex[index].DeviceIterator->second.WaitingQueue.Insert(*q).second;   
								if (PositionRelation(iteratort, it, QueueIndex[index].DeviceIterator->second.WaitingQueue) == 1)
								{
									int temp = TimeIncrementComputing(iteratort, it, QueueIndex[index].DeviceIterator, QueueIndex[index].InsertScheme);
									ProductQueueWaitingTime[q->ProductCode].ProductTime[index] += temp;

									if (ProductQueueWaitingTime[q->ProductCode].index == index)
									{
										int mintime;
										vector<int>::size_type minindex;
										for (vector<int>::size_type i = 0; i != ProductQueueWaitingTime[q->ProductCode].ProductTime.size(); ++i)
										{
											if (i == 0)
											{
												mintime = ProductQueueWaitingTime[q->ProductCode].ProductTime[i];
												minindex = i;
											}
											else
											{
												if (ProductQueueWaitingTime[q->ProductCode].ProductTime[i] < mintime)
												{
													mintime = ProductQueueWaitingTime[q->ProductCode].ProductTime[i];
													minindex = i;
												}
											}
										}
										ProductQueueWaitingTime[q->ProductCode].index = minindex;         //这里无符号类型不匹配,注意,可以采用使用迭代器的改进方案
										ProductQueueWaitingTime[q->ProductCode].tmin = mintime;
									}
								}
								QueueIndex[index].DeviceIterator->second.WaitingQueue.erase(iteratort);
								
								Priority_Queue::iterator tempinsert = QueueIndex[ProductQueueWaitingTime[q->ProductCode].index].DeviceIterator->second.WaitingQueue.Insert(*q).second;
								if (CurrentOptimization.find(ProductQueueWaitingTime[q->ProductCode].index) == CurrentOptimization.end())
								{
									Priority_Queue::iterator after = tempinsert;
									++after;
									if (ProductQueueWaitingTime[q->ProductCode].tmin > tmax)
									{
										tmaxt = ProductQueueWaitingTime[q->ProductCode].tmin;
										iteratort = after;
										iteratormaxt = after;
										indexmaxt = ProductQueueWaitingTime[q->ProductCode].index;
									}
									else
									{
										tmaxt = tmax;
										iteratort = after;
										iteratormaxt = itmax;
										indexmaxt = indexmax;
									}
								}
								else
								{
									Priority_Queue::iterator after = tempinsert;
									++after;
									Priority_Queue::iterator indexit = CurrentOptimization[ProductQueueWaitingTime[q->ProductCode].index].It;
									if (PositionRelation(tempinsert, indexit, QueueIndex[ProductQueueWaitingTime[q->ProductCode].index].DeviceIterator->second.WaitingQueue) == 1)
									{
										if (ProductQueueWaitingTime[q->ProductCode].tmin > tmax)
										{
											tmaxt = ProductQueueWaitingTime[q->ProductCode].tmin;
											iteratort = after;
											iteratormaxt = after;
											indexmaxt = ProductQueueWaitingTime[q->ProductCode].index;
										}
										else
										{
											tmaxt = tmax;
											iteratort = after;
											iteratormaxt = itmax;
											indexmaxt = indexmax;
										}
									}
									else
									{
										int DeltaTime = TimeIncrementComputing(indexit, tempinsert, QueueIndex[ProductQueueWaitingTime[q->ProductCode].index].DeviceIterator, QueueIndex[ProductQueueWaitingTime[q->ProductCode].index].InsertScheme);
										int WaitTime = CurrentOptimization[ProductQueueWaitingTime[q->ProductCode].index].WaitTime;
										if (WaitTime + DeltaTime > tmax)
										{
											tmaxt = WaitTime + DeltaTime;
											iteratort = after;
											iteratormaxt = CurrentOptimization[ProductQueueWaitingTime[q->ProductCode].index].It;
											++iteratormaxt;
											indexmaxt = ProductQueueWaitingTime[q->ProductCode].index;
										}
										else
										{
											tmaxt = tmax;
											iteratort = after;
											iteratormaxt = itmax;
											indexmaxt = indexmax;
										}
									}
								}

								if (q == p->second.begin())
								{
									tmaxop = tmaxt;
									iteratormaxop = iteratormaxt;
									iteratorop = iteratort;
									indexmaxop = indexmaxt;
									indexop = ProductQueueWaitingTime[q->ProductCode].index;
									qop = q;
								}
								else
								{
									if (tmaxt < tmaxop)
									{
										tmaxop = tmaxt;
										iteratormaxop = iteratormaxt;
										iteratorop = iteratort;
										indexmaxop = indexmaxt;
										indexop = ProductQueueWaitingTime[q->ProductCode].index;
										qop = q;
									}
								}
								QueueIndex[ProductQueueWaitingTime[q->ProductCode].index].DeviceIterator->second.WaitingQueue.erase(tempinsert);
							}

							iteratorop = QueueIndex[indexop].DeviceIterator->second.WaitingQueue.insert(iteratorop, *qop);
							map<string, Product, comparatorless>::iterator s = SetofProduct.find(qop->ProductCode);
							s->second.DeviceID = QueueIndex[indexop].DeviceIterator->first;
							if (CurrentOptimization.find(indexop) != CurrentOptimization.end())
							{
								if (PositionRelation(iteratorop, CurrentOptimization[indexop].It, QueueIndex[indexop].DeviceIterator->second.WaitingQueue) == 0)
								{
									int TimeCorrrect = TimeIncrementComputing(CurrentOptimization[indexop].It, iteratorop, QueueIndex[indexop].DeviceIterator, QueueIndex[indexop].InsertScheme);
									CurrentOptimization[indexop].WaitTime += TimeCorrrect;
								}
								else
								{
									CurrentOptimization[indexop].ProductCode = qop->ProductCode;
									CurrentOptimization[indexop].It = iteratorop;
									CurrentOptimization[indexop].WaitTime = ProductQueueWaitingTime[qop->ProductCode].tmin;
								}
							}
							else
							{
								CurrentOptimization.insert(make_pair(indexop, IncompleteOptimization(qop->ProductCode, iteratorop, ProductQueueWaitingTime[qop->ProductCode].tmin)));
							}
							tmax = tmaxop;
							index = indexop;
							it = iteratorop;
							indexmax = indexmaxop;
							itmax = iteratormaxop;

							ProductQueueWaitingTime.erase(qop->ProductCode);
							p->second.erase(qop);
						}
						else
						{ 
							multiset<CodePriorityNAT>::iterator q = p->second.begin();
							Priority_Queue::iterator iteratort = QueueIndex[index].DeviceIterator->second.WaitingQueue.Insert(*q).second;
							if (PositionRelation(iteratort, it, QueueIndex[index].DeviceIterator->second.WaitingQueue) == 1)
							{
								int temp = TimeIncrementComputing(iteratort, it, QueueIndex[index].DeviceIterator, QueueIndex[index].InsertScheme);
								ProductQueueWaitingTime[q->ProductCode].ProductTime[index] += temp;

								if (ProductQueueWaitingTime[q->ProductCode].index == index)
								{
									int mintime;
									vector<int>::size_type minindex;
									for (vector<int>::size_type i = 0; i != ProductQueueWaitingTime[q->ProductCode].ProductTime.size(); ++i)
									{
										if (i == 0)
										{
											mintime = ProductQueueWaitingTime[q->ProductCode].ProductTime[i];
											minindex = i;
										}
										else
										{
											if (ProductQueueWaitingTime[q->ProductCode].ProductTime[i] < mintime)
											{
												mintime = ProductQueueWaitingTime[q->ProductCode].ProductTime[i];
												minindex = i;
											}
										}
									}
									ProductQueueWaitingTime[q->ProductCode].index = minindex;         //这里无符号类型不匹配,注意,可以采用使用迭代器的改进方案
									ProductQueueWaitingTime[q->ProductCode].tmin = mintime;
								}
							}
							QueueIndex[index].DeviceIterator->second.WaitingQueue.erase(iteratort);
							QueueIndex[ProductQueueWaitingTime[q->ProductCode].index].DeviceIterator->second.WaitingQueue.Insert(*q);
							map<string, Product, comparatorless>::iterator s = SetofProduct.find(q->ProductCode);
							s->second.DeviceID = QueueIndex[ProductQueueWaitingTime[q->ProductCode].index].DeviceIterator->first;
							p->second.clear();
						}
					}
				}    
			}
		}
		//设备状态调整
		map<string, RepairTime>::iterator p = RepairTimeOfDevice.begin();
		map<string, Device, comparatorless>::iterator s = SetofDevice.begin();
		for (map<string, DeviceState>::iterator q = StateQuery.begin(); q != StateQuery.end(); )
		{
			if (q->first.substr(0, q->first.find_first_of('+')) == p->first)
			{
				if (q->second.flag == true)
				{
					q->second.T1 += GCDTime;
					if (q->second.sign == 0)
					{
						if (p->second.T1 == q->second.T1)         
						{
							q->second.T1 = 0;
						    q->second.flag = false;
							s->second.isTakenUp = false;
						}
					}
					else
					{
						if (q->second.T1 == p->second.T2)
						{
							q->second.T1 = 0;
							q->second.flag = false;
							s->second.isTakenUp = false;
						}
					}
				}
				else
				{
					if (q->second.sign == 0)
					{
						if (q->second.T2 == p->second.DeltaT1 || (q->second.T2 += GCDTime, q->second.T2 == p->second.DeltaT1))
						{
							if (s->second.ProductisProducing.empty() == true)
							{
								q->second.T2 = 0;
								q->second.sign = 1;
								q->second.flag = true;
								s->second.isTakenUp = true;
							}
						}
						else
						{
							if (s->second.ProductisProducing.empty() == true)
								s->second.isTakenUp = false;
						}
					}
					else
					{
						if (q->second.T2 == p->second.DeltaT2 || (q->second.T2 += GCDTime, q->second.T2 == p->second.DeltaT2))
						{
							if (s->second.ProductisProducing.empty() == true)
							{
								q->second.T2 = 0;
								q->second.sign = 0;
								q->second.flag = true;
								s->second.isTakenUp = true;
							}
						}
						else
						{
							if (s->second.ProductisProducing.empty() == true)
								s->second.isTakenUp = false;
						}
					}
				}
				++q;
				++s;
			}
			else
			{
				++p;
			}
		}
		t += GCDTime;
	}
}

int ProductionCycleComputing::WaitingTimeComputing(Priority_Queue::iterator it, map<string, Device, comparatorless>::iterator m, int InsertScheme)
{
	Priority_Queue::iterator before = m->second.WaitingQueue.begin();
	Priority_Queue::iterator after = before;
	++after;
	int t = 0;
	if (InsertScheme == 1)
	{
		if (m->second.ProductisProducing.empty() == false)
		{
			const string &temp = (*m->second.ProductisProducing.begin()).ProductCode;
			PrepareTimeProduceTime &temp1 = m->second.DeviceCategorySet.find(m->first.substr(0, m->first.find_first_of('+')))->second[temp.substr(0, temp.find_first_of('+'))].ProductionTime[SetofProduct.find(temp)->second.index];
			t+=temp1.ProduceTime;
			if (temp.substr(0, temp.find_first_of('+')) != m->second.LatestProductCategory)
				t += temp1.PrepareTime;
			t -= SetofProduct.find(temp)->second.time;
		}
	}
	else
	{
		if (StateQuery[m->first].sign == 0)
			t += RepairTimeOfDevice[m->first.substr(0, m->first.find_first_of('+'))].T1;
		else
			t += RepairTimeOfDevice[m->first.substr(0, m->first.find_first_of('+'))].T2;
		t -= StateQuery[m->first].T1;
	}

	if (before->ProductCode != it->ProductCode)
	{
		string temp = before->ProductCode;
		PrepareTimeProduceTime &temp1 = m->second.DeviceCategorySet.find(m->first.substr(0, m->first.find_first_of('+')))->second[temp.substr(0, temp.find_first_of('+'))].ProductionTime[SetofProduct.find(temp)->second.index];
		t += temp1.ProduceTime;
		if (InsertScheme == 1 && m->second.ProductisProducing.empty() == false)
		{
			if (temp.substr(0, temp.find_first_of('+')) != (*m->second.ProductisProducing.begin()).ProductCode.substr(0, (*m->second.ProductisProducing.begin()).ProductCode.find_first_of('+')))
				t += temp1.PrepareTime;
		}
		else
		{
			if (temp.substr(0, temp.find_first_of('+')) != m->second.LatestProductCategory)
				t += temp1.PrepareTime;
		}

		if (after->ProductCode != it->ProductCode)
		{
			while (after != it)
			{
				string temp = after->ProductCode;
				PrepareTimeProduceTime &temp1 = m->second.DeviceCategorySet.find(m->first.substr(0, m->first.find_first_of('+')))->second[temp.substr(0, temp.find_first_of('+'))].ProductionTime[SetofProduct.find(temp)->second.index];
				t += temp1.ProduceTime;

				if (temp.substr(0, temp.find_first_of('+'))!=before->ProductCode.substr(0, before->ProductCode.find_first_of('+')))
					t+=temp1.PrepareTime;

				++after;
				++before;
			}
		}
	}
	return t;
}

int ProductionCycleComputing::TimeIncrementComputing(const Priority_Queue::iterator &BeCalculated, const Priority_Queue::iterator &CauseCalculating, map<string, Device, comparatorless>::iterator m, int InsertScheme)
{
	int t = 0; //修正等待时间的时间增量
	string code;
	if (CauseCalculating == m->second.WaitingQueue.begin())
	{
		if (InsertScheme == 1 && m->second.ProductisProducing.empty() == false)
		{
			code = (*m->second.ProductisProducing.begin()).ProductCode;
			code = code.substr(0, code.find_first_of('+'));
		}
		else
		{
			code = m->second.LatestProductCategory;
		}
	}
	else
	{
		Priority_Queue::iterator temp = CauseCalculating;
		--temp;
		code = temp->ProductCode.substr(0, temp->ProductCode.find_first_of('+'));
	}

	{
		PrepareTimeProduceTime temp1;
		if (SetofProduct.find(CauseCalculating->ProductCode) != SetofProduct.end())
		{
			temp1 = m->second.DeviceCategorySet.find(m->first.substr(0, m->first.find_first_of('+')))->second[CauseCalculating->ProductCode.substr(0, CauseCalculating->ProductCode.find_first_of('+'))].ProductionTime[SetofProduct.find(CauseCalculating->ProductCode)->second.index];
		}
		else
		{
			temp1 = m->second.DeviceCategorySet.find(m->first.substr(0, m->first.find_first_of('+')))->second[CauseCalculating->ProductCode.substr(0, CauseCalculating->ProductCode.find_first_of('+'))].ProductionTime[0];
		}
		t += temp1.ProduceTime;
		if (CauseCalculating->ProductCode.substr(0, CauseCalculating->ProductCode.find_first_of('+')) != code)
			t += temp1.PrepareTime;
	}

	Priority_Queue::iterator temp = CauseCalculating;
	++temp;
	if (temp != BeCalculated)
	{
		string Cat1 = temp->ProductCode.substr(0, temp->ProductCode.find_first_of('+'));
		string Cat2 = CauseCalculating->ProductCode.substr(0, CauseCalculating->ProductCode.find_first_of('+'));
		if ((Cat1 != code && Cat1 == Cat2) || (Cat1 == code && Cat1 != Cat2))
		{
			PrepareTimeProduceTime &temp1 = m->second.DeviceCategorySet.find(m->first.substr(0, m->first.find_first_of('+')))->second[Cat1].ProductionTime[SetofProduct.find(temp->ProductCode)->second.index];
			if (Cat1 != code && Cat1 == Cat2)
			{
				t -= temp1.PrepareTime;
			}
			else
			{
				t += temp1.PrepareTime;
			}
		}
	}
	return t;
}

int ProductionCycleComputing::PositionRelation(const Priority_Queue::iterator &it1, const Priority_Queue::iterator &it2, Priority_Queue &m) //判断优先级队列中it1是在it2之后还是在之前,在之前返回0,在之后返回1,调用者保证it1,it2不等
{
	Priority_Queue::iterator before = it1;
	Priority_Queue::iterator after = it1;

	while (before != it2 && (after != m.end() && after != it2))
	{
		if (before != m.begin())
		{
			--before;
		}	
		++after;
	}

	if (before == it2)
	{
		return 1;
	}
	else
	{
		if (after == m.end())
		{
			if (it2 == m.end())
				return 0;
			else
				return 1;
		}
		else
		{
			return 0;
		}
	}
}

int main()
{
	Product::initProductCategoryWorkingProcedure();
	Device::initDeviceCategorySet();
	ProductionCycleComputing CalculationExample;
	cout << "生产完成时间为" << CalculationExample.ProductionCycleCalculation() << endl;
	system("Pause");
	return 0;
}