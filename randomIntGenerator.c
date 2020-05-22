#include<stdio.h> 
#include<stdlib.h> 
#include<time.h>

#define N 5


int main (int argc, char** argv){
	if (argc != 3){
		fprintf(stderr, "invalid argument\n");
		fprintf(stderr, "usase: integers to be generated, interval");
		return 1;
	}

	int num = atoi(argv[1]);
	int interval = atoi(argv[2]);

	if (interval < 0 || interval > N - 1){
		fprintf(stderr, "Inputed Interval is invalid . It should be >= 0 && <= N - 1\n");
		return 1;
	}

	int intervals[N] = {};
	int cur = 0;

	for(int i = 0 ; i < N ; i++){
//		printf("intervals[%d]=%d\n", i, intervals[i]);
	}

	srand((unsigned)time(NULL));
	while(cur < num){
		int v = rand() % N;
	//	printf("v=%d\n", v );
//		printf("intervals[v]=%d\n", intervals[v]);

		if (intervals[v] <= 0){
			printf("%d ",  v+1);
			for(int i = 0 ; i < N ; i++){
			       intervals[i]--;
			}	       
			intervals[v] = interval;
		       	cur++;
	       	}
       	} 
	printf("\n");

	return 0;
}


