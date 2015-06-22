#include "bb.h"
/*
*	this cpp file is for all the methods 
*	of classes in critical path algorithm
*
*/

osarcSet Fgenerate( deque <osnode*> os ){
	deque <osnode*> t = os;
	osarcSet osset;
	osnode * o = os.front();
	t.pop_front();
	deque <osnode*>::iterator it = t.begin();
	while( it != t.end() ){
		osset.insert( osarc( o , *it) );
		it++;
	}
	return osset;
}
osarcSet Lgenerate( deque <osnode*> os ){
	deque <osnode*> t = os;
	osarcSet osset;
	osnode * o = os.back();
	t.pop_back();
	deque <osnode*>::iterator it = t.begin();
	while( it != t.end() ){
		osset.insert( osarc( *it , o ) );
		it++;
	}
	return osset;
}
void printOsarcSet( osarcSet os ){
	osarcSet::iterator it = os.begin();
	while( it != os.end() ){
		it->print_arc();
		cout<<"\n";
		it++;
	}
}
/*
*
* -----------------------------methods for osarcsFr class-----------------------------
*/
void osarcsFr::setFr( bool b ){
	osarcSet::iterator it = arcs.begin();
	arcsfr.clear();
	if( b ){
		while( it != arcs.end() ){
			it->setdeter(true);
			//it->setdeter(true);
			//it->deter = true;
			//(*it).setdeter(true);
		it++;
		}// end of while
		arcsfr = arcs;
		return;
	}
	while( it != arcs.end() ){
		if ( it-> getdeter()  ){
			arcsfr.insert( (*it));
		}
		it++;
	}
}
// for bounders 
bool osarcsFr::rqCheck( ){
	osarcSet::iterator it = arcsfr.begin();
	
	forwardset.clear();
	backwardset.clear();
	
	r.clear();

	while( it != arcsfr.end() ){
		if( forwardset.find( it->getI()) == forwardset.end() ){
			osarcSet tmptset;
			tmptset.insert( (*it) );
			forwardset.insert( pair< osnode*, osarcSet > (it->getI(),tmptset) );
		}else{
			forwardset.find( it->getI())->second.insert( (*it) );
		}

		if( backwardset.find( it->getJ()) == backwardset.end() ){
			osarcSet tmptset;
			tmptset.insert( (*it) );
			backwardset.insert( pair< osnode *, osarcSet > (it->getJ(),tmptset) );
			// noticing that only nodes with pred are inserted in to set r here.
			r.insert( pair< osnode *, double > (it->getJ(),0.00) );
		}else{
			backwardset.find( it->getJ())->second.insert( (*it) );
		}


		it++;
	}

	map <osnode *, osarcSet> f = forwardset;
	map <osnode *, osarcSet> b = backwardset;
	map <osnode *, set<osnode *> > succt;
	map <osnode *, set<osnode *> > predt;
	
	while( !f.empty() ){
		set <osnode *> C;
		C.clear();
		
			map <osnode *, osarcSet>::iterator itf = f.begin();	
			// first find all the nodes without backwardnodes at the beginning
			while( itf != f.end()){
				if( b.find(itf->first) == b.end() || b.find(itf->first)->second.empty()){
					C.insert( itf->first );
				}// find all the nodes without pred
				itf ++;
			}
			
		if( C.empty() ){
			// circles exists
			//cout << "there is a circle";
			return false;
		}
		set <osnode *>::iterator itn = C.begin();
		while( itn != C.end() ){
			osnode * head = *itn;
			osarcSet tails = f.find( head )->second;
			osarcSet::iterator itt = tails.begin();
			while( itt != tails.end() ){
				// for each arcs in the head's forward set
				osnode * tail = itt->getJ();
				// 1. add the tail node of head into head's succ set
				if( succt.find(head) == succt.end() ){
					set<osnode *> osset;
					osset.insert( tail );
					succt.insert(pair<osnode *, set<osnode *> > ( head, osset ) );
				}else{
					succt.find(head)->second.insert(tail);
				}
				// 2. add the head node of tail into tail's pred set
				if( predt.find(tail) == predt.end() ){
					set<osnode *> osset;
					osset.insert( head );
					predt.insert(pair<osnode *, set<osnode *> > ( tail, osset ) );
				}else{
					predt.find(tail)->second.insert(head);
				}
				// 3. check the pred set of head
				if( predt.find(head) != predt.end() ){
					set <osnode *> predset = predt.find(head)->second;
					set <osnode *>::iterator ittp = predset.begin();
					while( ittp != predset.end() ){
						// 3.1 add the tail to the preds of heads
						// ! noticing that the find method wont return .end() here in a non-circle graph
						succt.find( *ittp )->second.insert( tail );
						// 3.2 add the preds to the tail's pred set
						predt.find( tail )->second.insert( *ittp );
						ittp++;
					}
				}
				// 4. delete this arc from the tails backward set
				b.find(tail)->second.erase( *itt ) ;
				itt++;
			}
			f.erase (*itn);// delete this element from f
			b.erase(*itn);	// delete this element from b 
			itn ++;
		}// * end if while loop for C

	}// * end of while loop for set f

	succ = succt;
	pred = predt;

	succj.clear();
	predj.clear();
	succm.clear();
	predm.clear();
	map <osnode *, set< osnode *> >::iterator itm = succt.begin();
	while( itm != succt.end() ){


		osnode * head = itm->first;
		set <osnode *> ::iterator itmn= itm->second.begin();
		while(itmn != itm->second.end()) {
			if( head->getJob() == (*itmn)->getJob() ){
				// of the same job
				if(succj.find(head) == succj.end() ){
					set <osnode*> tmptset;
					tmptset.insert(*itmn);
					succj.insert(pair<osnode *, set<osnode*>>(head,tmptset));
				}else{
					succj.find(head)->second.insert(*itmn);
				}
			}
			if( head->getMach() == (*itmn)->getMach() ){
				// of the same machine
				if(succm.find(head) == succm.end() ){
					set <osnode*> tmptset;
					tmptset.insert(*itmn);
					succm.insert(pair<osnode *, set<osnode*>>(head,tmptset));
				}else{
					succm.find(head)->second.insert(*itmn);
				}
			}
			itmn ++;
		}

		itm++;
	}
	itm = predt.begin();
	while( itm != predt.end() ){


		osnode * head = itm->first;
		set <osnode *> ::iterator itmn= itm->second.begin();
		while(itmn != itm->second.end()) {
			if( head->getJob() == (*itmn)->getJob() ){
				// of the same job
				if(predj.find(head) == predj.end() ){
					set <osnode*> tmptset;
					tmptset.insert(*itmn);
					predj.insert(pair<osnode *, set<osnode*>>(head,tmptset));
				}else{
					predj.find(head)->second.insert(*itmn);
				}
			}
			if( head->getMach() == (*itmn)->getMach() ){
				// of the same machine
				if(predm.find(head) == predm.end() ){
					set <osnode*> tmptset;
					tmptset.insert(*itmn);
					predm.insert(pair<osnode *, set<osnode*>>(head,tmptset));
				}else{
					predm.find(head)->second.insert(*itmn);
				}
			}
			itmn ++;
		}

		itm++;
	}

	map <osnode *, set< osnode *> > predjt = predj;
	map <osnode *, set< osnode *> > predmt = predm;

	itm = succt.begin();
	set <osnode *> start;
	while( itm != succt.end() ){
		//find all the nodes without preds
		if( predt.find( itm->first) == predt.end() ){
			start.insert( itm->first );
		}
		itm ++ ;
	}
	while( !predt.empty() ){
	
		set <osnode *>::iterator its = start.begin();
		while( its != start.end() ){
			// for each nodes whose preds' r are determined
			osnode * head = *its;
			// computing its r
			if( r.find( head ) == r.end() ){
				// when the node head has no pred
				r.insert( pair< osnode *, double > (head,0.00));
			}else{
				double rmin;
				double sump;
				double rtmpt;
				set <osnode *> predss;
				set <osnode *>::iterator itos;
				// preds of the same job
				rmin = -1;
				sump = 0;
				if( predj.find(head) == predj.end() ){
					rmin = 0;
				}else{
					predss = predj.find(head)->second;
					itos = predss.begin();
					while( itos != predss.end() ){
						double dr = r.find(*itos)->second;
						if( rmin < 0 || rmin > dr ){
							rmin = dr;
						}
						sump = sump + (*itos)->getTime();
						itos ++;
					}
				}
				
				rtmpt = sump + rmin;

				// preds of the same machine
				rmin = -1;
				sump = 0;
				if( predm.find(head) == predm.end() ){
					rmin = 0;
				}else{
					predss = predm.find(head)->second;
					itos = predss.begin();
					while( itos != predss.end() ){
						double dr = r.find(*itos)->second;
						if( rmin < 0 || rmin > dr ){
							rmin = dr;
						}
						sump = sump + (*itos)->getTime();
						itos ++;
					}
				}
				
				if( rtmpt < sump + rmin ){
					rtmpt = sump + rmin ;
				}
				r.find(head)->second = rtmpt;
			}// * end of r value generating

			// find all the sucessors of this node
			if( succt.find( head ) != succt.end() ){
				set <osnode *> preds = succt.find( head )->second ;
				set <osnode *>::iterator itp = preds.begin();
				while( itp != preds.end() ){
					// erase this from its successors' pred nodes, since it's r is calculated
					predt.find( *itp )->second.erase(head);
					itp ++;
				}
			}
			// remove this node from predt set
			predt.erase( head );
			its++;

		}// *end of foreach start

		itm = predt.begin();
		start.clear();
		while( itm != predt.end() ){
			// find all the nodes whose preds are all scheduled
			if(itm->second.empty()){
				start.insert( itm->first );
			}
			itm ++;
		}
	}

	// generating the head and tails for each operation

	return true;

}
bool osarcsFr::circleCheck(){
	osarcSet::iterator it = arcsfr.begin();
	forwardset.clear();
	backwardset.clear();
	
	while( it != arcsfr.end() ){
		if( forwardset.find( it->getI()) == forwardset.end() ){
			osarcSet tmptset;
			tmptset.insert( (*it) );
			forwardset.insert( pair< osnode*, osarcSet > (it->getI(),tmptset) );
		}else{
			forwardset.find( it->getI())->second.insert( (*it) );
		}

		if( backwardset.find( it->getJ()) == backwardset.end() ){
			osarcSet tmptset;
			tmptset.insert( (*it) );
			backwardset.insert( pair< osnode *, osarcSet > (it->getJ(),tmptset) );
		}else{
			backwardset.find( it->getJ())->second.insert( (*it) );
		}


		it++;
	}

	map <osnode *, osarcSet> f = forwardset;
	map <osnode *, osarcSet> b = backwardset;
	
	while( !f.empty() ){
		set <osnode *> C;
		C.clear();
		
			map <osnode *, osarcSet>::iterator itf = f.begin();	
			// first find all the nodes without backwardnodes at the beginning
			while( itf != f.end()){
				if( b.find(itf->first) == b.end() || b.find(itf->first)->second.empty()){
					C.insert( itf->first );
				}// find all the nodes without pred
				itf ++;
			}
		if( C.empty() ){
			// circles exists
			//cout << "there is a circle";
			return true;
		}
		set <osnode *>::iterator itn = C.begin();
		while( itn != C.end() ){
			osnode * head = *itn;
			osarcSet tails = f.find( head )->second;
			osarcSet::iterator itt = tails.begin();
			while( itt != tails.end() ){
				// for each arcs in the head's forward set
				osnode * tail = itt->getJ();
				b.find(tail)->second.erase( *itt ) ;
				itt++;
			}
			f.erase (*itn);// delete this element from f
			b.erase(*itn);	// delete this element from b 
			itn ++;
		}// * end if while loop for C

	}// * end of while loop for set f
	return false;
}
void osarcsFr::print(){
	
	map <osnode *, osarcSet>::iterator it = forwardset.begin();
	cout <<"------ forwardset --------\n";
	while( it != forwardset.end() ){
		cout <<"----";
		it->first->print_node();
		cout<<"-----\n";
		printOsarcSet( it->second );
		it++;
	}
	it = backwardset.begin();
	cout <<"------ backwardset --------\n";
	while( it != backwardset.end() ){
		cout <<"----";
		it->first->print_node();
		cout<<"-----\n";
		printOsarcSet( it->second );
		it++;
	}
	map <osnode *, set< osnode *> >::iterator itm = succj.begin();
	cout <<"------ successor sam job--------\n";
	while( itm != succj.end() ){
		cout <<"----";
		itm->first->print_node();
		cout<<"-----\n";
		set< osnode *>::iterator ito = itm->second.begin();
		while ( ito != itm->second.end() ){
			(*ito)->print_node();
			cout <<"\n";
			ito ++ ;
		}
		itm++;
	}
	itm = predj.begin();
	cout <<"------ predecessors same job--------\n";
	while( itm != predj.end() ){
		cout <<"----";
		itm->first->print_node();
		cout<<"-----\n";
		set< osnode *>::iterator ito = itm->second.begin();
		while ( ito != itm->second.end() ){
			(*ito)->print_node();
			cout <<"\n";
			ito ++ ;
		}
		itm++;
	}
	itm = succm.begin();
	cout <<"------ successor sam machine--------\n";
	while( itm != succm.end() ){
		cout <<"----";
		itm->first->print_node();
		cout<<"-----\n";
		set< osnode *>::iterator ito = itm->second.begin();
		while ( ito != itm->second.end() ){
			(*ito)->print_node();
			cout <<"\n";
			ito ++ ;
		}
		itm++;
	}
	itm = predm.begin();
	cout <<"------ predecessors same machine--------\n";
	while( itm != predm.end() ){
		cout <<"----";
		itm->first->print_node();
		cout<<"-----\n";
		set< osnode *>::iterator ito = itm->second.begin();
		while ( ito != itm->second.end() ){
			(*ito)->print_node();
			cout <<"\n";
			ito ++ ;
		}
		itm++;
	}
}
void osarcsFr::print_rj(){
	map <osnode *, double>::iterator it = r.begin();
	while( it != r.end() ){
		it->first->print_node();
		cout <<"---->"<<it->second;
		cout<<"\n";
		it++;
	}
}
double osarcsFr::LB_C_max(){
	double cmax = 0;
	if( r.empty() ){
		rqCheck();
	}
	map <osnode *, double>::iterator it = r.begin();
	while( it != r.end() ){
		if( cmax < it->second ){
			cmax = it->second;
		}
		it++;
	}
	return cmax;
}
// return the lower bound for c_j_k, o[JOBN][MACHN] is the operation time matrix
double osarcsFr::LB_C_k( int k, float o[JOBN][MACHN] ){
	if( r.empty() ){
		rqCheck();
	}
	double ck[JOBN];
	for( int i = 0; i < JOBN ; i ++ ){
		double sum=0;
		for( int j = 0 ; j < MACHN ; j ++ ){
			sum += o[i][j];
		}
		ck[i]= sum;
	}
	// noticing that for jobs not mentioned here, the C_j's lower bound can be ituitively set as sum(p_jm)
	map <osnode *, double>::iterator it = r.begin();
	while( it != r.end() ){
		int job = it->first->getJob();
		if( ck[job] < ( it->second + it->first->getTime() ) ){
			ck[job] = it->second + it->first->getTime();
		}
		it++;
	}
	double tmpt = 0;
	for( int i = 0; i < JOBN ; i ++ ){
		tmpt += pow(ck[i],k);
	}
	return tmpt;
}
/*
*
* -----------------------------methods for osunits class-----------------------------
*/
// generate the nodes in the unit
void osunits::generateNodes(){
	deque < osnode* > allnodes;
	asize = 0;
	if( ul.empty()){
		cout<< "the bloock is empty!";
		exit(0);
	}
	deque <osarc>::iterator tn = ul.begin();
	osnode * node = tn->getI();
	u1 = node;
	asize = 0;
	freedom = 0;

	set< osnode * > fixedhead;
	set< osnode * > fixedtail;
	while( tn != ul.end() ){
		node = tn->getJ();
		allnodes.push_back( node );
		if( !tn->getdeter() ){
			freedom ++;
		}else{
			fixedhead.insert(tn->getI());
			fixedtail.insert(tn->getJ());
		}
		asize ++;
		tn ++;
	}
	um = node;
	ulnodes = allnodes;
	ulnodes.push_front(u1);

	// generating the arcs in F and L
	F = Fgenerate(ulnodes);
	L = Lgenerate(ulnodes);
	
	EB.clear();
	EA.clear();
	if( this->freesize() == 0 ){
	}
	deque <osnode * > ::iterator it = allnodes.begin();
	while( it != allnodes.end() ){
		if( fixedtail.find(*it) != fixedtail.end() ){
			it++;
			continue;
		}
		deque <osnode * > tq = ulnodes;
		osnode * u = *it;
		tq.erase (remove(tq.begin(), tq.end(), u), tq.end());
		tq.push_front ( u );
		osarcSet e = Fgenerate(tq);
		EB.insert( pair<osnode *, osarcSet>(*it, e) );
		it++;
	}
	allnodes = ulnodes;
	allnodes.pop_back();
	
	it = allnodes.begin();
	while( it != allnodes.end() ){
		if( fixedhead.find(*it) != fixedhead.end() ){
			it++;
			continue;
		}
		deque <osnode * > tq = ulnodes;
		osnode * u = *it;
		tq.erase (remove(tq.begin(), tq.end(), u), tq.end());
		tq.push_back ( u );
		osarcSet e = Lgenerate(tq);
		EA.insert( pair<osnode *, osarcSet>(*it, e) );
		it++;
	}

}
void osunits::print_nodes(){
	if( NULL == u1 ){
		generateNodes();
	}
	cout << "\nfirst:";
	u1->print_node();
	deque <osnode*>::iterator tn = ulnodes.begin();
	cout << "\nallnodes:\n";
	while( tn != ulnodes.end() ){
		(*tn)->print_node();
		cout << "\n";
		tn ++;
	}
	cout << "last:";
	um->print_node();
	cout<<"\n--------------\n";
}
void osunits::print_FLE(){
	cout<< "-----------F-----------\n";
	printOsarcSet(F);
	cout<< "\n-----------L-----------\n";
	printOsarcSet(L);
	cout<< "\n-----------EB-----------\n";
	map < osnode* , osarcSet >::iterator it = EB.begin();
	while( it != EB.end()){
		it->first->print_node();
		cout << ":\n";
		printOsarcSet(it->second);
		it++;
	}
	cout<< "\n-----------EA-----------\n";
	it = EA.begin();
	while( it != EA.end()){
		it->first->print_node();
		cout << ":\n";
		printOsarcSet(it->second);
		it++;
	}
}
/*
*
* -----------------------------methods for osselection class-----------------------------
*/


osselection::osselection( osarcSet as ){
	arcs = as;
	osarcSet::iterator it = arcs.begin();
	while( it != arcs.end() ){
		osnode head = *(it->getI());
		osnode tail = *(it->getJ());
		if( forwardset.find(head) == forwardset.end() ){
			osarcSet oss;
			oss.insert( *it);
			forwardset.insert( pair <osnode, osarcSet> (head,oss));
		}else{
			forwardset.find(head)->second.insert( *it);
		}
		if( backwardset.find(tail) == backwardset.end() ){
			osarcSet oss;
			oss.insert( *it);
			backwardset.insert( pair <osnode, osarcSet> (tail,oss));
		}else{
			backwardset.find(tail)->second.insert( *it);
		}
		it ++;
	}
	circle = false;
};
// insertions
void osselection::insert( osarc arc){
		arcs.insert(arc);
		// modify forward set
		if( forwardset.end() == forwardset.find( (*arc.getI()) )){
			osarcSet tmpt = *(new osarcSet);
			tmpt.insert(arc);
			forwardset.insert(  pair <osnode, osarcSet> ((*arc.getI()),tmpt));
			
		}else{
			forwardset.find( (*arc.getI()) )->second.insert( arc );
		}
		// modify backward set
		if( backwardset.end() == backwardset.find( (*arc.getJ()) )){
			osarcSet tmpt = *(new osarcSet);
			tmpt.insert(arc);
			backwardset.insert(  pair <osnode, osarcSet> ((*arc.getJ()),tmpt));
		}else{
			backwardset.find( (*arc.getJ()) )->second.insert( arc );
		}
	}
void osselection::insert( int arc[2*JOBN*MACHN - JOBN - MACHN][4] , osnode *oss[JOBN][MACHN] ){
	osarcSet asarcs[JOBN][MACHN];// forward arcs
	osarcSet asarcs2[JOBN][MACHN];// backward arcs
	for( int i = 0 ; i < 2*JOBN*MACHN - JOBN - MACHN ; i ++){
		osarc tmptarc = osarc(oss[arc[i][0]][arc[i][1]],oss[arc[i][2]][arc[i][3]]);
		arcs.insert(tmptarc);
		asarcs[arc[i][0]][arc[i][1]].insert(tmptarc);
		asarcs2[arc[i][2]][arc[i][3]].insert(tmptarc);
	}
	for(int i = 0 ; i < JOBN ; i++)
	{
		for( int j = 0; j < MACHN ; j++){
			forwardset.insert( pair <osnode, osarcSet> (*(oss[i][j]),asarcs[i][j]));
			backwardset.insert( pair <osnode, osarcSet> (*(oss[i][j]),asarcs2[i][j]));
		}
	}
}

// delete one arc from the graph
void osselection::del( osarc arc ){
	arcs.erase(arc);
	if ( forwardset.end() != forwardset.find( (*arc.getI()) )){
		forwardset.find( (*arc.getI()) )->second.erase(arc);
		backwardset.find( (*arc.getJ()) )->second.erase(arc);
	}else{
		cout<< "the arc is not in the graph";
	}
}

// generating critical path length and all the related values
bool osselection::generate_critical_path( ){
	deque <osnode> C;
	map <osnode, double > distance;
	map <osnode, osarcSet> tmpt;
	map <osnode, osnode> criticalpa;// denote the updated arces

	deque <osnode>::iterator itt;
	map <osnode, osarcSet> ::iterator it = forwardset.begin();
		
	for (; it != forwardset.end();)
	{
		osnode tn = it->first;
		distance.insert( pair<osnode,double> ( tn , tn.getTime() ));
		// add alll the node with forward set but without backward set
		if( backwardset.find( tn ) == backwardset.end() ){
			C.push_back( tn );
			backwardset.insert(pair <osnode, osarcSet> (tn,(* new osarcSet) ) );
		}
		it++;
	}

	tmpt = backwardset;
	criticalpath.clear(); // clear the critical path
	while( ! tmpt.empty() )
	{
		it = tmpt.begin();
		for (; it != tmpt.end();)
		{// get all the nodes without backward node and put into C
			if( it->second.empty() ){
				C.push_back(it->first);// add all the starting nodes into set C
			}
			it ++;
		}
		if( C.empty() ){	// !notice: this is only for rougbust consideration
								// In practice, this should never happen.
			cout << "ERROR:A circle occurs in the graph\n";
			return false;
		}
		
		itt = C.begin();

		for (; itt != C.end();)
		{
			// for each node without backward node
			osnode head = *itt;
			osarcSet ta = forwardsubset(head);
			osarcSet ::iterator ittt = ta.begin();
			for(; ittt != ta.end();)
			{
				// for each node in the forward node set
				osarc ar = *ittt;
				osnode tail = *(ar.getJ());
				if(  distance.find(tail) == distance.end() ){
					distance.insert( pair<osnode,double> ( tail , tail.getTime() ));
					forwardset.insert(pair <osnode, osarcSet> (tail,(* new osarcSet) ) );
				}
				else{
					if( distance.find(tail)->second < tail.getTime() + distance.find(head)->second ){
						// update the longest distance
						distance.find(tail)->second = tail.getTime() + distance.find(head)->second;
						if( criticalpa.find(tail)!= criticalpa.end() ){
							criticalpa.find(tail)->second = head;
						}else{
							criticalpa.insert( pair<osnode,osnode>( tail, head ) );// renew the arc in the critical path
						}
					}
				}
				// remove the arc from tail node's backward arc set
				if( tmpt.find(tail) != tmpt.end() ){
					tmpt.find(tail)->second.erase(ar);
				}
				ittt ++;
			}
			itt ++;
			tmpt.erase(head);
		}// end of loop for itt
		C.clear();
	}// * end of while loop


	// generating the c_j for each job
	critical.clear();// clear all the value of c_ks
	osnode lastnode;
	double lastnode_path = 0;
	map <osnode, double >::iterator itd = distance.begin();
	for (; itd != distance.end();)
	{	
		osnode tn = itd->first;
		if( lastnode_path < itd->second ){
			lastnode = tn;
			lastnode_path = itd->second;
		}
		if( critical.find(tn.getJob()) == critical.end()){
			critical.insert( pair<int, double>(tn.getJob(),itd->second));
		}else{
			if( critical.find(tn.getJob())->second < itd->second ){
				critical.find(tn.getJob())->second = itd->second;
			}
		}
		//tn.print_node();
		//cout<<"\tdistance:"<< itd->second <<"\n";
		itd++;
	}

	// get the criticalpath
	osnode tn = lastnode;
	criticalpath.clear();
	while( criticalpa.find(tn) != criticalpa.end() ){
		osnode next = criticalpa.find(tn)->second;
		criticalpath.push_front( *arcs.find( osarc(&next, &tn)) );
		tn = next;
	}

	/*   
	*	generating the units in the critical path
	*/
	units.clear();
	deque <osarc> ::iterator itc = criticalpath.begin();
		

	osunits osettmpt; // denote a tmpt set
	while( itc != criticalpath.end() ){
		if( osettmpt.empty() ){
			osettmpt.insert( *itc );
		}else{
			if( osettmpt.samemach() ){
				if( itc->getI()->getMach() == itc->getJ()->getMach()  ){
					osettmpt.insert(*itc); // if the current arc are of the same machine
				}else{
					units.push_back(osettmpt);
					osettmpt.clear();
					osettmpt.insert(*itc);
				}
			}
			else{
				if( itc->getI()->getJob() == itc->getJ()->getJob()  ){
					osettmpt.insert(*itc); // if the current arc are of the same job
				}else{
					units.push_back(osettmpt);
					osettmpt.clear();
					osettmpt.insert(*itc);
				}
			}
		}
		itc ++; // move to the next arc in the critical path
	}
	if( ! osettmpt.empty() ){ // add the last unit to the units set
		units.push_back(osettmpt);
	}
	//std::sort
	osunitSorter osort;
	std::sort(units.begin(),units.end(), osort);
	// print the C_j for job j
	/*map <int, int>::iterator itm = critical.begin();
	for (; itm != critical.end();)
	{
		cout<<"job:"<< itm->first;
		cout<<"\tdistance:"<< itm->second <<"\n";
		itm++;
	}*/
	return true;
}// * end of function generate_critical_path


/*			Branching scheme						*/
deque<osarcSet> osselection::Sj(){
	deque <osarcSet> s;
	deque < osunits >::iterator it = units.begin();
	osarcSet Rtmp;	// denoting R_1 + R_2 + ... R_m-1
	while( it != units.end() ){
		// For each unit
		// ********* R_i ***************
		map < osnode* , osarcSet >::iterator itb = it->EB.begin();
		while( itb != it->EB.end() ){
			// for all the elements in EB
			osarcSet st;	// for the S_a and S_b
			osarcSet targ = itb->second;
			st = Rtmp;
			st.insert(targ.begin(),targ.end());
			// check whether there is a loop
			if( !osarcsFr(st, true).circleCheck()  ){
				s.push_back(st);
			}
			itb ++;
		}
		Rtmp.insert(it->F.begin(),it->F.end());
		// ********* R_i+1 **************
		if( it->size() == 1 ){ // when there is only one arc
			it ++ ;
			continue;
		}
		itb = it->EA.begin();
		while( itb != it->EA.end() ){
			// for all the elements in EA
			osarcSet st;	// for the S_a and S_b
			osarcSet targ = itb->second;
			st = Rtmp;
			st.insert(targ.begin(),targ.end());
			// check whether there is a loop
			if( !osarcsFr(st, true).circleCheck() ){
				s.push_back(st);
			}
			itb ++;
		}
		Rtmp.insert(it->L.begin(),it->L.end());
		it ++ ;
	}
	return s;
}

/*			value calculating methods				*/
//**************** get the objective function value C^k 
double osselection::getC_k( int k ){
	double tmpt = 0;
	if( critical.empty() ){
		generate_critical_path();
	}
	// in case of loop
	if( critical.empty() ){
		return INFNTY;
	}
	map <int, double>::iterator itm = critical.begin();
	for (; itm != critical.end();)
	{
		tmpt += pow(itm->second,k);
		itm++;
	}
	return tmpt;
}
//**************** get the objective function value Cmax
double osselection::getC_max( )
{
	double tmpt = 0;
	if( critical.empty() ){
		generate_critical_path();
	}
	// in case of loop
	if( critical.empty() ){
		return INFNTY;
	}
	map <int, double>::iterator itm = critical.begin();
	for (; itm != critical.end();)
	{
		if( tmpt < itm->second ){
			tmpt = itm->second;
		}
		itm++;
	}
	
	return tmpt;
}

/*			printing methods						*/
//**************** print all the arcs
void osselection::print(){
	printOsarcSet( arcs );
	cout<<"----------end------------\n";
}
//**************** print the critical path
void osselection::print_critical() 
{
	if( criticalpath.empty() ){
		generate_critical_path();
	}
	deque <osarc> ::iterator it = criticalpath.begin();
	while( it != criticalpath.end() ){
		it->print_arc();
		cout<<"\n";
		it ++;
	}
}
//**************** print all the units
void osselection::print_units(){
	if( units.empty() ){
			generate_critical_path();
		}
		deque < osunits > ::iterator it = units.begin();
		while( it != units.end() ){
			it->print_nodes();
			it->print();			
			it ++;
		}
	} 
//**************** print the forward arcs of node on
void osselection::print_subset(osnode on){
		
	cout <<"----------- for node ";
	on.print_node();
	cout <<"-----------\n";
	cout <<"---forward---\n";
	osarcSet os = forwardsubset(on);
	printOsarcSet( os );
	cout <<"---backward---\n";
	printOsarcSet( os );
}