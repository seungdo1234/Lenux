#include<stdio.h>

struct Student {
	int studentNumber[100];
	char name [100][10];
	char birthYear[100][10];
	char phoneNumber [100][12];
	float courseCredit[100];
}stu;

void printMenu() {
	printf("1) ������ �л� ��� �Է�(n)\n");
	printf("2) �л�struct ���� �Է�(n��)\n");
	printf("3) �л� ��� ���\n");
	printf("4) �л� ���� ����(�ڵ��� ��ȣ)\n");
	printf("5) �л� ���� ����(����)\n");
	printf("6) �л� ���� �߰�\n");
	printf("7) �л� ���� ����\n");
	printf("0) ����\n");
	printf("\n�Է� : ");
}
void inputData(int *num ,int *count ) {
	for (int i = 0; i < *num; i++) {
		printf("%d�� �л� : ", *count + 1);
		scanf("%d %s %s %s %f", &stu.studentNumber[*count] , stu.name[*count], stu.birthYear[*count], stu.phoneNumber[*count], &stu.courseCredit[*count]);
		*count+=1;
	}
	*num = 0;
}
void printList(int count) {
	for (int i = 0; i < count; i++) printf("%d�� �л� : %d  %s  %s  %s  %.2f \n", i + 1, stu.studentNumber[i], stu.name[i], stu.birthYear[i], stu.phoneNumber[i], stu.courseCredit[i]);
}
void revisePhoneNumber(int count) {
	int search;
	printf("\n�ڵ��� ��ȣ�� ������ �л��� �й��� �Է��Ͻÿ�.  ");
	scanf("%d", &search);
	for (int i = 0; i < count; i++) if (stu.studentNumber[i] == search) scanf("%s", stu.phoneNumber[i]);
}
void reviseCourseCredit(int count) {
	int search;
	printf("\n������ ������ �л��� �й��� �Է��Ͻÿ�.  ");
	scanf("%d", &search);
	for (int i = 0; i < count; i++) if (stu.studentNumber[i] == search) scanf("%f", &stu.courseCredit[i]);
}
void studentManagement() {
	int sel = -1, num = 0, count = 0;;

	while (sel != 0) {
		printMenu();
		scanf(" %d", &sel);
		if (sel == 1) {
			printf("������ �л� ����� �Է��Ͻÿ�. ");
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