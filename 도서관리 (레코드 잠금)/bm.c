#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "BooKData.h"

void printMenu(){
    printf("1. List up All Book (Sort by 식별자)\n");
    printf("2. List up All Book (Sort by 도서명)\n");
    printf("3. Add New Book\n");
    printf("4. Update Book\n");
    printf("5. Remove a Book\n");
    printf("6. Search Book Information by Title(minimum 2char) \n");
    printf("7. Search Book Information by ID \n");
    printf("8. List up all c files in current directory \n");
    printf("0. Quit\n");
    printf("Choose num >>>   ");
}

void dbLength(char fileName[] , struct BookData record , int *count,struct flock lock){
    int fd = open(fileName,O_RDONLY);\
    for(int i =0; ; i++){
        lock.l_type = F_RDLCK;
        lock.l_whence = SEEK_SET;
        lock.l_start = i*sizeof(record);
        lock.l_len = sizeof(record);
        if (fcntl(fd,F_SETLKW, &lock) == -1) { /* 읽기 잠금 */
            perror(fileName);
            exit(3);
        }
        if(read(fd,(char*)&record,sizeof(record)) < 1)break;
        if(record.bookNum != 0)*count +=1;
        lseek(fd,i * sizeof(record),SEEK_SET);
        lock.l_type = F_UNLCK;
        fcntl(fd,F_SETLK, &lock);
    }
    close(fd);
}
void printList(BOOK temp){
    printf("\n1. 도서번호 :  %d\n",temp.bookNum);
    printf("2. 도서명 :  %s\n",temp.bookName);
    printf("3. 저자명 :  %s\n",temp.wrtier);
    printf("4. 출판년월일 :  %s\n",temp.publishing);
    printf("5. 가격 :  %d\n",temp.price);
    printf("6. 추천리뷰 :  %s\n\n",temp.review);
}

void printListByNum(char fileName[] , struct BookData record,struct flock lock ){
    int fd = open(fileName,O_RDONLY);
    for(int i =0; ; i++){
        lock.l_type = F_RDLCK;
        lock.l_whence = SEEK_SET;
        lock.l_start = i*sizeof(record);
        lock.l_len = sizeof(record);
        if (fcntl(fd,F_SETLKW, &lock) == -1) { /* 읽기 잠금 */
            perror(fileName);
            exit(3);
        }
        if(read(fd,(char*)&record,sizeof(record)) < 1)break;
        if(record.bookNum != 0) printList(record);
        lseek(fd,i * sizeof(record),SEEK_SET);
        lock.l_type = F_UNLCK;
        fcntl(fd,F_SETLK, &lock);
    }
    close(fd);
}
int compare(const void* a, const void *b){
    return strcmp(((BOOK*)a)->bookName,((BOOK*)b)->bookName);
}
void printListByName(char fileName[] , struct BookData record , int count,struct flock lock){
    int fd = open(fileName,O_RDONLY) ,num =0;
    lock.l_type = F_RDLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;
    if (fcntl(fd,F_SETLKW, &lock) == -1) { /* 읽기 잠금 */
        perror(fileName);
        exit(3);
    }
    lock.l_type = F_UNLCK;
    fcntl(fd,F_SETLK, &lock);
    BOOK* temp = (BOOK*)malloc(sizeof(BOOK)*count);
    for(int i =0; ; i++){
        int ret = read(fd,(char*)&record,sizeof(record));
        if(ret != sizeof(record))break;
        if(record.bookNum != 0){
            temp[num] =record;
            num++;
        }
        lseek(fd,i * sizeof(record),SEEK_SET);
    }
    qsort(temp,count,sizeof(BOOK),compare);
    for(int i = 0 ; i<count;i++) printList(temp[i]);
    close(fd);
    free(temp);
}
void addData(char fileName[] , struct BookData record , int *count,struct flock lock){
    int fd = open(fileName,O_RDWR);
    char sel;
    int search = 0;
    printf("도서번호   도서명   저자명   출판년월일    가격    추천리뷰\n");
    scanf("%d",&search);
    lseek(fd,(long)search* sizeof(record),SEEK_SET);
    read(fd,(char*)&record,sizeof(record));
    if(record.bookNum != 0) {
        printf("데이터가 이미 존재합니다 !! \n\n");
        return;
    }
    record.bookNum = search;
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = record.bookNum*sizeof(record) + sizeof(record);
    lock.l_len = sizeof(record);
    /* 쓰기 잠금 */
    if (fcntl(fd,F_SETLKW, &lock) == -1) {
        perror(fileName);
        exit(3);
    }
    scanf("%s %s %s %d %[^\n]s",record.bookName,record.wrtier,record.publishing, &record.price,record.review);
    lseek(fd,(long) -sizeof(record),SEEK_CUR);
    write(fd,(char*)&record,sizeof(record));
    *count +=1;
    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &lock);
    close(fd);
    printf("입력을 계속 하시겠습니까 ?  (Y N)   ");
    scanf(" %c",&sel);
    if(sel == 'Y') addData(fileName,record,count,lock
    );
}
void updateData(char fileName[] , struct BookData record,struct flock lock ){
    int fd = open(fileName,O_RDWR), search = 0;
    printf("수정할 데이터의 식별자를 입력하시오.    ");
    scanf("%d",&search);
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = search *sizeof(record);
    lock.l_len = sizeof(record);
    /* 쓰기 잠금 */
    if (fcntl(fd,F_SETLKW, &lock) == -1) {
        perror(fileName);
        exit(3);
    }
    lseek(fd,(long)search* sizeof(record),SEEK_SET);
    read(fd,(char*)&record,sizeof(record));
    if(record.bookNum != 0){
        printf("도서명   저장명   출판년월일    가격    추천리뷰\n");
        scanf("%s %s %s %d %[^\n]s",record.bookName,record.wrtier,record.publishing, &record.price,record.review);
        lseek(fd,(long) -sizeof(record),SEEK_CUR);
        write(fd,(char*)&record,sizeof(record));
    }
    else printf("헤딩 데이터가 존재하지 않습니다!!! \n\n");
    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &lock);
    close(fd);
}
void removeData(char fileName[] , struct BookData record , int *count ,struct flock lock){
    int fd = open(fileName,O_RDWR), search = 0;
    printf("삭제할 데이터의 식별자를 입력하시오.  ");
    scanf("%d",&search);
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = search *sizeof(record);
    lock.l_len = sizeof(record);
    /* 쓰기 잠금 */
    if (fcntl(fd,F_SETLKW, &lock) == -1) {
        perror(fileName);
        exit(3);
    }
    lseek(fd,(long)search* sizeof(record),SEEK_SET);
    read(fd,(char*)&record,sizeof(record));
    if(record.bookNum != 0){
        record.bookNum = 0;
        lseek(fd,(long) -sizeof(record),SEEK_CUR);
        write(fd,(char*)&record,sizeof(record));
        *count -=1;
    }
    else printf("헤딩 데이터가 존재하지 않습니다!!! \n\n");
    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &lock);
    close(fd);
}
void searchKeyData(char fileName[] , struct BookData record ,struct flock lock){
    int search;
    int fd = open(fileName, O_RDONLY);
    printf("검색하실 도서정보의 도서번호를 입력하시오.   ");
    scanf("%d", &search);
    lock.l_type = F_RDLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = search *sizeof(record);
    lock.l_len = sizeof(record);
    /* 쓰기 잠금 */
    if (fcntl(fd,F_SETLKW, &lock) == -1) {
        perror(fileName);
        exit(3);
    }
    lseek(fd, search * sizeof(record), SEEK_SET);
    read(fd, (char *)&record, sizeof(record));
    if (record.bookNum != 0)
        printList(record);
    else
        printf("해당 번호를 가진 도서정보가 존재하지 않습니다. \n\n");
    
    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &lock);
    close(fd);
}
void searchData(char fileName[] , struct BookData record,struct flock lock ){
    int fd = open(fileName,O_RDONLY);
    char search[MAX];
    printf("검색할 데이터의 이름을 입력하시오.  (최소 2자)  ");
    scanf("%s",search);
    for(int i =0; ; i++){
        lock.l_type = F_RDLCK;
        lock.l_whence = SEEK_SET;
        lock.l_start = 0;
        lock.l_len = sizeof(record);
        if (fcntl(fd,F_SETLKW, &lock) == -1) { /* 읽기 잠금 */
            perror(fileName);
            exit(3);
        }
        int ret = read(fd,(char*)&record,sizeof(record));
        if(ret != sizeof(record))break;
        if(record.bookNum != 0 && strstr(record.bookName,search)) printList(record);
        lseek(fd,i * sizeof(record),SEEK_SET);
        lock.l_type = F_UNLCK;
        fcntl(fd, F_SETLK, &lock);
    }
    close(fd);
}
void    cFilePrint(void)
{
    DIR                *dp;
    char            *dir;
    struct dirent    *d;
    struct stat        st;
    char            path[BUFSIZ + 1];

    dir = ".";
    if ((dp = opendir(dir)) == NULL)
    {
        perror(dir);
    }
    while ((d = readdir(dp)) != NULL)
    {
        if (strrchr(d->d_name, '.') != NULL && (strcmp(strrchr(d->d_name, '.'),
                    ".c")) == 0)
        {
            printf("\n%s \n\n", d->d_name);
        }
    }
    closedir(dp);
}
void loop(char fileName[]){
    int sel = -1,count =0;
    struct BookData record;
    struct flock lock;

    dbLength(fileName,record,&count,lock);
    while(sel != 0){
        printMenu();
        scanf("%d",&sel);
        if(sel == 1) printListByNum(fileName,record,lock);
        if(sel == 2) printListByName(fileName,record,count,lock);
        if(sel == 3) addData(fileName,record,&count,lock);
        if(sel == 4) updateData(fileName,record,lock);
        if(sel == 5) removeData(fileName,record,&count,lock);
        if(sel == 6) searchData(fileName,record,lock);
        if(sel == 7) searchKeyData(fileName,record,lock);
        if(sel == 8) cFilePrint();
    }
}
int main(int argc, char* argv[]){
    int fd = open(argv[1],O_RDWR|O_CREAT ,0660);
    if(argc < 2){
        fprintf(stderr,"사용법 : %s file \n",argv[1]);
        exit(1);
    }
    if(fd == -1){
        perror(argv[1]);
        exit(1);
    }
    close(fd);
    loop(argv[1]);
}

