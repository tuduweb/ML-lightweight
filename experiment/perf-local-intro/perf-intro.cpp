#include <iostream>
#include <vector>
#include <string>
#include <unistd.h>

using namespace std;
void bar(){
//   usleep(40*1000);
  /* do something here */
  for(int i=0;i< 4000;i++)
  {

  }
}

void foo(){
//   usleep(60*1000);
  for(int i=0;i< 5700;i++)
  {
      
  }      
  bar();
}

void do_main() {
  foo();
}

int main(int argc,char** argv){
    while(1)
    {
        do_main();
    }
}
