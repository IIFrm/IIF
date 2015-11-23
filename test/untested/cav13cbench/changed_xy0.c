int nondet();

int main (){

	int x = 0;
	int  y = 0;

	while (nondet()){
		if (nondet()) {
			x += 1;
			y += 1;
		}
	}

	while (x != 0){
		if (nondet()) {
			x = x - 1;
			y = y - 1;
		}
	}

	if (y != 0)
		goto ERROR;

	return 1;

ERROR:;
}
