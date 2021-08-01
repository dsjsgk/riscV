//#include<bits/stdc++.h>
//#include "translate.hpp"
//using namespace std;

struct RS{
	bool status=0;
	uint vi=0,vj=0,imm=0,idx=0;
	int qi=-1,qj=-1;
	int op=0;
};
//const int MAXN=15;
class Reservation_Station{
	public:
	RS queue[MAXN];
//	int head=0,tail=0;
//	Reservation_Station() {
//		head=tail=0;
//	}
//	int get_nxt(int cur) {
//		cur++;
//		if(cur>=MAXN) cur-=MAXN;
//		return cur;
//	}
//	void push_back(RS tmp) {
//		queue[tail]=tmp;
//		tail=get_nxt(tail);
//	}
	bool find_empty() {
		int pos=-1;
		for(int i=0;i<MAXN;++i) {
			if(queue[i].status==0) pos=i;
		}
		return pos;
	}
	int find_available() {
		int pos=-1;
		for(int i=0;i<MAXN;++i) {
			if(queue[i].status==1) {
				if(queue[i].qi!=-1||queue[i].qj!=-1) continue;
				pos=i; 
			}
		}
		return pos;
	}
	void update(int idx,uint res) {
		for(int i=0;i<MAXN;++i) {
			if(queue[i].status==1) {
				if(queue[i].qi==idx) queue[i].vi=res,queue[i].qi=-1;
				if(queue[i].qj==idx) queue[i].vj=res,queue[i].qj=-1;
			}
		}
	}
};
