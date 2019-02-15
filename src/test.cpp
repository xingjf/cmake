#include <string>
#include <fstream>
#include <streambuf>
#include <iostream>
#include <vector>
#include <chrono>
#include "ThreadPool.h"
#include <string.h>
#include <unistd.h>
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
 
    // To get the last substring (or only, if delimiter is not found)
    //J strings.push_back(str.substr(prev));
    return strings;
}
void solution(char *res){
    int len=strlen(res);
    int j2; 
    std::vector<char> v;
    for(int i=0;i<len;i++) {
       	for(int j=len-1;j>=i;j--){
    		j2=j-1;
		if(res[j2]==' '){
			sleep(1);
			while(j2<len-2) {
				res[j2]=res[j2+1];
				j2++;
			}
			len--;
			continue;
  		}
		if(j2<j-1){
			res[j2+1]=res[j];
			len=len-(j-j2-1);
		}
   		if (res[j2+1]<res[j2] && j2>=i)
    			std::swap(res[j2+1],res[j2]);
   	}
     }
}
int main()
{
std::ifstream t("input.txt");
std::string str,tmp;

t.seekg(0, std::ios::end);   
str.reserve(t.tellg());
t.seekg(0, std::ios::beg);

str.assign((std::istreambuf_iterator<char>(t)),
            std::istreambuf_iterator<char>());
auto strings = split_string(str, "\n");
    int i = 1;
    for (auto itr = strings.begin(); itr != strings.end(); itr++){
        std::cout << "#" << i++ << " - \"" << *itr << "\"\n";
	tmp=*itr;
    	solution((char*)tmp.data());
    }
    ThreadPool pool(4);
    std::vector< std::future<int> > results;
       for(int i = 0; i < 8; ++i) {
         results.emplace_back(
            pool.enqueue([i] {
            std::cout << "hello " << i << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
             return i*i;
      	    })
         );
	}
   for(auto && result: results)
        std::cout << result.get() << ' ';
        std::cout << std::endl;
}
