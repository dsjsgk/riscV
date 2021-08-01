struct LSB{
	uint idx;
	uint tp;
	uint addr=0,imm=0;
	int q=-1,q2=-1;
	uint v=0,v2=0; 
};
class Load_Store_Buffer{
	public:
	LSB queue[MAXN];
	int head=0,tail=0;
	Load_Store_Buffer() {
		head=tail=0;
	}
	int get_nxt(int cur) {
		cur++;
		if(cur>=MAXN) cur-=MAXN;
		return cur;
	}
	void push_back(LSB tmp) {
		queue[tail]=tmp;
		tail=get_nxt(tail);
	}
	bool isempty() {
		return head==tail;
	}
	bool isfull() {
		return get_nxt(tail)==head;
	}
	LSB get_front() {return queue[head];}
	void pop_front() {
		head++;
		head%=MAXN;
	}
	void Update(uint idx,uint res) {
		int tmp=head;
		while(tmp!=tail) {
			if(queue[tmp].q==idx) queue[tmp].v=res;
			if(queue[tmp].q2==idx) queue[tmp].v2=res;
			tmp=get_nxt(tmp);
		}
	}
};
