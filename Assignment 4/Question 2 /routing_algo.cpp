#include "node.h"
#include <iostream>
#include <unordered_map>

using namespace std;


void printRT(vector<RoutingNode*> nd){
/*Print routing table entries*/
	for (int i = 0; i < nd.size(); i++) {
	  nd[i]->printTable();
	}
}

int find_min_dist_index(vector<bool> vis, vector<int> d){
  int retval=-1;
  for(int i=0; i<vis.size(); ++i){
    if(!vis[i]){
      if(retval==-1){
        retval=i;
      }
      else if(d[i]<d[retval]){
        retval=i;
      }
    }
  }
  return retval;
}

pair<string, string> find_path(vector<RoutingNode*> nd, int src, int dest, vector<int> parent){
  // The function does not check for u==i as that must be handled by the caller
  int p=parent[dest], child=dest;
  while(p!=src){
    child=p;
    p=parent[p];
  }
  pair<string, string> retVal = {"extra", "extra"};
  vector<pair<NetInterface, Node*> > connected_interfaces = nd[p]->getInterfaces();
  for(int i=0; i<connected_interfaces.size(); ++i){
    if(connected_interfaces[i].second==nd[child]){
      retVal={connected_interfaces[i].first.getConnectedIp(), connected_interfaces[i].first.getip()};
      break;
    }
  }

  return retVal;
}

/*
void routingAlgo(vector<RoutingNode*> nd){
  // For all entries:
  int i=0;
  for(; i<nd.size(); ++i)
  {
    cout<<nd[i]->getName()<<endl;
    // cout<<nd[i]->getInterfaceSize()<<endl;
    cout<<"Routing Table Size: "<<nd[i]->getTable().tbl.size()<<endl;
    
    // Implementation of Djikstra Algo
    
      vector<bool> visited_nodes(nd.size(), false);
      vector<int> parent_node(nd.size(), -1);
      vector<int> distances(nd.size(), INT_MAX);
      
      distances[i] = 0;
      int u = find_min_dist_index(visited_nodes, distances);
      while(u!=-1){
        cout<<u<<endl;
        visited_nodes[u]=true;
        
        vector<pair<NetInterface, Node*> > connected_interfaces = nd[u]->getInterfaces();
        // string u_ip = nd[u]->
        if(u!=i){
          for(int i1=0;i1<nd[u]->getInterfaceSize(); ++i1){
            string ip1 = connected_interfaces[i1].first.getip();
            // string ip2 = connected_interfaces[i1].first.getConnectedIp();
            pair<string, string> ips = find_path(nd, i, u, parent_node);
            nd[i]->addTblEntry(ip1, ips.first, ips.second, distances[u]);
          }
        }

        for(int j=0; j<nd[u]->getInterfaceSize(); ++j){
          string adjacent_node = connected_interfaces[j].second->getName();
          int adj=0;
          for(; adj<nd.size(); ++adj){
            if(adjacent_node == nd[adj]->getName())
              break;
          }
          // cout<<"here: "<<u<<" "<<adj<<endl;
          if(!visited_nodes[adj] and (distances[u]+1)<distances[adj]){
            distances[adj]=distances[u]+1;
            parent_node[adj]=u;
          }
        }

        u = find_min_dist_index(visited_nodes, distances);
      }

    
    // vector<pair<NetInterface, Node*> > connected_interfaces = nd[i]->getInterfaces();
    // for(int j=0; j<nd[i]->getInterfaceSize(); ++j){
    //   cout<<connected_interfaces[j].second->getName();
    // }
    cout<<endl;
  }

  printf("Printing the routing tables after the convergence \n");
  printRT(nd);
}

*/


/*
  Commenting out the old function here
*/


void routingAlgo(vector<RoutingNode*> nd, unordered_map<string, int> edge_cost){
 
  bool saturation=false;
 
  for(int i=0; i<nd.size(); ++i) {
    nd[i]->sendMsg(i, nd, edge_cost);
  }
 
  /*Print routing table entries after routing algo converges */
  printf("Printing the routing tables after the convergence \n");
  printRT(nd);
}



void RoutingNode::recvMsg(int curr, vector<RoutingNode*> nd, vector<bool> &visited_nodes, vector<int> &parent_node, vector<int> &distances, unordered_map<string, int> edge_cost) {
  int i=curr;
  // for(; i<nd.size(); ++i)

    // cout<<nd[i]->getName()<<endl;
    // cout<<nd[i]->getInterfaceSize()<<endl;
    // cout<<"Routing Table Size: "<<nd[i]->getTable().tbl.size()<<endl;
    
    // Implementation of Djikstra Algo
    
      // vector<bool> visited_nodes(nd.size(), false);
      // vector<int> parent_node(nd.size(), -1);
      // vector<int> distances(nd.size(), INT_MAX);
      
      // distances[i] = 0;
      int u = find_min_dist_index(visited_nodes, distances);
      while(u!=-1){
        // cout<<u<<endl;
        visited_nodes[u]=true;
        
        vector<pair<NetInterface, Node*> > connected_interfaces = nd[u]->getInterfaces();
        // string u_ip = nd[u]->
        if(u!=i){
          for(int i1=0;i1<nd[u]->getInterfaceSize(); ++i1){
            string ip1 = connected_interfaces[i1].first.getip();
            // string ip2 = connected_interfaces[i1].first.getConnectedIp();
            pair<string, string> ips = find_path(nd, i, u, parent_node);
            nd[i]->addTblEntry(ip1, ips.first, ips.second, distances[u]);
          }
        }

        for(int j=0; j<nd[u]->getInterfaceSize(); ++j){
          string adjacent_node = connected_interfaces[j].second->getName();
          int adj=0;
          for(; adj<nd.size(); ++adj){
            if(adjacent_node == nd[adj]->getName())
              break;
          }
          // cout<<"here: "<<u<<" "<<adj<<endl;
          string e1=to_string(u), e2=",", e3=to_string(adj);
          string e=e1+e2+e3;
          if(!visited_nodes[adj] and (distances[u]+edge_cost[e])<distances[adj]){
            distances[adj]=distances[u]+edge_cost[e];
            parent_node[adj]=u;
          }
        }

        u = find_min_dist_index(visited_nodes, distances);
      }
}



