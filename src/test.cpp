#include <string>
#include <fstream>
#include <streambuf>
#include <iostream>
#include <vector>
#include <chrono>
#include <string.h>
#include <unistd.h>
#include <Semaphore.h>
static const int THREADS_NUM = 4;
static int ItemAvail = 0; 
static int ItemNum;
static Semaphore xx;
static Semaphore BufferLock("BufferLock",1); 
auto buffer,logger;
std::vector<std::string> split_string(const std::string& str,
                                      const std::string& delimiter)
{
    std::vector<std::string> strings;
 
    std::string::size_type pos = 0;
    std::string::size_type prev = 0;
    while ((pos = str.find(delimiter, prev)) != std::string::npos)
    {
        strings.push_back(str.substr(prev, pos - prev));
        prev = pos + 1;
    }
    return strings;
}
std::vector<char> split_item(const std::string& str)
{
	std::vector<char> res;
	char x;
	for(auto i=str.begin();i!=str.end();i++){
		x=*i;
		if(*i==' '){
			sleep(1);
			continue;
		}
		res.push_back(x);
	}
	return res;
}
void bubblesort(std::vector<char>& a){
    int len=a.size();
    int j2; 
    for(int i=0;i<len;i++) {
       	for(int j=len-1;j>=i;j--){
    		j2=j-1;
   			if (a[j2+1]<a[j2] && j2>=i)
    			std::swap(a[j2+1],a[j2]);
   		}
    }
}
void hashsort(std::vector<char>& a){
	int charCount[10]={0};
	for(int i=0;i<a.size();i++)
		charCount[a[i]-'0']++;
	a.clear();
	for(int i=0;i<10;i++)
		for(int j=0;j<charCount[i];j++)
			a.push_back('0'+i);
}
void spawnThread(int tid,int sort)
{
	std::vector<char> v;
	while (true){
		NotEmpty.Wait(); //wait util the buffer has data
		BufferLock.Wait(); //lock the buffer
		if(Out<ItemNum){
			v=split_item(buffer.at(ItemAvail));
			ItemAvail++;
			BufferLock.Signal();
			if(sort==0)
				bubblesort(v);
			else
				hashsort(v);
				ostringstream s;
			for(const auto&i:v){
				if(&i !=&v[0]){
				s<<','
			}
			s<<i;	
			logger->write("Tread "+std::to_string(tid)+":"+s.str());	
		}
		else{
			std::cout<<"Tread "+std::to_string(tid)<<" No available work"<<endl;
			BufferLock.Signal();
			break;
		}
	}
}
int main()
{
	string InputName,OutputName;
	int sorttype;
	if(argc<2){
		InputName="input.txt";
		OutputName="output.txt";
		sorttype=0;
	}	
	else{
		InputName=argv[1];
		OutputName=argv[2];
		sorttype=argv[3];
	}
	std::ifstream t(InputName);
	std::string str;
	std::vector<char> v;	
	t.seekg(0, std::ios::end);   
	str.reserve(t.tellg());
	t.seekg(0, std::ios::beg);
	
	str.assign((std::istreambuf_iterator<char>(t)),
	            std::istreambuf_iterator<char>());
	buffer = split_string(str, "\n");
	
	std::thread threadPool[THREADS_NUM];
    	logger = std::make_shared<Logger>(OutputName);
 	ItemAvail=0;
 	ItemNum=buffer.size();
    	for(int i=0;i<THREADS_NUM;++i)
    	{
    		threadPool[i]=std::thread(spawnThread,i,sorttype);
    		threadPool[i].join();
	}  
        system("pause");
        return 0;
}
