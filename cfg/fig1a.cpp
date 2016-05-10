/***************************************************************
 *  @file cfg/fig1a.cpp
 *  @brief           
 *             
 *  @author Li Jiaying
 *  @bug no known bugs
 ***************************************************************/
void main () {
	int x,y;
	x=0;
	y=0;
	
	while (nondet()) {
		x++;
		y++;
	}
	
	while (x > 0 || x < 0) {
		x--;
		y--;
	}
	
	assert (y >= 0 && y <= 0);
	
}
