#pragma once

#include "UnweightedDirectedLinkGraph.h"

/*注意内存对齐*/
template<class W>
struct _DefaultWeightedEdgeType
{
	_DefaultWeightedEdgeType* next;		//指向下一个节点
	size_t vertex;						//定位顶点下标
	W weight;							//权重
};

/*比起无权图，有全图边界点多了个weight域，边界点格式请参考@UnweightedDirectedLinkGraph
优化默认边结构体：
x86:按4字节内存对齐
	next -> 4 byte
	vertex : size_t	->	short		4 -> 2 byte
	weight : int	->	short		4 -> 2 byte
这样比默认的结构体就少了4字节
*/
template<class T, class W = int, W NullValue = -1, class E = _DefaultWeightedEdgeType<W>>
class WeightedDirectedLinkGraph :public UnweightedDirectedLinkGraph<T, E, W, NullValue>
{
public:

	using typename GraphBase<T, W, NullValue>::VertexType;
	using typename GraphBase<T, W, NullValue>::WeightType;
	using typename UnweightedDirectedLinkGraph<T, E, W, NullValue>::EdgeType;
	using typename GraphBase<T, W, NullValue>::VertexPosType;
	using typename GraphBase<T, W, NullValue>::OnPassVertex;
	using typename GraphBase<T, W, NullValue>::OnPassEdge;

	static_assert(std::is_same<W, decltype(E::weight)>::value, "无[weight]字段或者该字段类型与权重类型不符");

	/*插入或删除一条边，对于无向图，两个参数顺序无所谓 O(VertexEdgeNum)*/
	virtual void InsertEdge(VertexPosType from, VertexPosType to, const W& weight) override;

	/*获取从from到to的权重 无权图中为=ExistEdge O(VertexEdgeNum)*/
	virtual W GetWeight(VertexPosType from, VertexPosType to)const override;

	/*设置从from到to的权重 weight=NullValue删除该边，如果没有则添加 O(VertexEdgeNum)*/
	virtual void SetWeight(VertexPosType from, VertexPosType to, const W& weight)override;

	/*遍历所有边，回调函数第三个参数恒为true O(EdgeNum)*/
	virtual void ForeachEdge(OnPassEdge func)const override;

protected:

	/*构造一个节点*/
	E* CreateEdgeNode(VertexPosType v, const W& w);
};

template<class T, class W, W NullValue, class E>
inline void WeightedDirectedLinkGraph<T, W, NullValue, E>::InsertEdge(VertexPosType from, VertexPosType to, const W& weight)
{
	if (weight == NullValue)
	{
		this->RemoveEdge(from, to);
		return;
	}

	if (this->m_entry[from] == nullptr)
	{
		this->m_entry[from] = CreateEdgeNode(to, weight);
		return;
	}
	E* edgeNode = this->m_entry[from];
	while (edgeNode->next != nullptr)
	{
		if ((VertexPosType)edgeNode->vertex == to)
			return;
		edgeNode = edgeNode->next;
	}
	if ((VertexPosType)edgeNode->vertex != to)
		edgeNode->next = CreateEdgeNode(to, weight);
}

template<class T, class W, W NullValue, class E>
inline W WeightedDirectedLinkGraph<T, W, NullValue, E>::GetWeight(VertexPosType from, VertexPosType to) const
{
	E* e = this->GetNode(from, to);
	return (e == nullptr ? NullValue : e->weight);
}

template<class T, class W, W NullValue, class E>
inline void WeightedDirectedLinkGraph<T, W, NullValue, E>::SetWeight(VertexPosType from, VertexPosType to, const W& weight)
{
	if (weight == NullValue)
	{
		this->RemoveEdge(from, to);
		return;
	}
	/*查找该节点，如果没有就插入*/
	if (this->m_entry[from]->vertex == to)
	{
		this->m_entry[from]->weight = weight;
		return;
	}
	E* e;
	for (e = this->m_entry[from]; e->next != nullptr; e = e->next)
		if (e->next->vertex == to)
		{
			e->next->weight = weight;
			break;
		}
	if (e->next == nullptr)
		e->next = CreateEdgeNode(to, weight);
}

template<class T, class W, W NullValue, class E>
inline void WeightedDirectedLinkGraph<T, W, NullValue, E>::ForeachEdge(OnPassEdge func) const
{
	for (VertexPosType i = 0; i < this->m_entry.size(); ++i)
		for (E* e = this->m_entry[i]; e != nullptr; e = e->next)
			func(i, e->vertex, e->weight);
}

template<class T, class W, W NullValue, class E>
inline E* WeightedDirectedLinkGraph<T, W, NullValue, E>::CreateEdgeNode(VertexPosType v, const W& w)
{
	E* e = UnweightedDirectedLinkGraph<T, E, W, NullValue>::CreateEdgeNode(v);
	e->weight = w;
	return e;
}
