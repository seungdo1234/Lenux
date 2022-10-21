#include<stdio.h>

typedef struct Student {
	int studentNumber;
	char name[10];
	char birthYear[10];
	char phoneNumber[12];
	float courseCredit;
	struct Student* next;
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
void inputData(int num , stu** head) {

	for (int i = 0; i <num; i++) {
		stu* temp = (stu*)malloc(sizeof(stu));
		scanf("%d %s %s %s %f", &temp->studentNumber, temp->name, temp->birthYear, temp->phoneNumber, &temp->courseCredit);
		temp->next = NULL;
		if (*head == NULL)	*head = temp;
		else
		{
			stu* cur = *head;
			// 마지막 노드를 찾는 루프
			while (cur ->next != NULL) cur = cur->next;
			cur->next = temp;
		}
	}
}
void printList(stu** head) {
	stu* curr = *head;
	while (curr != NULL) {
		printf("%d   %s   %s   %s   %.2f\n", curr->studentNumber, curr->name, curr->birthYear, curr->phoneNumber, curr->courseCredit);
		curr = curr-> next;
	}
}
void revisePhoneNumber(stu** head) {
	int search;
	printf("\n핸드폰 번호를 수정할 학생의 학번을 입력하시오.  ");
	scanf("%d", &search);
	stu* curr = *head;
	while (curr != NULL) {
		if (curr->studentNumber == search) {
			printf("수정할 핸드폰 번호를 입력하시오.   ");
			scanf("%s", &curr->phoneNumber);
		}
		curr = curr->next;
	}	
}
void reviseCourseCredit(stu** head) {
	int search;
	printf("\n학점을 수정할 학생의 학번을 입력하시오.  ");
	scanf("%d", &search);
	stu* curr = *head;
	while (curr != NULL) {
		if (curr->studentNumber == search) {
			printf("수정할 학점을 입력하시오.   ");
			scanf("%f", &curr->courseCredit);	
		}
		curr = curr->next;
	}
}
void deleteData(stu** head) {
	int search;
	printf("\n삭제할 학생의 학번을 입력하시오.  ");
	scanf("%d", &search);
	stu* curr = *head;
	if (*head == NULL) return;
	else if (curr->studentNumber == search) {
		*head = curr->next;
		free(curr);
	}
	else {
		while (curr != NULL) {
			if (curr->next->studentNumber == search) {
				curr->next = curr->next->next;
				free(curr->next);
			}
			curr = curr->next;
		}
	}
}
void studentManagement() {
	int sel = -1, num = 0 ;
	stu* head = NULL;
	while (sel != 0) {
		printMenu();
		scanf(" %d", &sel);
		if (sel == 1) {
			printf("저장할 학생 명수를 입력하시오. ");
			scanf("%d", &num);
		}
		if (sel == 2) inputData(num ,&head);
		if (sel == 3) printList( &head);
		if (sel == 4) revisePhoneNumber(&head);
		if (sel == 5) reviseCourseCredit(&head);
		if (sel == 6) {
			printf("\n추가할 학생의 학번, 이름, 생년월일, 핸드폰번호, 학점을 입력하시오.  ");
			inputData(1, &head);
		}
		if (sel == 7) deleteData(&head);
	}
}
void main() {
	studentManagement();
}
