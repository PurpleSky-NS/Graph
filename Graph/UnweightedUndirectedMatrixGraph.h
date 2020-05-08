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

	virtual constexpr bool IsWeighted()const override;

};

template<class T>
inline void UnweightedUndirectedMatrixGraph<T>::InsertEdge(VertexPosType v1, VertexPosType v2, const char& weight)
{
	WeightedUndirectedMatrixGraph<T, char, 0>::InsertEdge(v1, v2, weight && 1);
}

template<class T>
inline constexpr bool UnweightedUndirectedMatrixGraph<T>::IsWeighted() const
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

	virtual constexpr bool IsWeighted()const override;

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
inline constexpr bool UnweightedUndirectedMatrixGraph_Tiny<T>::IsWeighted() const
{
	return false;
}
