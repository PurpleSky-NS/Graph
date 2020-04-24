#pragma once

#include "GraphBase.h"

/*注意内存对齐*/
struct _DefaultUnweightedEdgeType
{
	_DefaultUnweightedEdgeType* next;	//指向下一个节点
	size_t vertex;						//定位顶点下标
};

/*无权有向图，模板参数W与NullValue不能修改
E为边节点类型，对于总体内存空间占用有很大影响，对于自定义边界点类型来说，其中必须有两个作用域：
	E  *next	//指向下一个边节点
	整形 vertex	//用来存储顶点下标
对于边节点类型来说，应该注意内存对齐问题，另外在x86与x64环境下指针所占空间也不同*/
template<class T, class E = _DefaultUnweightedEdgeType, class W = bool, W NullValue = false>
class UnweightedDirectedLinkGraph :public GraphBase<T, W, NullValue>
{
public:

	using typename GraphBase<T, W, NullValue>::VertexPos;
	using typename GraphBase<T, W, NullValue>::OnPassVertex;
	using typename GraphBase<T, W, NullValue>::OnPassEdge;

	using EdgeType = E;

	/*静态断言，检测类型E是否符合要求*/
	static_assert(std::is_same<decltype(E::next), E*>::value, "未定义字段名为[next]指向自己的指针");
	static_assert(std::is_integral<decltype(E::vertex)>::value, "未定义名为[vertex]的整形字段");
	static_assert(sizeof(E::vertex) <= sizeof(VertexPos), "[vertex]的整形字段过大");

	UnweightedDirectedLinkGraph() = default;
	virtual ~UnweightedDirectedLinkGraph();

	/*插入一个顶点 O(1)*/
	virtual void InsertVertex(const T& v) override;

	/*插入或删除一条边 O(VertexEdgeNum)*/
	virtual void InsertEdge(VertexPos from, VertexPos to, const W& weight = true) override;

	/*查找从from到to是否存在边 O(VertexEdgeNum)*/
	virtual bool ExistEdge(VertexPos from, VertexPos to)const override;

	/*获取从from到to的权重 无权图中为=ExistEdge O(VertexEdgeNum)*/
	virtual W GetWeight(VertexPos from, VertexPos to)const override;

	/*设置从from到to的权重 weight=NullValue删除该边，如果没有则添加 O(VertexEdgeNum)*/
	virtual void SetWeight(VertexPos from, VertexPos to, const W& weight)override;

	/*删除顶点，删完后下标会改变 O(EdgeNum)*/
	virtual void RemoveVertex(VertexPos v) override;

	/*删除边 O(VertexEdgeNum)*/
	virtual void RemoveEdge(VertexPos from, VertexPos to) override;

	/*遍历出邻接点 O(VertexEdgeNum)*/
	virtual void ForeachOutNeighbor(VertexPos v, OnPassVertex func)const;

	/*遍历入邻接点 O(EdgeNum)*/
	virtual void ForeachInNeighbor(VertexPos v, OnPassVertex func)const;

	/*遍历所有边，回调函数第三个参数恒为true O(EdgeNum)*/
	virtual void ForeachEdge(OnPassEdge func)const override;

	/*获取完整邻接矩阵，二维的邻接矩阵会以行为单位，存储在一维线性表中 O(EdgeNum)*/
	virtual std::vector<W> GetAdjacencyMatrix()const override;

	/*获取图的主要内存占用量(byte)，不包括顶点信息(无法精确测量)以及其容器等次要因素的占用量
	该占用量与权重类型以及该类的实现类的密切相关*/
	virtual unsigned long long GetMemoryUsage()const override;

protected:
	std::vector<E*> m_entry;

	/*构造一个节点*/
	E* CreateEdgeNode(VertexPos v);

	/*销毁一个节点*/
	void DestroyEdgeNode(E* e);

	/*获取边节点*/
	E* GetNode(VertexPos from, VertexPos to)const;

};

template<class T, class E, class W, W NullValue>
inline UnweightedDirectedLinkGraph<T, E, W, NullValue>::~UnweightedDirectedLinkGraph()
{
	E* edgeNode, * tmp;
	for (VertexPos i = 0; i < m_entry.size(); ++i)
	{
		edgeNode = m_entry[i];
		while (edgeNode != nullptr)
		{
			tmp = edgeNode;
			edgeNode = edgeNode->next;
			DestroyEdgeNode(tmp);
		}
	}
}

template<class T, class E, class W, W NullValue>
inline void UnweightedDirectedLinkGraph<T, E, W, NullValue>::InsertVertex(const T& v)
{
	this->m_vertexData.push_back(v);
	m_entry.push_back(nullptr);
}

template<class T, class E, class W, W NullValue>
inline void UnweightedDirectedLinkGraph<T, E, W, NullValue>::InsertEdge(VertexPos from, VertexPos to, const W& weight)
{
	if (!weight)
	{
		RemoveEdge(from, to);
		return;
	}

	//遍历出邻接点如果存在这条边就退出，否则直到末尾插入
	if (m_entry[from] == nullptr)
	{
		m_entry[from] = CreateEdgeNode(to);
		return;
	}
	E* edgeNode = m_entry[from];
	while (edgeNode->next != nullptr)
	{
		if ((VertexPos)edgeNode->vertex == to)
			return;
		edgeNode = edgeNode->next;
	}
	if ((VertexPos)edgeNode->vertex != to) //末尾插入
		edgeNode->next = CreateEdgeNode(to);
}

template<class T, class E, class W, W NullValue>
inline bool UnweightedDirectedLinkGraph<T, E, W, NullValue>::ExistEdge(VertexPos from, VertexPos to) const
{
	return this->GetNode(from, to) != nullptr;
}

template<class T, class E, class W, W NullValue>
inline W UnweightedDirectedLinkGraph<T, E, W, NullValue>::GetWeight(VertexPos from, VertexPos to) const
{
	return ExistEdge(from, to);
}

template<class T, class E, class W, W NullValue>
inline void UnweightedDirectedLinkGraph<T, E, W, NullValue>::SetWeight(VertexPos from, VertexPos to, const W& weight)
{
	if (weight == NullValue)
		RemoveEdge(from, to);
	else
		InsertEdge(from, to, weight);
}

template<class T, class E, class W, W NullValue>
inline void UnweightedDirectedLinkGraph<T, E, W, NullValue>::RemoveVertex(VertexPos v)
{
	E* edgeNode = m_entry[v], * tmp;
	while (edgeNode != nullptr) //删除该节点的邻接节点
	{
		tmp = edgeNode;
		edgeNode = edgeNode->next;
		DestroyEdgeNode(tmp);
	}
	m_entry.erase(m_entry.begin() + v);
	this->m_vertexData.erase(this->m_vertexData.begin() + v);

	/*需要遍历所有边，将所有记录下标>v的节点数据-1，将所有入邻接点(下标=v)删除*/
	for (VertexPos i = 0; i < m_entry.size(); ++i)
	{
		while (m_entry[i] != nullptr && (VertexPos)m_entry[i]->vertex == v)//如果头节点是v的入邻接点的话就删除头节点
		{
			tmp = m_entry[i];
			m_entry[i] = tmp->next;
			DestroyEdgeNode(tmp);
		}
		if (m_entry[i] == nullptr)//该列已经空了，可以找下一列了
			continue;

		//继续对该列其他节点操作，现在头节点不可能是v的入邻接点了
		if ((VertexPos)m_entry[i]->vertex > v) //判断头节点需不需要操作下标
			--m_entry[i]->vertex;

		for (E* e = m_entry[i]; e->next != nullptr;)
		{
			if ((VertexPos)e->next->vertex == v) //删除该节点
			{
				tmp = e->next;
				e->next = tmp->next;
				DestroyEdgeNode(tmp);
				continue;
			}
			else if ((VertexPos)e->next->vertex > v) //操作节点下标
				--e->next->vertex;
			e = e->next;
		}
	}
}

template<class T, class E, class W, W NullValue>
inline void UnweightedDirectedLinkGraph<T, E, W, NullValue>::RemoveEdge(VertexPos from, VertexPos to)
{
	if (m_entry[from] == nullptr)
		return;
	if (m_entry[from]->vertex == to)
	{
		E* newHead = m_entry[from]->next;
		DestroyEdgeNode(m_entry[from]);
		m_entry[from] = newHead;
		return;
	}
	E* edgeNode = m_entry[from];
	while (edgeNode->next != nullptr && edgeNode->next->vertex != to)
		edgeNode = edgeNode->next;
	if (edgeNode->next != nullptr)
	{
		E* desEdge = edgeNode->next;
		edgeNode->next = desEdge->next;
		DestroyEdgeNode(desEdge);
	}
}

template<class T, class E, class W, W NullValue>
inline void UnweightedDirectedLinkGraph<T, E, W, NullValue>::ForeachOutNeighbor(VertexPos v, OnPassVertex func) const
{
	for (E* e = m_entry[v]; e != nullptr; e = e->next)
		func((VertexPos)e->vertex);
}

template<class T, class E, class W, W NullValue>
inline void UnweightedDirectedLinkGraph<T, E, W, NullValue>::ForeachInNeighbor(VertexPos v, OnPassVertex func) const
{
	for (VertexPos i = 0; i < m_entry.size(); ++i)
		if (ExistEdge(i, v))
			func(i);
}

template<class T, class E, class W, W NullValue>
inline void UnweightedDirectedLinkGraph<T, E, W, NullValue>::ForeachEdge(OnPassEdge func) const
{
	for (VertexPos from = 0; from < m_entry.size(); ++from)
		for (E* e = m_entry[from]; e != nullptr; e = e->next)
			func(from, e->vertex, true);
}

template<class T, class E, class W, W NullValue>
inline std::vector<W> UnweightedDirectedLinkGraph<T, E, W, NullValue>::GetAdjacencyMatrix() const
{
	std::vector<W> adjaMetrix(this->m_vertexData.size() * this->m_vertexData.size(), false);
	ForeachEdge(
		[&](auto v1, auto v2, auto w)
		{
			adjaMetrix[v1 * this->GetVertexNum() + v2] = w;
		});
	return adjaMetrix;
}

template<class T, class E, class W, W NullValue>
inline unsigned long long UnweightedDirectedLinkGraph<T, E, W, NullValue>::GetMemoryUsage() const
{
	return m_entry.size() * sizeof(E*) + (unsigned long long)this->GetEdgeNum() * sizeof(E) + sizeof(m_entry);
}

template<class T, class E, class W, W NullValue>
inline E* UnweightedDirectedLinkGraph<T, E, W, NullValue>::CreateEdgeNode(VertexPos v)
{
	E* e = new E;
	e->vertex = (decltype(e->vertex))v;
	e->next = nullptr;
	++this->m_edgeNum;
	return e;
}

template<class T, class E, class W, W NullValue>
inline void UnweightedDirectedLinkGraph<T, E, W, NullValue>::DestroyEdgeNode(E* e)
{
	if (e == nullptr)
		return;
	delete e;
	--this->m_edgeNum;
}

template<class T, class E, class W, W NullValue>
inline E* UnweightedDirectedLinkGraph<T, E, W, NullValue>::GetNode(VertexPos from, VertexPos to)const
{
	E* edgeNode = m_entry[from];
	while (edgeNode != nullptr)
	{
		if ((VertexPos)edgeNode->vertex == to)
			return edgeNode;
		edgeNode = edgeNode->next;
	}
	return nullptr;
}
