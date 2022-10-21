#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include<string.h>
#include <unistd.h>
#include <fcntl.h>
#include "BookManage.h"

void printMenu(){
    printf("1) 도서정보 출력 (Sort by 도서번호) \n");
    printf("2) 도서정보 출력 (Sort by 도서명) \n");
    printf("3) 도서정보 추가 \n");
    printf("4) 도서정보 수정 \n");
    printf("5) 도서정보 삭제 \n");
    printf("6) 도서명 검색 (최소 2글자) \n");
    printf("0) 종료 \n");
    printf("입력 :  ");
}
void printList( BOOK temp){
        printf("\n도서번호 :  %d \n",temp.book_Num );
        printf("도서명 :  %s \n",temp.bookName );
        printf("저자명 :  %s \n",temp.writer );
        printf("출판년월일 :  %s \n",temp.publishing );
        printf("추천리뷰 :  %s \n",temp.reveiw );
        printf("가격 :  %d원 \n",temp.price );
}
void dbLength( struct tagBOOK record , char *argv[] , int *count){ 
  int fd = open(argv[1], O_RDONLY | O_CREAT);
  for (int i = 1; ; i++){
	    int ret = read(fd, (char*)&record, sizeof(record));
    	if (ret != sizeof(record)) break;
        if(record.book_Num != 0) *count += 1; 
		lseek(fd, sizeof(record)*i, SEEK_SET);
    }
    close(fd);	
}
int compare( const void *a,const void *b){  
    return strcmp(((BOOK*)a)->bookName,((BOOK*)b)->bookName);
}
void dbSortBookName( struct tagBOOK record , char *argv[] ,int count){
  int fd = open(argv[1], O_RDONLY) , num= 0;
  BOOK* temp = (BOOK*)malloc(sizeof(BOOK) * count); 
  for (int i = 1; ; i++){ 
	    int ret = read(fd, (char*)&record, sizeof(record));
    	if (ret != sizeof(record)) break;
        if(record.book_Num != 0) { 
            temp[num] =record;
            num++;
            }
		lseek(fd, sizeof(record)*i, SEEK_SET);
    }	
    close(fd);
    qsort(temp,count,sizeof(BOOK),compare);
    for(int i=0; i<num; i++) printList(temp[i]); 
    free(temp); 
}
void dbSortBook_Num(struct tagBOOK record , char *argv[] ){
int fd = open(argv[1], O_RDONLY) ;
  for (int i = 1; ; i++){
	    int ret = read(fd, (char*)&record, sizeof(record));
    	if (ret != sizeof(record)) break;
        if(record.book_Num != 0) printList(record); 
		lseek(fd, sizeof(record)*i, SEEK_SET);
    }	
    close(fd);
}
void addData(struct tagBOOK record , char *argv[] ,int* count){
    int fd = open(argv[1], O_WRONLY , 0640) ;
    printf("%d\n\n",fd);
    char sel;
	printf("도서번호   도서명   저자명   출판년월일  가격  추천리뷰(공백포함) \n");  
    scanf("%d %s %s %s %d %[^\n]s", &record.book_Num, record.bookName, record.writer, record.publishing , &record.price, record.reveiw);
	lseek(fd, (record.book_Num) * sizeof(record), SEEK_SET);
	write(fd, (char *)&record, sizeof(record));
    *count += 1;
    close(fd);
    printf("\n도서정보를 추가로 더 입력하시겠습니까 ?  (Y  N)  \n");
    scanf(" %c",&sel);
    if(sel == 'Y') addData(record ,argv, count);
}
void updateDB(struct tagBOOK record, char *argv[]){   
    int search ,fd = open(argv[1], O_RDWR , 0640);
    printf("\n수정할 도서정보의 도서번호 입력:");
    scanf("%d", &search);
 	lseek(fd, (long)search * sizeof(record), SEEK_SET);
	if ((read(fd, (char *)&record, sizeof(record)) > 0) && (record.book_Num != 0)){
        printf("도서명  저자명   출판년월일  가격  추천리뷰(공백포함) \n");
        scanf("%s %s %s %d %[^\n]s",  record.bookName, record.writer, record.publishing,  &record.price,record.reveiw );
        lseek(fd, (long) -sizeof(record), SEEK_CUR);
        write(fd, (char *)&record, sizeof(record));
      }
	else printf("검색하신 해당 도서정보가 없습니다.\n");
    close(fd);
}
void deleteDB(struct tagBOOK record, char *argv[],int *count){
    int search ,fd = open(argv[1], O_RDWR , 0640);;
    printf("\n삭제할 도서정보의 도서번호 입력:");
    scanf("%d", &search);
 	lseek(fd, (long)search* sizeof(record), SEEK_SET);
	if ((read(fd, (char *)&record, sizeof(record)) > 0) && (record.book_Num != 0)){
        record.book_Num = 0 ; 
        lseek(fd, (long) -sizeof(record), SEEK_CUR);
        write(fd, (char *)&record, sizeof(record));
        *count -=1;
      }
	else printf("검색하신 해당 도서정보가 없습니다.\n");
    close(fd);
}
void searchDB(struct tagBOOK record, char *argv[]){
    int fd = open(argv[1], O_RDONLY);
    char search [50];
    printf("검색할 도서이름 입력 :  ");
    scanf("%s", search);
    for (int i = 1; ; i++){
	    int ret = read(fd, (char*)&record, sizeof(record));
    	if (ret != sizeof(record)){
	    	break;
        }
		if (record.book_Num !=0 && strstr(record.bookName,search) ) printList(record); 
        lseek(fd, sizeof(record)*i, SEEK_SET);
    }	
    close(fd);
}
void loop(int argc, char *argv[]){
    int sel , count = 0;
	struct tagBOOK record;
	if (argc < 2) {
		fprintf(stderr, "사용법 : %s file\n", argv[0]);
		exit(1);
	}
    dbLength(record,argv,&count);  
    while(1){
        printMenu(); 
        scanf("%d",&sel);
        if(sel == 1) dbSortBook_Num(record ,argv); 
        if(sel == 2) dbSortBookName(record,argv,count);
        if(sel == 3) addData(record,argv,&count);
        if(sel == 4) updateDB(record,argv);
        if(sel == 5) deleteDB( record,argv,&count);
        if(sel == 6) searchDB(record,argv);      
        if(sel == 0) break; 
        printf("\n");
    }
}
int main(int argc, char *argv[]){
    loop(argc,argv);
}
