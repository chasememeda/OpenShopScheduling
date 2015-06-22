#include "bb.h"
/*
*	this cpp file is for a heuristic algorithm to generate a feasible
*	solution given the fixed arcs
*
*/

void osarcsFr::levelGeneration(){
	level.clear();
	map <osnode *, osarcSet>::iterator it = backwardset.begin();
	// check the special nodes
	while( it != backwardset.end() ){
		osnode * tail = it->first;
		osarcSet preds = it->second;
		if( preds.size() == 1 ){
			// if a node is only with one pred
			osnode * head = preds.begin()->getI();
			if( backwardset.find(head) != backwardset.end() ){
				level.insert(pair<osnode *,int >(tail,1) );
			}else{
				if( forwardset.find(tail) == forwardset.end() ){
					level.insert(pair<osnode *,int >(tail,2) );
				}else{
					level.insert(pair<osnode *,int >(tail,3) );
				}
			}
			it++;
			continue;
		}
		bool flag = false;
		osarcSet::iterator itp = preds.begin();
		while( itp != preds.end() ){
			osnode * head = preds.begin()->getI();
			if( backwardset.find(head) != backwardset.end() ){
				level.insert(pair<osnode *,int >(tail,1) );
				flag = true;
				break;
			}
			itp ++;
		}
		if(!flag){
			if( forwardset.find(tail) == forwardset.end() ){
					level.insert(pair<osnode *,int >(tail,2) );
			}
		}
		it++;
	}
};
osselection osarcsFr::graphGenerate( osnode * oss[JOBN][MACHN] ){
	set <int> mchns;
	set <int> jobs;

	for( int i = 0; i < MACHN ; i ++ ){
		mchns.insert( i );
	}
	for( int i = 0; i < JOBN ; i ++ ){
		jobs.insert( i );
	}
	osarcSet arcs = arcsfr;

	map <osnode*, int> levelt = level;	
	//osarcSet arcs2 = graphGenerateLine(true,2,oss);
	//printOsarcSet(arcs2);
	while( !levelt.empty() ){
			
		map <osnode*, int>::iterator itthis = levelt.begin();
		map <osnode*, int>::iterator itm = levelt.begin();
		while( itm != levelt.end() ){
		//1. find the one with the priority to start
			// higher lever prefered
			if( itm->second < itthis->second ){
				itthis = itm;		
			}else{
			// smaller number prefered
				if( itm->second == itthis->second && (*itm->first) < (*itthis->first) ){
					itthis = itm;
				}
			}
			itm++;
		}
		//2. deal with the job line
		int jobid = itthis->first->getJob();
		int machid = itthis->first->getMach();

		if(!jobs.empty() &&  jobs.find(jobid) != jobs.end() ){
			osarcSet arcst = graphGenerateLine(true,jobid,oss);
			arcs.insert(arcst.begin(),arcst.end());
			jobs.erase( jobid );
				// erase the other nodes with higher level and a finished machine line
			for( int i = 0; i < MACHN ; i ++ ){
				osnode * n = oss [jobid][i];
				if( mchns.find( i ) == mchns.end() ){
					levelt.erase(n);
				}
			}
		}// * end of if the jobs line is not finished
		//3. deal with the machine line
		if( !mchns.empty() && mchns.find(machid) != mchns.end() ){
			osarcSet arcst = graphGenerateLine(false,machid,oss);
			arcs.insert(arcst.begin(),arcst.end());
			mchns.erase( machid );
				// erase the other nodes with higher level and a finished job line
			for( int i = 0; i < JOBN ; i ++ ){
				osnode * n = oss [i][machid];
				if( jobs.find( i ) == jobs.end() ){
					levelt.erase(n);
				}
			}
		}// * end of if the machine line is not finished
	}// end of level is empty;
	set <int>::iterator itint = jobs.begin();
	while(itint != jobs.end()){
		osarcSet arcst = graphGenerateLine(true,*itint,oss);
		arcs.insert(arcst.begin(),arcst.end());
		itint ++;
	}	
	itint = mchns.begin();
	while(itint != mchns.end()){
		osarcSet arcst = graphGenerateLine(false,*itint,oss);
		arcs.insert(arcst.begin(),arcst.end());
		itint ++;
	}	
	
	osselection fea = osselection(arcs);
	return fea;
}
osarcSet osarcsFr::graphGenerateLine( bool job, int id, osnode * oss[JOBN][MACHN]){
	vector< osnode * > que;
	map <osnode *, set< osnode *> > succ;
	map <osnode*, int> levelt;
	if(job){
		for( int i = 0; i < MACHN ; i ++ ){
			osnode * n = oss [id][i];
			que.push_back( n );
			if( level.find(n) != level.end() ){
				levelt.insert(pair<osnode*,int>(n,level.find(n)->second));
			}
			if( succj.find(n) != succj.end() ){
				succ.insert( pair < osnode*, set <osnode*> > ( n, succj.find(n)->second));
			}
		}
	}else{
		for( int i = 0; i < JOBN ; i ++ ){
			osnode * n = oss [i][id];
			que.push_back( n );
			if( level.find(n) != level.end() ){
				levelt.insert(pair<osnode*,int>(n,level.find(n)->second));
			}
			if( succm.find(n) != succm.end() ){
				succ.insert( pair< osnode*, set <osnode*> > ( n, succm.find(n)->second));
			}
		}
	}

	vector< osnode * >::iterator it;
	if( (!succ.empty()) || (!levelt.empty())  ){
		it = que.begin() ;
		osnode * tail = que.back();
		int clevel = 4;
		// set the high level nodes to the end of the nodes
		map <osnode*, int>::iterator itlvl = levelt.begin();
		while( itlvl != levelt.end() ){
			if( itlvl->second < clevel ){
				if( succ.find(itlvl->first) == succ.end() ){
						// the node has no successors
						tail = itlvl->first;
						clevel = itlvl->second;
					}
			}
			itlvl++;
		}
		if( clevel < 4 ){
			que.erase( remove(que.begin(), que.end(), tail) );
		}
		it = que.begin();
		vector< osnode * > que2 = que;
		// changing the positions of the nodes according to the 
		while( it != que.end() ){
			osnode * node = *it;
			if( succ.find( node ) == succ.end() ){
				it++;
				continue;
			}
			vector< osnode * >::iterator getit = que2.end();
			vector< osnode * >::iterator thisit = find(que2.begin(),que2.end(),node);
			set < osnode * > tails = succ.find( node )->second;
			set < osnode * >::iterator itt = tails.begin();
			// for all the succs of this node
			// find the first one
			while( itt != tails.end() ){
				vector< osnode * >::iterator getitt = find(que2.begin(),que2.end(),*itt);
				if( getit > getitt ){
					getit = getitt;
				}
				itt ++;
			}
			// if is behind one of the tails, put it before to the earliest one
			if( thisit > getit ){
				que2.erase( thisit );
				que2.insert(getit,node);
			}
			it++;
		}
		que = que2;
		if( clevel < 4 ){
			que.push_back( tail );
		}
	}// * end of if there is no arc fixed in this problem

	osarcSet arcs;
	// arcs generation
	it = que.begin();
	while( it  != que.end() ){
		osnode * head = *it;
		it++;
		if( it == que.end() ){
			break;
		}
		osnode * tail = *it;
		arcs.insert( osarc(head,tail));
	}
	return arcs;
}
osselection feasible( Setosarc arcs )
{
	osselection fea = *(new osselection);
	// for certain method, we can generate a suitable feasible solution

	return fea;
}