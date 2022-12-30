#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

/*
  Each row in the table will have these fields
  dstip:	Destination IP address
  nexthop: 	Next hop on the path to reach dstip
  ip_interface: nexthop is reachable via this interface (a node can have multiple interfaces)
  cost: 	cost of reaching dstip (number of hops)
*/
class RoutingEntry{
 public:
  string dstip, nexthop;
  string ip_interface;
  int cost;
};

/*
 * Class for specifying the sort order of Routing Table Entries
 * while printing the routing tables
 * 
*/
class Comparator{
 public:
  bool operator()(const RoutingEntry &R1,const RoutingEntry &R2){
    if (R1.cost == R2.cost) {
      return R1.dstip.compare(R2.dstip)<0;
    }
    else if(R1.cost > R2.cost) {
      return false;
    }
    else {
      return true;
    }
  }
} ;

/*
  This is the routing table
*/
struct routingtbl {
  vector<RoutingEntry> tbl;
};

/*
  Message format to be sent by a sender
  from: 		Sender's ip
  mytbl: 		Senders routing table
  recvip:		Receiver's ip
*/
class RouteMsg {
 public:
  string from;			// I am sending this message, so it must be me i.e. if A is sending mesg to B then it is A's ip address
  struct routingtbl *mytbl;	// This is routing table of A
  string recvip;		// B ip address that will receive this message
};

/*
  Emulation of network interface. Since we do not have a wire class, 
  we are showing the connection by the pair of IP's
  
  ip: 		Own ip
  connectedTo: 	An address to which above mentioned ip is connected via ethernet.
*/
class NetInterface {
 private:
  string ip;
  string connectedTo; 	//this node is connected to this ip
  
 public:
  string getip() {
    return this->ip;
  }
  string getConnectedIp() {
    return this->connectedTo;
  }
  void setip(string ip) {
    this->ip = ip;
  }
  void setConnectedip(string ip) {
    this->connectedTo = ip;
  }
  
};

/*
  Struct of each node
  name: 	It is just a label for a node
  interfaces: 	List of network interfaces a node have
  Node* is part of each interface, it easily allows to send message to another node
  mytbl: 		Node's routing table
*/
class Node {
 private:
  string name;
  vector<pair<NetInterface, Node*> > interfaces;
 protected:
  struct routingtbl mytbl;
  virtual void recvMsg(RouteMsg* msg) {
    cout<<"Base"<<endl;
  }
  bool isMyInterface(string eth) {
    for (int i = 0; i < interfaces.size(); ++i) {
      if(interfaces[i].first.getip() == eth)
	return true;
    }
    return false;
  }
 public:
  void setName(string name){
    this->name = name;
  }
  
  void addInterface(string ip, string connip, Node *nextHop) {
    NetInterface eth;
    eth.setip(ip);
    eth.setConnectedip(connip);
    interfaces.push_back({eth, nextHop});
  }
  
  void addTblEntry(string dip, string nip, string myip, int cost) {
    RoutingEntry entry;
    entry.dstip = dip;
    entry.nexthop = nip;
    entry.ip_interface = myip;
    entry.cost = cost;
    mytbl.tbl.push_back(entry);
  }

// Editing here
  void addTblEntry(string myip, int cost) {
    RoutingEntry entry;
    entry.dstip = myip;
    entry.nexthop = myip;
    entry.ip_interface = myip;
    entry.cost = cost;
    mytbl.tbl.push_back(entry);
  }

  void updateTblEntry(string dstip, int cost) {
    // to update the dstip hop count in the routing table (if dstip already exists)
    // new hop count will be equal to the cost 
    for (int i=0; i<mytbl.tbl.size(); i++){
      RoutingEntry entry = mytbl.tbl[i];

      if (entry.dstip == dstip) 
        mytbl.tbl[i].cost = cost;

    }

    // remove interfaces 
    for(int i=0; i<interfaces.size(); ++i){
      // if the interface ip is matching with dstip then remove
      // the interface from the list
      if (interfaces[i].first.getConnectedIp() == dstip) {
        interfaces.erase(interfaces.begin() + i);
      }
    }
  }
  
  string getName() {
    return this->name;
  }
  
  struct routingtbl getTable() {
    return mytbl;
  }
  
  void printTable() {
    Comparator myobject;
    sort(mytbl.tbl.begin(),mytbl.tbl.end(),myobject);
    cout<<this->getName()<<":"<<endl;
    for (int i = 0; i < mytbl.tbl.size(); ++i) {
      cout<<mytbl.tbl[i].dstip<<" | "<<mytbl.tbl[i].nexthop<<" | "<<mytbl.tbl[i].ip_interface<<" | "<<mytbl.tbl[i].cost <<endl;
    }
  }
  
  /* OLD

  void sendMsg(){
    struct routingtbl ntbl;
    for (int i = 0; i < mytbl.tbl.size(); ++i) {
      ntbl.tbl.push_back(mytbl.tbl[i]);
    }
    
    for (int i = 0; i < interfaces.size(); ++i) {
      RouteMsg msg;
      msg.from = interfaces[i].first.getip();
      // printf("i=%d, msg-from-interface=%s\n",i, msg.from.c_str());
      msg.mytbl = &ntbl;
      msg.recvip = interfaces[i].first.getConnectedIp();		
      interfaces[i].second->recvMsg(&msg);
    }
  }
  */

  // New sendMsg defined in RoutingNode class

  // Functions added by me
  int getInterfaceSize(){
    return interfaces.size();
  }

  vector<pair<NetInterface, Node*> > getInterfaces(){
    return interfaces;
  }
  
};

class RoutingNode: public Node {
 public:
  void recvMsg(int curr, vector<RoutingNode*> nd, vector<bool> &visited_nodes, vector<int> &parent_node, vector<int> &distances);
  void sendMsg(int curr, vector<RoutingNode*> nd){
    vector<bool> visited_nodes(nd.size(), false);
    vector<int> parent_node(nd.size(), -1);
    vector<int> distances(nd.size(), INT_MAX);
    
    distances[curr] = 0;
    visited_nodes[curr]=true;

    vector<pair<NetInterface, Node*> > connected_interfaces = this->getInterfaces();
    for(int j=0; j<this->getInterfaceSize(); ++j){
      string adjacent_node = connected_interfaces[j].second->getName();
      int adj=0;
      for(; adj<nd.size(); ++adj){
        if(adjacent_node == nd[adj]->getName())
          break;
      }
      // cout<<"here: "<<u<<" "<<adj<<endl;
      if(!visited_nodes[adj] and (distances[curr]+1)<distances[adj]){
        distances[adj]=distances[curr]+1;
        parent_node[adj]=curr;
      }
    }

    this->recvMsg(curr, nd, visited_nodes, parent_node, distances);
  }
};
