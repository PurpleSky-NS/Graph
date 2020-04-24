#pragma once

#include "UnweightedDirectedLinkGraph.h"

template<class T, class E = _DefaultUnweightedEdgeType>
class UnweightedUndirectedLinkGraph :public UnweightedDirectedLinkGraph<T, E>
{
public:

	using typename GraphBase<T, bool, false>::VertexType;
	using typename GraphBase<T, bool, false>::WeightType;
	using typename UnweightedDirectedLinkGraph<T, E>::EdgeType;
	using typename GraphBase<T, bool, false>::VertexPosType;
	using typename GraphBase<T, bool, false>::OnPassVertex;
	using typename GraphBase<T, bool, false>::OnPassEdge;

	/*插入或删除一条边 O(VertexEdgeNum)*/
	virtual void InsertEdge(VertexPosType v1, VertexPosType v2, const bool& weight = true) override;

	/*删除边 O(VertexEdgeNum)*/
	virtual void RemoveEdge(VertexPosType v1, VertexPosType v2) override;

	/*遍历入邻接点 O(EdgeNum)*/
	virtual void ForeachInNeighbor(VertexPosType v, OnPassVertex func)const;

	/*遍历所有边，回调函数第三个参数恒为true O(EdgeNum)*/
	virtual void ForeachEdge(OnPassEdge func)const override;

	/*获取完整邻接矩阵，二维的邻接矩阵会以行为单位，存储在一维线性表中 O(EdgeNum)*/
	virtual std::vector<bool> GetAdjacencyMatrix()const override;

	/*获取图的主要内存占用量(byte)，不包括顶点信息(无法精确测量)以及其容器等次要因素的占用量
	该占用量与权重类型以及该类的实现类的密切相关*/
	virtual unsigned long long GetMemoryUsage()const override;
};

template<class T, class E>
inline void UnweightedUndirectedLinkGraph<T, E>::InsertEdge(VertexPosType v1, VertexPosType v2, const bool& weight)
{
	size_t prevEdgeNum = this->m_edgeNum;
	UnweightedDirectedLinkGraph<T, E>::InsertEdge(v1, v2, weight); //调用父类插入边v1->v2
	if (prevEdgeNum == this->m_edgeNum || v1 == v2) //边的数量没改变，说明已经存在这条边了或者如果是环只用添加一条边即可
		return;
	UnweightedDirectedLinkGraph<T, E>::InsertEdge(v2, v1, weight); //因为是无向图所以再插v2->v1
	//因为插入或者删除了两次，所以把边的数量修正一下
	this->m_edgeNum += (this->m_edgeNum > prevEdgeNum ? -1 : 1);
}

template<class T, class E>
inline void UnweightedUndirectedLinkGraph<T, E>::RemoveEdge(VertexPosType v1, VertexPosType v2)
{
	//和插入同理
	size_t prevEdgeNum = this->m_edgeNum;
	UnweightedDirectedLinkGraph<T, E>::RemoveEdge(v1, v2); //调用父类删除边v1->v2
	if (prevEdgeNum == this->m_edgeNum || v1 == v2) //边的数量没改变，说明不存在这条边或者是个环[同InsertEdge]
		return;
	UnweightedDirectedLinkGraph<T, E>::RemoveEdge(v2, v1); //因为是无向图所以再删v2->v1
	//因为删除了两次，所以把边的数量修正一下
	++this->m_edgeNum;
}

template<class T, class E>
inline void UnweightedUndirectedLinkGraph<T, E>::ForeachInNeighbor(VertexPosType v, OnPassVertex func) const
{
	//对于无向图，出入相同
	UnweightedDirectedLinkGraph<T, E>::ForeachOutNeighbor(v, func);
}

template<class T, class E>
inline void UnweightedUndirectedLinkGraph<T, E>::ForeachEdge(OnPassEdge func) const
{
	for (VertexPosType v1 = 0; v1 < this->m_entry.size(); ++v1)
		for (E* e = this->m_entry[v1]; e != nullptr; e = e->next)
			if (v1 <= e->vertex)
				func(v1, e->vertex, true);
}

template<class T, class E>
inline std::vector<bool> UnweightedUndirectedLinkGraph<T, E>::GetAdjacencyMatrix() const
{
	std::vector<bool> adjaMetrix(this->m_vertexData.size() * this->m_vertexData.size(), false);
	ForeachEdge(
		[&](auto v1, auto v2, auto w)
		{
			adjaMetrix[v1 * this->GetVertexNum() + v2] = true;
			adjaMetrix[v2 * this->GetVertexNum() + v1] = true;
		});
	return adjaMetrix;
}

template<class T, class E>
inline unsigned long long UnweightedUndirectedLinkGraph<T, E>::GetMemoryUsage() const
{
	return (unsigned long long)this->m_entry.size() * sizeof(E*) + (unsigned long long)this->GetEdgeNum() * sizeof(E) * 2 + sizeof(this->m_entry);
}
