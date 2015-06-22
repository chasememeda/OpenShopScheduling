#include <iostream>
#include <sstream>
#include <string>
#include <set>
#include <map>
#include <deque>
#include <vector>
#include<stdio.h>
#include <algorithm>
#include <iterator>

#define JOBN 3 // the total number of jobs
#define MACHN 3 // the total number of machines
#define INFNTY 9999999;

//#define POWER_K 1 // the power velue of k in the objective function

#define MIN(a,b) ( ((a)>(b)) ? (b):(a) )
#define MAX(a,b) ( ((a)>(b)) ? (a):(b) )
using namespace std;

//---------------------- critical path method -----------------
class osnode{
	
	int job; // job id;
	int machine; // machine id;
	double processt; // processing time;
public:
	osnode(int j, int m, double t){
		job = j;
		machine = m;
		processt = t;
	}
	osnode(){// default node
		processt = -1;
	}
	int getJob(){
		return job;
	}
	int getMach(){
		return machine;
	}
	double getTime(){
		return processt;
	}
	long getMark()const{
		return job*100 + machine;
	}
	void print_node(){
		cout<< "("<< job <<"," << machine <<":" << processt<<")";
	}
	bool operator < (const osnode &n1) const
    {
       return getMark() < n1.getMark() ; ;
    }
};

class osarc{
	osnode *i;
	osnode *j;// arc from node i to node j
	mutable bool deter;

	friend class osselection; 
public:
	osarc(osnode *a, osnode *b, bool d):i(a),j(b),deter(d){};
	osarc(osnode *a, osnode *b):i(a),j(b),deter(false){};
	osarc():i(NULL),j(NULL),deter(false){};
	void print_arc()const{
		i->print_node();
		cout<<"-";
		if( deter ){
			cout<< "*";
		}
		cout<<">";
		j->print_node();
	};
	osnode *getI()const{
		return i;
	};
	osnode *getJ()const{
		return j;
	};
	// only for set comparison
	long getMark()const{
		//! noticing that two fixed and unfixed arcs with the same nodes can not appear at the same time
		return i->getMark() *100000 + j->getMark();
	}
	void setdeter( bool de )const{
		deter = de;
	}
	bool getdeter()const{
		return deter;
	}
	// get whether an acr is in the given positive position
	bool getDirect()const{
		if( i->getMach() + i->getJob() < j->getMach() + j->getJob()){
			return true;
		}
		return false;	
	}
	bool operator < (const osarc &n1) const
    {
       return getMark() < n1.getMark() ; ;
    }
};
struct Setosarc
{
	 bool operator()(const osarc &a1, const osarc &a2) const
	 {
		 return a1.getMark() > a2.getMark();
	 }
};
typedef set <osarc, Setosarc> osarcSet;
// functions

void printOsarcSet( osarcSet os );

osarcSet Fgenerate( deque <osnode*> os );
osarcSet Lgenerate( deque <osnode*> os );
class osunits{
	friend class osselection;
	deque<osarc> ul;		// the arcs in the block;
	deque<osnode*> ulnodes;	// the midle nodes in the block
	int asize;				// this equals ulnodes.size(), however, considering that .size() is slow in c++, we add this for faster comparison
	int freedom;
	osnode* u1;				// the first node in the block
	osnode* um;				// the last node in the block

	osarcSet F;			//Fj in the B&B algorithm
	osarcSet L;			//Lj in the B&B algorithm

	map < osnode* , osarcSet > EB;	// the arcsets in Eb
	map < osnode* , osarcSet > EA;	// the arcsets in Ea

public:
	osunits(){ 
		u1 = NULL; 
		um = NULL;
	};
	void insert( osarc a){
		ul.push_back( a );
		generateNodes();
	};
	void clear(){
		ul.clear();
	};
	bool empty(){
		return ul.empty();
	};
	void print(){
		cout << "\n[----------------unit--------------]\n";
		cout << asize << endl;
			deque <osarc> :: iterator ita = ul.begin();
			while( ita != ul.end() ){
				ita->print_arc();
				cout<<"\n";
				ita ++;
			}
		print_FLE();
		cout << "\n[----------------end--------------]\n";
	};
	void print_nodes();
	void print_FLE();
	int size()const{		return asize;	};
	int freesize()const{	return freedom;	};
	bool samemach(){
		if( ul.empty() ){// for roughbost
			cout << " block cannot be empty";
			exit(0);
		}
		deque <osarc> ::iterator a = ul.begin();
		return (a->getI()->getMach() == a->getJ()->getMach());
	};
	//****************** generating the nodes in the unit
	void generateNodes();
};
struct osunitSorter
{
  bool osunitSorter::operator ()(const osunits &a, const osunits &b) {
	  return a.size() > b.size();
  }
};




//denote the graph for critical path method, given a full selection
class osselection{

	
	map <osnode, osarcSet> forwardset;
	map <osnode, osarcSet> backwardset;
	map <int, double> critical;	// denote the C_j for each job
	deque <osarc> criticalpath;	// denote the critical path
	
	deque < osunits > units;		// denote the units in the critical path;
	bool circle;				// denote whether circle exists

public:
	osarcSet arcs;
	osselection(){ circle = false; };
	osselection( osarcSet as );
	~osselection(){};
	// add one arc to the graph
	void insert( osarc arc);
	// delete one arc from the graph
	void del( osarc arc );
	// inset the whole graph
	void insert( int arc[2*JOBN*MACHN - JOBN - MACHN][4] , osnode *oss[JOBN][MACHN] );
	
	// get the forward nodes of node on
	osarcSet forwardsubset(osnode on){
		return forwardset.find(on)->second;
	}
	// get the backward nodes of node on
	osarcSet backwardsubset(osnode on){
		return backwardset.find(on)->second;
	}
	

	// generating critical path length and all the related values
	bool generate_critical_path( );
	
	deque < osarcSet> Sj();			// return the list of the set of arcs for branching

	double getC_k( int k );	// get the objective function value C^k 
	double getC_max( );		// get the objective function value Cmax
	bool get_circle(){ return circle; };	//in case circles exists
	
	void print();			// print all the arcs in the selection
	void print_subset(osnode on);	// print the forward arcs of node "on"
	void print_critical();	// print the critical path
	void print_units();		// print all the units

};

// for calculating the head and tail of a arc set
class osarcsFr{
	
private:
	osarcSet arcs;		// the arcs
	osarcSet arcsfr;		// the fixed arcs
	map <osnode *, double> r;	// head
	map <osnode *, double> q;	// tail
	map <osnode *, osarcSet> forwardset;
	map <osnode *, osarcSet> backwardset;
	map <osnode*, int> level;	// denote the nodes level
								// level1: end of two adjacent arcs
								// level2: with preds but no suss
								// level3: with only one pred



	map <osnode *, set< osnode *> > succ; // successors of same job
	map <osnode *, set< osnode *> > pred; // predecessors of same job 

	map <osnode *, set< osnode *> > succj; // successors of same job
	map <osnode *, set< osnode *> > predj; // predecessors of same job 
	map <osnode *, set< osnode *> > succm; // successors on same machine
	map <osnode *, set< osnode *> > predm; // predecessors on same machine 
public:
	osarcsFr( osarcSet a ){
		arcs = a;
		setFr(false);
	};
	osarcsFr( osarcSet a, bool b){
		arcs = a;
		setFr(b);
	};
	osarcsFr(){};
	~osarcsFr(){};
	//return the head of a node
	//double getHead( osnode * n );
	////return the tail of a node
	//double getTail( osnode * n );

	// select all the fixed arcs from the arc set a
	//	b: if true, all arcs inserted will set fixed.
	void setFr(bool b);	

	// generate all the pred and succ of the fixed arcs, if circle exists, return false
	bool rqCheck( );


	// circle check true= circle exist
 	bool circleCheck();

	double LB_C_max();										// return the lower bound for makespan
	double LB_C_k(int k, float o[JOBN][MACHN] );			// return the lower bound for c_j_k, o[JOBN][MACHN] is the operation time matrix


	/*
	*heuristic algorithm to generate a feasible solution
	*/
	void levelGeneration();
	osselection graphGenerate( osnode * oss[JOBN][MACHN] );
	osarcSet graphGenerateLine( bool job, int id, osnode * oss[JOBN][MACHN] );

	void print();		// print the succesors and predecessors
	void print_rj();	// print all the rjs
	osarcSet getFixArcs(){
		return arcsfr;
	};
};


//---------------------- feasible schedule generating methods ----------------------


//---------------------- for branch and bound algorithm with max heap method-----------------

//
class bbnode
{
	friend class MaxLoading;

	bbnode *parent;				// father node
	set < bbnode *> children;	// child nodes
	osselection data;	
	osarcsFr fr;
	double uweight;		// lowerbound
	double upper;		// new upper bound

public:
	//bbnode( osselection d):data(d){};
	bbnode( osarcsFr f ){
		fr = f;
	};
	~bbnode(){
	};
	void setweight( float op[JOBN][MACHN], int kp, bool b  ){
		if(b){
			uweight = fr.LB_C_max();
		}
		uweight = fr.LB_C_k(kp,op);
	};
	void branch( osnode * o[JOBN][MACHN] , int k, bool b  );
	double getweight()const{	return uweight;	};
	bool operator < (const bbnode &n1) const
    {// the lower the better
       return getweight() > n1.getweight() ;
    }
};

bool lessmark( bbnode* a, bbnode* b);

class MaxLoading
{
	float		o[JOBN][MACHN];
	osnode *	oss[JOBN][MACHN];
	osselection result;
	int k;
	bool makespan;	// denote the objective function

public:
	MaxLoading( float op[JOBN][MACHN], int kp, bool b ){
		// creating all the nodes
		for(int i = 0 ; i < JOBN ; i++)
		{
			for( int j = 0; j < MACHN ; j++){
				oss[i][j] = new osnode(i,j,op[i][j]);
				this->o[i][j] = op[i][j];
			}
		}
		k = kp;
		makespan = b;
	};
	void bbRun();
	double get_result(){
		if( makespan ){
			return result.getC_max();
		}
		return result.getC_k( k );
	};
	osselection osrslt(){	return result;	};
};
