#include "bb.h"
/*
*	This cpp file is for all the methods 
*	of classes in branch and bound algorithm
*
//*/
bool lessmark( bbnode* a, bbnode* b)  
 {  
     return (a->getweight() < b->getweight());  
 };


void MaxLoading::bbRun(){
	cout<<"start!\n";
	bbnode* root = new bbnode( (* new osarcsFr) );
	root->parent = NULL;
	vector< bbnode* > heap;
	heap.push_back( root );
	double upperbound = -1 ;
	int i=0;
	while(!heap.empty()){

		vector< bbnode* >::iterator it = heap.begin();
		// select the one with the lowest lower bound
		bbnode * father = heap.front() ;
		osarcSet arcs = father->fr.getFixArcs();

		father->branch(oss,k,makespan);
		//father->data.print();
		if( upperbound < 0 || upperbound > father->upper){
			// upper bound is updated
			cout << "new upper bound\n";
			upperbound = father->upper;
			result = father->data;
			// cut branches in heap
			//sort_heap (heap.begin(),heap.end(),lessmark);
			vector< bbnode* >::iterator it = heap.begin();
			vector< bbnode* >::iterator move = heap.end();
			bbnode * o = heap.front();
			while( it != heap.end() ){
			// find the first node with a lower bound larger than the current upper bound
				if( (*it)->getweight() > upperbound ){
					break;
				}
				it++;
			}
			move = it;
			
			set < bbnode * > delset;
			while( it != heap.end() ){
				bbnode * node = *it;
				// delete this branch from the tree
				if( NULL != node->parent ){
					node->parent->children.erase( node );
				}
				set< bbnode * > children;
				set< bbnode * > all;
				children = node->children;
				while(!children.empty()){
					set< bbnode * >::iterator itt = children.begin();
					set< bbnode * > newchild;
					all.insert(children.begin(),children.end());
					while( itt != children.end() ){
						bbnode * child = *itt;
						newchild.insert(child->children.begin(),child->children.end());
						itt++;
					}
					children = newchild;
				}
				delset.insert(all.begin(),all.end());
				it++;
			}
			
			heap.erase(move,heap.end());
			
			set < bbnode * >::iterator itb = delset.begin();
			while( itb != delset.end() ){
				if(find(heap.begin(),heap.end(),*itb) != heap.end()){
					heap.erase(find(heap.begin(),heap.end(),*itb));
				}
				itb ++;
			}
			std::make_heap(heap.begin(),heap.end(),lessmark);
		}// * end of if upper bound is updated

		if( father->data.Sj().empty() ){
		// if this is a leaf, remove it from the node, delete it and continue; 
			if( find(heap.begin(),heap.end(),father) != heap.end() ){
				heap.erase(find(heap.begin(),heap.end(),father));
				std::make_heap(heap.begin(),heap.end(),lessmark);
			}
			continue;
		}
		//if this is not a leaf, then do the branch
		deque<osarcSet> sj = father->data.Sj();
		deque<osarcSet>::iterator itsj = sj.begin();
		osarcSet fixed = father->fr.getFixArcs();
		while( itsj != sj.end() ){
			osarcSet arcs = fixed;
			arcs.insert(itsj->begin(),itsj->end());
			osarcsFr f = osarcsFr(arcs,true);
			if( !f.circleCheck() ){
				bbnode *next = new bbnode(f);
				next->setweight(o,k,makespan);
				if( next->getweight() < upperbound ){
					// if lower than upper bound, then add to the tree and heap
					next->parent = father;
					father->children.insert( next );
					heap.push_back( next );
					std::push_heap(heap.begin(),heap.end(),lessmark);
				}
			}
			itsj ++;
		}
		// erase this node from the heap since it's no more active
		if( find(heap.begin(),heap.end(),father) != heap.end());
		heap.erase(find(heap.begin(),heap.end(),father));
		std::make_heap(heap.begin(),heap.end(),lessmark);
		i++;
	}
	cout<<"finished!\n";
}

void bbnode::branch( osnode * o[JOBN][MACHN], int k, bool b  ){
		data = fr.graphGenerate( o );
		data.generate_critical_path();
		if( b ){
			upper = data.getC_max();
			return;
		}
		upper = data.getC_k(k);
}
