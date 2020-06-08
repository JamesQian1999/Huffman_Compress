// Author：錢承
//Date：2019.12.20
//Purpose：製作 Huffman 壓縮軟體
#include <iostream>
#include <fstream>
#include <iomanip>
#include <stdlib.h>
using namespace std;

fstream fin,fout,tmp,tmp2;
long long int freq[256]= {0},mycount=0;
int table=0;
long double before=0,after=0,Ratio=0;
string encoding_table[256]= {"\0"};

void frequency(unsigned char p)
{
	freq[p]++;
}

class freqListNode
{
	public:
		long long int freq;
		unsigned char ch;
		freqListNode *next,*left,*right;
		freqListNode(long long int i,unsigned char data=0)
		{
			freq=i;
			ch=data;
			next=NULL;
			left=NULL;
			right=NULL;
		}
};

class freqList
{
	public:
		freqListNode *first;
		freqList()
		{
			first=NULL;
		}
		void newNode(int i,unsigned char data);
		void makeTree( freqListNode *a, freqListNode*b);
};

void freqList::makeTree( freqListNode *a, freqListNode*b)
{
	freqListNode *tmp=new freqListNode(a->freq + b->freq);
	tmp->next=b->next;
	tmp->left=a;
	tmp->right=b;
	a->next=NULL;
	b->next=NULL;
	first=tmp;
}
void freqList::newNode(int i,unsigned char data)
{
	if(first)
	{
		freqListNode *tmp=new freqListNode(i,data);
		tmp->next=first;
		first=tmp;
	}
	else
	{
		freqListNode *tmp=new freqListNode(i,data);
		first=tmp;
	}
}

freqList myList;

void makeList()
{
	int i, j ;
	for(i=255; i>=0; i--)
		if(freq[i]!=0)
			myList.newNode(freq[i],i);
}

void sortByLinklist()
{
	freqListNode *ptr,*ptr2,*ptr3,*last,*TMP;
	for(ptr=myList.first; ptr!=NULL; ptr=ptr->next)
		if(ptr->next==NULL)
			last=ptr;
	for(ptr=myList.first; ptr!=last; )
	{
		for(ptr2=ptr,ptr3=ptr->next; ptr2!=last; ptr2=ptr2->next,ptr3=ptr3->next)
		{
			if(ptr2->freq > ptr3->freq)
			{
				long long int tmp=ptr2->freq;
				unsigned char ch=ptr2->ch;
				ptr2->freq=ptr3->freq;
				ptr2->ch=ptr3->ch;
				ptr3->freq=tmp;
				ptr3->ch=ch;
			}
			if(ptr2->next==last)
				TMP=ptr2;
		}
		last=TMP;
	}
	for(ptr=myList.first; ptr!=NULL; ptr=ptr->next)
		table++;
	tmp2<<table<<endl;
}


void reSortTree()
{
	freqListNode *sortme=myList.first,*ptr=myList.first->next;
	if(myList.first->freq <= myList.first->next->freq)
		return;
	myList.first=myList.first->next;
	for( ; ptr!=NULL ; ptr=ptr->next )
	{
		if((ptr->freq < sortme->freq && ptr->next==NULL)||(ptr->freq < sortme->freq && ptr->next->freq >=sortme->freq))
		{
			sortme->next=ptr->next;
			ptr->next=sortme;
		}
	}
}

void huffmanTree()
{
	freqListNode *ptr,*ptr2,*ptr3;
	for(; myList.first->next!=NULL; )
	{
		ptr2=myList.first;
		ptr3=myList.first->next;
		myList.makeTree(ptr2,ptr3);
		if(myList.first->next)
			reSortTree();
	}
}

int bin_to_deci(char *s)
{
	int i, two=1,dec=0;
	for(i=7 ; i>=0 ; i--,two*=2)
		dec+=((int)s[i]-48)*two;
	return dec;
}

string deci_to_bin(int i)
{
	string s="\0";
	char input[8]= {0};
	int j;
	for(j=0; j<8; j++)
	{
		input[j]=i%2;
		i=i/2;
	}
	for(j=7 ; j>=0 ; j--)
	{
		if(input[j]==1)
			s=s+'1';
		else
			s=s+'0';
	}
	return s;
}

int bin_to_deci_2(string s)
{
	int i, two=1,dec=0;
	for(i=0;s[i]!='\0';i++);
	//cout<<"i= "<<i<<endl;
	for(i=i-1; i>=0 ; i--,two*=2)
		{
		//	cout<<"(int)s[i] "<<(int)s[i]-48<<endl;
			dec+=((int)s[i]-48)*two;
		}
	return dec;
}

string deci_to_bin_2(int i,int Size)
{
	string s="\0";
	int j;//,tmp;
	long int t=i;
	//for(tmp=0; t!=0 ;tmp++)
		//t=t/2;
	//cout<<"tmp= "<<tmp<<endl;

	char input[Size]= {0};
	for(j=0; j<Size; j++)
	{
		input[j]=i%2;
		i=i/2;
	}
	for(j=Size-1 ; j>=0 ; j--)
	{
		if(input[j]==1)
			s=s+'1';
		else
			s=s+'0';
	}
	return s;
}

void I_am_walking_Tree(freqListNode *root, string a)
{
	if(!root)
		return;
	I_am_walking_Tree(root->left, a+'0');
	if(root->left==NULL && root->right==NULL)
	{
		tmp2<<(int)root->ch<<" "<<a.size()<<" "<<bin_to_deci_2(a)<<endl;
		encoding_table[(int)root->ch]=a;
	}
	I_am_walking_Tree(root->right, a+'1');
}

int main(int argc, char* argv[])
{
	char ch;
	int i, j, k;
	long long int sum=0;

	if(argc != 6)
	{
		cout << "指令: *.exe -(c,u) -i infile -o outfile" << endl;
		return 1;
	}
	fin.open(argv[3], ios::in | ios::binary);
	fout.open(argv[5], ios::out | ios::binary);
	if(!fin)
	{
		cout << "無法讀取檔案" << endl;
		return 1;
	}

	if(argv[1][1]=='c')
	{
		cout<<endl;
		while(fin.get(ch))
		{
			frequency(ch+128);
			mycount++;
		}
		fin.close();
		cout<<endl<<"Frequency Table: "<<endl<<endl;
		for(i=0 ; i<256; i++)
		{
			cout<<"ASCII("<<setw(3)<<i<<"): "<<setw(8)<<freq[i]<<"       ";
			sum+=freq[i];
			if((i+1)%6==0)
				cout<<endl;
		}
		tmp2.open("tmp2.txt",ios::out | ios::binary);

		makeList();
		sortByLinklist();
		huffmanTree();
		I_am_walking_Tree(myList.first,"\0");

		fin.open(argv[3], ios::in | ios::binary);
		cout<<endl<<endl<<"compressing......."<<endl;
		tmp.open("tmp1.txt", ios::out | ios::binary);
		long long unsigned int countbit=0;
		while(fin.get(ch))
		{
			tmp<<encoding_table[ch+128];
			countbit+=encoding_table[ch+128].size();
		}
		tmp.close();
		char out[8];
		int bit=0,persent=0;
		tmp.open("tmp1.txt", ios::in | ios::binary);
		tmp2<<countbit<<endl;
		while(tmp.get(ch))
		{
			out[bit]=ch;
			bit++;
			persent++;
			if(bit==8)
			{
				tmp2<<(char)(bin_to_deci(out)-128);
				bit=0;
			}
			if(persent==countbit/8)
				cout<<endl<<".......12.5%"<<endl;
			if(persent==countbit/4)
				cout<<endl<<".......25%"<<endl;
			if(persent==countbit*3/8)
				cout<<endl<<".......37.5%"<<endl;
			if(persent==countbit/2)
				cout<<endl<<".......50%"<<endl;
			if(persent==countbit*5/8)
				cout<<endl<<".......62.5%"<<endl;
			if(persent==countbit*3/4)
				cout<<endl<<".......75%"<<endl;
			if(persent==countbit*7/8)
				cout<<endl<<".......87.5%"<<endl;
		}
		for(i=bit; i<8; i++)
			out[i]='0';
		tmp2<<(char)(bin_to_deci(out)-128);
		cout<<endl<<".......100%"<<endl;
		tmp.close();
		system("del tmp1.txt");
		tmp2.close();
		fin.close();
		ifstream q;
		fin.open(argv[3], ios::in | ios::binary| ios::ate);
		ifstream::pos_type eof = fin.tellg();
		before=eof;
		cout << endl<<"Input file size :" << before/8<< " bytes" << endl;
		fin.close();
		fin.open("tmp2.txt", ios::in | ios::binary| ios::ate);
		ifstream::pos_type eof2 = fin.tellg();
		after=eof2;
		cout << "Output file size :" <<setw(8)<< after/8 << " bytes" << endl;
		fin.close();
		Ratio=before/after;
		fout<<before/8.0<<" "<<after/8.0<<" "<<Ratio<<endl;
		cout<<"Compression ratio :"<<setw(8)<<before/after<<endl;
		fin.open("tmp2.txt", ios::in | ios::binary);
		while(fin.get(ch))
			fout<<ch;
		fin.close();
		cout<<endl<<"Complete!!!"<<endl;
		system("del tmp2.txt");
		fout.close();
	}
	else if(argv[1][1]=='u')
	{
		cout<<endl;
		fin>>before>>after>>Ratio;
		cout<<"Uncompressed size :"<<setw(8)<<before<< " bytes"<<endl;
		cout<<"Compressed size :"<<setw(8)<<after<< " bytes"<<endl;
		cout<<"Compression Ratio :"<<setw(8)<<Ratio<<endl<<endl;
		long long unsigned int num;
		int encode,Size,S;
		char Get;
		string Sin;
		cout<<endl<<"Encoding Table: "<<endl<<endl;
		fin>>num;
		for(i=0 ; i<num ; i++)
		{
			fin>>encode>>Size>>S;
			Sin=deci_to_bin_2(S,Size);
			cout<<"ASCII("<<setw(4)<<encode<<") Code:"<<setw(15)<<Sin<<"       ";
			if((i+1)%4==0)
				cout<<endl;
			encoding_table[encode]=Sin;
		}
		cout<<endl<<endl<<"Uncompressing......."<<endl;
		fin>>num;
		fin.get(Get);
		tmp.open("tmp.txt", ios::out | ios::binary);
		while(fin.get(Get))
			tmp<<deci_to_bin((int)(Get+128));
		tmp.close();
		fin.close();
		tmp.open("tmp.txt", ios::in | ios::binary);
		string decode="\0";
		for(i=0; i<num; i++)
		{
			tmp.get(Get);
			decode=decode+Get;
			for(j=0 ; j<256 ; j++)
				if(encoding_table[j]==decode)
				{
					fout<<(char)(j-128);
					decode="\0";
					break;
				}
			if(i==num/8)
				cout<<endl<<".......12.5%"<<endl;
			if(i==num/4)
				cout<<endl<<".......25%"<<endl;
			if(i==num*3/8)
				cout<<endl<<".......37.5%"<<endl;
			if(i==num/2)
				cout<<endl<<".......50%"<<endl;
			if(i==num*5/8)
				cout<<endl<<".......62.5%"<<endl;
			if(i==num*3/4)
				cout<<endl<<".......75%"<<endl;
			if(i==num*7/8)
				cout<<endl<<".......87.5%"<<endl;
			if(i==num-1)
				cout<<endl<<".......100%"<<endl;
		}
		cout<<endl<<"Complete!!!"<<endl;
		tmp.close();
		system("del tmp.txt");
		fout.close();
	}
	else
	{
		cout << "指令: *.exe -(c,u) -i infile -o outfile" << endl;
		return 1;
	}
	return 0;
}
