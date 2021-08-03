#include<bits/stdc++.h>
#include "translate.hpp"
#include "RS.hpp"
#include "ROB.hpp"
#include "LSB.hpp"
#include "IQ.hpp"
//#define debug(x) cerr<<#x<<"="<<x<<"\n"
using namespace std;
struct _register{
	uint v=0;
	int q=0;
	bool status=0;
};
vector<pair<uint,uint>> up_reg_q,up_reg_q_ex;
vector<pair<uint,pair<uint,uint> > >  up_reg_v,up_reg_v_ex;
vector<RS> EX_Run_;

int Number_ROB=MAXN+1;
uint Number_Result; 

vector<ROB> up_ROB;
vector<pair<ROB,uint> > commit_;
vector<RS> up_RS;
vector<LSB> up_LSB;
pair<uint,uint> EX_to_ROB=make_pair(MAXN+1,0); 
pair<uint,pair<uint,uint>> LSB_to_ROB_pre=make_pair(MAXN+1,make_pair(0,0)),LSB_to_ROB=make_pair(MAXN+1,make_pair(0,0));
pair<uint,uint> EX_to_ROB_pre=make_pair(MAXN+1,0); 
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
//				debug(pos);
//				debug((uint)mem[pos]);
				pos++;
			}
		}
	}
	inline void IF_Run() {
		if(_IQ.isfull()) return ;
		uint tmp=(mem[pc])+(mem[pc+1]<<8)+(mem[pc+2]<<16)+(mem[pc+3]<<24);
//		debug(tmp); 
		pc+=4;
//		debug(pc);
		_IQ.push_back(__translate(tmp,pc-4));
	}
	//finished;
	inline void ISSUE_Run() {
		if(_IQ_pre.isempty()) return;
		if(_ROB.isfull()) return ;
		if(_LSB.isfull()) return ;
		int pos=_RS_pre.find_empty();
		if(pos==-1) return ;
		Instructions tmp=_IQ_pre.get_front();
		ROB nw2;
		nw2.code=tmp.code;
		if(tmp.end) {
			up_ROB.push_back(nw2);
			return ;
		} 
		nw2.pc=tmp.pc;
		nw2.debuger=tmp.tp;
		nw2.rs1=tmp.rs1;
		nw2.rs2=tmp.rs2;
		nw2.temp=tmp;
//		debug(tmp.code);
//		debug(tmp.tp);
//		debug(tmp.pc);
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
				nw.idx=_ROB.tail;
				nw.tp=tmp.tp;
				nw.code=tmp.code;
				if(tmp.tp<=4)
				{
					if(reg_pre[tmp.rs1].status) {
						nw.q=reg_pre[tmp.rs1].q; 
					}
					else nw.v=reg_pre[tmp.rs1].v;
					nw.imm=tmp.imm;
					nw.code=tmp.code; 
//					debug(nw.v);
					nw.addr=tmp.rd;
					nw2.addr=tmp.rd;
					up_reg_q_ex.push_back(make_pair(tmp.rd,nw.idx));
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
//					debug(nw.q);
				}
//				if(tmp.code==15181859u) {
//					cerr<<"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\n";
//					debug(nw.v);
//					debug(tmp.rs1);
//				}
				nw2.ready=0;
				up_ROB.push_back(nw2);
				up_LSB.push_back(nw);
				break;
			}
			case 8:{
				RS nw;
//				ROB nw2;nw2.code=tmp.code;
				if(reg_pre[tmp.rs1].status) {
					nw.qi=reg_pre[tmp.rs1].q; 
				}
				else nw.vi=reg_pre[tmp.rs1].v;
				if(reg_pre[tmp.rs2].status) {
					nw.qj=reg_pre[tmp.rs2].q; 
				}
				else nw.vj=reg_pre[tmp.rs2].v;
				nw.code=tmp.code;
				nw.status=1;
				nw.op=0;//0:+
//				_RS.queue[pos]=nw;
				nw2.ready=0;
				nw.idx=_ROB.tail;
				nw2.addr=tmp.rd;
//				_ROB.push_back(nw2);
				up_ROB.push_back(nw2);
				up_reg_q_ex.push_back(make_pair(tmp.rd,nw.idx));
				up_RS.push_back(nw);
//				reg_pre[tmp.rd].status=1;
//				reg_pre[tmp.rd].q=nw.idx;
				//add
				break;
			}
			case 9:{
				RS nw;
//				ROB nw2;
				if(reg_pre[tmp.rs1].status) {
					nw.qi=reg_pre[tmp.rs1].q; 
//					debug("FUCK");
				}
				else nw.vi=reg_pre[tmp.rs1].v;
				nw.code=tmp.code;
//				debug(nw.code);
				nw.vj=tmp.imm;
				nw.status=1;
				nw.op=0;//0:+
//				_RS.queue[pos]=nw;
				nw2.ready=0;
				nw.idx=_ROB.tail;
				nw2.addr=tmp.rd;
//				if(tmp.code==1426326547u) {
//					cerr<<"Attentions!\n";
//					debug(nw.vi);
//					debug(nw.qi);
//					debug(nw.vj);
//					debug(nw.qj);
//					
//				}
//				_ROB.push_back(nw2);
//				debug(tmp.code);
//				debug(nw.qi);
//				debug(nw.vj);
//				debug(_ROB.queue[nw.qi].code);
				up_ROB.push_back(nw2);
				up_reg_q_ex.push_back(make_pair(tmp.rd,nw.idx));
//				debug(tmp.rd); 
				up_RS.push_back(nw);
//				reg_pre[tmp.rd].status=1;
//				reg_pre[tmp.rd].q=nw.idx;
				//addi;
				break;
			}
			case 10:{
				RS nw;
//				ROB nw2;
				if(reg_pre[tmp.rs1].status) {
					nw.qi=reg_pre[tmp.rs1].q; 
				}
				else nw.vi=reg_pre[tmp.rs1].v;
				if(reg_pre[tmp.rs2].status) {
					nw.qj=reg_pre[tmp.rs2].q; 
				}
				else nw.vj=reg_pre[tmp.rs2].v;
				nw.code=tmp.code;
				nw.status=1;
				nw.op=1;//1:-
//				_RS.queue[pos]=nw;
				nw2.ready=0;
				nw.idx=_ROB.tail;
				nw2.addr=tmp.rd;
//				_ROB.push_back(nw2);
				up_ROB.push_back(nw2);
				up_reg_q_ex.push_back(make_pair(tmp.rd,nw.idx));
				up_RS.push_back(nw);
//				reg_pre[tmp.rd].status=1;
//				reg_pre[tmp.rd].q=nw.idx;
				//sub
				break;
			}
			case 11:{
				RS nw;
				nw.code=tmp.code;
				nw.status=1;
				nw.op=0;//0:+
				nw2.ready=0;
				nw.idx=_ROB.tail;
//				debug(nw.idx);
				nw2.addr=tmp.rd;
				nw.vi=tmp.imm;
				nw.vj=0;
				up_ROB.push_back(nw2);
				up_reg_q_ex.push_back(make_pair(tmp.rd,nw.idx));
				up_RS.push_back(nw);
//				debug(tmp.imm);
				//lui;
				break;
			}
			case 12:{
				RS nw;
				nw.code=tmp.code;
				nw.status=1;
				nw.op=0;//0:+
				nw2.ready=0;
				nw.idx=_ROB.tail;
				nw2.addr=tmp.rd;
				nw.vi=tmp.imm+pc-4;
				nw.vj=0;
				
				up_ROB.push_back(nw2);
				up_reg_q_ex.push_back(make_pair(tmp.rd,nw.idx));
				up_RS.push_back(nw);
				//apuic;
				break;
			}
			case 13:{
				RS nw;
				nw.code=tmp.code;
//				ROB nw2;
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
				up_reg_q_ex.push_back(make_pair(tmp.rd,nw.idx));
				up_RS.push_back(nw);
//				reg_pre[tmp.rd].status=1;
//				reg_pre[tmp.rd].q=nw.idx;
				//xor
				break;
			}
			case 14:{
				RS nw;
				nw.code=tmp.code;
//				ROB nw2;
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
				up_reg_q_ex.push_back(make_pair(tmp.rd,nw.idx));
				up_RS.push_back(nw);
//				reg_pre[tmp.rd].status=1;
//				reg_pre[tmp.rd].q=nw.idx;
				//xori
				break;
			}
			case 15:{
				RS nw;
				nw.code=tmp.code;
//				ROB nw2;
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
				up_reg_q_ex.push_back(make_pair(tmp.rd,nw.idx));
				up_RS.push_back(nw);
//				reg_pre[tmp.rd].status=1;
//				reg_pre[tmp.rd].q=nw.idx;
				//or
				break;
			}
			case 16:{
				RS nw;
				nw.code=tmp.code;
//				ROB nw2;
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
				up_reg_q_ex.push_back(make_pair(tmp.rd,nw.idx));
				up_RS.push_back(nw);
//				reg_pre[tmp.rd].status=1;
//				reg_pre[tmp.rd].q=nw.idx;
				//ori
				break;
			}
			case 17:{
				RS nw;
				nw.code=tmp.code;
//				ROB nw2;
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
				up_reg_q_ex.push_back(make_pair(tmp.rd,nw.idx));
				up_RS.push_back(nw);
//				reg_pre[tmp.rd].status=1;
//				reg_pre[tmp.rd].q=nw.idx;
				//and
				break;
			}
			case 18:{
				RS nw;
				nw.code=tmp.code;
//				ROB nw2;
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
				up_reg_q_ex.push_back(make_pair(tmp.rd,nw.idx));
				up_RS.push_back(nw);
//				reg_pre[tmp.rd].status=1;
//				reg_pre[tmp.rd].q=nw.idx;
				//andi
				break;
			}
			case 19:{
				RS nw;
				nw.code=tmp.code;
//				ROB nw2;
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
				up_reg_q_ex.push_back(make_pair(tmp.rd,nw.idx));
				up_RS.push_back(nw);
//				reg_pre[tmp.rd].status=1;
//				reg_pre[tmp.rd].q=nw.idx;
				//sll
				break;
			}
			case 20:{
				RS nw;
				nw.code=tmp.code;
//				ROB nw2;
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
				up_reg_q_ex.push_back(make_pair(tmp.rd,nw.idx));
				up_RS.push_back(nw);
//				reg_pre[tmp.rd].status=1;
//				reg_pre[tmp.rd].q=nw.idx;
				//slli
				break;
			}
			case 21:{
				RS nw;
				nw.code=tmp.code;
//				ROB nw2;
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
				up_reg_q_ex.push_back(make_pair(tmp.rd,nw.idx));
				up_RS.push_back(nw);
//				reg_pre[tmp.rd].status=1;
//				reg_pre[tmp.rd].q=nw.idx;
				//srl
				break;
			}
			case 22:{
				RS nw;
				nw.code=tmp.code;
//				ROB nw2;
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
				up_reg_q_ex.push_back(make_pair(tmp.rd,nw.idx));
				up_RS.push_back(nw);
//				reg_pre[tmp.rd].status=1;
//				reg_pre[tmp.rd].q=nw.idx;
				//srli
				break;
			}
			case 23:{
				RS nw;
				nw.code=tmp.code;
//				ROB nw2;
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
				up_reg_q_ex.push_back(make_pair(tmp.rd,nw.idx));
				up_RS.push_back(nw);
//				reg_pre[tmp.rd].status=1;
//				reg_pre[tmp.rd].q=nw.idx;
				//sra
				break;
			}
			case 24:{
				RS nw;
				nw.code=tmp.code;
//				ROB nw2;
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
				up_reg_q_ex.push_back(make_pair(tmp.rd,nw.idx));
				up_RS.push_back(nw);
//				reg_pre[tmp.rd].status=1;
//				reg_pre[tmp.rd].q=nw.idx;
				//srai
				break;
			}
			case 25:{
				RS nw;
				nw.code=tmp.code;
//				ROB nw2;
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
				up_reg_q_ex.push_back(make_pair(tmp.rd,nw.idx));
				up_RS.push_back(nw);
//				reg_pre[tmp.rd].status=1;
//				reg_pre[tmp.rd].q=nw.idx;
				//slt
				break;
			}
			case 26:{
				RS nw;
				nw.code=tmp.code;
//				ROB nw2;
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
				up_reg_q_ex.push_back(make_pair(tmp.rd,nw.idx));
				up_RS.push_back(nw);
//				reg_pre[tmp.rd].status=1;
//				reg_pre[tmp.rd].q=nw.idx;
				//slti
				break;
			}
			case 27:{
				RS nw;
				nw.code=tmp.code;
//				ROB nw2;
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
				up_reg_q_ex.push_back(make_pair(tmp.rd,nw.idx));
				up_RS.push_back(nw);
//				reg_pre[tmp.rd].status=1;
//				reg_pre[tmp.rd].q=nw.idx;
				//sltu
				break;
			}
			case 28:{
				RS nw;
				nw.code=tmp.code;
//				ROB nw2;
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
				up_reg_q_ex.push_back(make_pair(tmp.rd,nw.idx));
				up_RS.push_back(nw);
//				reg_pre[tmp.rd].status=1;
//				reg_pre[tmp.rd].q=nw.idx;
				//sltui
				break;
			}
			case 29:{
				RS nw;
				nw.code=tmp.code;
//				ROB nw2;
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
			case 30:{
				RS nw;
				nw.code=tmp.code;
//				ROB nw2;
				if(reg_pre[tmp.rs1].status) {
					nw.qi=reg_pre[tmp.rs1].q; 
				}
				else nw.vi=reg_pre[tmp.rs1].v;
				if(reg_pre[tmp.rs2].status) {
					nw.qj=reg_pre[tmp.rs2].q; 
				}
				else nw.vj=reg_pre[tmp.rs2].v;
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
			case 31:{
				RS nw;
				nw.code=tmp.code;
//				ROB nw2;
				if(reg_pre[tmp.rs1].status) {
					nw.qi=reg_pre[tmp.rs1].q; 
				}
				else nw.vi=reg_pre[tmp.rs1].v;
				if(reg_pre[tmp.rs2].status) {
					nw.qj=reg_pre[tmp.rs2].q; 
				}
				else nw.vj=reg_pre[tmp.rs2].v;
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
			case 32:{
				RS nw;
				nw.code=tmp.code;
//				ROB nw2;
				if(reg_pre[tmp.rs1].status) {
					nw.qi=reg_pre[tmp.rs1].q; 
				}
				else nw.vi=reg_pre[tmp.rs1].v;
				if(reg_pre[tmp.rs2].status) {
					nw.qj=reg_pre[tmp.rs2].q; 
				}
				else nw.vj=reg_pre[tmp.rs2].v;
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
			case 33:{
				RS nw;
				nw.code=tmp.code;
//				ROB nw2;
				if(reg_pre[tmp.rs1].status) {
					nw.qi=reg_pre[tmp.rs1].q; 
				}
				else nw.vi=reg_pre[tmp.rs1].v;
				if(reg_pre[tmp.rs2].status) {
					nw.qj=reg_pre[tmp.rs2].q; 
				}
				else nw.vj=reg_pre[tmp.rs2].v;
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
			case 34:{
				RS nw;
				nw.code=tmp.code;
//				ROB nw2;
				if(reg_pre[tmp.rs1].status) {
					nw.qi=reg_pre[tmp.rs1].q; 
				}
				else nw.vi=reg_pre[tmp.rs1].v;
				if(reg_pre[tmp.rs2].status) {
					nw.qj=reg_pre[tmp.rs2].q; 
				}
				else nw.vj=reg_pre[tmp.rs2].v;
				nw.status=1;
				nw.op=15;//15: unsgined >= pc+
				nw2.ready=0;
				nw.idx=_ROB.tail;
				nw2.addr=tmp.rd;
				nw2.tp=1;
				nw2.offset=tmp.imm;
				up_ROB.push_back(nw2);
				up_RS.push_back(nw);
				//bgeu
				break;
			}
			case 35:{
				RS nw;
				nw.code=tmp.code;
//				ROB nw2;
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
				up_reg_q_ex.push_back(make_pair(tmp.rd,nw.idx));
				up_RS.push_back(nw);
//				debug(tmp.pc);
//				debug(tmp.imm);
				//jal
				break;
			}
			case 36:{
				RS nw;
				nw.code=tmp.code;
//				ROB nw2;
				if(reg_pre[tmp.rs1].status) {
					nw.qi=reg_pre[tmp.rs1].q; 
					
				}
				else nw.vi=reg_pre[tmp.rs1].v;//debug(nw.vi);
				nw.vj=tmp.imm;
//				debug("-----------------\n");
//				debug(tmp.rs1);
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
//		debug("-----------\n");
//		_RS.print();
//		_RS_pre.print();
		for(uint i=0;i<up_RS.size();++i){
			uint temp=_RS_pre.find_empty();
//			debug("in");
//			debug(temp);
			_RS.queue[temp]=up_RS[i];
//			if(_RS.queue[temp].code==4271119075u) {
//							assert(0);
//							cerr<<"Attention"<<endl;
//							debug(_RS.queue[temp].code);
//							debug(_RS.queue[temp].qj);
//							debug(_RS.queue[temp].vj); 
//						} 
//			debug(_RS.queue[temp].code);
//			debug(_RS.queue[temp].vi);
//			debug(_RS.queue[temp].vj); 
//			debug(up_RS[i].idx);
//			debug(temp);
		}
		
		up_RS.clear();
		int pos=_RS_pre.find_available();
		
		if(pos!=-1) 
		{
//			if(_RS_pre.que15796531 
//			debug("out");
//			debug(pos);
//			if(_RS_pre.queue[pos].idx==6) {
//				cerr<<"Warning!!!!!\n";
//				debug(_RS_pre.queue[pos].code);
//				assert(0);
//			}
			EX_Run_.push_back(_RS_pre.queue[pos]),_RS.queue[pos].status=0;
		}
		if(Number_ROB>=MAXN) return ; 
		_RS.update(Number_ROB,Number_Result);
	}
	//finished 
	void Execute() {
		EX_to_ROB.first=MAXN+1;
		if(EX_Run_.empty()) return ;
		RS tmp=EX_Run_[0];EX_Run_.clear();
		EX_to_ROB.first=tmp.idx;
//		assert(EX_to_ROB.first!=6);
//		cout<<"Execute--------------------\n"; 
//		debug(tmp.idx);
//		debug(tmp.code);
//		cout<<"Execute--------------------\n"; 
//		if(tmp.code==4271119075u) {
//			cerr<<"Attention"<<endl;
//			debug(tmp.code);
//			debug(tmp.vi);
//			debug(tmp.vj); 
//		} 
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
		//if(EX_to_ROB.second==4180)
		//cout<<tmp.idx<<"----------------------------------------------------------\n";
	} 
	////finished
	void REG_Run() {
		for(uint i=0;i<up_reg_v_ex.size();++i) {
			
			uint idx=up_reg_v_ex[i].first,v=up_reg_v_ex[i].second.first;
			if(idx==0) continue;
			reg[idx].v=v;
			if(reg[idx].q==up_reg_v_ex[i].second.second) reg[idx].status=0;
		}
		for(uint i=0;i<up_reg_q_ex.size();++i) {
			uint idx=up_reg_q_ex[i].first,v=up_reg_q_ex[i].second;
			if(idx==0) continue;
			reg[idx].status=1;reg[idx].q=v;
		}
		up_reg_q_ex.clear();
		up_reg_v_ex.clear();
	}
	void Clear(){
//		debug("Clear\n");
		_RS.clear();
		_RS_pre.clear();
//		debug(2);
		_LSB.clear();
		_LSB_pre.clear();
		
		_ROB.head=_ROB.tail=0;
		_ROB_pre.head=_ROB_pre.tail=0;
		_IQ.head=_IQ.tail=_IQ_pre.head=_IQ_pre.tail=0;
		_RS.clear();
		_RS_pre.clear();
		up_reg_q.clear(),up_reg_v.clear();up_reg_q_ex.clear(),up_reg_v_ex.clear();
		EX_Run_.clear();
		Number_ROB=MAXN+1;
		up_ROB.clear();
		commit_.clear();
		up_RS.clear();
		up_LSB.clear();
		EX_to_ROB=make_pair(MAXN+1,0); 
		LSB_to_ROB_pre=make_pair(MAXN+1,make_pair(0,0)),LSB_to_ROB=make_pair(MAXN+1,make_pair(0,0));
		EX_to_ROB_pre=make_pair(MAXN+1,0);
		for(uint i=0;i<=31;++i) {
			reg[i].q=0,reg[i].status=0;
			reg_pre[i].q=0; 
			reg_pre[i].status=0;
		}
	}
	////finished
	void Commit() {
		 Number_ROB=MAXN+1;
		 if(commit_.empty()) return ;
		 
		 ROB tmp=commit_[0].first;
//		 for(int i=0;i<=31;++i) {
//			cout<<reg[i].v<<" ";
//		}
//		cout<<endl;
//		 printf("%u %u\n",tmp.code,tmp.pc);
		
		 if(tmp.code == 0x0ff00513){
                std::cout << std::dec << ((unsigned int)reg_pre[10].v& 255u);
                exit(0);
            }
//		 debug(tmp.code);
		 Number_ROB=commit_[0].second;
		 commit_.clear();
//		 debug(Number_ROB);
		 if(tmp.tp==0) {
		 	Number_Result=tmp.result;
		 	up_reg_v_ex.push_back(make_pair(tmp.addr,make_pair(tmp.result,Number_ROB)));
		 }
		 else if(tmp.tp==1) {
		 	if(tmp.result==1) {
		 		Clear();
		 		pc=tmp.offset+tmp.pc;
//		 		debug("YYDS\n"); 
			}
			else {
				return ;
			}
		 }
		 else if(tmp.tp==2) {
		 	Clear();
		 	pc=tmp.offset+tmp.pc;
		 	Number_Result=tmp.result;
		 	up_reg_v_ex.push_back(make_pair(tmp.addr,make_pair(tmp.result,Number_ROB)));
		 }
		 else if(tmp.tp==3){
		 	Clear();
//			debug(tmp.result);
			pc=(tmp.result)&~1;
//			debug(pc);
		 	Number_Result=tmp.offset;
		 	up_reg_v_ex.push_back(make_pair(tmp.addr,make_pair(tmp.result,Number_ROB)));
		 }
		 else {
		 	return ;
		 }
	}
	////finished
	void ROB_Run() {
		if(!_ROB_pre.isfull()) {
			for(uint i=0;i<up_ROB.size();++i) {
				ROB nw = up_ROB[i];
				_ROB.push_back(nw); 
//				debug(_ROB.tail);
			}
		}
//		debug(_ROB.head);
//		debug(_ROB.tail);
		up_ROB.clear();
		if(LSB_to_ROB_pre.first<MAXN) {
//			debug(_ROB.head);
//			debug(_ROB.tail); 
//			assert(LSB_to_ROB_pre.first!=6);
			_ROB.queue[LSB_to_ROB_pre.first].ready=1;
			_ROB.queue[LSB_to_ROB_pre.first].result=LSB_to_ROB_pre.second.first;
		}
		if(EX_to_ROB.first<MAXN) {
//			debug(EX_to_ROB_pre.first);
//			debug(EX_to_ROB_pre.first)
			
			_ROB.queue[EX_to_ROB.first].ready=1;
//			debug(_ROB.queue[EX_to_ROB_pre.first].code);
			_ROB.queue[EX_to_ROB.first].result=EX_to_ROB.second;
		}
		if(_ROB_pre.isempty()) return ;
		ROB tmp=_ROB_pre.get_front();
//		debug(tmp.code); 
//		debug(_ROB_pre.head);
		if(tmp.code==0x0ff00513) {
			std::cout << std::dec << ((unsigned int)reg_pre[10].v& 255u);
                exit(0);
		}
		if(tmp.ready||tmp.tp==10) {
			commit_.clear(); 
			
//			debug("---------------");debug(__tot);
////			debug(tmp.debuger);
////			if(tmp.code==0) exit(0);
//			debug(tmp.ready);
//			debug(tmp.code);
//			debug(tmp.tp);
//			debug(_ROB_pre.head);
//			cout<<tmp.code<<" "<<endl;
//			debug(tmp.rs1);
//			debug(tmp.rs2);
//			debug(tmp.debuger);

//			if(__tot==10000) exit(0);
//			if(tmp.code==15181859u){
//				for(int i=0;i<=31;++i) cerr<<reg[i].v<<" ";
//				cerr<<endl;
//			} 
//			debug(tmp.rs1);
//			debug(tmp.rs2); 
//			debug(tmp.tp);
//			debug("---------------");
			commit_.push_back(make_pair(tmp,(uint)_ROB.head));_ROB.pop_front();
		}
	}
	void LSB_Run(){
		
		if(up_LSB.size()) {
			for(uint i=0;i<up_LSB.size();++i) {
				_LSB.push_back(up_LSB[i]);
			}
		}
		up_LSB.clear();
		if(Number_ROB<MAXN) _LSB.Update(Number_ROB,Number_Result);
		LSB_to_ROB.first=MAXN+1; 
		if(!_LSB_pre.isempty()) {
			LSB tmp = _LSB_pre.get_front();
//			debug(tmp.idx);
//			debug(tmp.q);
//			debug(tmp.q2);
			if(tmp._commited==0&&tmp.tp>4) return ;
//			debug(tmp.q);
//			debug(tmp.q2);
			if(tmp.q!=-1||tmp.q2!=-1) return ;
//			debug(1);
			_LSB.queue[_LSB.head]._round--;
//			debug(_LSB.queue[_LSB.head]._round);
			if(_LSB.queue[_LSB.head]._round!=0) return ;
			
		 	_LSB.pop_front();
			uint res=0;
			LSB_to_ROB.first=tmp.idx;
//			debug(tmp.imm);
			if(tmp.tp==0) {
				res=mem[tmp.v+tmp.imm];
				if(res&128u) {
					for(uint i=8;i<=31;++i) res|=((uint)1<<i);
				}
			}
			if(tmp.tp==1) {
				res=((uint)mem[tmp.v+tmp.imm])+((uint)mem[tmp.v+tmp.imm+1]<<8);
				if(res&32768u) {
					for(uint i=16;i<=31;++i) res|=((uint)1<<i);
				}
			}
			if(tmp.tp==2) {
//				debug(tmp.code);
//				debug(tmp.v+tmp.imm);
//				debug(tmp.imm); 
				res=((uint)mem[tmp.v+tmp.imm])+((uint)mem[tmp.v+tmp.imm+1]<<8)+((uint)mem[tmp.v+tmp.imm+2]<<16)+((uint)mem[tmp.v+tmp.imm+3]<<24);
//				debug(res); 
			}
			if(tmp.tp==3) {
				res=mem[tmp.v+tmp.imm];
			}
			if(tmp.tp==4) {
				res=((uint)mem[tmp.v+tmp.imm])+((uint)mem[tmp.v+tmp.imm+1]<<8);
			}
			if(tmp.tp<=4) {
				LSB_to_ROB.second.first=res;
				return ;
			}
			LSB_to_ROB.first=MAXN+1;
			res=tmp.v2;
//			debug("PRINT!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"); 
			
//			debug(tmp.code);
//			debug(tmp.v+tmp.imm);
//			debug(res);
			if(tmp.v+tmp.imm==4112u) exit(0);  
			if(tmp.tp==5) {
				res=res&255u;
				mem[tmp.v+tmp.imm]=res;
			}
			if(tmp.tp==6) {
//				res=res&32768u;
				mem[tmp.v+tmp.imm]=res&255u;
				res>>=8;
				mem[tmp.v+tmp.imm+1]=res&255u; 
			}
			if(tmp.tp==7) {
				mem[tmp.v+tmp.imm]=res&255u;
				res>>=8;
				mem[tmp.v+tmp.imm+1]=res&255u;
				res>>=8;
				mem[tmp.v+tmp.imm+2]=res&255u;
				res>>=8;
				mem[tmp.v+tmp.imm+3]=res&255u;  
			}
		}
	}
	void Update(){
		LSB_to_ROB_pre=LSB_to_ROB;
		EX_to_ROB_pre=EX_to_ROB;
		_RS_pre=_RS;
		_ROB_pre=_ROB;
//		debug(_ROB_pre.head);
		_IQ_pre=_IQ;
		_LSB_pre=_LSB;
		for(int i=0;i<32;++i) reg_pre[i]=reg[i];
	}
	void run() {
		int T=0;
		while(1) {
			++T;
//			debug(T);
//			debug(1); 
			ROB_Run();//
//			debug(2);
			IF_Run();//
//			debug(3);
			RS_Run();//
//			debug(4);
			LSB_Run();//
//			debug(5);
			REG_Run();//
//			debug(6);
			Update();//
//			debug(7);
//			_ROB.print(); 

			
			Execute();//
//			debug(8);
			ISSUE_Run();//
//			debug(9); 
			Commit();//
//			if(T==500) break;
//			for(int i=0;i<=31;++i) cerr<<reg[i].v<<" ";
//			cerr<<endl;
//			cout<<(reg[1].v&255u)<<"\n";
		}
	}
	void MAIN() {
		input();
		run();
	}
};
int main() {
//	freopen("gcd.data","r",stdin);
//	freopen("1.out","w",stdout);
	CPU Intel_Core_i9;
	Intel_Core_i9.MAIN();
	exit(0);
} 
