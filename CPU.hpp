#include<bits/stdc++.h>
#include "translate.hpp"
#include "RS.hpp"
#include "ROB.hpp"
#include "LSB.hpp"
#include "IQ.hpp"

using namespace std;
struct _register{
	uint v=0;
	int q=0;
	bool status=0;
};
//uint RS_to_ROB=0,RS_to_ROB_id=MAXN+1;
vector<pair<uint,uint>> up_reg_q,up_reg_v,up_reg_q_ex,up_reg_v_ex;
vector<uint> EX_Run_;

int Number_ROB=MAXN+1;
uint Number_Result; 

vector<ROB> up_ROB,commit_;
vector<RS> up_RS;
vector<LSB> up_LSB;
pair<uint,uint> EX_to_ROB; 
pair<uint,pair<uint,uint>> LSB_to_ROB_pre,LSB_to_ROB;
pair<uint,uint> EX_to_ROB_pre; 
class CPU {
	public :
		unsigned char mem[1001000];
		_register reg_pre[32],reg[32];
		unsigned int pc=0;
		Reservation_Station _RS,_RS_pre;
		Reorder_Buffer _ROB,_ROB_pre;
		Instructions_Queue _IQ,_IQ_pre;
		Load_Store_Buffer _LSB,_LSB_pre;
	uint hex_to_dec(char *s,int l,int r) {
		uint res=0;
		for(int i=l;i<=r;++i) {
			uint tmp;
			res=res*16;
			if(isdigit(s[i])) tmp=s[i]-'0';
			else tmp=s[i]-'A'+10;
			res+=tmp;
		}
		return res;
	}
	void input() {
		char s[111];
		uint pos=0;
		while(scanf("%s",s)!=EOF) {
			if(s[0]=='@') {
				pos=hex_to_dec(s,1,8);
			}
			else {
				mem[pos]=hex_to_dec(s,0,1);
				pos++;
			}
		}
	}
	inline void IF_Run() {
		if(_IQ.isfull()) return ;
		uint tmp=(mem[pc]<<24)+(mem[pc+1]<<16)+(mem[pc+2]<<8)+mem[pc+3];
		pc+=4;
		_IQ.push_back(__translate(tmp));
	}
	//finished;
	inline void ISSUE_Run() {
		if(_IQ_pre.isempty()) return;
		if(_ROB_pre.isfull()) return ;
//		if(_LSB_pre.isfull()) return ;
		int pos=_RS_pre.find_empty();
		if(pos==-1) return ;
		Instructions tmp=_IQ_pre.get_front();
		_IQ.pop_front();//更新当前状态
		switch(tmp.tp) {
			case 0:
			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
			case 6:
			case 7:{
				LSB nw;
				ROB nw2;
				nw.idx=_ROB.tail;
				nw.tp=tmp.tp;
				if(tmp.tp<=4)
				{
					if(reg_pre[tmp.rs1].status) {
						nw.q=reg_pre[tmp.rs1].q; 
					}
					else nw.v=reg_pre[tmp.rs1].v;
					nw.imm=tmp.imm;
					nw.addr=tmp.rd;
					nw2.addr=tmp.rd;
					
					up_reg_q.push_back(make_pair(tmp.rd,nw.idx));
					//lb; 
				}
				else {
					if(reg_pre[tmp.rs1].status) {
						nw.q=reg_pre[tmp.rs1].q; 
					}
					else nw.v=reg_pre[tmp.rs1].v;
					if(reg_pre[tmp.rs2].status) {
						nw.q2=reg_pre[tmp.rs2].q; 
					}
					else nw.v2=reg_pre[tmp.rs2].v;
					nw2.tp=10;
					nw.imm=tmp.imm;
					nw.addr=tmp.rd;
				}
				nw2.ready=0;
				
				up_ROB.push_back(nw2);
				up_LSB.push_back(nw);
				break;
			}
			case 8:{
				RS nw;
				ROB nw2;
				if(reg_pre[tmp.rs1].status) {
					nw.qi=reg_pre[tmp.rs1].q; 
				}
				else nw.vi=reg_pre[tmp.rs1].v;
				if(reg_pre[tmp.rs2].status) {
					nw.qj=reg_pre[tmp.rs2].q; 
				}
				else nw.vj=reg_pre[tmp.rs2].v;
				nw.status=1;
				nw.op=0;//0:+
//				_RS.queue[pos]=nw;
				nw2.ready=0;
				nw.idx=_ROB.tail;
				nw2.addr=tmp.rd;
//				_ROB.push_back(nw2);
				up_ROB.push_back(nw2);
				up_reg_q.push_back(make_pair(tmp.rd,nw.idx));
				up_RS.push_back(nw);
//				reg_pre[tmp.rd].status=1;
//				reg_pre[tmp.rd].q=nw.idx;
				//add
				break;
			}
			case 9:{
				RS nw;
				ROB nw2;
				if(reg_pre[tmp.rs1].status) {
					nw.qi=reg_pre[tmp.rs1].q; 
				}
				else nw.vi=reg_pre[tmp.rs1].v;
				nw.vj=tmp.imm;
				nw.status=1;
				nw.op=0;//0:+
//				_RS.queue[pos]=nw;
				nw2.ready=0;
				nw.idx=_ROB.tail;
				nw2.addr=tmp.rd;
//				_ROB.push_back(nw2);
				up_ROB.push_back(nw2);
				up_reg_q.push_back(make_pair(tmp.rd,nw.idx));
				up_RS.push_back(nw);
//				reg_pre[tmp.rd].status=1;
//				reg_pre[tmp.rd].q=nw.idx;
				//addi;
				break;
			}
			case 10:{
				RS nw;
				ROB nw2;
				if(reg_pre[tmp.rs1].status) {
					nw.qi=reg_pre[tmp.rs1].q; 
				}
				else nw.vi=reg_pre[tmp.rs1].v;
				if(reg_pre[tmp.rs2].status) {
					nw.qj=reg_pre[tmp.rs2].q; 
				}
				else nw.vj=reg_pre[tmp.rs2].v;
				nw.status=1;
				nw.op=1;//1:-
//				_RS.queue[pos]=nw;
				nw2.ready=0;
				nw.idx=_ROB.tail;
				nw2.addr=tmp.rd;
//				_ROB.push_back(nw2);
				up_ROB.push_back(nw2);
				up_reg_q.push_back(make_pair(tmp.rd,nw.idx));
				up_RS.push_back(nw);
//				reg_pre[tmp.rd].status=1;
//				reg_pre[tmp.rd].q=nw.idx;
				//sub
				break;
			}
			case 11:{
				
				//lui;
				break;
			}
			case 12:{
				
				//apuic;
				break;
			}
			case 13:{
				RS nw;
				ROB nw2;
				if(reg_pre[tmp.rs1].status) {
					nw.qi=reg_pre[tmp.rs1].q; 
				}
				else nw.vi=reg_pre[tmp.rs1].v;
				if(reg_pre[tmp.rs2].status) {
					nw.qj=reg_pre[tmp.rs2].q; 
				}
				else nw.vj=reg_pre[tmp.rs2].v;
				nw.status=1;
				nw.op=2;//2:^
//				_RS.queue[pos]=nw;
				nw2.ready=0;
				nw.idx=_ROB.tail;
				nw2.addr=tmp.rd;
//				_ROB.push_back(nw2);
				up_ROB.push_back(nw2);
				up_reg_q.push_back(make_pair(tmp.rd,nw.idx));
				up_RS.push_back(nw);
//				reg_pre[tmp.rd].status=1;
//				reg_pre[tmp.rd].q=nw.idx;
				//xor
				break;
			}
			case 14:{
				RS nw;
				ROB nw2;
				if(reg_pre[tmp.rs1].status) {
					nw.qi=reg_pre[tmp.rs1].q; 
				}
				else nw.vi=reg_pre[tmp.rs1].v;
				nw.vj=tmp.imm;
				nw.status=1;
				nw.op=2;//2:^
//				_RS.queue[pos]=nw;
				nw2.ready=0;
				nw.idx=_ROB.tail;
				nw2.addr=tmp.rd;
//				_ROB.push_back(nw2);
				up_ROB.push_back(nw2);
				up_reg_q.push_back(make_pair(tmp.rd,nw.idx));
				up_RS.push_back(nw);
//				reg_pre[tmp.rd].status=1;
//				reg_pre[tmp.rd].q=nw.idx;
				//xori
				break;
			}
			case 15:{
				RS nw;
				ROB nw2;
				if(reg_pre[tmp.rs1].status) {
					nw.qi=reg_pre[tmp.rs1].q; 
				}
				else nw.vi=reg_pre[tmp.rs1].v;
				if(reg_pre[tmp.rs2].status) {
					nw.qj=reg_pre[tmp.rs2].q; 
				}
				else nw.vj=reg_pre[tmp.rs2].v;
				nw.status=1;
				nw.op=3;//3:|
//				_RS.queue[pos]=nw;
				nw2.ready=0;
				nw.idx=_ROB.tail;
				nw2.addr=tmp.rd;
//				_ROB.push_back(nw2);
				up_ROB.push_back(nw2);
				up_reg_q.push_back(make_pair(tmp.rd,nw.idx));
				up_RS.push_back(nw);
//				reg_pre[tmp.rd].status=1;
//				reg_pre[tmp.rd].q=nw.idx;
				//or
				break;
			}
			case 16:{
				RS nw;
				ROB nw2;
				if(reg_pre[tmp.rs1].status) {
					nw.qi=reg_pre[tmp.rs1].q; 
				}
				else nw.vi=reg_pre[tmp.rs1].v;
				nw.vj=tmp.imm;
				nw.status=1;
				nw.op=3;//3:|
//				_RS.queue[pos]=nw;
				nw2.ready=0;
				nw.idx=_ROB.tail;
				nw2.addr=tmp.rd;
//				_ROB.push_back(nw2);
				up_ROB.push_back(nw2);
				up_reg_q.push_back(make_pair(tmp.rd,nw.idx));
				up_RS.push_back(nw);
//				reg_pre[tmp.rd].status=1;
//				reg_pre[tmp.rd].q=nw.idx;
				//ori
				break;
			}
			case 17:{
				RS nw;
				ROB nw2;
				if(reg_pre[tmp.rs1].status) {
					nw.qi=reg_pre[tmp.rs1].q; 
				}
				else nw.vi=reg_pre[tmp.rs1].v;
				if(reg_pre[tmp.rs2].status) {
					nw.qj=reg_pre[tmp.rs2].q; 
				}
				else nw.vj=reg_pre[tmp.rs2].v;
				nw.status=1;
				nw.op=4;//4:&
//				_RS.queue[pos]=nw;
				nw2.ready=0;
				nw.idx=_ROB.tail;
				nw2.addr=tmp.rd;
//				_ROB.push_back(nw2);
				up_ROB.push_back(nw2);
				up_reg_q.push_back(make_pair(tmp.rd,nw.idx));
				up_RS.push_back(nw);
//				reg_pre[tmp.rd].status=1;
//				reg_pre[tmp.rd].q=nw.idx;
				//and
				break;
			}
			case 18:{
				RS nw;
				ROB nw2;
				if(reg_pre[tmp.rs1].status) {
					nw.qi=reg_pre[tmp.rs1].q; 
				}
				else nw.vi=reg_pre[tmp.rs1].v;
				nw.vj=tmp.imm;
				nw.status=1;
				nw.op=4;//4:&
//				_RS.queue[pos]=nw;
				nw2.ready=0;
				nw.idx=_ROB.tail;
				nw2.addr=tmp.rd;
//				_ROB.push_back(nw2);
				up_ROB.push_back(nw2);
				up_reg_q.push_back(make_pair(tmp.rd,nw.idx));
				up_RS.push_back(nw);
//				reg_pre[tmp.rd].status=1;
//				reg_pre[tmp.rd].q=nw.idx;
				//andi
				break;
			}
			case 19:{
				RS nw;
				ROB nw2;
				if(reg_pre[tmp.rs1].status) {
					nw.qi=reg_pre[tmp.rs1].q; 
				}
				else nw.vi=reg_pre[tmp.rs1].v;
				if(reg_pre[tmp.rs2].status) {
					nw.qj=reg_pre[tmp.rs2].q; 
				}
				else nw.vj=reg_pre[tmp.rs2].v;
				nw.status=1;
				nw.op=5;//5:<<
//				_RS.queue[pos]=nw;
				nw2.ready=0;
				nw.idx=_ROB.tail;
				nw2.addr=tmp.rd;
//				_ROB.push_back(nw2);
				up_ROB.push_back(nw2);
				up_reg_q.push_back(make_pair(tmp.rd,nw.idx));
				up_RS.push_back(nw);
//				reg_pre[tmp.rd].status=1;
//				reg_pre[tmp.rd].q=nw.idx;
				//sll
				break;
			}
			case 20:{
				RS nw;
				ROB nw2;
				if(reg_pre[tmp.rs1].status) {
					nw.qi=reg_pre[tmp.rs1].q; 
				}
				else nw.vi=reg_pre[tmp.rs1].v;
				nw.vj=tmp.imm;
				nw.status=1;
				nw.op=5;//5:<<
//				_RS.queue[pos]=nw;
				nw2.ready=0;
				nw.idx=_ROB.tail;
				nw2.addr=tmp.rd;
//				_ROB.push_back(nw2);
				up_ROB.push_back(nw2);
				up_reg_q.push_back(make_pair(tmp.rd,nw.idx));
				up_RS.push_back(nw);
//				reg_pre[tmp.rd].status=1;
//				reg_pre[tmp.rd].q=nw.idx;
				//slli
				break;
			}
			case 21:{
				RS nw;
				ROB nw2;
				if(reg_pre[tmp.rs1].status) {
					nw.qi=reg_pre[tmp.rs1].q; 
				}
				else nw.vi=reg_pre[tmp.rs1].v;
				if(reg_pre[tmp.rs2].status) {
					nw.qj=reg_pre[tmp.rs2].q; 
				}
				else nw.vj=reg_pre[tmp.rs2].v;
				nw.status=1;
				nw.op=6;//6:>>
//				_RS.queue[pos]=nw;
				nw2.ready=0;
				nw.idx=_ROB.tail;
				nw2.addr=tmp.rd;
//				_ROB.push_back(nw2);
				up_ROB.push_back(nw2);
				up_reg_q.push_back(make_pair(tmp.rd,nw.idx));
				up_RS.push_back(nw);
//				reg_pre[tmp.rd].status=1;
//				reg_pre[tmp.rd].q=nw.idx;
				//srl
				break;
			}
			case 22:{
				RS nw;
				ROB nw2;
				if(reg_pre[tmp.rs1].status) {
					nw.qi=reg_pre[tmp.rs1].q; 
				}
				else nw.vi=reg_pre[tmp.rs1].v;
				nw.vj=tmp.imm;
				nw.status=1;
				nw.op=6;//6:>>
//				_RS.queue[pos]=nw;
				nw2.ready=0;
				nw.idx=_ROB.tail;
				nw2.addr=tmp.rd;
//				_ROB.push_back(nw2);
				up_ROB.push_back(nw2);
				up_reg_q.push_back(make_pair(tmp.rd,nw.idx));
				up_RS.push_back(nw);
//				reg_pre[tmp.rd].status=1;
//				reg_pre[tmp.rd].q=nw.idx;
				//srli
				break;
			}
			case 23:{
				RS nw;
				ROB nw2;
				if(reg_pre[tmp.rs1].status) {
					nw.qi=reg_pre[tmp.rs1].q; 
				}
				else nw.vi=reg_pre[tmp.rs1].v;
				if(reg_pre[tmp.rs2].status) {
					nw.qj=reg_pre[tmp.rs2].q; 
				}
				else nw.vj=reg_pre[tmp.rs2].v;
				nw.status=1;
				nw.op=7;//7:>> tianchong
//				_RS.queue[pos]=nw;
				nw2.ready=0;
				nw.idx=_ROB.tail;
				nw2.addr=tmp.rd;
//				_ROB.push_back(nw2);
				up_ROB.push_back(nw2);
				up_reg_q.push_back(make_pair(tmp.rd,nw.idx));
				up_RS.push_back(nw);
//				reg_pre[tmp.rd].status=1;
//				reg_pre[tmp.rd].q=nw.idx;
				//sra
				break;
			}
			case 24:{
				RS nw;
				ROB nw2;
				if(reg_pre[tmp.rs1].status) {
					nw.qi=reg_pre[tmp.rs1].q; 
				}
				else nw.vi=reg_pre[tmp.rs1].v;
				nw.vj=tmp.imm;
				nw.status=1;
				nw.op=7;//7:>> tianchong
//				_RS.queue[pos]=
				nw2.ready=0;
				nw.idx=_ROB.tail;
				nw2.addr=tmp.rd;
//				_ROB.push_back(nw2);
				up_ROB.push_back(nw2);
				up_reg_q.push_back(make_pair(tmp.rd,nw.idx));
				up_RS.push_back(nw);
//				reg_pre[tmp.rd].status=1;
//				reg_pre[tmp.rd].q=nw.idx;
				//srai
				break;
			}
			case 25:{
				RS nw;
				ROB nw2;
				if(reg_pre[tmp.rs1].status) {
					nw.qi=reg_pre[tmp.rs1].q; 
				}
				else nw.vi=reg_pre[tmp.rs1].v;
				if(reg_pre[tmp.rs2].status) {
					nw.qj=reg_pre[tmp.rs2].q; 
				}
				else nw.vj=reg_pre[tmp.rs2].v;
				nw.status=1;
				nw.op=8;//8: <
//				_RS.queue[pos]=nw;
				nw2.ready=0;
				nw.idx=_ROB.tail;
				nw2.addr=tmp.rd;
//				_ROB.push_back(nw2);
				up_ROB.push_back(nw2);
				up_reg_q.push_back(make_pair(tmp.rd,nw.idx));
				up_RS.push_back(nw);
//				reg_pre[tmp.rd].status=1;
//				reg_pre[tmp.rd].q=nw.idx;
				//slt
				break;
			}
			case 26:{
				RS nw;
				ROB nw2;
				if(reg_pre[tmp.rs1].status) {
					nw.qi=reg_pre[tmp.rs1].q; 
				}
				else nw.vi=reg_pre[tmp.rs1].v;
				nw.vj=tmp.imm;
				nw.status=1;
				nw.op=8;//8: <
//				_RS.queue[pos]=nw;
				nw2.ready=0;
				nw.idx=_ROB.tail;
				nw2.addr=tmp.rd;
//				_ROB.push_back(nw2);
				up_ROB.push_back(nw2);
				up_reg_q.push_back(make_pair(tmp.rd,nw.idx));
				up_RS.push_back(nw);
//				reg_pre[tmp.rd].status=1;
//				reg_pre[tmp.rd].q=nw.idx;
				//slti
				break;
			}
			case 27:{
				RS nw;
				ROB nw2;
				if(reg_pre[tmp.rs1].status) {
					nw.qi=reg_pre[tmp.rs1].q; 
				}
				else nw.vi=reg_pre[tmp.rs1].v;
				if(reg_pre[tmp.rs2].status) {
					nw.qj=reg_pre[tmp.rs2].q; 
				}
				else nw.vj=reg_pre[tmp.rs2].v;
				nw.status=1;
				nw.op=9;//9: unsigned <
//				_RS.queue[pos]=nw;
				nw2.ready=0;
				nw.idx=_ROB.tail;
				nw2.addr=tmp.rd;
//				_ROB.push_back(nw2);
				up_ROB.push_back(nw2);
				up_reg_q.push_back(make_pair(tmp.rd,nw.idx));
				up_RS.push_back(nw);
//				reg_pre[tmp.rd].status=1;
//				reg_pre[tmp.rd].q=nw.idx;
				//sltu
				break;
			}
			case 28:{
				RS nw;
				ROB nw2;
				if(reg_pre[tmp.rs1].status) {
					nw.qi=reg_pre[tmp.rs1].q; 
				}
				else nw.vi=reg_pre[tmp.rs1].v;
				nw.vj=tmp.imm;
				nw.status=1;
				nw.op=9;//9: unsigned <
//				_RS.queue[pos]=nw;
				nw2.ready=0;
				nw.idx=_ROB.tail;
				nw2.addr=tmp.rd;
//				_ROB.push_back(nw2);
				up_ROB.push_back(nw2);
				up_reg_q.push_back(make_pair(tmp.rd,nw.idx));
				up_RS.push_back(nw);
//				reg_pre[tmp.rd].status=1;
//				reg_pre[tmp.rd].q=nw.idx;
				//sltui
				break;
			}
			case 29:{
				RS nw;
				ROB nw2;
				if(reg_pre[tmp.rs1].status) {
					nw.qi=reg_pre[tmp.rs1].q; 
				}
				else nw.vi=reg_pre[tmp.rs1].v;
				if(reg_pre[tmp.rs2].status) {
					nw.qj=reg_pre[tmp.rs2].q; 
				}
				else nw.vj=reg_pre[tmp.rs2].v;
				nw.status=1;
				nw.op=9;//9: unsigned <
//				_RS.queue[pos]=nw;
				nw2.ready=0;
				nw.idx=_ROB.tail;
				nw2.addr=tmp.rd;
//				_ROB.push_back(nw2);
				up_ROB.push_back(nw2);
				up_reg_q.push_back(make_pair(tmp.rd,nw.idx));
				up_RS.push_back(nw);
//				reg_pre[tmp.rd].status=1;
//				reg_pre[tmp.rd].q=nw.idx;
				//sltu
				break;
			}
			case 30:{
				RS nw;
				ROB nw2;
				if(reg_pre[tmp.rs1].status) {
					nw.qi=reg_pre[tmp.rs1].q; 
				}
				else nw.vi=reg_pre[tmp.rs1].v;
				nw.vj=tmp.imm;
				nw.status=1;
				nw.op=9;//9: unsigned <
//				_RS.queue[pos]=nw;
				nw2.ready=0;
				nw.idx=_ROB.tail;
				nw2.addr=tmp.rd;
//				_ROB.push_back(nw2);
				up_ROB.push_back(nw2);
				up_reg_q.push_back(make_pair(tmp.rd,nw.idx));
				up_RS.push_back(nw);
//				reg_pre[tmp.rd].status=1;
//				reg_pre[tmp.rd].q=nw.idx;
				//sltui
				break;
			}
			case 31:{
				RS nw;
				ROB nw2;
				if(reg_pre[tmp.rs1].status) {
					nw.qi=reg_pre[tmp.rs1].q; 
				}
				else nw.vi=reg_pre[tmp.rs1].v;
				if(reg_pre[tmp.rs2].status) {
					nw.qj=reg_pre[tmp.rs2].q; 
				}
				else nw.vj=reg_pre[tmp.rs2].v;
				nw.status=1;
				nw.op=10;//10:== pc+
//				_RS.queue[pos]=nw;
				nw2.ready=0;
				nw.idx=_ROB.tail;
				nw2.addr=tmp.rd;
				nw2.tp=1;
				nw2.offset=tmp.imm;
//				_ROB.push_back(nw2);
				up_ROB.push_back(nw2);
				up_RS.push_back(nw);
//				reg_pre[tmp.rd].status=1;
//				reg_pre[tmp.rd].q=nw.idx;
				//beq
				break;
			}
			case 32:{
				RS nw;
				ROB nw2;
				if(reg_pre[tmp.rs1].status) {
					nw.qi=reg_pre[tmp.rs1].q; 
				}
				else nw.vi=reg_pre[tmp.rs1].v;
				nw.vj=tmp.imm;
				nw.status=1;
				nw.op=11;//11: != pc+
//				_RS.queue[pos]=nw;
				nw2.ready=0;
				nw.idx=_ROB.tail;
				nw2.addr=tmp.rd;
				nw2.tp=1;
				nw2.offset=tmp.imm;
//				_ROB.push_back(nw2);
				up_ROB.push_back(nw2);
				up_RS.push_back(nw);
//				reg_pre[tmp.rd].status=1;
//				reg_pre[tmp.rd].q=nw.idx;
				//bne
				break;
			}
			case 33:{
				RS nw;
				ROB nw2;
				if(reg_pre[tmp.rs1].status) {
					nw.qi=reg_pre[tmp.rs1].q; 
				}
				else nw.vi=reg_pre[tmp.rs1].v;
				nw.vj=tmp.imm;
				nw.status=1;
				nw.op=12;//12: < pc+
//				_RS.queue[pos]=nw;
				nw2.ready=0;
				nw.idx=_ROB.tail;
				nw2.addr=tmp.rd;
				nw2.tp=1;
				nw2.offset=tmp.imm;
//				_ROB.push_back(nw2);
				up_ROB.push_back(nw2);
				up_RS.push_back(nw);
//				reg_pre[tmp.rd].status=1;
//				reg_pre[tmp.rd].q=nw.idx;
				//blt
				break;
			}
			case 34:{
				RS nw;
				ROB nw2;
				if(reg_pre[tmp.rs1].status) {
					nw.qi=reg_pre[tmp.rs1].q; 
				}
				else nw.vi=reg_pre[tmp.rs1].v;
				nw.vj=tmp.imm;
				nw.status=1;
				nw.op=13;//13: >= pc+
//				_RS.queue[pos]=nw;
				nw2.ready=0;
				nw.idx=_ROB.tail;
				nw2.addr=tmp.rd;
				nw2.tp=1;
				nw2.offset=tmp.imm;
//				_ROB.push_back(nw2);
				up_ROB.push_back(nw2);
				up_RS.push_back(nw);
//				reg_pre[tmp.rd].status=1;
//				reg_pre[tmp.rd].q=nw.idx;
				//bge
				break;
			}
			case 35:{
					RS nw;
				ROB nw2;
				if(reg_pre[tmp.rs1].status) {
					nw.qi=reg_pre[tmp.rs1].q; 
				}
				else nw.vi=reg_pre[tmp.rs1].v;
				nw.vj=tmp.imm;
				nw.status=1;
				nw.op=14;//14: unsgined < pc+
//				_RS.queue[pos]=nw;
				nw2.ready=0;
				nw.idx=_ROB.tail;
				nw2.addr=tmp.rd;
				nw2.tp=1;
				nw2.offset=tmp.imm;
//				_ROB.push_back(nw2);
				up_ROB.push_back(nw2);
				up_RS.push_back(nw);
//				reg_pre[tmp.rd].status=1;
//				reg_pre[tmp.rd].q=nw.idx;
				//bltu
				break;
			}
			case 36:{
				RS nw;
				ROB nw2;
				if(reg_pre[tmp.rs1].status) {
					nw.qi=reg_pre[tmp.rs1].q; 
				}
				else nw.vi=reg_pre[tmp.rs1].v;
				nw.vj=tmp.imm;
				nw.status=1;
				nw.op=15;//15: unsgined >= pc+
//				_RS.queue[pos]=nw;
				nw2.ready=0;
				nw.idx=_ROB.tail;
				nw2.addr=tmp.rd;
				nw2.tp=1;
				nw2.offset=tmp.imm;
//				_ROB.push_back(nw2);
				up_ROB.push_back(nw2);
				up_RS.push_back(nw);
//				reg_pre[tmp.rd].status=1;
//				reg_pre[tmp.rd].q=nw.idx;
				//bgeu
				break;
			}
			case 37:{
				RS nw;
				ROB nw2;
				nw.vi=pc;
				nw.vj=0;
				nw.status=1;
				nw.op=0;
				nw2.ready=0;
				nw.idx=_ROB.tail;
				nw2.addr=tmp.rd;
				nw2.offset=tmp.imm;
				nw2.tp=2;
				up_ROB.push_back(nw2);
				up_reg_q.push_back(make_pair(tmp.rd,nw.idx));
				up_RS.push_back(nw);
				//jal
				break;
			}
			case 38:{
				RS nw;
				ROB nw2;
				if(reg_pre[tmp.rs1].status) {
					nw.qi=reg_pre[tmp.rs1].q; 
				}
				else nw.vi=reg_pre[tmp.rs1].v;
				nw.vj=tmp.imm;
				nw.status=1;
				nw.op=0;
				nw2.ready=0;
				nw.idx=_ROB.tail;
				nw2.addr=tmp.rd;
				nw2.tp=3;
				nw2.offset=pc;
				up_ROB.push_back(nw2);
				up_RS.push_back(nw);
				//jalr
				break;
			}
		}
	}
	////finished
	void RS_Run() {
		for(uint i=0;i<up_RS.size();++i){
			uint temp=_RS.find_empty();
			_RS.queue[temp]=up_RS[i];
		}
		int pos=_RS_pre.find_available();
		if(pos!=-1) 
		EX_Run_.push_back(pos);
		if(Number_ROB>=MAXN) return ; 
		_RS.update(Number_ROB,Number_Result);
	}
	//finished 
	void Execute() {
		EX_to_ROB.first=MAXN+1;
		if(EX_Run_.empty()) return ;
		RS tmp=_RS_pre.queue[EX_Run_[0]];
		EX_to_ROB.first=tmp.idx;
		switch(tmp.op) {
			case 0: EX_to_ROB.second=tmp.vi+tmp.vj;break;
			case 1: EX_to_ROB.second=tmp.vi-tmp.vj;break;
			case 2: EX_to_ROB.second=tmp.vi^tmp.vj;break;
			case 3: EX_to_ROB.second=tmp.vi|tmp.vj;break;
			case 4: EX_to_ROB.second=tmp.vi&tmp.vj;break;
			case 5: EX_to_ROB.second=tmp.vi<<(tmp.vj&31u);break;
			case 6: EX_to_ROB.second=tmp.vi>>(tmp.vj&31u);break;
			case 7: {
				uint o=((uint)1<<31)&tmp.vi;
				EX_to_ROB.second=tmp.vi>>(tmp.vj&31u);
				if(o)for(uint i=31;i>=31-(tmp.vj&31u)+1;++i) EX_to_ROB.second|=((uint)1<<i);
				break;
			}
			case 8: 
			{
				EX_to_ROB.second=((int)tmp.vi)<((int)tmp.vj);break;
			}
			case 9: EX_to_ROB.second=tmp.vi<tmp.vj;break;
			case 10: EX_to_ROB.second=tmp.vi==tmp.vj;break;
			case 11: EX_to_ROB.second=tmp.vi!=tmp.vj;break;
			case 12: EX_to_ROB.second=((int)tmp.vi)<((int)tmp.vj);break;
			case 13: EX_to_ROB.second=((int)tmp.vi)>=((int)tmp.vj);break;
			case 14: EX_to_ROB.second=tmp.vi<tmp.vj;break;
			case 15: EX_to_ROB.second=tmp.vi>=tmp.vj;break;
		}
	} 
	////finished
	void REG_Run() {
		for(uint i=0;i<up_reg_q_ex.size();++i) {
			uint idx=up_reg_q_ex[i].first,v=up_reg_q_ex[i].second;
			reg[idx].status=1;reg[idx].q=v;
		}
		for(uint i=0;i<up_reg_v_ex.size();++i) {
			uint idx=up_reg_v_ex[i].first,v=up_reg_v_ex[i].second;
			reg[idx].status=0;reg[idx].v=v;
		}
	}
	////
	void Commit() {
		 
	}
	////finished
	void ROB_Run() {
		if(!_ROB_pre.isfull()) {
			for(uint i=0;i<up_ROB.size();++i) {
				ROB nw = up_ROB[i];
				_ROB.push_back(nw); 
			}
		}
		if(LSB_to_ROB_pre.first<MAXN) {
			_ROB.queue[LSB_to_ROB_pre.first].ready=1;
			_ROB.queue[LSB_to_ROB_pre.first].result=LSB_to_ROB_pre.second.first;
			if(_ROB.queue[LSB_to_ROB_pre.first].tp==10)
			_ROB.queue[LSB_to_ROB_pre.first].result2=LSB_to_ROB_pre.second.second;
		}
		if(EX_to_ROB_pre.first<MAXN) {
			_ROB.queue[EX_to_ROB_pre.first].ready=1;
			_ROB.queue[EX_to_ROB_pre.first].result=EX_to_ROB_pre.second;
		}
		if(_ROB_pre.isempty()) return ;
		ROB tmp=_ROB_pre.get_front();
		if(tmp.ready) {
			_ROB.pop_front();
			commit_.push_back(tmp);
			if(tmp.code == 0x0ff00513){
                std::cout << std::dec << ((unsigned int)reg_pre[10].v& 255u);
                exit(0);
            }
		}
	}
	void LSB_Run(){
		if(up_LSB.size()) {
			for(uint i=0;i<up_LSB.size();++i) {
				_LSB.push_back(up_LSB[i]);
			}
		}
		if(Number_ROB<MAXN) _LSB.Update(Number_ROB,Number_Result);
		LSB_to_ROB.first=MAXN+1; 
		if(!_LSB_pre.isempty()) {
			LSB tmp = _LSB_pre.get_front();
			if(tmp.q!=-1||tmp.q2!=-1) return ;
			uint res=0;
			if(tmp.tp==0) {
				res=mem[tmp.v+tmp.imm];
				if(res&128u) {
					for(uint i=8;i<=31;++i) res|=((uint)1<<i);
				}
			}
			if(tmp.tp==1) {
				res=((uint)mem[tmp.v+tmp.imm]<<8)+(uint)mem[tmp.v+tmp.imm+1];
				if(res&32768u) {
					for(uint i=16;i<=31;++i) res|=((uint)1<<i);
				}
			}
			if(tmp.tp==2) {
				res=((uint)mem[tmp.v+tmp.imm]<<24)+((uint)mem[tmp.v+tmp.imm+1]<<16)+((uint)mem[tmp.v+tmp.imm+2]<<8)+((uint)mem[tmp.v+tmp.imm+3]);
			}
			if(tmp.tp==3) {
				res=mem[tmp.v+tmp.imm];
			}
			if(tmp.tp==4) {
				res=((uint)mem[tmp.v+tmp.imm]<<8)+(uint)mem[tmp.v+tmp.imm+1];
			}
			if(tmp.tp<=4) {
				LSB_to_ROB.second.first=res;
				return ;
			}
			res=tmp.v2;
			if(tmp.tp==5) res=res&255u;
			if(tmp.tp==6) res=res&32768u;
			LSB_to_ROB.second.first=res;
			LSB_to_ROB.second.second=tmp.v+tmp.imm; 
		}
	}
	void Update(){
		
	}
	void run() {
		while(1) {
			ROB_Run();//
			IF_Run();//
			RS_Run();//
			ROB_Run();//
			LSB_Run();//
			REG_Run();//
			Update();
			Execute();//
			ISSUE_Run();//
			Commit(); 
		}
	}
	void MAIN() {
		input();
		run();
	}
};
