#include <bits/stdc++.h>
using namespace std;

int V,E;

// class edge{
// public:
// 	int flow = 0;
// 	int capacity = 0;
// };

typedef struct{
	int flow;
	int residual;
	int capacity;
} edge;

edge** G;
list<pair<int,int>> edges;


void print_graph(){

	cout << "  ";

	for(int i=0;i<V;i++)
		printf("     v%d",i);


	cout << endl;

	for(int i=0;i<V;i++){
		cout << "v" << i;
		for(int j=0;j<V;j++)
			// cout << graph[i][j] << " ";
			printf("%5d/%d",G[i][j].flow,G[i][j].capacity);
		cout << endl;
	}


}

bool augment(int s,int t,int* parent)
{
	// Augment using BFS

	bool visited[V];
	memset(visited, sizeof(visited),0);

	queue<int> q;
	q.push(s);
	visited[s] = true;

	parent[s] = -1;

	while(!q.empty()){
		int u = q.front();

		q.pop();

		for(int v =0;v < V;v++){
			if(!visited[v] && G[u][v].residual > 0){
				if(v == t){
					parent[v] = u;
					return true;
				}

				q.push(v);
				parent[v] = u;
				visited[v] = true;
			}
		}
	}

	return false;
}

bool dfs(int s,int t,int* child){

	bool visited[V];
	memset(visited,sizeof(visited),0);

	deque<int> dq;
	dq.push_front(s);
	visited[s] = true;

	while(!dq.empty()){
		int u = dq.front();

		dq.pop_front();

		// for(int v=V-1;v >= 0; v--){
		// 	if(!visited[v] && G[u][v].residual > 0){
		// 		if(v == t){
		// 			child[u] = v;
		// 			return true;
		// 		}

		// 		dq.push_front(v);
		// 		child[u] = v;
		// 		visited[u] = true;
		// 	}
		// }

		for(int v=0;v <V; v++){
			if(!visited[v] && G[u][v].residual > 0){
				if(v == t){
					child[u] = v;
					return true;
				}

				dq.push_front(v);
				child[u] = v;
				visited[u] = true;
			}
		}

	}


	return false;

}

int EdmondKarp(int s=0, int t = V-1)
{
	int flow = 0;

	int parent[V];

	while(augment(s,t,parent)){

		cout << "Found Augmenting Path!" << endl;


		int p = INT_MAX;

		for(int v=t;v != s;v = parent[v]){
			cout << v << " <- ";
			int u = parent[v];
			p = min(p, G[u][v].residual);
		}

		cout << s;

		for(int v = t; v != s; v = parent[v]){
			int u = parent[v];
			G[u][v].flow += p;
			G[u][v].residual -= p;
		}

		cout << " , augmenting by " << p << endl;

		flow += p;

	}

	return flow;
}

int FordFulkerson(int s = 0, int t = V-1){
	int flow = 0;

	int child[V];

	while(dfs(s,t,child)){

		cout << "Found Augmenting Path!" << endl;


		int p = INT_MAX;

		for(int u=s;u != t;u = child[u]){
			cout << u << " -> ";
			int v = child[u];
			p = min(p, G[u][v].residual);
		}

		cout << t;

		for(int u=s;u != t;u = child[u]){
			int v = child[u];
			G[u][v].flow += p;
			G[u][v].residual -= p;
		}

		cout << " , augmenting by " << p << endl;

		flow += p;

	}

	return flow;
}


int main()
{

	cin>>V;
	cin>>E;


	G = new edge*[V];

	for(int i=0;i<V;i++) G[i] = new edge[V];

	for(int i=0;i<V;i++){
		for(int j=0;j<V;j++)
			G[i][j].flow = G[i][j].residual = G[i][j].capacity = 0;
	}


	for(int e=0;e<E;e++){
		int u,v,cap;
		cin>>u>>v>>cap;

		G[u][v].capacity = cap;
		G[u][v].residual = cap;
		edges.push_back({u,v});
	}

	int max_flow = FordFulkerson(0,V-1);

	print_graph();

	cout << endl << max_flow << endl;
}