#include "node.h"
#include <unordered_map>

vector<RoutingNode*> distanceVectorNodes;

unordered_map<string, int> edge_cost;

void routingAlgo(vector<RoutingNode*> distanceVectorNodes, unordered_map<string, int> edge_cost);
//void routingAlgo2(vector<RoutingNode*> distanceVectorNodes);

int main() {
    int n; // number of nodes
    cin>>n;
    string name; //Node label
    distanceVectorNodes.clear();
	string comma=",";
    for (int i = 0 ; i < n; i++) {
      RoutingNode *newnode = new RoutingNode();
      cin>>name;
      newnode->setName(name);
      distanceVectorNodes.push_back(newnode);
    }
    cin>>name;
    /*
      For each node label(@name), it's own ip address, ip address of another node
      defined by @oname will be inserted in the node's own datastructure interfaces 
    */
    while(name != "EOE") { //End of entries
      for (int i =0 ; i < distanceVectorNodes.size(); i++) {
	string myeth,oeth, oname;
	int cost;
	if(distanceVectorNodes[i]->getName() == name) {
	  //node interface ip
	  cin>>myeth; 	
	  //ip of another node connected to myeth (nd[i])
	  cin>>oeth;
	  //label of the node whose ip is oeth
	  cin>>oname;
	  //Taking cost of edge as input
	  cin>>cost;
	  for(int j = 0 ; j < distanceVectorNodes.size(); j++) {
	    if(distanceVectorNodes[j]->getName() == oname) {
			string e1 = to_string(i);
			string e2 = to_string(j);
			string e = e1+comma+e2;
			edge_cost[e]=cost;
			// cout<<e<<endl;

	      /*
		@myeth: ip address of my (distanceVectorNodes[i]) end of connection.
		@oeth: ip address of other end of connection.
		@distanceVectorNodes[j]: pointer to the node whose one of the interface is @oeth
	      */
	      distanceVectorNodes[i]->addInterface(myeth, oeth, distanceVectorNodes[j]);
	      //Routing table initialization
	      /*
		@myeth: ip address of my (distanceVectorNodes[i]) ethernet interface.
		@0: hop count, 0 as node does not need any other hop to pass packet to itself.
		
	      */
	      distanceVectorNodes[i]->addTblEntry(myeth, 0);
	      break;
	    }
	  }
	}
      }
      cin>>name;
    }

    /* The logic of the routing algorithm should go here */
    routingAlgo(distanceVectorNodes, edge_cost);
    /* Add the logic for periodic update (after every 1 sec) here */
    // routingAlgo(distanceVectorNodes);

	// cout<<"\n\nThe edge-costs are:\n";
	// for(auto ec:edge_cost){
	// 	cout<<ec.first<<" "<<ec.second<<endl;
	// }
    
}

