#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define ERROR_TOLERANCE 1e-6
#define MAX_ITERATION 100
#define MAX_TEST_INTERATION(n) ((int)pow(10, n))

static float newton_iter = 0;
static float binarySearch_iter = 0;
static float magic_iter = 0;

float abs_f(float n) {
	return n > 0.0 ? n : -n;
}

float randNum() {
	float integer = rand(rand()) % 1000;
	float fraction = rand(rand()) % 1000;
	fraction = fraction == 0 ? fraction : 1 / fraction;
	
	return integer + fraction;
}

float sqrtNewton(float n) {
	float root = n / 2, error = root * root - n;
	int i = 0;
	for (; (error > ERROR_TOLERANCE || error < -ERROR_TOLERANCE) && i < MAX_ITERATION; i++) {
		root -= error / (2 * root);
		error = root * root - n;
		//printf("iteration %2d: %.20llf, error = %.20llf\n", i + 1, root, error);
	}
	newton_iter += i;
	return root;
}

float sqrtBinarySearch(float n) {
	float low = 1 < n ? 1 : n, high = 1 > n ? 1 : n;
	float root = (low + high) / 2, error = root * root - n;
	int i = 0;
	for (; (error > ERROR_TOLERANCE || error < -ERROR_TOLERANCE) && i < MAX_ITERATION; i++) {
		if (error > 0) {
			high = root;
		}
		else if (error < 0) {
			low = root;
		}
		else {
			break;
		}
		root = (low + high) / 2;
		error = root * root - n;
		//printf("iteration %2d: %.20llf, error = %.20llf\n", i + 1, root, error);
	}
	binarySearch_iter += i;
	return root;
}

//reference from wiki: https://wikipedia.org/wiki/Fast_inverse_square_root
float sqrtMagic(float n) {
	float root = n;
	long i = *(long*)&root; // evil floating point bit level hacking
	i = 0x5f3759df - (i >> 1); // what the fuck?
	root = *(float*)&i;
	float error = root * root - n;
	i = 0;
	for (; i < 3; i++) {
		//3 interation can obtain a similar total error
		//to newton's and binary search method
		root = root * (1.5 - (n * 0.5 * root * root));
		error = root * root - n;
		//printf("iteration %2d: %.20llf, error = %.20llf\n", i, root, error);
	}
	magic_iter += i;
	return 1 / root;
}

int main() {
	float inputs[4] = { 0, 36.0, 2.25, 0.01 };
	float input, library, newton, binarySearch, magic;
	for (int i = 0; i < 4; i++) {
		printf("\ncomputing the root of the value: %f...\n", inputs[i]);
		library = sqrt(inputs[i]);
		printf("C library sqrt:\n%.20llf\n", library);
		newton = sqrtNewton(inputs[i]);
		printf("newton's method sqrt:\n%.20llf, error = %.20llf\n", newton, newton - library);
		binarySearch = sqrtBinarySearch(inputs[i]);
		printf("binary search method sqrt:\n%.20llf, error = %.20llf\n", binarySearch, binarySearch - library);
		magic = sqrtMagic(inputs[i]);
		printf("fast & magical sqrt:\n%.20f, error = %.20f\n", magic, magic - library);
	}

	int test_num = 6;//10^(test_num)
	clock_t t1, t2;
	printf("\n\nrun %d times for calculating total cost times...\n", MAX_TEST_INTERATION(test_num));
	t1 = clock();
	for (int i = 0; i < MAX_TEST_INTERATION(test_num); i++) {
		input = randNum();
		input = sqrt(input);
	}
	t2 = clock();
	printf("\nC library cost %f secs\n", (float)(t2 - t1) / 1000);
	
	t1 = clock();
	for (int i = 0; i < MAX_TEST_INTERATION(test_num); i++) {
		input = randNum();
		input = sqrtNewton(input);
	}
	t2 = clock();
	printf("\nNewton's method cost %f secs\n", (float)(t2 - t1) / 1000);
	
	t1 = clock();
	for (int i = 0; i < MAX_TEST_INTERATION(test_num); i++) {
		input = randNum();
		input = sqrtBinarySearch(input);
	}
	t2 = clock();
	printf("\nBinary search method cost %f secs\n", (float)(t2 - t1) / 1000);

	t1 = clock();
	for (int i = 0; i < MAX_TEST_INTERATION(test_num); i++) {
		input = randNum();
		input = sqrtMagic(input);
	}
	t2 = clock();
	printf("\nfast & magical method cost %f secs\n", (float)(t2 - t1) / 1000);

	printf("\n\nrun %d times for calculating avg error and iterations...\n", MAX_TEST_INTERATION(test_num));
	float newton_err = 0;
	float binarySearch_err = 0;
	float magic_err = 0;
	newton_iter = 0;
	binarySearch_iter = 0;
	magic_iter = 0;

	for (int i = 0; i < MAX_TEST_INTERATION(test_num); i++) {
		input = randNum();
		library = sqrt(input);
		newton_err += abs_f(sqrtNewton(input) - library);
		binarySearch_err += abs_f(sqrtBinarySearch(input) - library);
		magic_err += abs_f(sqrtMagic(input) - library);
	}
	printf("\nNewton's method avg error: %f, iterations: %f\n", 
		newton_err / MAX_TEST_INTERATION(test_num), 
		newton_iter / MAX_TEST_INTERATION(test_num));
	printf("\nBinary search method avg error: %f, iterations: %f\n", 
		binarySearch_err / MAX_TEST_INTERATION(test_num), 
		binarySearch_iter / MAX_TEST_INTERATION(test_num));
	printf("\nfast & magical method avg error: %f, iterations: %f\n", 
		magic_err / MAX_TEST_INTERATION(test_num), 
		magic_iter / MAX_TEST_INTERATION(test_num));
}