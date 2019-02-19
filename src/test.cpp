#include <string>
#include <fstream>
#include <streambuf>
#include <iostream>
#include <sstream>
#include <iterator>
#include <vector>
#include <string.h>
#include <unistd.h>
#include <thread>
#include <mutex>
#include <condition_variable>
std::mutex mtx;
std::condition_variable cv;
static const int THREADS_NUM = 4;
static int ItemAvail = 0; 
static int ItemNum;
std::ofstream file;
std::vector<std::string> buffer;
std::vector<std::string> split_string(const std::string& str, const std::string& delimiter)
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
void bubblesort(std::vector<char>& a)
{
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
void hashsort(std::vector<char>& a)
{
	int charCount[10]={0};
	for(int i=0;i<a.size();i++)
		charCount[a[i]-'0']++;
	a.clear();
	for(int i=0;i<10;i++)
		for(int j=0;j<charCount[i];j++)
			a.push_back('0'+i);
}
void consumer(int tid,char sort)
{
	std::vector<char> v;
	std::unique_lock<std::mutex> lck(mtx);
	while (true){
		cv.wait(lck); 
		if(ItemAvail<ItemNum){
			v=split_item(buffer.at(ItemAvail));
			ItemAvail++;
			cv.notify_all();
			if(sort=='0')
				bubblesort(v);
			else
				hashsort(v);
			std::ostringstream vts;
			if (!v.empty()) 
  			{ 
	   			std::copy(v.begin(),v.end()-1, std::ostream_iterator<int>(vts, ", ")); 
				vts << v.back(); 
  			} 
			file<<"thread "<<tid<<":"<<vts.str()<<std::endl;	
		}
		else{
			cv.notify_all();
			std::cout<<"Tread "+std::to_string(tid)<<" No available work"<<std::endl;
			break;
		}
	}
}
int main(int argc,char *argv[])
{
	std::string InputName,OutputName;
	char sorttype;
	if(argc<2){
		InputName="input.txt";
		OutputName="output.txt";
		sorttype='0';
	}	
	else{
		InputName=argv[1];
		OutputName=argv[2];
		sorttype=argv[3][0];
	}
	std::ifstream t(InputName);
	std::string str;
	std::vector<char> v;	
	t.seekg(0, std::ios::end);   
	str.reserve(t.tellg());
	t.seekg(0, std::ios::beg);	
	str.assign((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
	buffer = split_string(str, "\n");
	std::thread consumers[THREADS_NUM];
	file.open(OutputName);
 	ItemAvail=0;
 	ItemNum=buffer.size();
    for(int i=0;i<THREADS_NUM;++i)
    {
    	consumers[i]=std::thread(consumer,i,sorttype);
    	consumers[i].join();
	}    
    file.close();
    system("pause");
    return 0;
}
