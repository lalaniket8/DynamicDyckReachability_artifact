#include "DSU/DSU.h"
#include <algorithm>
// #include <cassert>

void DSU::init(int n){
	this->N = n;
	parent_pos.resize(n);
	vtxatpos.resize(n);
	posofvtx.resize(n);
	dsuContent.resize(n);
	for(int i=0;i<n;i++){
		parent_pos[i]=-1;
		vtxatpos[i]=i;
		posofvtx[i]=i;
		dsuContent[i] = {i};
	}
}

int DSU::getN(){
	return this->N;
}

//returns you the vertexAAid of the root
int DSU::root(int vtx){
	// cout << "***root(" << vtx << "):" << endl;
	if(vtx >= posofvtx.size()){
		cout << "vtx >= posofvtx.size() vtx >= posofvtx.size()=" << posofvtx.size() << " vtx=" << vtx << endl;
		exit(1);
	}
	int calc_root = vtxatpos[rootpos(posofvtx[vtx])];
	// cout << calc_root << endl;
	return calc_root;
	// return (parent_pos[v]<0?v:parent_pos[v]=DSU::root(parent_pos[v]));
}

int DSU::getVerticesCount(){
	return this->N;
	// return vtxatpos.size();
}

// returns you the position of the root
int DSU::rootpos(int pos){ //Time complexity? looks like O(1) amortized //pos is not vertexID
	// return parent_pos[pos]<0?pos:parent_pos[pos]=rootpos(parent_pos[pos]);
	
	if(parent_pos[pos]<0){
		return pos;
	}else{
		// int size = parent_pos.size();
		// if(size <= pos){cout << "parent_pos.size()[" << size << "] <= pos[" << pos << "]" << endl; exit(1);}
		// int prevRootpos = parent_pos[pos];
		// return rootpos(parent_pos[pos]);
		parent_pos[pos] = rootpos(parent_pos[pos]);

		// // nodes parent_pos[pos] -> pos
		// // cout << nodes[vtxatpos[pos]].size() << "[move from " << vtxatpos[pos] << " to " << vtxatpos[parent_pos[pos]] << "]" << endl; 
		// // for(auto iter = dsuContent[vtxatpos[pos]].begin(); iter != dsuContent[vtxatpos[pos]].end(); iter++){
		// for(const int& iter : dsuContent[vtxatpos[pos]]){
		// 	//cout << "[rootpos]Transfering " << *iter << " from " << vtxatpos[pos] << " to " << vtxatpos[parent_pos[pos]] << endl; 
		// 	dsuContent[vtxatpos[parent_pos[pos]]].push_back(iter);
		// 	// if(posofvtx[iter] != parent_pos[posofvtx[iter]]){
		// 	// 	rootpos(posofvtx[iter]);
		// 	// }
		// }
		// dsuContent[vtxatpos[pos]].clear();

		// dsuContent[vtxatpos[parent_pos[pos]]].splice(dsuContent[vtxatpos[parent_pos[pos]]].end(), dsuContent[vtxatpos[prevRootpos]]);
		// if(prevRootpos > -1)
			// dsuContent[parent_pos[pos]].splice(dsuContent[parent_pos[pos]].end(), dsuContent[prevRootpos]);
		// dsuContent[vtxatpos[parent_pos[pos]]].splice(dsuContent[vtxatpos[parent_pos[pos]]].end(), dsuContent[vtxatpos[pos]]);
		// dsuContent[parent_pos[pos]].splice(dsuContent[parent_pos[pos]].end(), dsuContent[pos]);
		
		return parent_pos[pos];
	}
}

// gives the rank of the vtx r
int DSU::rank(int r){
	// assert(r==root(r));
	return -1*parent_pos[posofvtx[r]];
}


// merge by height
/*
int DSU::merge(int a,int b){// a,b are vertexAAID
	// assert(a==root(a) and b==root(b));
	//precondition a and b are root
	// if( (a=root(a)) == (b=root(b)) )
	if(a==b)
		return a;
	int posa = posofvtx[a];
	int posb = posofvtx[b];
	N--;
	if(parent_pos[posa]>parent_pos[posb]){
		int prevRootpos_a = parent_pos[posa];
		parent_pos[posa] = posb;

		// //dsuContent a -> b
		// // for(auto iter = dsuContent[a].begin(); iter != dsuContent[a].end(); iter++){
		// for(const auto& v : dsuContent[a]){	
		// 	dsuContent[b].push_back(v);
		// 	// if(!HUSH) cout << "[merge]Transfering " << *iter << " from " << a << " to " << b << endl;
		// 	// parent_pos[posofvtx[v]] = parent_pos[posa];
		// }
		// dsuContent[a].clear();

		// dsuContent[b].splice(dsuContent[b].end(), dsuContent[vtxatpos[prevRootpos_a]]);
		// dsuContent[b].splice(dsuContent[b].end(), dsuContent[a]);
		// dsuContent[posb].splice(dsuContent[posb].end(), dsuContent[posa]);
		if(prevRootpos_a > -1)
			dsuContent[posb].splice(dsuContent[posb].end(), dsuContent[prevRootpos_a]);

		return b;
	}

	if(parent_pos[posa]==parent_pos[posb])
		parent_pos[posa]--;
	int prevRootpos_b = parent_pos[posb];
	parent_pos[posb] = posa;

	// //dsuContent posb -> posa
	// // for(auto iter = dsuContent[b].begin(); iter != dsuContent[b].end(); iter++){
	// for(const auto& v : dsuContent[b]){	
	// 	dsuContent[a].push_back(v);
	// 	// if(!HUSH) cout << "Transfering " << iter << " from " << b << " to " << a << endl;
	// 	// parent_pos[posofvtx[v]] = parent_pos[posb];
	// }
	// dsuContent[b].clear();
	
	// dsuContent[a].splice(dsuContent[a].end(), dsuContent[b]);
	if(prevRootpos_b > -1)
		dsuContent[posa].splice(dsuContent[posa].end(), dsuContent[prevRootpos_b]);
	// dsuContent[posa].splice(dsuContent[posa].end(), dsuContent[posb]);

	return a;
}
*/

void DSU::hardmerge(int r,int a){//r,a are vertexAAID
	// assert(r==root(r) and a==root(a));
	
	if(a==r)
		return;
	int posr = posofvtx[r];
	int posa = posofvtx[a];
	N--;
	if(parent_pos[posa] > -1 || parent_pos[posr] > -1){
		cout << "ERROR: hardmerge of non roots, parent_pos[posa]:" << parent_pos[posa] << " parent_pos[posr]:" << parent_pos[posr] << endl;
		exit(1);
	}
	if(parent_pos[posa]==parent_pos[posr])
		parent_pos[posr]--;
	int prevRootpos_a = parent_pos[posa];
	parent_pos[posa] = posr; 
	
	// //dsuContent a -> r
	// // for(auto iter = dsuContent[a].begin(); iter != dsuContent[a].end(); iter++){
	// for(const auto& v : dsuContent[a]){
	// 	dsuContent[r].push_back(v);
	// 	// if(!HUSH) cout << "[hardmerge]Transfering " << *iter << " from " << a << " to " << r << endl;
	// 	// parent_pos[posofvtx[v]] = parent_pos[posa];
	// }
	// dsuContent[a].clear();

	// dsuContent[r].splice(dsuContent[r].end(), dsuContent[a]);
	// dsuContent[r].splice(dsuContent[r].end(), dsuContent[vtxatpos[prevRootpos_a]]);
	// dsuContent[posr].splice(dsuContent[posr].end(), dsuContent[posa]);
	// if(prevRootpos_a > -1)
		// dsuContent[posr].splice(dsuContent[posr].end(), dsuContent[prevRootpos_a]);
	// else
	dsuContent[posr].splice(dsuContent[posr].end(), dsuContent[posa]);
}

void DSU::exchange(int uid,int vid){
	int posu = posofvtx[uid];
	int posv = posofvtx[vid];
	vtxatpos[posu] = vid;
	vtxatpos[posv] = uid;
	posofvtx[uid] = posv;
	posofvtx[vid] = posu;
}

int DSU::getArraySize(){
	return posofvtx.size();
}


void DSU::display(){
	cout << "parent_pos:" <<endl;
	displayVector(parent_pos);
	cout << "vtxatpos:" << endl;
	displayVector(vtxatpos);
	cout << "posofvtx:" << endl;
	displayVector(posofvtx);
}

void DSU::displayVector(vector<int> v){
	for(int i=0; i<v.size(); i++){
		cout << "index:" << i << " value:" << v[i] << endl;
	}
	cout << endl;
}

list<int>& DSU::getDSUContent(int vertexAA_pos){
	return dsuContent[vertexAA_pos];
}
