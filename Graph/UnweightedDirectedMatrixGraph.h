#pragma once

#include "WeightedDirectedMatrixGraph.h"

/*该类为W=char的特化，在vector中有较低的存储效率，但是使用效率较高，要使用存储效率较高的类请使用@UnweightedDirectedMatrixGraph_Tiny*/
template<class T>
class UnweightedDirectedMatrixGraph : public WeightedDirectedMatrixGraph<T, char, 0>
{
public:

	using typename GraphBase<T, char, 0>::VertexType;
	using typename GraphBase<T, char, 0>::WeightType;
	using typename GraphBase<T, char, 0>::VertexPosType;
	using typename GraphBase<T, char, 0>::OnPassVertex;
	using typename GraphBase<T, char, 0>::OnPassEdge;

	/*插入一个边 O(1)*/
	virtual void InsertEdge(VertexPosType from, VertexPosType to, const char& weight = 1) override;

};

template<class T>
inline void UnweightedDirectedMatrixGraph<T>::InsertEdge(VertexPosType from, VertexPosType to, const char& weight)
{
	WeightedDirectedMatrixGraph<T, char, 0>::InsertEdge(from, to, weight && 1);
}


/*该类为W=bool的特化，在vector中有较高的存储效率，但是使用效率较低
注：在vector存储bool时，并不是按照byte存储的而是按照bits打包存储，所以无法获取某一个bool的地址,详见vectot<bool>*/
template<class T>
class UnweightedDirectedMatrixGraph_Tiny : public WeightedDirectedMatrixGraph<T, bool, false>
{
public:

	using typename GraphBase<T, bool, false>::VertexType;
	using typename GraphBase<T, bool, false>::WeightType;
	using typename GraphBase<T, bool, false>::VertexPosType;
	using typename GraphBase<T, bool, false>::OnPassVertex;
	using typename GraphBase<T, bool, false>::OnPassEdge;

	/*插入一个边 O(1)*/
	virtual void InsertEdge(VertexPosType from, VertexPosType to, const bool& weight = true) override;

	/*该数值为主要占用的准确数值*/
	virtual unsigned long long GetMemoryUsage()const override;

	virtual bool IsDirected()const;

	virtual bool IsWeighted()const;

};

template<class T>
inline void UnweightedDirectedMatrixGraph_Tiny<T>::InsertEdge(VertexPosType from, VertexPosType to, const bool& weight)
{
	WeightedDirectedMatrixGraph<T, bool, false>::InsertEdge(from, to, weight);
}

template<class T>
inline unsigned long long UnweightedDirectedMatrixGraph_Tiny<T>::GetMemoryUsage() const
{
	return (this->m_adjaMetrix.empty() ? 0 : this->m_adjaMetrix[0].capacity() * this->GetVertexNum() / 8) + (unsigned long long)sizeof(this->m_adjaMetrix);
}

template<class T>
inline bool UnweightedDirectedMatrixGraph_Tiny<T>::IsDirected() const
{
	return true;
}

template<class T>
inline bool UnweightedDirectedMatrixGraph_Tiny<T>::IsWeighted() const
{
	return false;
}
