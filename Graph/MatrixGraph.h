#pragma once

#include "GraphBase.h"

/*邻接矩阵图的父类，实现了一些邻接矩阵图的共有操作*/
template<class T, class W, W NullValue>
class MatrixGraph :public GraphBase<T, W, NullValue>
{
public:

	using typename GraphBase<T, W, NullValue>::VertexPos;
	using typename GraphBase<T, W, NullValue>::OnPassVertex;
	using typename GraphBase<T, W, NullValue>::OnPassEdge;

	/*插入一条边，对于无向图，两个参数顺序无所谓 O(1)*/
	virtual void InsertEdge(VertexPos from, VertexPos to, const W& weight)override;

	/*查找从from到to是否存在边 O(1)*/
	virtual bool ExistEdge(VertexPos from, VertexPos to)const override;

	/*删除边 O(1)*/
	virtual void RemoveEdge(VertexPos from, VertexPos to)override;

	/*遍历出邻接点 O(VertexNum)*/
	virtual void ForeachOutNeighbor(VertexPos v, OnPassVertex func)const override;

	/*遍历入邻接点 O(VertexNum)*/
	virtual void ForeachInNeighbor(VertexPos v, OnPassVertex func)const override;

	/*收缩内存占用，在每次移除一个顶点时并不会真的释放内存，从而提高再次插入顶点效率，详见@vector
	如果不需要插入顶点或者需要收缩内存，请调用这个
	内部调用@vector.shrink_to_fit*/
	virtual void Shrink_To_Fit() = 0;
};

template<class T, class W, W NullValue>
inline void MatrixGraph<T, W, NullValue>::InsertEdge(VertexPos from, VertexPos to, const W& weight)
{
	if (ExistEdge(from, to))
		return;
	++this->m_edgeNum;
	this->SetWeight(from, to, weight);
}

template<class T, class W, W NullValue>
inline bool MatrixGraph<T, W, NullValue>::ExistEdge(VertexPos from, VertexPos to) const
{
	return this->GetWeight(from, to) != NullValue;
}

template<class T, class W, W NullValue>
inline void MatrixGraph<T, W, NullValue>::RemoveEdge(VertexPos from, VertexPos to)
{
	if (!ExistEdge(from, to))
		return;
	--this->m_edgeNum;
	this->SetWeight(from, to, NullValue);
}

template<class T, class W, W NullValue>
inline void MatrixGraph<T, W, NullValue>::ForeachOutNeighbor(VertexPos v, OnPassVertex func) const
{
	for (VertexPos i = 0; i < this->m_vertexData.size(); ++i)
		if (ExistEdge(v, i))
			func(i);
}

template<class T, class W, W NullValue>
inline void MatrixGraph<T, W, NullValue>::ForeachInNeighbor(VertexPos v, OnPassVertex func) const
{
	for (VertexPos i = 0; i < this->m_vertexData.size(); ++i)
		if (this->ExistEdge(i, v))
			func(i);
}
