#include <iostream>

using namespace std;
class cell;

class grid{
	int height, width;
	//cell cells[height*width];
	public:
	grid(int x, int y){
		height=x; width=y;
	}
	
	void display(){
		for(int j=0; j<height; j++){
			for(int i=0; i<width; i++){
					cout<<". ";
			}
			cout<<endl;
		}
	}
			
};

class cell{
	
};

int main(){
	grid g(5, 5);
	g.display();
	return 0;
}
