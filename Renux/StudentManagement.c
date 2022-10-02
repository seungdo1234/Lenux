#include<stdio.h>

struct Student {
	int studentNumber[100];
	char name [100][10];
	char birthYear[100][10];
	char phoneNumber [100][12];
	float courseCredit[100];
}stu;

void printMenu() {
	printf("1) 저장할 학생 명수 입력(n)\n");
	printf("2) 학생struct 정보 입력(n명)\n");
	printf("3) 학생 목록 출력\n");
	printf("4) 학생 정보 수정(핸드폰 번호)\n");
	printf("5) 학생 정보 수정(학점)\n");
	printf("6) 학생 정보 추가\n");
	printf("7) 학생 정보 삭제\n");
	printf("0) 종료\n");
	printf("\n입력 : ");
}
void inputData(int *num ,int *count ) {
	for (int i = 0; i < *num; i++) {
		printf("%d번 학생 : ", *count + 1);
		scanf("%d %s %s %s %f", &stu.studentNumber[*count] , stu.name[*count], stu.birthYear[*count], stu.phoneNumber[*count], &stu.courseCredit[*count]);
		*count+=1;
	}
	*num = 0;
}
void printList(int count) {
	for (int i = 0; i < count; i++) printf("%d번 학생 : %d  %s  %s  %s  %.2f \n", i + 1, stu.studentNumber[i], stu.name[i], stu.birthYear[i], stu.phoneNumber[i], stu.courseCredit[i]);
}
void revisePhoneNumber(int count) {
	int search;
	printf("\n핸드폰 번호를 수정할 학생의 학번을 입력하시오.  ");
	scanf("%d", &search);
	for (int i = 0; i < count; i++) if (stu.studentNumber[i] == search) scanf("%s", stu.phoneNumber[i]);
}
void reviseCourseCredit(int count) {
	int search;
	printf("\n학점을 수정할 학생의 학번을 입력하시오.  ");
	scanf("%d", &search);
	for (int i = 0; i < count; i++) if (stu.studentNumber[i] == search) scanf("%f", &stu.courseCredit[i]);
}
void studentManagement() {
	int sel = -1, num = 0, count = 0;;

	while (sel != 0) {
		printMenu();
		scanf(" %d", &sel);
		if (sel == 1) {
			printf("저장할 학생 명수를 입력하시오. ");
			scanf("%d", &num);
		}
		if (sel == 2) inputData(&num ,&count);
		if (sel == 3) printList(count);
		if (sel == 4) revisePhoneNumber(count);
		if (sel == 5) reviseCourseCredit(count);
	}
}
void main() {
	studentManagement();
}