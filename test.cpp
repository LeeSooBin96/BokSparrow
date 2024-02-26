#include <iostream>
#include <iomanip>

int main()
{
    std::cout<<"여긴 왼쪽";
    std::cout<<std::right;
    std::cout<<std::setw(20)<<"여긴 오른쪽 \n";
    std::cout<<"그다음줄도적용되나글자수넘어가면어떻게되는거지자동띄어쓰기되면좋겠다\n";

    return 0;
}