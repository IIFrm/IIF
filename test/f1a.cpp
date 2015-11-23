
int main (int argc, char** argv) {
	int x;	
	int y;
	//Precondition: x + y >= 0
	//assume (x + y >= 0)

	while (x >= 0) {
		x--;
		y++;
	}

	// Postcondition: y > 0
	// assert (y > 0);
	assert (y > 0);

	return 0;
}

