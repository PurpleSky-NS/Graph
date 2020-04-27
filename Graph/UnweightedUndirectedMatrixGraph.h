#pragma once

#include "WeightedUndirectedMatrixGraph.h"

/*该类为W=char的特化，在vector中有较低的存储效率，但是使用效率较高，要使用存储效率较高的类请使用@UnweightedUndirectedMatrixGraph_Tiny*/
template<class T>
class UnweightedUndirectedMatrixGraph : public WeightedUndirectedMatrixGraph<T, char, 0>
{
public:

	using typename GraphBase<T, char, 0>::VertexType;
	using typename GraphBase<T, char, 0>::WeightType;
	using typename GraphBase<T, char, 0>::VertexPosType;
	using typename GraphBase<T, char, 0>::OnPassVertex;
	using typename GraphBase<T, char, 0>::OnPassEdge;

	/*插入一个边 O(1)*/
	virtual void InsertEdge(VertexPosType v1, VertexPosType v2, const char& weight = 1) override;

	/*获取最小生成树(详见MST.h)，返回最小权值，最小生成树若为空则表示生成失败，模板参数为权值累加类型[默认为double]，采用Prim算法 O(VertexNum^2)*/
	template<class MST_PT = size_t, class MST_WT = unsigned long long>
	MST_Parent<MST_PT, MST_WT> GetMST()const;

	virtual bool IsWeighted()const;

};

template<class T>
inline void UnweightedUndirectedMatrixGraph<T>::InsertEdge(VertexPosType v1, VertexPosType v2, const char& weight)
{
	WeightedUndirectedMatrixGraph<T, char, 0>::InsertEdge(v1, v2, weight && 1);
}

template<class T>
inline bool UnweightedUndirectedMatrixGraph<T>::IsWeighted() const
{
	return false;
}

/*该类为W=bool的特化，在vector中有较高的存储效率，但是使用效率较低
注：在vector存储bool时，并不是按照byte存储的而是按照bits打包存储，所以无法获取某一个bool的地址,详见vectot<bool>*/
template<class T>
class UnweightedUndirectedMatrixGraph_Tiny : public WeightedUndirectedMatrixGraph<T, bool, false>
{
public:

	using typename GraphBase<T, bool, false>::VertexType;
	using typename GraphBase<T, bool, false>::WeightType;
	using typename GraphBase<T, bool, false>::VertexPosType;
	using typename GraphBase<T, bool, false>::OnPassVertex;
	using typename GraphBase<T, bool, false>::OnPassEdge;

	/*插入一个边 O(1)*/
	virtual void InsertEdge(VertexPosType v1, VertexPosType v2, const bool& weight = true) override;

	/*该数值为主要占用的准确数值*/
	virtual unsigned long long GetMemoryUsage()const override;

	/*获取最小生成树(详见MST.h)，返回最小权值，最小生成树若为空则表示生成失败，模板参数为权值累加类型[默认为double]，采用Prim算法 O(VertexNum^2)*/
	template<class MST_PT = size_t, class MST_WT = unsigned long long>
	MST_Parent<MST_PT, MST_WT> GetMST()const;

	virtual bool IsWeighted()const;

};

template<class T>
inline void UnweightedUndirectedMatrixGraph_Tiny<T>::InsertEdge(VertexPosType v1, VertexPosType v2, const bool& weight)
{
	WeightedUndirectedMatrixGraph<T, bool, false>::InsertEdge(v1, v2, weight);
}

template<class T>
inline unsigned long long UnweightedUndirectedMatrixGraph_Tiny<T>::GetMemoryUsage() const
{
	return (this->m_adjaMetrix.empty() ? 0 : (unsigned long long)this->m_adjaMetrix.capacity() / 8) + sizeof(this->m_adjaMetrix);
}

template<class T>
inline bool UnweightedUndirectedMatrixGraph_Tiny<T>::IsWeighted() const
{
	return false;
}

template<class T>
template<class MST_PT, class MST_WT>
inline MST_Parent<MST_PT, MST_WT> UnweightedUndirectedMatrixGraph<T>::GetMST() const
{
	return this->MatrixGraph<T, char, 0>::_GetMST<MST_PT, MST_WT>();
}

template<class T>
template<class MST_PT, class MST_WT>
inline MST_Parent<MST_PT, MST_WT> UnweightedUndirectedMatrixGraph_Tiny<T>::GetMST() const
{
	return this->MatrixGraph<T, bool, false>::_GetMST<MST_PT, MST_WT>();
}
