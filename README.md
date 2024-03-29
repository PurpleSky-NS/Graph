# Graph
一个架构好的图数据结构库，包括邻接表和邻接矩阵两种实现方式，以及从有无权有无向分化出来的多个图的实例，可根据自己的需要挑选使用<br>
## 使用
该库使用了较多c++11特性，如lambda，static_assert，functional库，模板类型判断等
## 包含
包含Graph.h文件即可，或者根据自己的需要包含某一个图的实现类<br>
## 图的操作
大多数图的操作的时间复杂度都在实现该操作的接口注释<br>
- VertexNum为所有顶点数量<br>
- EdgeNum为所有边的数量<br>
- Ele为Element缩写，在邻接矩阵中表示所有元素(邻接矩阵存储的元素)的数量<br>
- VertexEdgeNum为邻接表中某顶点链接的所有边的数量，通常情况下该数值小于VertexNum<br>
## 提供图算法
* 深度优先遍历：DFS<br>
* 广度优先遍历：BFS<br>
* 最小生成树：MST<br>
* 最短路径：单源(SSSP)，多源(MSSP)<br>
## 所有类型与别名
* T(VertexType):模板类型，顶点的类型，所有顶点信息都会存储在一个vector中<br>
* W(WeightType):模板类型，权重的类型，影响存储效率的主要类型，必须是可算数类型<br>
* E(EdgeType):模板类型，只存在于邻接表实现的图中，为边节点类型<br>
* VertexPosType:顶点下标类型，是整数类型，表示一个顶点在vector中存储的下标，删除顶点后该数值可能会发生变动<br>
* OnPassVertex:遍历顶点的回调函数<br>
  原型为std::function<void(VertexPosType)><br>
  函数原型为void (*OnPassVertex)(VertexPosType pos)<br>
* OnPassEdge:遍历边的回调函数<br>
  原型为std::function<void(VertexPosType from, VertexPosType to, const W& weight)><br>
  函数原型为void (*OnPassEdge)(VertexPosType from, VertexPosType to, const W&)，在无权图中第三个参数恒为true<br>
## 说明
- GraphBase 该模板类为所有图实现类的基类<br>
- **(Weighted/Unweighted)(Directed/Undirected)(Matrix/Link)Graph**为实现类，分别为有无权重/有无向/邻接矩阵和邻接表实现<br>
- 稀疏图请用邻接表(Link)版本的实现类，稠密图请用邻接矩阵(Matrix)版本的实现类<br>
- XXXMatrix_Tiny 类为邻接矩阵使用vector\<bool>存储，而vector\<bool>会将8个bool打包成一个byte，按位存储<br>
  所以在顶点数量较多的时候会节省空间，提高了存储效率但是降低了使用效率，具体请搜索vector\<bool> <br>
  非Tiny版本使用的是vector\<char>来存储邻接矩阵<br>
- 请不要在有权图中将bool设置为权重的模板参数(这没有意义呀)，请使用无权图版本<br>
- 在邻接矩阵图的实现中，存储空间受模板中 权重类型(W) 影响很大，请尽量使用较小的类型<br>
- 在邻接表图的实现中，存储空间受模板中 边节点类型(E) 的影响很大，不用默认的边节点类型 *(_Default(Un)WeightedEdgeType)* 的话，最好自定义更小的类型，具体的定义方法在那两个类的注释中<br>
- 这里等有时间放一张类图用来说明架构时的继承关系
## MST
只有在无向图中才有的最小生成树算法，在邻接表图中使用Kruskal算法，在邻接矩阵中使用Prim算法<br>
  - 邻接矩阵图的MST算法会返回一个名为MST_Parent的类，该类中存储的是vector\<PT>，使用树的双亲表示法表示最小生成树<br>
  - 邻接表图的MST算法会返回一个名为MST_Edge的类，该类中存储的是vetoer\<Edge>，使用边集表示最小生成树<br>
  - MST_X类都做了一些简单的包装，而且不允许修改其中的内容
  - 在调用GetMST函数时可以修改默认模板类型，如GetMST<unsigned char,float>(g)，其中g必须是无向图(Undirected)
  - 关于MST_X类的模板：
    - PT :表示用什么整数类型存储顶点的下标（默认为size_t)
    - WT :表示用什么类型存储权值总和(默认为double或unsigned long long)
      (不直接用W类型是因为多个W相加可能会超出范围，所以提供一个用更大类型表示的方法)
    - W :权重类型，存储边时需要用到，会自动赋值，不需要用户关心
使用示例：
```c++
void TestMST()
{
	/*下面为两种图，邻接矩阵图(Matrix)和邻接表图(Link)*/
	//WeightedUndirectedMatrixGraph<string> g; //邻接矩阵图
	WeightedUndirectedLinkGraph<string> g; //邻接表图

	g.InsertVertex("v1");
	g.InsertVertex("v2");
	g.InsertVertex("v3");
	g.InsertVertex("v4");
	g.InsertVertex("v5");
	g.InsertVertex("v6");
	g.InsertVertex("v7");
	g.InsertEdge(0, 1, 2);
	g.InsertEdge(0, 3, 1);
	g.InsertEdge(1, 3, 3);
	g.InsertEdge(1, 4, 10);
	g.InsertEdge(2, 0, 4);
	g.InsertEdge(2, 5, 5);
	g.InsertEdge(3, 2, 2);
	g.InsertEdge(3, 4, 2);
	g.InsertEdge(3, 5, 8);
	g.InsertEdge(3, 6, 4);
	g.InsertEdge(4, 6, 6);
	g.InsertEdge(6, 5, 1);

	auto mst = MST::GetMST(g); //如果g是Matrix则会返回MST_Parent，反之，返回MST_Edge

	/* 如果是MST_Parent则这样遍历
	for (size_t i = 0, parent; i < mst.GetVertexNum(); ++i)
	{
		parent = mst.GetParent(i);
		if (parent != mst.GetVertexNum())
			cout << g.GetVertex(i)  //起始点
			<< " <-" << g.GetWeight(i, parent) //获取该边权重，因为是邻接矩阵所以直接用g获取很快
			<< "-> " << g.GetVertex(parent) //根节点
			<< endl;
	}*/
	/* 如果是MST_Edge则这样遍历 */
	mst.Foreach([&](auto from, auto to, auto w)
		{
			cout << g.GetVertex(from)  //起始点
				<< " <-" << w	//该边权重
				<< "-> " << to	//根节点
				<< endl;
		});

	cout << "总权重：" << mst.GetTotalWeight() << endl;
}
```
## 最短路径算法
最短路径算法在ShortestPath中实现，分别为单源最短路径类SSSP，多源最短路径类MSSP，模板参数参考之前的MST以及类注释，具体使用起来很简单，每个接口都有注释
  - 两个最短路径类都有现成的实例类，分别为S(M)SSP<unsigned long long>=IntegerS(M)SSP,S(M)SSP<double>=DecimalS(M)SSP，直接使用即可
  - SSSP区别有权图和无权图，分别采用BFS改造算法和Dijkstra算法
  - MSSP使用Floyd算法
  - 具体使用时，需要构造一个最短路径对象，然后使用Execute方法传入一个图的类，如：
  ```c++
  UnweightedDirectedMatrixGraph g;
  //对g的一些操作
  //...
  InergerSSSP sssp; //MSSP类的用法和SSSP类似
  sssp.Execute(g); //使用g来执行sssp算法
  //然后你就可以调用sssp.Foreach(xxx),sssp.GetDistance(xxx)之类的方法了
  ```
## 实例
  我可能会用这个库做点东西，如果做了，会把地址发在这里当一个例程
