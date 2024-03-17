#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define NARRAY 7   // Array size
#define NBUCKET 6  // Number of buckets
#define INTERVAL 10  // Each bucket capacity

struct Node {
	int data;
	struct Node *next;
};

struct BucketSortArgs {
	struct Node **buckets;
};

void *BucketSort(void *args);
struct Node *InsertionSort(struct Node *list);
void print(int arr[]);
void printBuckets(struct Node *list);
int getBucketIndex(int value);

pthread_mutex_t mutex;

void *BucketSort(void *args) {
	struct BucketSortArgs *buck_args = (struct BucketSortArgs *)args;
 	struct Node **buckets = buck_args->buckets;

	for (int i=0;i<NBUCKET; ++i){
		pthread_mutex_lock(&mutex);
		buckets[i] = InsertionSort(buckets[i]);
		pthread_mutex_unlock(&mutex);
	}
	
	pthread_exit(NULL);
}


struct Node *InsertionSort(struct Node *list) {
	struct Node *k, *nodeList;
	
	if (list == 0 || list->next == 0) {
		return list;
	}

	nodeList = list;
	k = list->next;
	nodeList->next = 0;
	
	while (k != 0) {
		struct Node *ptr;
		if (nodeList->data > k->data) {
			struct Node *tmp;
			tmp = k;
			k = k->next;
			tmp->next = nodeList;
			nodeList = tmp;
			continue;
		}

		for (ptr = nodeList; ptr->next != 0; ptr = ptr->next) {
			if (ptr->next->data > k->data)
				break;
		}

		if (ptr->next != 0) {
			struct Node *tmp;
			tmp = k;
			k = k->next;
			tmp->next = ptr->next;
			ptr->next = tmp;
			continue;
		} 
		else {
			ptr->next = k;
			k = k->next;
			ptr->next->next = 0;
			continue;
		}
	}
	return nodeList;
}

int getBucketIndex(int value) {
	return value / INTERVAL;
}

void print(int ar[]) {
	for (int i = 0; i < NARRAY; ++i) {
		printf("%d ", ar[i]);
	}
	printf("\n");
}

void printBuckets(struct Node *list) {
	struct Node *cur = list;
	while (cur) {
    		printf("%d ", cur->data);
    		cur = cur->next;
  	}
}

int main(void) {
	int array[NARRAY] = {42, 32, 33, 52, 37, 47, 51};
	
	clock_t start = clock();
	
	printf("Initial array: ");
	print(array);
	printf("-------------\n");

	struct Node **buckets = (struct Node **)malloc(sizeof(struct Node*) *NBUCKET);
	
	for (int i=0;i<NBUCKET;++i)
		buckets[i] = NULL;
		
	for (int i=0;i<NARRAY;++i){
		struct Node *current;
		int position = getBucketIndex(array[i]);
		current = (struct Node*) malloc (sizeof (struct Node));
		current->data = array[i];
		current->next = buckets[position];
		buckets[position] = current;
	}
	
	for (int i=0;i<NBUCKET;i++){
		printf("Bucket[%d]: ", i);
		printBuckets(buckets[i]);
		printf("\n");
	}
	
	printf("------------\n");
	printf("Buckets after sorting\n");
	
	pthread_t th;
	
	struct BucketSortArgs buck_args = {buckets};
	
	pthread_create(&th, NULL, BucketSort, &buck_args);
	
	pthread_join(th, NULL);
	
	for (int i=0;i<NBUCKET;i++) {
		printf("Bucket[%d]: ", i);
		printBuckets(buckets[i]);
		printf("\n");
	}
	
	for (int i=0,j=0;i<NBUCKET;++i) {
		struct Node *n;
		n = buckets[i];
		while (n) {
			array[j++] = n->data;
			n = n->next;
		}
	}

	clock_t end = clock();	
	
	printf("-------------\n");
	printf("Sorted array: ");
	print(array);
	
	  printf("Parallel Time = %fs", ((double) (end-start)) /CLOCKS_PER_SEC);
	return 0;
}
