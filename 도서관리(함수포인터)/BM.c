#include <stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<fcntl.h>
#include<sys/types.h>
#include"Book.h"
#include <time.h>
void printMenu(){
    printf("Book Management\n");
    printf("1. List up All Book (Sort by 식별자)\n");
    printf("2. List up All Book (Sort by 도서명)\n");
    printf("3. Add a New Book\n");
    printf("4. Update a New Book\n");
    printf("5. Remove a Book\n");
    printf("6. Search a Book Information by Title (minimum 2char)\n");
    printf("7. Search a Book Information by ID\n");
    printf("8. 정렬 성능 테스트\n");
    printf("0. Quit\n");
    printf("Choose Num -->   ");
}
void printList(BK temp){
    printf("\n1. 도서번호 : %d\n",temp.book_Num);
    printf("2. 도서명 : %s \n",temp.book_Name);
    printf("3. 저자명 : %s \n",temp.writer);
    printf("4. 출판년월일 : %s \n",temp.publishing);
    printf("4. 가격 : %d \n",temp.price);
    printf("5. 추천리뷰 : %s \n\n",temp.review);
}
void fileLock(char fileName[], struct Book record, struct flock lock,int fd,int search, int lockType){
    if(lockType == 1) lock.l_type= F_RDLCK;
    else lock.l_type= F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = search * sizeof(record);
    if(search == 0) lock.l_len = 0;
    else lock.l_len = sizeof(record);
    if (fcntl(fd,F_SETLKW,&lock) == -1){
        perror(fileName);
        exit(2);
    }
}
int dbLength(char fileName[], struct Book record, struct flock lock){
    int fd = open(fileName,O_RDONLY),count = 0;
    for(int i =1; ; i++){
        if( i != 1) fileLock(fileName,record,lock,fd,i-1,1);
        if(read(fd, (char*)&record,sizeof(record)) != sizeof(record)) break;
        if(record.book_Num >0 ) {count ++;;}
        lock.l_type = F_ULOCK;
        fcntl(fd,F_SETLK,&lock);
        lseek(fd, i*sizeof(record),SEEK_SET);
    }
    return count;
    close(fd);
}
void listUpID(char fileName[], struct Book record, struct flock lock){
    int fd = open(fileName,O_RDONLY);
    for(int i =1; ; i++){
        fileLock(fileName,record,lock,fd,i,1);
        lseek(fd, i*sizeof(record),SEEK_SET);
        if(read(fd, (char*)&record,sizeof(record)) != sizeof(record)) break;
        if(record.book_Num >0 ) {printf("%d",i);printList(record);}
        lock.l_type = F_ULOCK;
        fcntl(fd,F_SETLK,&lock);
    }
    close(fd);
}
int cmp(const void* a, const void* b){
    return strcmp(((BK*)a)->book_Name, ((BK*)b)->book_Name);
}
void listUpName(char fileName[], struct Book record, struct flock lock){
    int fd = open(fileName,O_RDONLY);
    fileLock(fileName,record,lock,fd,0,1);
    lock.l_type = F_ULOCK;
    fcntl(fd,F_SETLK,&lock);
    int count = dbLength(fileName,record,lock);
    BK* temp = (BK*) malloc(sizeof(BK)* count);
    int num = 0;
    for(int i =1; ; i++){
        if(read(fd, (char*)&record,sizeof(record)) != sizeof(record)) break;
        if(record.book_Num >0 ) {
            temp[num] = record;
            num++;
        }
        lseek(fd, i*sizeof(record),SEEK_SET);
    }
    qsort(temp,count,sizeof(BK),cmp);
    for(int i=0;i<count;i++) printList(temp[i]);
    free(temp);
    close(fd);
}
void addData(char fileName[], struct Book record, struct flock lock){
    int search = 0;
    char sel;
    printf("추가 할 도서정보의 도서번호를 입력하시오 (1 이상)   ");
    scanf("%d",&search);
    if(search < 1){
        printf("Error ... (도서번호 1이상) \n\n");
        return;
    }
    int fd = open(fileName,O_RDWR);
    lseek(fd,search * sizeof(record),SEEK_SET);
    read(fd, (char*)&record,sizeof(record));
    if(record.book_Num > 0){
        printf("Error ... (중복 데이터)\n\n");
        close(fd);
        return;
    }
    record.book_Num = search;
    fileLock(fileName,record,lock,fd,record.book_Num,2);
    printf("도서명, 저자명, 출판년월일, 가격, 추천리뷰(공백포함) \n");
    scanf("%s %s %s %d %[^\n]s",record.book_Name,record.writer,record.publishing,&record.price,record.review);
    lseek(fd,record.book_Num*sizeof(record),SEEK_SET);
    write(fd,(char*)&record,sizeof(record));
    lock.l_type = F_ULOCK;
    fcntl(fd,F_SETLK,&lock);
    record.book_Num= 0;
    close(fd);
    printf("데이터를 추가적으로 입력하시겠습니까 ?  (Y N)  ");
    scanf(" %c",&sel);
    if(sel == 'Y') addData(fileName,record,lock);
}
void  updateData(char fileName[], struct Book record, struct flock lock){
    int search = 0;
    printf("수정할 도서정보의 도서번호를 입력하시오  ");
    scanf("%d",&search);
    int fd = open(fileName,O_RDWR);
    lseek(fd,search * sizeof(record),SEEK_SET);
    read(fd, (char*)&record,sizeof(record));
    if(record.book_Num > 0){
        fileLock(fileName,record,lock,fd,record.book_Num,2);
        printf("도서명, 저자명, 출판년월일, 가격, 추천리뷰(공백포함) \n");
        scanf("%s %s %s %d %[^\n]s",record.book_Name,record.writer,record.publishing,&record.price,record.review);
        lseek(fd,record.book_Num*sizeof(record),SEEK_SET);
        write(fd,(char*)&record,sizeof(record));
        lock.l_type = F_ULOCK;
        fcntl(fd,F_SETLK,&lock);       
    }
    else printf("\n해당 도서정보가 존재하지 않습니다. \n\n");
    close(fd);
}
void  removeData(char fileName[], struct Book record, struct flock lock){
    int search = 0;
    printf("삭제할 도서정보의 도서번호를 입력하시오  ");
    scanf("%d",&search);
    int fd = open(fileName,O_RDWR);

    lseek(fd,search * sizeof(record),SEEK_SET);
    read(fd, (char*)&record,sizeof(record));
    if(record.book_Num > 0){
        fileLock(fileName,record,lock,fd,record.book_Num,2);
        record.book_Num = -1;
        lseek(fd,search*sizeof(record),SEEK_SET);
        write(fd,(char*)&record,sizeof(record));
        lock.l_type = F_ULOCK;
        fcntl(fd,F_SETLK,&lock); 
        printf("%d번 삭제완료 !!! \n\n",search);      
    }
    else printf("\n해당 도서정보가 존재하지 않습니다. \n\n");
    close(fd);
}
void searchDataTitle(char fileName[], struct Book record, struct flock lock){
    int fd = open(fileName,O_RDONLY);
    char search[50];
    printf("검색할 도서정보의 도서명을 입력하시오 (최소 2자) ");
    scanf("%s",search);
    for(int i = 1; ; i++){
        fileLock(fileName,record,lock,fd,i,1);
        lseek(fd, i*sizeof(record),SEEK_SET);
        if(read(fd, (char*)&record,sizeof(record)) != sizeof(record)) break;
        if(record.book_Num >0 && strstr(record.book_Name,search) ) {printList(record);}
        lock.l_type = F_ULOCK;
        fcntl(fd,F_SETLK,&lock);
    }
    close(fd);
}
void  searchDataKey(char fileName[], struct Book record, struct flock lock){
    int search = 0;
    printf("검색할 도서정보의 도서번호를 입력하시오  ");
    scanf("%d",&search);
    int fd = open(fileName,O_RDONLY);
    fileLock(fileName,record,lock,fd,search,1);
    lseek(fd,search * sizeof(record),SEEK_SET);
    read(fd, (char*)&record,sizeof(record));
    lock.l_type = F_ULOCK;
    fcntl(fd,F_SETLK,&lock); 
    if(record.book_Num > 0){
        printList(record);
    }
    else printf("\n해당 도서정보가 존재하지 않습니다. \n\n");
    close(fd);
}
void printSortFunc(){
    printf("1. Bubble sort\n");
    printf("2. Insert sort\n");
    printf("3. Seletion sort\n");
    printf("4. Quick sort\n");
    printf("5. Merge sort\n");
    printf("Choose Num -->   ");
}
void bubbleSort(BK* temp,int count){
    printf("Bubble ");
    BK x;
    for(int i=count-1; i>0; i--){
        for(int j=0; j<i; j++){
            if(strcmp(temp[j].book_Name,temp[j+1].book_Name) > 0){
                x = temp[j];
                temp[j] = temp[j+1];
                temp[j+1] = x;
            }
        }
    }
}
void insertSort(BK* temp,int count){
    printf("Insert ");
    BK x;
    for(int i=0;i<count;i++){
        int j=i-1;
        while(strcmp(temp[j].book_Name,temp[j+1].book_Name) > 0&&j>=0){
            x=temp[j];
            temp[j]=temp[j+1];
            temp[j+1]=x;
            j--;
        }
    }
}
void selectionSort(BK* temp,int count){
    printf("Selection ");
    BK x;
    char name[50];
    for(int i=0;i<count;++i){
		strcpy(name, temp[i].book_Name);
		int index = i;
		for(int j=i+1;j<count;++j){
			if(strcmp(name,temp[j].book_Name)  > 0){
				strcpy(name, temp[j].book_Name);
				index = j;
			}
		}
		x = temp[i];
		temp[i] = temp[index];
		temp[index] = x;
	}
}
void quickSort(BK* temp,int L,int R){
    int left = L, right = R;
    BK pivot = temp[(L + R) / 2];   
    BK x;
    do{
        while (strcmp(pivot.book_Name,temp[left].book_Name)  > 0)    
            left++;
        while (strcmp(pivot.book_Name,temp[right].book_Name)  < 0)    
            right--;
        if (left<= right)    
        {
            x = temp[left];
            temp[left] = temp[right];
            temp[right] = x;
            left++;
            right--;
        }
      } while (left<= right);    
    if (L < right)
        quickSort(temp, L, right);   
 
    if (left < R)
        quickSort(temp, left, R);  
}
void merge(BK* temp, int left, int mid, int right,int count){
  int i, j, k, l;
  i = left;
  j = mid+1;
  k = left;
  BK* x = (BK*)malloc(sizeof(BK)*count);

  while(i<=mid && j<=right){
    if(strcmp(temp[i].book_Name,temp[j].book_Name) <= 0)
      x[k++] = temp[i++];
    else
      x[k++] = temp[j++];
  }
  if(i>mid){
    for(l=j; l<=right; l++)
      x[k++] = temp[l];
  }
  else{
    for(l=i; l<=mid; l++)
      x[k++] = temp[l];
  }
  for(l=left; l<=right; l++){
    temp[l] = x[l];
  }
  free(x);
}
void mergeSort(BK* temp,int left,int right,int count){
  int mid;
  if(left<right){
    mid = (left+right)/2 ;
    mergeSort(temp, left, mid,count); 
    mergeSort(temp, mid+1, right,count); 
    merge(temp, left, mid, right,count); 
  }
}
void sortFunc(char fileName[], struct Book record, struct flock lock){
    int fd ,sel =0;
    double start,end;
    
    printSortFunc();
    scanf("%d",&sel);

    fd = open(fileName,O_RDONLY);
    fileLock(fileName,record,lock,fd,0,1);
    lock.l_type = F_ULOCK;
    fcntl(fd,F_SETLK,&lock);
    int count = dbLength(fileName,record,lock);
    BK* temp = (BK*) malloc(sizeof(BK)* count);
    int num = 0;
    for(int i =1; ; i++){
        if(read(fd, (char*)&record,sizeof(record)) != sizeof(record)) break;
        if(record.book_Num >0 ) {
            temp[num] = record;
            num++;
        }
        lseek(fd, i*sizeof(record),SEEK_SET);
    }
    close(fd);
    if(sel >0 && sel < 4){
        void (*fp)(BK*,int);
        if(sel == 1) fp = bubbleSort;
        if(sel == 2) fp = insertSort;
        if(sel == 3) fp = selectionSort;
        start = (double)clock() / CLOCKS_PER_SEC; 
        fp(temp,count);
        end = (((double)clock()) / CLOCKS_PER_SEC);
    }
    else if(sel == 4) {
        printf("Quick ");
        void (*fp)(BK*,int,int);
        fp = quickSort;
        start = (double)clock() / CLOCKS_PER_SEC; 
        fp(temp,0,count-1);
        end = (((double)clock()) / CLOCKS_PER_SEC);
        }
    else if(sel == 5) {
        printf("Merge ");
        void (*fp)(BK*,int,int,int);
        fp = mergeSort;
        start = (double)clock() / CLOCKS_PER_SEC; 
        fp(temp,0,count-1,count);
        end = (((double)clock()) / CLOCKS_PER_SEC);
        }
    else {
        printf("잘못된 번호입니다 !! ");
        free(temp);
        return;
    }
    printf("정렬 걸린 시간 : %1f \n",(end - start));
    for(int i=0;i<count;i++) printList(temp[i]);
    free(temp);
}
void loop(char fileName []){
    struct Book record;
    struct flock lock;
    int sel = -1;

    while(sel != 0){
        printMenu();
        scanf("%d",&sel);
        if(sel == 1) listUpID(fileName,record,lock);
        if(sel == 2) listUpName(fileName,record,lock);
        if(sel == 3) addData(fileName,record,lock);
        if(sel == 4) updateData(fileName,record,lock);
        if(sel == 5) removeData(fileName,record,lock);
        if(sel == 6) searchDataTitle(fileName,record,lock);
        if(sel == 7) searchDataKey(fileName,record,lock);
        if(sel == 8) sortFunc(fileName,record,lock);
    }
    
}
int main(int argc, char*argv[]){
    if(argc < 2){
        fprintf(stderr,"사용법 : ./%s file",argv[0]);
        exit(2);
    }
    int fd = open(argv[1],O_RDWR|O_CREAT, 0660);
    if(fd == -1){
        close(fd);
        perror(argv[1]);
        exit(2);
    }
    loop(argv[1]);
}