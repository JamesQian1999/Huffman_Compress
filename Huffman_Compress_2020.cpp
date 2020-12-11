#include<iostream>
#include<vector>
#include<fstream>
#include<iomanip>
#include<cstdlib>
using namespace std;

class Node
{
    public:
        friend class List;
        friend void walk_through(Node *root , string encode);
        Node(long int Freq , int Ch)
        {
            freq=Freq;
	        ch=Ch;
	        next=NULL;
            left=NULL;
            right=NULL;
        }
    private:
        long int freq;
	    int ch;
	    Node *next,*left,*right;

};
class List
{
    public:
        List() { first=NULL; };
        Node *first;
        int makeList();
        void push(int index);
        void sorted();
        void huffman();

}list;

#define DEBUG false
#define offset 128
long table[256]={0};
string encoding[256]={"\0"};
fstream tmpEncodingTable,tmpEncoding,inputfile,outputfile;
void checkfile(fstream &inputfile);
int frequency(fstream &inputfile);
void Encoding(int count, int OriginalSize,char *outputfileNAME);
void walk_through(Node *root , string encode);
int tmpWrite();
void tmpWrite(fstream &inputfile);
int output(int OriginalSize,int bitNUM);
int toDecimal(string binaryNUM);
char toDecimal(string bytes,int count);
int creatTable(fstream &inputfile);
string toBinary(int data,int size);
void decoding(int num,char *outputfileNAME);

int main(int argc,char **argv)
{
    if( (argc!=6) || ((argv[1][1]!='c') && (argv[1][1]!='u')) )
    {
        cout<<"Error!"<<endl;
        cout<<"huffman [-c|-u] –i InputFile –o OutputFile"<<endl;
        exit(1);
    }
    if(argv[1][1]=='c' ) //huffman –c –i infile –o outfile     (進行壓縮，infile為輸入檔案，outfile為輸出檔案)
    {
        inputfile.open(argv[3], ios::in | ios::binary );
        checkfile(inputfile);
        int OriginalSize = frequency(inputfile);
        inputfile.close();
        inputfile.seekg(0 ,ios::beg);
        int count = list.makeList();
        list.sorted();
        list.huffman();
        Encoding(count,OriginalSize,argv[5]);
    }
    else if(argv[1][1]=='u' )//huffman –u –i infile –o outfile     (進行解壓縮，infile為輸入檔案，outfile為輸出檔案)
    {
        inputfile.open(argv[3], ios::in | ios::binary );
        checkfile(inputfile);
        int bit = creatTable(inputfile);
        tmpWrite(inputfile);
        decoding(bit,argv[5]);
        inputfile.close();
    }
 
}

void checkfile(fstream &inputfile)
{
    if(!inputfile)
        {
            cout<<"File did't exit!"<<endl;
            exit(1);
        }
}

int frequency(fstream &inputfile)
{
    char index;
    int check=0;
    while(inputfile.get(index))
    {
        check=1;
        table[(int)index+offset]++;
    }
    if(!check)
    {
        cout<<"File is empty!"<<endl;
        exit(1);
    }
    cout<<"Frequency Table:"<<endl;
    for(int i=0 ; i<256 ;i++)
    {
        cout<<"ascii["<<setw(4)<<i-offset<<"]="<<setw(8)<<table[i]<<"  ";
        if((i+1)%8==0)
            cout<<endl;
    }
    ifstream::pos_type eof = inputfile.tellg();
    return eof;
}

int List::makeList()
{
    int count=0;
    for(int i=0;i<256;i++)
        if(table[i])
        {
            count++;
            push(i);
        }
    if(DEBUG)
    {
        cout<<endl<<"Before sort:"<<endl;
        for(Node *i=first ; i ; i=i->next)
            cout<<"table["<<i->ch-offset<<"]= "<<i->freq<<" ";  
        cout<<endl;
    } 
    return count; 
}

void List::push(int index)
{
    Node *tmp=new Node(table[index],index);
    if(first)
    {
        tmp->next=first;
        first=tmp;
    }
    else
        first=tmp;
}

void List::sorted()
{
    for(Node *i=first; i ; i=i->next)
    {
        Node *small = i;
        for(Node *j=i->next ; j ; j=j->next)
            if(j->freq < small->freq)
                small=j;
        int chTmp;
        long freqTmp;
        Node *Ltmp,*Rtmp;
        chTmp = i->ch;
        freqTmp = i->freq;
        Ltmp = i->left;
        Rtmp = i->right;
        i->ch = small->ch;
        i->freq = small->freq;
        i->left = small->left;
        i->right = small->right;
        small->ch = chTmp;
        small->freq = freqTmp;
        small->left = Ltmp;
        small->right = Rtmp;
    }
    if(DEBUG)
    {
        cout<<"After sort:"<<endl;
        for(Node *i=first ; i ; i=i->next)
        {
            cout<<"table["<<setw(4)<<i->ch-offset<<"]= "<<setw(4)<<i->freq<<"  Next= "<<setw(15)<<i->next;  
            cout<<"  Left= "<<setw(15)<<i->left;
            cout<<"  Right= "<<setw(15)<<i->right<<endl;
        }
        cout<<endl;
    } 
}

void List::huffman()
{
    while(first->next)
    {
        Node *tmp = new Node(first->freq + first->next->freq,-871);
        Node *L= first ,*R= first->next;
        tmp->next = first->next->next;
        tmp->left = L;
        tmp->right = R;
        first->next = NULL;
        first = tmp;
        sorted();
        if(DEBUG)
            cout<<"tmp->freq= "<<tmp->freq<<"\ttmp->ch= "<<tmp->ch<<"\ttmp->next= "<<tmp->next<<endl;
    }
}

void Encoding(int count, int OriginalSize,char *outputfileNAME)
{
    tmpEncodingTable.open("tmpEncodingTable",ios::out | ios::binary );
    tmpEncoding.open("tmpEncoding",ios::out | ios::binary );
    outputfile.open(outputfileNAME, ios::out | ios::binary);
    if(DEBUG)
    {
        if(!tmpEncodingTable) cout<<"~~~~~~~~~~~~~~~~~~~FUCK 1~~~~~~~~~~~~~~~~~~~"<<endl;
        if(!tmpEncoding) cout<<"~~~~~~~~~~~~~~~~~~~FUCK 2~~~~~~~~~~~~~~~~~~~"<<endl;
    }
    tmpEncodingTable<<count<<" ";
    walk_through(list.first , "\0");
    tmpEncodingTable<<endl;
    int bitNUM = tmpWrite();
    tmpEncodingTable.close();
    tmpEncoding.close();
    tmpEncodingTable.open("tmpEncodingTable",ios::in | ios::binary );
    tmpEncoding.open("tmpEncoding",ios::in | ios::binary );
    int CompressedSize = output(OriginalSize,bitNUM);
    tmpEncodingTable.close();
    tmpEncoding.close();
    if(DEBUG) cout<<"OriginalSize= "<<OriginalSize<<" CompressedSize= "<<CompressedSize<<endl;
    cout<<endl<<"Retio: "<<(double)OriginalSize/(double)CompressedSize<<endl;
}

int toDecimal(string binaryNUM)
{
	int i, two=1,decimal=0;
	for(i=binaryNUM.size()-1; i>=0 ; i--,two*=2)
			decimal+=((int)binaryNUM[i]-48)*two;
	return decimal;
}

char toDecimal(string bytes,int count)
{
	char ans;
	int i, two=1,decimal=0;
	for(i=0 ; i<8-count ; i++)
		bytes+="0";
	for(i=7; i>=0 ; i--,two*=2 )
		decimal+=((int)bytes[i]-48)*two;
	ans=(char)(decimal-offset);
	return ans;
}

void walk_through(Node *root , string encode)
{
    if(!root) return;
	walk_through(root->left, encode+'0');
	if(root->left==NULL && root->right==NULL)
	{
		tmpEncodingTable<<root->ch<<" "<<encode.size()<<" "<<toDecimal(encode)<<" ";
        encoding[root->ch]=encode;
        if(DEBUG)
            cout<<"root->ch= "<<root->ch-offset<<" encode.size()= "<<encode.size()<<" encode="<<setw(7)<<encode<<" toDecimal(encode)= "<<toDecimal(encode)<<endl;
	}
	walk_through(root->right, encode+'1');
}

int tmpWrite()
{
    char index;
    int count=0;
    while(inputfile.get(index))
    {
        tmpEncoding<<encoding[(int)index+offset];
        count+=encoding[(int)index+offset].size();
        if(DEBUG)
            cout<<encoding[(int)index+offset]<<" ";
    }
    if(DEBUG) cout<<endl;
    return count;
}

int output(int OriginalSize,int bitNUM)
{
    char tmp;
    while(tmpEncodingTable.get(tmp))
        outputfile<<tmp;
    outputfile<<bitNUM<<" ";
    system("rm tmpEncodingTable");

    string bytes="";
    while(tmpEncoding.get(tmp))
    {
        bytes+=tmp;
        if(bytes.size()==8)
        {
            outputfile<<toDecimal(bytes,8);
            bytes="";
        }
    }
    outputfile<<toDecimal(bytes,bytes.size());
    system("rm tmpEncoding");

    ifstream::pos_type eof = outputfile.tellg();
	return eof;
}

int creatTable(fstream &inputfile)
{
    int i,count,num;
    inputfile>>count;
    if(DEBUG) cout<<count<<endl;
    for(i=0;i<count;i++)
    {
        int index,size,data;
        inputfile>>index>>size>>data;
        if(DEBUG) cout<<index<<endl;
        encoding[index]=toBinary(data,size);
    }
    inputfile>>num;
    char trash;
    inputfile.get(trash);
    return num;
}

string toBinary(int data,int size)
{
    string ans="\0";
	int i;
	long int t=data;
	char input[size]= {0};
	for(i=0; i<size; i++)
	{
		input[i]=data%2;
		data/=2;
	}
	for(i=size-1 ; i>=0 ; i--)
	{
		if(input[i]==1)
			ans+='1';
		else
			ans+='0';
	}
	return ans;
}

void tmpWrite(fstream &inputfile)
{
    char index;
    tmpEncoding.open("tmpEncoding",ios::out | ios::binary );
    while(inputfile.get(index))
        tmpEncoding<<toBinary(index+offset,8);
    tmpEncoding.close();
}

void decoding(int num,char *outputfileNAME)
{
    outputfile.open(outputfileNAME, ios::out | ios::binary);
    tmpEncoding.open("tmpEncoding",ios::in | ios::binary );
    int i,j;
    char get;
    string decode="\0";
    cout<<"Decoding......"<<endl;
    for(i=0; i<num; i++)
		{
			tmpEncoding.get(get);
			decode+=get;
			for(j=0 ; j<256 ; j++)
				if(encoding[j]==decode)
				{
                    if(DEBUG) cout<<"decode= "<<decode<<endl;
					outputfile<<(char)(j-offset);
					decode="\0";
					break;
				}
		}
    cout<<"Complete!"<<endl;
    tmpEncoding.close();
    outputfile.close();
    system("rm tmpEncoding");
}